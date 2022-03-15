#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

CU_ImplementCUSmartPtr( castor3d, AnimatedObjectGroupCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::AnimatedObjectGroup, castor::String >::Name = cuT( "AnimatedObjectGroup" );
}

namespace castor
{
	using namespace castor3d;

	namespace
	{
		void doInitialiseBuffer( GpuBufferOffsetT< castor3d::SkinningTransformsConfiguration > & transforms )
		{
			auto buffer = transforms.getData();

			for ( auto & dst : buffer )
			{
				std::fill_n( dst.bonesMatrix.begin()
					, dst.bonesMatrix.size()
					, castor::Matrix4x4f::getIdentity() );
			}

			transforms.buffer->markDirty( transforms.getOffset()
				, transforms.getSize()
				, VK_ACCESS_UNIFORM_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
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
		, m_morphingData{ m_device.bufferPool->getBuffer< MorphingBufferConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, 10'000ull
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
		, m_skinningTransformsData{ m_device.bufferPool->getBuffer< SkinningTransformsConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, 1'000ull
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
		doInitialiseBuffer( m_skinningTransformsData );
	}

	void ResourceCacheT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_morphingData )
		{
			m_morphingData = m_device.bufferPool->getBuffer< MorphingBufferConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, 10'000ull
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			m_skinningTransformsData = m_device.bufferPool->getBuffer< SkinningTransformsConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, 1'000ull
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			doInitialiseBuffer( m_skinningTransformsData );
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::cleanup()
	{
		ResourceCacheBaseT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::cleanup();
		m_device.bufferPool->putBuffer( m_skinningTransformsData );
		m_device.bufferPool->putBuffer( m_morphingData );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		auto skinningTransformsBuffer = m_skinningTransformsData.getData();

		for ( auto & pair : m_skeletonEntries )
		{
			auto & entry = pair.second;
			auto max = entry.skeleton.fillShader( &skinningTransformsBuffer[entry.skeleton.getId() - 1u] );
			m_skinningTransformsData.buffer->markDirty( m_skinningTransformsData.getOffset() + ( entry.skeleton.getId() - 1u ) * sizeof( SkinningTransformsConfiguration )
				, sizeof( castor::Matrix4x4f ) * max
				, VK_ACCESS_UNIFORM_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
		}

		auto morphingBuffer = m_morphingData.getData();

		for ( auto & pair : m_meshEntries )
		{
			auto & entry = pair.second;

			if ( entry.mesh.isPlayingAnimation() )
			{
				auto & morphingData = morphingBuffer[entry.mesh.getId() - 1u];
				morphingData.time->x = entry.mesh.getPlayingAnimation().getRatio();
				m_morphingData.buffer->markDirty( m_morphingData.getOffset() + ( entry.mesh.getId() - 1u ) * sizeof( MorphingBufferConfiguration )
					, sizeof( MorphingBufferConfiguration )
					, VK_ACCESS_UNIFORM_READ_BIT
					, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
			}
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::clear( RenderDevice const & device )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		doClearNoLock();
		m_meshEntries.clear();
		m_skeletonEntries.clear();
	}

	std::vector< AnimatedObject * > ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::findObject( castor::String const & name )const
	{
		std::vector< AnimatedObject * > result;
		auto lock( castor::makeUniqueLock( *this ) );

		for ( auto it : *this )
		{
			if ( auto animObject = it.second->findObject( name ) )
			{
				result.push_back( animObject );
			}
		}

		return result;
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::MeshPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doCreateEntry( RenderDevice const & device
		, AnimatedObjectGroup const & group
		, AnimatedMesh const & mesh )
	{
		return
		{
			group,
			mesh,
		};
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::SkeletonPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doCreateEntry( RenderDevice const & device
		, AnimatedObjectGroup const & group
		, AnimatedSkeleton const & skeleton )
	{
		return
		{
			group,
			skeleton,
		};
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( RenderDevice const & device
		, AnimatedMesh const & mesh )
	{
		m_meshEntries.erase( &mesh );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( RenderDevice const & device
		, AnimatedSkeleton const & skeleton )
	{
		m_skeletonEntries.erase( &skeleton );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( castor3d::RenderDevice const & device
		, castor3d::AnimatedTexture const & texture )
	{
		getOwner()->getEngine()->getMaterialCache().unregisterTexture( texture );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRegister( AnimatedObjectGroup & group )
	{
		m_meshAddedConnections.emplace( &group
			, group.onMeshAdded.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedMesh & mesh )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &pgroup, &mesh]( RenderDevice const & device
							, QueueData const & queueData )
						{
							m_meshEntries.emplace( &mesh
								, doCreateEntry( device, pgroup, mesh ) );
							mesh.setId( uint32_t( m_meshEntries.size() ) );
						} ) );
				} ) );
		m_meshRemovedConnections.emplace( &group
			, group.onMeshRemoved.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedMesh & mesh )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &mesh]( RenderDevice const & device
							, QueueData const & queueData )
						{
							mesh.setId( 0u );
							doRemoveEntry( device, mesh );
						} ) );
				} ) );
		m_skeletonAddedConnections.emplace( &group
			, group.onSkeletonAdded.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedSkeleton & skeleton )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &pgroup, &skeleton]( RenderDevice const & device
							, QueueData const & queueData )
						{
							m_skeletonEntries.emplace( &skeleton
								, doCreateEntry( device, pgroup, skeleton ) );
							skeleton.setId( uint32_t( m_skeletonEntries.size() ) );
						} ) );
				} ) );
		m_skeletonRemovedConnections.emplace( &group
			, group.onSkeletonRemoved.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedSkeleton & skeleton )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &skeleton]( RenderDevice const & device
							, QueueData const & queueData )
						{
							skeleton.setId( 0u );
							doRemoveEntry( device, skeleton );
						} ) );
				} ) );
		m_textureRemovedConnections.emplace( &group
			, group.onTextureRemoved.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedTexture & texture )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &texture]( RenderDevice const & device
							, QueueData const & queueData )
						{
							doRemoveEntry( device, texture );
						} ) );
				} ) );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doUnregister( AnimatedObjectGroup & group )
	{
		m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
			, [this, &group]( RenderDevice const & device
				, QueueData const & queueData )
			{
				m_meshAddedConnections.erase( &group );
				m_meshRemovedConnections.erase( &group );
				m_skeletonAddedConnections.erase( &group );
				m_skeletonRemovedConnections.erase( &group );
				m_textureRemovedConnections.erase( &group );

				for ( auto & pair : group.getObjects() )
				{
					switch ( pair.second->getKind() )
					{
					case AnimationType::eMesh:
						doRemoveEntry( device, static_cast< AnimatedMesh const & >( *pair.second ) );
						break;
					case AnimationType::eSkeleton:
						doRemoveEntry( device, static_cast< AnimatedSkeleton const & >( *pair.second ) );
						break;
					case AnimationType::eTexture:
						doRemoveEntry( device, static_cast< AnimatedTexture const & >( *pair.second ) );
						break;
					default:
						break;
					}
				}
			} ) );
	}
}
