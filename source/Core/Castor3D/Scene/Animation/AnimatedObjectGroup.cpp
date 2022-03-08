#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"

#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"
#include "Castor3D/Scene/Geometry.hpp"

namespace castor3d
{
	namespace
	{
		template< typename FuncT, typename ParamT >
		bool applyAnimationFunc( GroupAnimationMap & animations
			, AnimatedObjectPtrStrMap & objects
			, castor::String const & name
			, FuncT func
			, ParamT const & value
			, size_t outputOffset )
		{
			auto itAnim = animations.find( name );
			auto result = itAnim != animations.end();

			if ( result )
			{
				for ( auto it : objects )
				{
					if ( it.second->hasAnimation( name ) )
					{
						auto & animation = it.second->getAnimation( name );
						( animation.*func )( value );
					}
				}

				auto buffer = reinterpret_cast< uint8_t * >( &itAnim->second );
				*reinterpret_cast< ParamT * >( buffer + outputOffset ) = value;
			}

			return result;
		}
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup( castor::String const & name, Scene & scene )
		: castor::Named( name )
		, castor::OwnedBy< Scene >( scene )
	{
		m_timer.getElapsed();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()
	{
		m_objects.clear();
		m_animations.clear();
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( SceneNode & node
		, castor::String const & name )
	{
		auto object = std::make_shared< AnimatedSceneNode >( name, node );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return std::static_pointer_cast< AnimatedObject >( object );
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( Mesh & mesh
		, Geometry & geometry
		, castor::String const & name )
	{
		auto object = std::make_shared< AnimatedMesh >( name, mesh, geometry );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return std::static_pointer_cast< AnimatedObject >( object );
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( Skeleton & skeleton
		, Mesh & mesh
		, Geometry & geometry
		, castor::String const & name )
	{
		auto object = std::make_shared< AnimatedSkeleton >( name, skeleton, mesh, geometry );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return std::static_pointer_cast< AnimatedObject >( object );
	}

	AnimatedObjectSPtr AnimatedObjectGroup::addObject( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
	{
		auto object = std::make_shared< AnimatedTexture >( sourceInfo, config , pass );

		if ( !addObject( object ) )
		{
			object.reset();
		}

		return std::static_pointer_cast< AnimatedObject >( object );
	}

	bool AnimatedObjectGroup::addObject( AnimatedObjectSPtr object )
	{
		bool result = object && m_objects.find( object->getName() ) == m_objects.end();

		if ( result )
		{
			m_objects.insert( { object->getName(), object } );

			switch ( object->getKind() )
			{
			case AnimationType::eSceneNode:
				onSceneNodeAdded( *this, static_cast< AnimatedSceneNode & >( *object ) );
				break;
			case AnimationType::eSkeleton:
				onSkeletonAdded( *this, static_cast< AnimatedSkeleton & >( *object ) );
				break;
			case AnimationType::eMesh:
				onMeshAdded( *this, static_cast< AnimatedMesh & >( *object ) );
				break;
			case AnimationType::eTexture:
				onTextureAdded( *this, static_cast< AnimatedTexture & >( *object ) );
				break;
			default:
				break;
			}
		}

		for ( auto it : m_animations )
		{
			object->addAnimation( it.first );
			auto & animation = object->getAnimation( it.first );
			animation.setLooped( it.second.looped );
			animation.setScale( it.second.scale );
			animation.setStartingPoint( it.second.startingPoint );
			animation.setStoppingPoint( it.second.stoppingPoint );
		}

		return result;
	}

	AnimatedObject * AnimatedObjectGroup::findObject( castor::String const & name )const
	{
		for ( auto & it : m_objects )
		{
			if ( it.first == name )
			{
				return it.second.get();
			}
		}

		return nullptr;
	}

	void AnimatedObjectGroup::addAnimation( castor::String const & name )
	{
		if ( m_animations.find( name ) == m_animations.end() )
		{
			m_animations.insert( { name, { name, AnimationState::eStopped, false, 1.0f } } );

			for ( auto it : m_objects )
			{
				it.second->addAnimation( name );
			}
		}
	}

	void AnimatedObjectGroup::setAnimationLooped( castor::String const & name
		, bool looped )
	{
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setLooped
			, looped
			, offsetof( GroupAnimation, looped ) );
	}

	void AnimatedObjectGroup::setAnimationScale( castor::String const & name
		, float scale )
	{
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setScale
			, scale
			, offsetof( GroupAnimation, scale ) );
	}

	void AnimatedObjectGroup::setAnimationStartingPoint( castor::String const & name
		, castor::Milliseconds value )
	{
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStartingPoint
			, value
			, offsetof( GroupAnimation, startingPoint ) );
	}

	void AnimatedObjectGroup::setAnimationStoppingPoint( castor::String const & name
		, castor::Milliseconds value )
	{
		applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStoppingPoint
			, value
			, offsetof( GroupAnimation, stoppingPoint ) );
	}

	void AnimatedObjectGroup::update( CpuUpdater & updater )
	{
#if defined( NDEBUG )

		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );

#else

		auto tslf = 25_ms;

#endif

		for ( auto it : m_objects )
		{
			it.second->update( tslf );
		}
	}

	void AnimatedObjectGroup::startAnimation( castor::String const & name )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->startAnimation( name );
			}

			itAnim->second.state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::stopAnimation( castor::String const & name )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->stopAnimation( name );
			}

			itAnim->second.state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::pauseAnimation( castor::String const & name )
	{
		auto itAnim = m_animations.find( name );

		if ( itAnim != m_animations.end() )
		{
			for ( auto it : m_objects )
			{
				it.second->pauseAnimation( name );
			}

			itAnim->second.state = AnimationState::ePaused;
		}
	}

	void AnimatedObjectGroup::startAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->startAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::stopAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->stopAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::pauseAllAnimations()
	{
		for ( auto it : m_objects )
		{
			it.second->pauseAllAnimations();
		}

		for ( auto it : m_animations )
		{
			it.second.state = AnimationState::ePaused;
		}
	}
}
