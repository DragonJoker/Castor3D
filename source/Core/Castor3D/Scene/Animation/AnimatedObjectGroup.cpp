#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"

#include "Castor3D/Animation/Animation.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObject.hpp"
#include "Castor3D/Scene/Animation/AnimatedMesh.hpp"
#include "Castor3D/Scene/Animation/AnimatedSceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"
#include "Castor3D/Scene/Geometry.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, AnimatedObjectGroup )

namespace c3d
{
	namespace anmobjgrp
	{
		template< typename FuncT, typename ParamT >
		static bool applyAnimationFunc( GroupAnimationMap & animations
			, AnimatedObjectGroup::AnimatedObjectMap const & objects
			, String const & name
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
				*reinterpret_cast< ParamPtr >( buffer + outputOffset ) = value;
			}

			return result;
		}

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedObject, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );

				if ( auto geometry = blockContext->scene->scene->findGeometry( name ) )
				{
					if ( auto node = geometry->getParent();
						node && node->hasAnimation() )
					{
						blockContext->animNode = blockContext->animGroup->addObject( *node
							, node->getName() );
					}

					if ( auto mesh = geometry->getMesh() )
					{
						if ( mesh->hasAnimation() )
						{
							blockContext->animMesh = blockContext->animGroup->addObject( *mesh
								, *geometry
								, geometry->getName() );
						}

						if ( auto skeleton = mesh->getSkeleton() )
						{
							blockContext->animSkeleton = blockContext->animGroup->addObject( *skeleton
								, *mesh
								, *geometry
								, geometry->getName() );
						}
					}
				}
				else
				{
					if ( auto node = blockContext->scene->scene->findSceneNode( name ) )
					{
						if ( node->hasAnimation() )
						{
							blockContext->animNode = blockContext->animGroup->addObject( *node
								, node->getName() );
						}
					}
					else
					{
						CU_ParsingError( cuT( "No geometry or node with name " ) + name );
					}
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedMesh, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );

				if ( auto geometry = blockContext->scene->scene->findGeometry( name ) )
				{
					if ( auto mesh = geometry->getMesh() )
					{
						if ( mesh->hasAnimation() )
						{
							blockContext->animMesh = blockContext->animGroup->addObject( *mesh
								, *geometry
								, geometry->getName() );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Geometry [" ) + name + cuT( "] has no mesh" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No geometry with name [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedSkeleton, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );

				if ( auto geometry = blockContext->scene->scene->findGeometry( name ) )
				{
					if ( auto mesh = geometry->getMesh() )
					{
						if ( auto skeleton = mesh->getSkeleton() )
						{
							blockContext->animSkeleton = blockContext->animGroup->addObject( *skeleton
								, *mesh
								, *geometry
								, geometry->getName() );
						}
						else
						{
							CU_ParsingError( cuT( "Geometry [" ) + name + cuT( "]'s mesh has no skeleton" ) );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Geometry [" ) + name + cuT( "] has no mesh" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No geometry with name [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedNode, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );

				if ( auto node = blockContext->scene->scene->findSceneNode( name ) )
				{
					if ( node->hasAnimation() )
					{
						blockContext->animNode = blockContext->animGroup->addObject( *node
							, node->getName() );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No node with name [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimation, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				params[0]->get( blockContext->animName );
				blockContext->animGroup->addAnimation( blockContext->animName );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eAnimation, blockContext )

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimationStart, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->startAnimation( params[0]->get< String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimationPause, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->pauseAnimation( params[0]->get< String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupEnd, AnimGroupContext )
		{
			if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
			else
			{
				log::info << "Loaded animated object group [" << blockContext->animGroup->getName() << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserAnimationLooped, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationLooped( blockContext->animName, params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationScale, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationScale( blockContext->animName, params[0]->get< float >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationStartAt, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationStartingPoint( blockContext->animName
					, Milliseconds{ uint64_t( params[0]->get< float >() * 1000.0f ) } );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationStopAt, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationStoppingPoint( blockContext->animName
					, Milliseconds{ uint64_t( params[0]->get< float >() * 1000.0f ) } );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationInterpolation, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationInterpolation( blockContext->animName
					, InterpolatorType( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationEnd, AnimGroupContext )
		{
			// Only push the block
		}
		CU_EndAttributePop()
	}

	//*************************************************************************************************

	AnimatedObjectGroup::AnimatedObjectGroup( String const & name, Scene & scene, bool serialisable )
		: Named{ name }
		, OwnedBy< Scene >{ scene }
		, m_serialisable{ serialisable }
	{
		m_timer.getElapsed();
	}

	AnimatedObjectGroup::~AnimatedObjectGroup()noexcept
	{
		m_objects.clear();
		m_animations.clear();
	}

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( SceneNode & node
		, String const & name )
	{
		auto object = makeUniqueDerived< AnimatedObject, AnimatedSceneNode >( name + cuT( "_Node" ), node );
		auto result = object.get();

		if ( !addObject( c3d::move( object ) ) )
		{
			result = {};
		}

		return result;
	}

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( Mesh & mesh
		, Geometry & geometry
		, String const & name )
	{
		auto object = makeUniqueDerived< AnimatedObject, AnimatedMesh >( name + cuT( "_Mesh" ), mesh, geometry );
		auto result = object.get();

		if ( !addObject( c3d::move( object ) ) )
		{
			result = {};
		}

		return result;
	}

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( Skeleton & skeleton
		, Mesh & mesh
		, Geometry & geometry
		, String const & name )
	{
		auto object = makeUniqueDerived< AnimatedObject, AnimatedSkeleton >( name + cuT( "_Skeleton" ), skeleton, mesh, geometry );
		auto result = object.get();

		if ( !addObject( c3d::move( object ) ) )
		{
			result = {};
		}

		return result;
	}

	AnimatedObjectRPtr AnimatedObjectGroup::addObject( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
	{
		auto object = makeUniqueDerived< AnimatedObject, AnimatedTexture >( sourceInfo, config , pass );
		auto result = object.get();

		if ( !addObject( c3d::move( object ) ) )
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
				m_objects.try_emplace( name, c3d::move( object ) );

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

			for ( auto & [nm, group] : m_animations )
			{
				obj->addAnimation( nm );
				auto & animation = obj->getAnimation( nm );
				animation.setLooped( group.looped );
				animation.setScale( group.scale );
				animation.setStartingPoint( group.startingPoint );
				animation.setStoppingPoint( group.stoppingPoint );

				if ( group.totalTime < animation.getAnimation().getLength() )
				{
					group.totalTime = animation.getAnimation().getLength();

					for ( auto const & [_, groupObject] : m_objects )
					{
						groupObject->getAnimation( nm ).setTotalLength( group.totalTime );
					}
				}
			}
		}

		return result;
	}

	AnimatedObject * AnimatedObjectGroup::findObject( StringView name )const
	{
		for ( auto const & [nm, obj] : m_objects )
		{
			if ( nm == name )
			{
				return obj.get();
			}
		}

		return nullptr;
	}

	bool AnimatedObjectGroup::addAnimation( String const & name )
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

	void AnimatedObjectGroup::setAnimationLooped( String const & name
		, bool looped )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setLooped
			, looped
			, offsetof( GroupAnimation, looped ) );
	}

	void AnimatedObjectGroup::setAnimationScale( String const & name
		, float scale )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setScale
			, scale
			, offsetof( GroupAnimation, scale ) );
	}

	void AnimatedObjectGroup::setAnimationStartingPoint( String const & name
		, Milliseconds value )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStartingPoint
			, value
			, offsetof( GroupAnimation, startingPoint ) );
	}

	void AnimatedObjectGroup::setAnimationStoppingPoint( String const & name
		, Milliseconds value )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setStoppingPoint
			, value
			, offsetof( GroupAnimation, stoppingPoint ) );
	}

	void AnimatedObjectGroup::setAnimationInterpolation( String const & name
		, InterpolatorType mode )
	{
		anmobjgrp::applyAnimationFunc( m_animations
			, m_objects
			, name
			, &AnimationInstance::setInterpolation
			, mode
			, offsetof( GroupAnimation, interpolation ) );
	}

	void AnimatedObjectGroup::update( [[maybe_unused]] CpuUpdater const & updater )
	{
#if defined( NDEBUG )

		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() );

#else

		auto tslf = 25_ms;

#endif

		for ( auto const & [nm, obj] : m_objects )
		{
			obj->update( tslf );
		}
	}

	void AnimatedObjectGroup::startAnimation( String const & name )
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

	void AnimatedObjectGroup::stopAnimation( String const & name )
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

	void AnimatedObjectGroup::pauseAnimation( String const & name )
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

	void AnimatedObjectGroup::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< AnimGroupContext > groupCtx{ result, CSCNSection::eAnimGroup, CSCNSection::eScene };
		BlockParserContextT< AnimGroupContext > animCtx{ result, CSCNSection::eAnimation, CSCNSection::eAnimGroup };

		groupCtx.addParser( cuT( "animated_object" ), anmobjgrp::parserAnimatedObjectGroupAnimatedObject, { makeParameter< ParameterType::eName >() } );
		groupCtx.addParser( cuT( "animated_mesh" ), anmobjgrp::parserAnimatedObjectGroupAnimatedMesh, { makeParameter< ParameterType::eName >() } );
		groupCtx.addParser( cuT( "animated_skeleton" ), anmobjgrp::parserAnimatedObjectGroupAnimatedSkeleton, { makeParameter< ParameterType::eName >() } );
		groupCtx.addParser( cuT( "animated_node" ), anmobjgrp::parserAnimatedObjectGroupAnimatedNode, { makeParameter< ParameterType::eName >() } );
		groupCtx.addParser( cuT( "start_animation" ), anmobjgrp::parserAnimatedObjectGroupAnimationStart, { makeParameter< ParameterType::eName >() } );
		groupCtx.addParser( cuT( "pause_animation" ), anmobjgrp::parserAnimatedObjectGroupAnimationPause, { makeParameter< ParameterType::eName >() } );
		groupCtx.addPushParser( cuT( "animation" ), CSCNSection::eAnimation, anmobjgrp::parserAnimatedObjectGroupAnimation, { makeParameter< ParameterType::eName >() } );
		groupCtx.addPopParser( cuT( "}" ), anmobjgrp::parserAnimatedObjectGroupEnd );

		animCtx.addParser( cuT( "looped" ), anmobjgrp::parserAnimationLooped, { makeParameter< ParameterType::eBool >() } );
		animCtx.addParser( cuT( "scale" ), anmobjgrp::parserAnimationScale, { makeParameter< ParameterType::eFloat >() } );
		animCtx.addParser( cuT( "start_at" ), anmobjgrp::parserAnimationStartAt, { makeParameter< ParameterType::eFloat >() } );
		animCtx.addParser( cuT( "stop_at" ), anmobjgrp::parserAnimationStopAt, { makeParameter< ParameterType::eFloat >() } );
		animCtx.addParser( cuT( "interpolation" ), anmobjgrp::parserAnimationInterpolation, { makeParameter< ParameterType::eCheckedText, InterpolatorType >() } );
		animCtx.addPopParser( cuT( "}" ), anmobjgrp::parserAnimationEnd );
	}

	String getPrefix( AnimGroupContext const & context )
	{
		return getPrefix( *context.scene );
	}

	Engine * getEngine( AnimGroupContext const & context )
	{
		return getEngine( *context.scene );
	}
}
