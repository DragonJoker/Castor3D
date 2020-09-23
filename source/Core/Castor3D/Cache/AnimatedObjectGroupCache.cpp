#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"

using namespace castor;

namespace castor3d
{
	template<> const String CacheTraits< AnimatedObjectGroup, String >::Name = cuT( "AnimatedObjectGroup" );

	AnimatedObjectGroupCache::Cache( Engine & engine
		, Producer && produce
		, Initialiser && initialise
		, Cleaner && clean
		, Merger && merge )
		: MyCache{ engine
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge ) }
	{
		getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_updateTimer = std::make_shared< RenderPassTimer >( *getEngine()
					, cuT( "Update" )
					, cuT( "Animation UBOs" )
					, 1u );
			} ) );
	}

	AnimatedObjectGroupCache::~Cache()
	{
	}

	void AnimatedObjectGroupCache::update()
	{
		for ( auto & pair : m_skeletonEntries )
		{
			auto & entry = pair.second;
			auto & skinningData = entry.skinningUbo.getData();
			entry.skeleton.fillShader( skinningData.bonesMatrix );
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

	AnimatedObjectGroupCache::MeshPoolsEntry AnimatedObjectGroupCache::getUbos( AnimatedMesh const & mesh )const
	{
		return m_meshEntries.at( &mesh );
	}

	AnimatedObjectGroupCache::SkeletonPoolsEntry AnimatedObjectGroupCache::getUbos( AnimatedSkeleton const & skeleton )const
	{
		return m_skeletonEntries.at( &skeleton );
	}

	void AnimatedObjectGroupCache::clear()
	{
		MyCache::clear();
		auto & uboPools = *getCurrentRenderDevice( *getEngine() ).uboPools;

		for ( auto & entry : m_meshEntries )
		{
			uboPools.putBuffer( entry.second.morphingUbo );
		}

		for ( auto & entry : m_skeletonEntries )
		{
			uboPools.putBuffer( entry.second.skinningUbo );
		}

		m_meshEntries.clear();
		m_skeletonEntries.clear();
	}

	void AnimatedObjectGroupCache::add( ElementPtr element )
	{
		MyCache::add( element->getName(), element );
		doRegister( *element );
	}

	AnimatedObjectGroupSPtr AnimatedObjectGroupCache::add( Key const & name )
	{
		auto result = MyCache::add( name );
		doRegister( *result );
		return result;
	}

	void AnimatedObjectGroupCache::remove( Key const & name )
	{
		auto lock( castor::makeUniqueLock( m_elements ) );

		if ( m_elements.has( name ) )
		{
			auto element = m_elements.find( name );
			m_elements.erase( name );
			doUnregister( *element );
		}
	}

	AnimatedObjectGroupCache::MeshPoolsEntry AnimatedObjectGroupCache::doCreateEntry( AnimatedObjectGroup const & group
		, AnimatedMesh const & mesh )
	{
		auto & uboPools = *getCurrentRenderDevice( *getEngine() ).uboPools;
		return
		{
			group,
			mesh,
			uboPools.getBuffer< MorphingUboConfiguration >( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
		};
	}

	AnimatedObjectGroupCache::SkeletonPoolsEntry AnimatedObjectGroupCache::doCreateEntry( AnimatedObjectGroup const & group
		, AnimatedSkeleton const & skeleton )
	{
		auto & uboPools = *getCurrentRenderDevice( *getEngine() ).uboPools;
		return
		{
			group,
			skeleton,
			uboPools.getBuffer< SkinningUboConfiguration >( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
		};
	}

	void AnimatedObjectGroupCache::doRemoveEntry( AnimatedMesh const & mesh )
	{
		auto & uboPools = *getCurrentRenderDevice( *getEngine() ).uboPools;
		auto entry = getUbos( mesh );
		m_meshEntries.erase( &mesh );
		uboPools.putBuffer( entry.morphingUbo );
	}

	void AnimatedObjectGroupCache::doRemoveEntry( AnimatedSkeleton const & skeleton )
	{
		auto & uboPools = *getCurrentRenderDevice( *getEngine() ).uboPools;
		auto entry = getUbos( skeleton );
		m_skeletonEntries.erase( &skeleton );
		uboPools.putBuffer( entry.skinningUbo );
	}

	void AnimatedObjectGroupCache::doRegister( AnimatedObjectGroup & group )
	{
		m_meshAddedConnections.emplace( &group, group.onMeshAdded.connect( [this]( AnimatedObjectGroup const & group
			, AnimatedMesh const & mesh )
		{
			m_meshEntries.emplace( &mesh
				, doCreateEntry( group, mesh ) );
		} ) );
		m_meshRemovedConnections.emplace( &group, group.onMeshRemoved.connect( [this]( AnimatedObjectGroup const & group
			, AnimatedMesh const & mesh )
		{
			doRemoveEntry( mesh );
		} ) );
		m_skeletonAddedConnections.emplace( &group, group.onSkeletonAdded.connect( [this]( AnimatedObjectGroup const & group
			, AnimatedSkeleton const & skeleton )
		{
			m_skeletonEntries.emplace( &skeleton
				, doCreateEntry( group, skeleton ) );
		} ) );
		m_skeletonRemovedConnections.emplace( &group, group.onSkeletonRemoved.connect( [this]( AnimatedObjectGroup const & group
			, AnimatedSkeleton const & skeleton )
		{
			doRemoveEntry( skeleton );
		} ) );
	}

	void AnimatedObjectGroupCache::doUnregister( AnimatedObjectGroup & group )
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
				doRemoveEntry( static_cast< AnimatedMesh const & >( *pair.second ) );
				break;

			case AnimationType::eSkeleton:
				doRemoveEntry( static_cast< AnimatedSkeleton const & >( *pair.second ) );
				break;

			default:
				break;
			}
		}
	}
}
