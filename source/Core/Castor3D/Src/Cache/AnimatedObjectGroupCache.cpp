#include "AnimatedObjectGroupCache.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Animation/AnimatedMesh.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Mesh/Submesh.hpp"
#include "Scene/Scene.hpp"

#include <Miscellaneous/Hash.hpp>

using namespace castor;

namespace castor3d
{
	template<> const String CacheTraits< AnimatedObjectGroup, String >::Name = cuT( "AnimatedObjectGroup" );

	AnimatedObjectGroupCache::Cache( Engine & engine
		, Producer && produce
		, Initialiser && initialise
		, Cleaner && clean
		, Merger && merge )
		: MyCache( engine
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge ) )
		, m_skinningUboPool{ *engine.getRenderSystem() }
		, m_morphingUboPool{ *engine.getRenderSystem() }
	{
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
	}

	void AnimatedObjectGroupCache::uploadUbos()
	{
		m_skinningUboPool.upload();
		m_morphingUboPool.upload();
	}

	void AnimatedObjectGroupCache::cleanupUbos()
	{
		m_skinningUboPool.cleanup();
		m_morphingUboPool.cleanup();
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

		for ( auto & entry : m_meshEntries )
		{
			m_morphingUboPool.putBuffer( entry.second.morphingUbo );
		}

		for ( auto & entry : m_skeletonEntries )
		{
			m_skinningUboPool.putBuffer( entry.second.skinningUbo );
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
		auto lock = castor::makeUniqueLock( m_elements );

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
		return
		{
			group,
			mesh,
			m_morphingUboPool.getBuffer( renderer::MemoryPropertyFlag::eHostVisible ),
		};
	}

	AnimatedObjectGroupCache::SkeletonPoolsEntry AnimatedObjectGroupCache::doCreateEntry( AnimatedObjectGroup const & group
		, AnimatedSkeleton const & skeleton )
	{
		return
		{
			group,
			skeleton,
			m_skinningUboPool.getBuffer( renderer::MemoryPropertyFlag::eHostVisible ),
		};
	}

	void AnimatedObjectGroupCache::doRemoveEntry( AnimatedMesh const & mesh )
	{
		auto entry = getUbos( mesh );
		m_meshEntries.erase( &mesh );
		m_morphingUboPool.putBuffer( entry.morphingUbo );
	}

	void AnimatedObjectGroupCache::doRemoveEntry( AnimatedSkeleton const & skeleton )
	{
		auto entry = getUbos( skeleton );
		m_skeletonEntries.erase( &skeleton );
		m_skinningUboPool.putBuffer( entry.skinningUbo );
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
