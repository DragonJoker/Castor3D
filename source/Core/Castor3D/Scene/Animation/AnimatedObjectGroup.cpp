#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"

#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"
#include "Castor3D/Scene/Geometry.hpp"

CU_ImplementSmartPtr( castor3d, AnimatedObjectGroup )

namespace castor3d
{
	namespace anmobjgrp
	{
		template< typename FuncT, typename ParamT >
		static bool applyAnimationFunc( GroupAnimationMap & animations
			, AnimatedObjectGroup::AnimatedObjectMap & objects
			, castor::String const & name
			, FuncT func
			, ParamT const & value
			, size_t outputOffset )
		{
			auto itAnim = animations.find( name );
			auto result = itAnim != animations.end();

			if ( result )
			{
				for ( auto const & [_, animated] : objects )
				{
					if ( animated->hasAnimation( name ) )
					{
						auto & animation = animated->getAnimation( name );
						( animation.*func )( value );
					}
				}

				auto buffer = BytePtr( &itAnim->second );
				using ParamPtr = ParamT *;
				*ParamPtr( buffer + outputOffset ) = value;
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

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( SceneNode & node
		, castor::String const & name )
	{
		auto object = castor::makeUniqueDerived< AnimatedObject, AnimatedSceneNode >( name + "_Node", node );
		auto result = object.get();

		if ( !addObject( std::move( object ) ) )
		{
			result = {};
		}

		return result;
	}

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( Mesh & mesh
		, Geometry & geometry
		, castor::String const & name )
	{
		auto object = castor::makeUniqueDerived< AnimatedObject, AnimatedMesh >( name + "_Mesh", mesh, geometry );
		auto result = object.get();

		if ( !addObject( std::move( object ) ) )
		{
			result = {};
		}

		return result;
	}

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( Skeleton & skeleton
		, Mesh & mesh
		, Geometry & geometry
		, castor::String const & name )
	{
		auto object = castor::makeUniqueDerived< AnimatedObject, AnimatedSkeleton >( name + "_Skeleton", skeleton, mesh, geometry );
		auto result = object.get();

		if ( !addObject( std::move( object ) ) )
		{
			result = {};
		}

		return result;
	}

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
	{
		auto object = castor::makeUniqueDerived< AnimatedObject, AnimatedTexture >( sourceInfo, config , pass );
		auto result = object.get();

		if ( !addObject( std::move( object ) ) )
		{
			result = {};
		}

		return result;
	}

	bool AnimatedObjectGroup::addObject( AnimatedObjectUPtr object )
	{
		auto name = object->getName();
		bool result = object && m_objects.find( name ) == m_objects.end();

		if ( auto obj = object.get() )
		{
			if ( result )
			{
				m_objects.try_emplace( name, std::move( object ) );

				switch ( obj->getKind() )
				{
				case AnimationType::eSceneNode:
					onSceneNodeAdded( *this, static_cast< AnimatedSceneNode & >( *obj ) );
					break;
				case AnimationType::eSkeleton:
					onSkeletonAdded( *this, static_cast< AnimatedSkeleton & >( *obj ) );
					break;
				case AnimationType::eMesh:
					onMeshAdded( *this, static_cast< AnimatedMesh & >( *obj ) );
					break;
				case AnimationType::eTexture:
					onTextureAdded( *this, static_cast< AnimatedTexture & >( *obj ) );
					break;
				default:
					break;
				}
			}

			for ( auto const & [nm, group] : m_animations )
			{
				obj->addAnimation( nm );
				auto & animation = obj->getAnimation( nm );
				animation.setLooped( group.looped );
				animation.setScale( group.scale );
				animation.setStartingPoint( group.startingPoint );
				animation.setStoppingPoint( group.stoppingPoint );
			}
		}

		return result;
	}

	AnimatedObject * AnimatedObjectGroup::findObject( castor::String const & name )const
	{
		for ( auto & [nm, obj] : m_objects )
		{
			if ( nm == name )
			{
				return obj.get();
			}
		}

		return nullptr;
	}

	bool AnimatedObjectGroup::addAnimation( castor::String const & name )
	{
		bool result = false;

		if ( m_animations.find( name ) == m_animations.end() )
		{
			result = true;
			m_animations.try_emplace( name, name, AnimationState::eStopped, false, 1.0f );

			for ( auto const & [nm, obj] : m_objects )
			{
				obj->addAnimation( name );
			}
		}

		return result;
	}

	void AnimatedObjectGroup::setAnimationLooped( castor::String const & name
		, bool looped )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setLooped
			, looped
			, offsetof( GroupAnimation, looped ) );
	}

	void AnimatedObjectGroup::setAnimationScale( castor::String const & name
		, float scale )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setScale
			, scale
			, offsetof( GroupAnimation, scale ) );
	}

	void AnimatedObjectGroup::setAnimationStartingPoint( castor::String const & name
		, castor::Milliseconds value )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStartingPoint
			, value
			, offsetof( GroupAnimation, startingPoint ) );
	}

	void AnimatedObjectGroup::setAnimationStoppingPoint( castor::String const & name
		, castor::Milliseconds value )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStoppingPoint
			, value
			, offsetof( GroupAnimation, stoppingPoint ) );
	}

	void AnimatedObjectGroup::setAnimationInterpolation( castor::String const & name
		, InterpolatorType mode )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setInterpolation
			, mode
			, offsetof( GroupAnimation, interpolation ) );
	}

	void AnimatedObjectGroup::update( [[maybe_unused]] CpuUpdater & updater )
	{
#if defined( NDEBUG )

		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );

#else

		auto tslf = 25_ms;

#endif

		for ( auto const & [nm, obj] : m_objects )
		{
			obj->update( tslf );
		}
	}

	void AnimatedObjectGroup::startAnimation( castor::String const & name )
	{
		if ( auto itAnim = m_animations.find( name );
			itAnim != m_animations.end() )
		{
			for ( auto const & [nm, obj] : m_objects )
			{
				obj->startAnimation( name );
			}

			itAnim->second.state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::stopAnimation( castor::String const & name )
	{
		if ( auto itAnim = m_animations.find( name );
			itAnim != m_animations.end() )
		{
			for ( auto const & [nm, obj] : m_objects )
			{
				obj->stopAnimation( name );
			}

			itAnim->second.state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::pauseAnimation( castor::String const & name )
	{
		if ( auto itAnim = m_animations.find( name );
			itAnim != m_animations.end() )
		{
			for ( auto const & [nm, obj] : m_objects )
			{
				obj->pauseAnimation( name );
			}

			itAnim->second.state = AnimationState::ePaused;
		}
	}

	void AnimatedObjectGroup::startAllAnimations()
	{
		for ( auto const & [nm, obj] : m_objects )
		{
			obj->startAllAnimations();
		}

		for ( auto & [nm, group] : m_animations )
		{
			group.state = AnimationState::ePlaying;
		}
	}

	void AnimatedObjectGroup::stopAllAnimations()
	{
		for ( auto const & [nm, obj] : m_objects )
		{
			obj->stopAllAnimations();
		}

		for ( auto & [nm, group] : m_animations )
		{
			group.state = AnimationState::eStopped;
		}
	}

	void AnimatedObjectGroup::pauseAllAnimations()
	{
		for ( auto const & [nm, obj] : m_objects )
		{
			obj->pauseAllAnimations();
		}

		for ( auto & [nm, group] : m_animations )
		{
			group.state = AnimationState::ePaused;
		}
	}

	Engine * getEngine( AnimGroupContext const & context )
	{
		return getEngine( *context.scene );
	}
}
