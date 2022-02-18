#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"

CU_ImplementCUSmartPtr( castor3d, AnimatedObjectGroupCache )

namespace castor3d
{
	const castor::String PtrCacheTraitsT< castor3d::AnimatedObjectGroup, castor::String >::Name = cuT( "AnimatedObjectGroup" );
}

namespace castor
{
	using namespace castor3d;

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
	{
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::update( CpuUpdater & updater )
	{
		for ( auto & pair : m_skeletonEntries )
		{
			auto & entry = pair.second;
			auto skinningData = entry.skinningSsbo.lock();
			entry.skeleton.fillShader( skinningData );
			entry.skinningSsbo.flush();
			entry.skinningSsbo.unlock();
		}

		for ( auto & pair : m_meshEntries )
		{
			auto & entry = pair.second;
			auto & morphingData = entry.morphingUbo.getData();

			if ( entry.mesh.isPlayingAnimation() )
			{
				morphingData.time = entry.mesh.getPlayingAnimation().getRatio();
			}
		}
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::MeshPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::getUbos( AnimatedMesh const & mesh )const
	{
		return m_meshEntries.at( &mesh );
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::SkeletonPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::getUbos( AnimatedSkeleton const & skeleton )const
	{
		return m_skeletonEntries.at( &skeleton );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::clear( RenderDevice const & device )
	{
		auto lock( castor::makeUniqueLock( *this ) );
		doClearNoLock();
		auto & uboPools = *device.uboPools;
		auto & bufferPool = *device.bufferPool;

		for ( auto & entry : m_meshEntries )
		{
			uboPools.putBuffer( entry.second.morphingUbo );
		}

		for ( auto & entry : m_skeletonEntries )
		{
			bufferPool.putBuffer( entry.second.skinningSsbo );
		}

		m_meshEntries.clear();
		m_skeletonEntries.clear();
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::MeshPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doCreateEntry( RenderDevice const & device
		, AnimatedObjectGroup const & group
		, AnimatedMesh const & mesh )
	{
		auto & uboPools = *device.uboPools;
		return
		{
			group,
			mesh,
			uboPools.getBuffer< MorphingUboConfiguration >( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
		};
	}

	ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::SkeletonPoolsEntry ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doCreateEntry( RenderDevice const & device
		, AnimatedObjectGroup const & group
		, AnimatedSkeleton const & skeleton )
	{
		auto & bufferPool = *device.bufferPool;
		return
		{
			group,
			skeleton,
			bufferPool.getBuffer< SkinningUboConfiguration >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, 400u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
		};
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( RenderDevice const & device
		, AnimatedMesh const & mesh )
	{
		auto & uboPools = *device.uboPools;
		auto entry = getUbos( mesh );
		m_meshEntries.erase( &mesh );
		uboPools.putBuffer( entry.morphingUbo );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRemoveEntry( RenderDevice const & device
		, AnimatedSkeleton const & skeleton )
	{
		auto & bufferPool = *device.bufferPool;
		auto entry = getUbos( skeleton );
		m_skeletonEntries.erase( &skeleton );
		bufferPool.putBuffer( entry.skinningSsbo );
	}

	void ResourceCacheT< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >::doRegister( AnimatedObjectGroup & group )
	{
		m_meshAddedConnections.emplace( &group
			, group.onMeshAdded.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedMesh const & mesh )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &pgroup, &mesh]( RenderDevice const & device
							, QueueData const & queueData )
						{
							m_meshEntries.emplace( &mesh
								, doCreateEntry( device, pgroup, mesh ) );
						} ) );
				} ) );
		m_meshRemovedConnections.emplace( &group
			, group.onMeshRemoved.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedMesh const & mesh )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &mesh]( RenderDevice const & device
							, QueueData const & queueData )
						{
							doRemoveEntry( device, mesh );
						} ) );
				} ) );
		m_skeletonAddedConnections.emplace( &group
			, group.onSkeletonAdded.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedSkeleton const & skeleton )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &pgroup, &skeleton]( RenderDevice const & device
							, QueueData const & queueData )
						{
							m_skeletonEntries.emplace( &skeleton
								, doCreateEntry( device, pgroup, skeleton ) );
						} ) );
				} ) );
		m_skeletonRemovedConnections.emplace( &group
			, group.onSkeletonRemoved.connect( [this]( AnimatedObjectGroup const & pgroup
				, AnimatedSkeleton const & skeleton )
				{
					m_engine.sendEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, &skeleton]( RenderDevice const & device
							, QueueData const & queueData )
						{
							doRemoveEntry( device, skeleton );
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

					default:
						break;
					}
				}
			} ) );
	}
}
