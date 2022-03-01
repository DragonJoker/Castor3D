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
		void doInitialiseBuffer( ashes::Buffer< castor3d::SkinningTransformsConfiguration > & transforms )
		{
			if ( auto buffer = transforms.lock( 0u, ashes::WholeSize, 0u ) )
			{
				auto view = castor::makeArrayView( buffer, buffer + 1000u );

				for ( auto & dst : view )
				{
					std::fill_n( dst.bonesMatrix.begin()
						, dst.bonesMatrix.size()
						, castor::Matrix4x4f::getIdentity() );
				}

				transforms.flush( 0u, ashes::WholeSize );
				transforms.unlock();
			}
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
		, m_morphingData{ makeBuffer< MorphingBufferConfiguration >( m_device
			, 10'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "MorphingNodesData" ) }
		, m_skinningTransformsData{ makeBuffer< SkinningTransformsConfiguration >( m_device
			, 1'000ull
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "SkinningTransformNodesData" ) }
	{
		doInitialiseBuffer( *m_skinningTransformsData );
	}

	void ResourceCacheT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::initialise( RenderDevice const & device )
	{
		if ( !m_morphingData )
		{
			m_morphingData = makeBuffer< MorphingBufferConfiguration >( m_device
				, 10'000ull
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "MorphingNodesData" );
			m_skinningTransformsData = makeBuffer< SkinningTransformsConfiguration >( m_device
				, 1'000ull
				, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "SkinningTransformNodesData" );
			doInitialiseBuffer( *m_skinningTransformsData );
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::cleanup()
	{
		ResourceCacheBaseT< AnimatedObjectGroup, castor::String, AnimatedObjectGroupCacheTraits >::cleanup();
		m_skinningTransformsData.reset();
		m_morphingData.reset();
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::update( CpuUpdater & updater )
	{
		auto lock( castor::makeUniqueLock( *this ) );

		if ( auto skinningTransformsBuffer = m_skinningTransformsData->lock( 0u, ashes::WholeSize, 0u ) )
		{
			for ( auto & pair : m_skeletonEntries )
			{
				auto & entry = pair.second;
				entry.skeleton.fillShader( &skinningTransformsBuffer[entry.skeleton.getId() - 1u] );
			}

			m_skinningTransformsData->flush( 0u, ashes::WholeSize );
			m_skinningTransformsData->unlock();
		}

		if ( auto morphingBuffer = m_morphingData->lock( 0u, ashes::WholeSize, 0u ) )
		{
			for ( auto & pair : m_meshEntries )
			{
				auto & entry = pair.second;
				auto & morphingData = morphingBuffer[entry.mesh.getId() - 1u];

				if ( entry.mesh.isPlayingAnimation() )
				{
					morphingData.time->x = entry.mesh.getPlayingAnimation().getRatio();
				}
			}

			m_morphingData->flush( 0u, ashes::WholeSize );
			m_morphingData->unlock();
		}
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::clear( RenderDevice const & device )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		doClearNoLock();
		m_meshEntries.clear();
		m_skeletonEntries.clear();
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
				m_textureAddedConnections.erase( &group );
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
