#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, AnimatedObjectGroupCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::AnimatedObjectGroup, castor::String >::Name = cuT( "AnimatedObjectGroup" );
}

namespace castor
{
	using namespace castor3d;

	namespace cacheanmgrp
	{
		static void doInitialiseBuffer( GpuBufferOffsetT< castor3d::SkinningTransformsConfiguration > & transforms )
		{
			for ( auto & dst : transforms.getData() )
			{
				std::fill_n( dst.bonesMatrix.begin()
					, dst.bonesMatrix.size()
					, castor::Matrix4x4f::getIdentity() );
			}

			transforms.markDirty( VK_ACCESS_SHADER_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
		}

		static size_t makeHash( castor3d::AnimatedMesh const & mesh
			, castor3d::Submesh const & submesh )
		{
			auto hash = std::hash< castor3d::AnimatedMesh const * >{}( &mesh );
			return hashCombinePtr( hash, submesh );
		}
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::ResourceCacheT( Scene & scene )
		: OwnedBy< Scene >{ scene }
		, ElementCacheT{ scene.getEngine()->getLogger()
			, [this]( ElementT & resource )
			{
				doRegister( resource );
			}
			, [this]( ElementT & resource )
			{
				doUnregister( resource );
			}
			, castor::ResourceMergerT< AnimatedObjectGroupCache >{ scene.getName() } }
		, m_engine{ *scene.getEngine() }
		, m_device{ m_engine.getRenderSystem()->getRenderDevice() }
		, m_morphingWeights{ m_device.bufferPool->getBuffer< MorphingWeightsConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, MaxMorphingDataCount
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_skinningTransformsData{ m_device.bufferPool->getBuffer< SkinningTransformsConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
			, MaxSkinningDataCount
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
#if C3D_DebugTimers
		m_timerAnimations = castor::makeUnique< crg::FramePassTimer >( m_device.makeContext(), getScene()->getName() + "/Animations", crg::TimerScope::eUpdate );
		m_engine.registerTimer( getScene()->getName() + "/Animations", *m_timerAnimations );
#endif
		cacheanmgrp::doInitialiseBuffer( m_skinningTransformsData );
	}

	ResourceCacheT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::~ResourceCacheT()noexcept
	{
#if C3D_DebugTimers
		m_engine.unregisterTimer( getScene()->getName() + "/Animations", *m_timerAnimations );
		m_timerAnimations.reset();
#endif

		if ( m_skinningTransformsData )
		{
			m_device.bufferPool->putBuffer( m_skinningTransformsData );
		}

		if ( m_morphingWeights )
		{
			m_device.bufferPool->putBuffer( m_morphingWeights );
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::initialise( RenderDevice const & )
	{
		if ( !m_morphingWeights )
		{
			m_morphingWeights = m_device.bufferPool->getBuffer< MorphingWeightsConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, MaxMorphingDataCount
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			m_skinningTransformsData = m_device.bufferPool->getBuffer< SkinningTransformsConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, MaxSkinningDataCount
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			cacheanmgrp::doInitialiseBuffer( m_skinningTransformsData );
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::cleanup()
	{
		ResourceCacheBaseT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::cleanup();

		if ( m_skinningTransformsData )
		{
			m_device.bufferPool->putBuffer( m_skinningTransformsData );
			m_skinningTransformsData = {};
		}

		if ( m_morphingWeights )
		{
			m_device.bufferPool->putBuffer( m_morphingWeights );
			m_morphingWeights = {};
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::update( CpuUpdater & updater )
	{
#if C3D_DebugTimers
		auto block( m_timerAnimations->start() );
#endif
		auto lock( castor::makeUniqueLock( *this ) );

		for ( auto const & [name, group] : *this )
		{
			group->update( updater );
		}

		auto skinningTransformsBuffer = m_skinningTransformsData.getData();

		for ( auto const & [skeleton, entry] : m_skeletonEntries )
		{
			if ( auto id = entry.skeleton.getId() )
			{
				if ( auto max = entry.skeleton.fillBuffer( &skinningTransformsBuffer[id - 1u] );
					max > 0 )
				{
					m_skinningTransformsData.buffer->markDirty( m_skinningTransformsData.getOffset() + ( id - 1u ) * sizeof( SkinningTransformsConfiguration )
						, sizeof( castor::Matrix4x4f ) * max
						, VK_ACCESS_UNIFORM_READ_BIT
						, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
				}
			}
		}

		auto morphingBuffer = m_morphingWeights.getData();

		for ( auto const & [mesh, entry] : m_meshEntries )
		{
			if ( auto id = entry.mesh.getId( entry.submesh );
				id && entry.submesh.getMorphTargetsCount() )
			{
				if ( auto max = entry.mesh.fillBuffer( entry.submesh, &morphingBuffer[id - 1u] ) )
				{
					auto offset = m_morphingWeights.getOffset() + ( id - 1u ) * sizeof( MorphingWeightsConfiguration );
					m_morphingWeights.buffer->markDirty( offset
						, sizeof( castor::Point4ui ) + sizeof( float ) * max
						, VK_ACCESS_UNIFORM_READ_BIT
						, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
					offset += sizeof( float ) * MaxMorphTargets * 4u;
					m_morphingWeights.buffer->markDirty( offset
						, sizeof( uint32_t ) * max
						, VK_ACCESS_UNIFORM_READ_BIT
						, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
				}
				else
				{
					m_morphingWeights.buffer->markDirty( m_morphingWeights.getOffset() + ( id - 1u ) * sizeof( MorphingWeightsConfiguration )
						, sizeof( castor::Point4ui )
						, VK_ACCESS_UNIFORM_READ_BIT
						, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
				}
			}
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::clear( RenderDevice const & )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		doClearNoLock();
		m_meshEntries.clear();
		m_skeletonEntries.clear();
	}

	castor::Vector< AnimatedObject * > ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::findObject( castor::String const & name )const
	{
		castor::Vector< AnimatedObject * > result;
		auto lock( castor::makeUniqueLock( *this ) );

		for ( auto const & [_, group] : *this )
		{
			if ( auto animObject = group->findObject( name ) )
			{
				result.push_back( animObject );
			}
		}

		return result;
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::MeshPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doCreateEntry( RenderDevice const &
		, AnimatedObjectGroup const & group
		, AnimatedMesh const & mesh
		, castor3d::Submesh const & submesh )const
	{
		return
		{
			group,
			mesh,
			submesh,
		};
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::SkeletonPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doCreateEntry( RenderDevice const &
		, AnimatedObjectGroup const & group
		, AnimatedSkeleton const & skeleton )const
	{
		return
		{
			group,
			skeleton,
		};
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( RenderDevice const &
		, AnimatedMesh const & mesh
		, castor3d::Submesh const & submesh )
	{
		m_meshEntries.erase( cacheanmgrp::makeHash( mesh, submesh ) );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( RenderDevice const &
		, AnimatedSkeleton const & skeleton )
	{
		m_skeletonEntries.erase( &skeleton );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( castor3d::RenderDevice const &
		, castor3d::AnimatedTexture const & texture )const
	{
		getOwner()->getEngine()->getMaterialCache().unregisterTexture( texture );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRegister( AnimatedObjectGroup & group )
	{
		m_meshAddedConnections.try_emplace( &group
			, group.onMeshAdded.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedMesh & mesh )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
						, [this, &pgroup, &mesh]( RenderDevice const & device
							, QueueData const & )
						{
							for ( auto & submesh : mesh.getMesh() )
							{
								if ( submesh->getMorphTargetsCount() )
								{
									m_meshEntries.try_emplace( cacheanmgrp::makeHash( mesh, *submesh )
										, doCreateEntry( device, pgroup, mesh, *submesh ) );
									mesh.setId( *submesh
										, uint32_t( m_meshEntries.size() ) );
								}
							}
						} ) );
				} ) );
		m_meshRemovedConnections.try_emplace( &group
			, group.onMeshRemoved.connect( [this]( AnimatedObjectGroup const &
				, AnimatedMesh & mesh )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
						, [this, &mesh]( RenderDevice const & device
							, QueueData const & )
						{
							for ( auto & submesh : mesh.getMesh() )
							{
								if ( submesh->getMorphTargetsCount() )
								{
									mesh.setId( *submesh, 0u );
									doRemoveEntry( device, mesh, *submesh );
								}
							}
						} ) );
				} ) );
		m_skeletonAddedConnections.try_emplace( &group
			, group.onSkeletonAdded.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedSkeleton & skeleton )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
						, [this, &pgroup, &skeleton]( RenderDevice const & device
							, QueueData const & )
						{
							m_skeletonEntries.try_emplace( &skeleton
								, doCreateEntry( device, pgroup, skeleton ) );
							skeleton.setId( uint32_t( m_skeletonEntries.size() ) );
						} ) );
				} ) );
		m_skeletonRemovedConnections.try_emplace( &group
			, group.onSkeletonRemoved.connect( [this]( AnimatedObjectGroup const &
				, AnimatedSkeleton & skeleton )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
						, [this, &skeleton]( RenderDevice const & device
							, QueueData const & )
						{
							skeleton.setId( 0u );
							doRemoveEntry( device, skeleton );
						} ) );
				} ) );
		m_textureRemovedConnections.try_emplace( &group
			, group.onTextureRemoved.connect( [this]( AnimatedObjectGroup const &
				, AnimatedTexture & texture )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
						, [this, &texture]( RenderDevice const & device
							, QueueData const & )
						{
							doRemoveEntry( device, texture );
						} ) );
				} ) );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doUnregister( AnimatedObjectGroup & group )
	{
		m_engine.sendEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
			, [this, &group]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_meshAddedConnections.erase( &group );
				m_meshRemovedConnections.erase( &group );
				m_skeletonAddedConnections.erase( &group );
				m_skeletonRemovedConnections.erase( &group );
				m_textureRemovedConnections.erase( &group );

				for ( auto & [name, object] : group.getObjects() )
				{
					switch ( object->getKind() )
					{
					case AnimationType::eMesh:
						for ( auto const & submesh : static_cast< AnimatedMesh const & >( *object ).getMesh() )
						{
							doRemoveEntry( device
								, static_cast< AnimatedMesh const & >( *object )
								, *submesh );
						}
						break;
					case AnimationType::eSkeleton:
						doRemoveEntry( device
							, static_cast< AnimatedSkeleton const & >( *object ) );
						break;
					case AnimationType::eTexture:
						doRemoveEntry( device
							, static_cast< AnimatedTexture const & >( *object ) );
						break;
					default:
						break;
					}
				}
			} ) );
	}
}
