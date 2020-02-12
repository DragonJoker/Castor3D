#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"

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
		: MyCache( engine
			, std::move( produce )
			, std::move( initialise )
			, std::move( clean )
			, std::move( merge ) )
		, m_skinningUboPool{ *engine.getRenderSystem(), cuT( "SkinningUboPool" ) }
		, m_morphingUboPool{ *engine.getRenderSystem(), cuT( "MorphingUboPool" ) }
	{
		getEngine()->sendEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_updateTimer = std::make_shared< RenderPassTimer >( *getEngine()
					, cuT( "Update" )
					, cuT( "Animation UBOs" )
					, 2u );
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

	void AnimatedObjectGroupCache::uploadUbos()
	{
		auto count = m_skinningUboPool.getBufferCount()
			+ m_morphingUboPool.getBufferCount();
		m_updateTimer->updateCount( std::max( count, 2u ) );

		if ( count )
		{
			auto timerBlock = m_updateTimer->start();
			uint32_t index = 0u;
			m_skinningUboPool.upload( *m_updateTimer, index );
			index += std::max( m_skinningUboPool.getBufferCount(), 1u );
			m_morphingUboPool.upload( *m_updateTimer, index );
		}
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
		return
		{
			group,
			mesh,
			m_morphingUboPool.getBuffer( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
		};
	}

	AnimatedObjectGroupCache::SkeletonPoolsEntry AnimatedObjectGroupCache::doCreateEntry( AnimatedObjectGroup const & group
		, AnimatedSkeleton const & skeleton )
	{
		return
		{
			group,
			skeleton,
			m_skinningUboPool.getBuffer( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),
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
