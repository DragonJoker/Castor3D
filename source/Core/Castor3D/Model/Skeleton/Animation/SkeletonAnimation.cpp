#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, SkeletonAnimation )

namespace c3d
{
	//*************************************************************************************************

	namespace sklanm
	{
		static Map< SkeletonNodeType, String > const MovingTypeNames
		{
			{ SkeletonNodeType::eNode, cuT( "Node_" ) },
			{ SkeletonNodeType::eBone, cuT( "Bone_" ) },
		};

		struct SkeletonAnimationObjectContext
		{
			SkeletonRPtr skeleton{};
			SkeletonAnimationRPtr animation{};
			SkeletonAnimationObjectRPtr parent{};
		};

		C3D_API String getPrefix( SkeletonAnimationContext const & context );
		C3D_API Engine * getEngine( SkeletonAnimationContext const & context );

		static String const & getMovingTypeName( SkeletonNodeType type )
		{
			return MovingTypeNames.at( type );
		}

		static CU_ImplementAttributeParserNewBlock( parserRoot, SkeletonContext, SkeletonAnimationContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			else
			{
				newBlockContext->skeleton = blockContext;
				newBlockContext->animation = &blockContext->skeleton->createAnimation( params[0]->get< String >() );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkeletonAnimation )

		static CU_ImplementAttributeParserNewBlock( parserNode, SkeletonAnimationContext, SkeletonAnimationObjectContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else
			{
				if ( auto node = blockContext->skeleton->skeleton->findNode( params[0]->get< String >() ) )
				{
					if ( node->getType() == SkeletonNodeType::eNode )
					{
						auto animNode = makeUnique< SkeletonAnimationNode >( *blockContext->animation );
						animNode->setNode( *node );
						newBlockContext->parent = animNode.get();
						newBlockContext->animation = blockContext->animation;
						newBlockContext->skeleton = blockContext->skeleton->skeleton;
						blockContext->animation->addObject( ptrRefCast< SkeletonAnimationObject >( animNode ), nullptr );
					}
					else
					{
						CU_ParsingError( cuT( "Skeleton node [" ) + params[0]->get< String >() + cuT( "] is not a node." ) );
					}
				}
				else
					CU_ParsingError( cuT( "Couldn't find skeleton node named [" ) + params[0]->get< String >() + cuT( "]." ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkeletonAnimationObject )

		static CU_ImplementAttributeParserNewBlock( parserBone, SkeletonAnimationContext, SkeletonAnimationObjectContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else if ( auto node = blockContext->skeleton->skeleton->findNode( params[0]->get< String >() ) )
			{
				if ( node->getType() == SkeletonNodeType::eBone )
				{
					auto animBone = makeUnique< SkeletonAnimationBone >( *blockContext->animation );
					animBone->setBone( static_cast< BoneNode & >( *node ) );
					newBlockContext->parent = animBone.get();
					newBlockContext->animation = blockContext->animation;
					newBlockContext->skeleton = blockContext->skeleton->skeleton;
					blockContext->animation->addObject( ptrRefCast< SkeletonAnimationObject >( animBone ), nullptr );
				}
				else
				{
					CU_ParsingError( cuT( "Skeleton node [" ) + params[0]->get< String >() + cuT( "] is not a bone." ) );
				}
			}
			else
				CU_ParsingError( cuT( "Couldn't find skeleton node named [" ) + params[0]->get< String >() + cuT( "]." ) );
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkeletonAnimationObject )

		static CU_ImplementAttributeParserNewBlock( parserNodeRec, SkeletonAnimationObjectContext, SkeletonAnimationObjectContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else if ( !blockContext->parent )
				CU_ParsingError( cuT( "No parent object initialised." ) );
			else
			{
				if ( auto node = blockContext->skeleton->findNode( params[0]->get< String >() ) )
				{
					if ( node->getType() == SkeletonNodeType::eNode )
					{
						auto animNode = makeUnique< SkeletonAnimationNode >( *blockContext->animation );
						animNode->setNode( *node );
						newBlockContext->parent = animNode.get();
						newBlockContext->animation = blockContext->animation;
						newBlockContext->skeleton = blockContext->skeleton;
						blockContext->parent->addChild( animNode.get() );
						blockContext->animation->addObject( ptrRefCast< SkeletonAnimationObject >( animNode ), blockContext->parent );
					}
					else
					{
						CU_ParsingError( cuT( "Skeleton node [" ) + params[0]->get< String >() + cuT( "] is not a node." ) );
					}
				}
				else
					CU_ParsingError( cuT( "Couldn't find skeleton node named [" ) + params[0]->get< String >() + cuT( "]." ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkeletonAnimationObject )

		static CU_ImplementAttributeParserNewBlock( parserBoneRec, SkeletonAnimationObjectContext, SkeletonAnimationObjectContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else if ( !blockContext->parent )
				CU_ParsingError( cuT( "No parent object initialised." ) );
			else if ( auto node = blockContext->skeleton->findNode( params[0]->get< String >() ) )
			{
				if ( node->getType() == SkeletonNodeType::eBone )
				{
					auto animBone = makeUnique< SkeletonAnimationBone >( *blockContext->animation );
					animBone->setBone( static_cast< BoneNode & >( *node ) );
					newBlockContext->parent = animBone.get();
					newBlockContext->animation = blockContext->animation;
					newBlockContext->skeleton = blockContext->skeleton;
					blockContext->parent->addChild( animBone.get() );
					blockContext->animation->addObject( ptrRefCast< SkeletonAnimationObject >( animBone ), blockContext->parent );
				}
				else
				{
					CU_ParsingError( cuT( "Skeleton node [" ) + params[0]->get< String >() + cuT( "] is not a bone." ) );
				}
			}
			else
				CU_ParsingError( cuT( "Couldn't find skeleton node named [" ) + params[0]->get< String >() + cuT( "]." ) );
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkeletonAnimationObject )

		static CU_ImplementAttributeParserBlock( parserKeyframe, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
		}
		CU_EndAttributePushBlock( CSCNSection::eSkeletonAnimationKeyframe, blockContext )

		static CU_ImplementAttributeParserBlock( parserKeyframeIndex, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else
				blockContext->keyframe = makeUnique< SkeletonAnimationKeyFrame >( *blockContext->animation
					, Milliseconds{ params[0]->get< uint64_t >() } );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeNode, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
			{
				params[0]->get( blockContext->objectName );
				blockContext->objectType = SkeletonNodeType::eNode;
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eSkeletonAnimationKeyframeObject, blockContext )

		static CU_ImplementAttributeParserBlock( parserKeyframeBone, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
			{
				params[0]->get( blockContext->objectName );
				blockContext->objectType = SkeletonNodeType::eBone;
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eSkeletonAnimationKeyframeObject, blockContext )

		static CU_ImplementAttributeParserBlock( parserKeyframeObjectPosition, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				params[0]->get( blockContext->objectTransform.translate );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeObjectRotation, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				blockContext->objectTransform.rotate = Quaternion::fromAxisAngle( params[0]->get< Point3f >()
					, Angle::fromDegrees( params[1]->get< float >() ) );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeObjectScale, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				params[0]->get( blockContext->objectTransform.scale );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeObjectEnd, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else if ( blockContext->objectName.empty() )
				CU_ParsingError( cuT( "Keyframe object was not initialised." ) );
			else
			{
				blockContext->keyframe->addAnimationObject( *blockContext->keyframe->getOwner()->getObject( blockContext->objectType, blockContext->objectName )
					, blockContext->objectTransform.translate, blockContext->objectTransform.rotate, blockContext->objectTransform.scale );
				blockContext->objectType = {};
				blockContext->objectName = {};
				blockContext->objectTransform = NodeTransform{};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserKeyframeEnd, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				blockContext->animation->addKeyFrame( ptrRefCast< AnimationKeyFrame >( blockContext->keyframe ) );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserEnd, SkeletonAnimationContext )
		{
			if ( !blockContext->skeleton )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else
			{
				log::info << "Loaded skeleton animation [" << blockContext->animation->getName() << "]" << std::endl;
				*blockContext = {};
			}
		}
		CU_EndAttributePop()
	}

	//*************************************************************************************************

	SkeletonAnimation::SkeletonAnimation( Animable & animable
		, String const & name )
		: Animation{ *animable.getOwner(), AnimationType::eSkeleton, animable, name }
	{
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::addObject( SkeletonNode & node
		, SkeletonAnimationObject const * parent )
	{
		auto result = makeUnique< SkeletonAnimationNode >( *this );
		result->setNode( node );
		return addObject( ptrRefCast< SkeletonAnimationObject >( result ), parent );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::addObject( BoneNode & bone
		, SkeletonAnimationObject const * parent )
	{
		auto result = makeUnique< SkeletonAnimationBone >( *this );
		result->setBone( bone );
		return addObject( ptrRefCast< SkeletonAnimationObject >( result ), parent );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::addObject( SkeletonAnimationObjectUPtr object
		, SkeletonAnimationObject const * parent )
	{
		String name = sklanm::getMovingTypeName( object->getType() ) + object->getName();
		auto it = m_toMove.find( name );
		SkeletonAnimationObjectRPtr result{};

		if ( it == m_toMove.end() )
		{
			result = object.get();
			m_toMove.try_emplace( name, c3d::move( object ) );

			if ( !parent )
			{
				m_rootObjects.push_back( result );
			}
		}
		else
		{
			log::warn << cuT( "This object was already added: [" ) << name << cuT( "]" ) << std::endl;
			result = it->second.get();
		}

		return result;
	}

	bool SkeletonAnimation::hasObject( SkeletonNodeType type
		, String const & name )const
	{
		return m_toMove.find( sklanm::getMovingTypeName( type ) + name ) != m_toMove.end();
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::getObject( SkeletonNode const & node )const
	{
		return getObject( SkeletonNodeType::eNode, node.getName() );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::getObject( BoneNode const & bone )const
	{
		return getObject( SkeletonNodeType::eBone, bone.getName() );
	}

	SkeletonAnimationObjectRPtr SkeletonAnimation::getObject( SkeletonNodeType type
		, String const & name )const
	{
		SkeletonAnimationObjectRPtr result{};

		if ( auto it = m_toMove.find( sklanm::getMovingTypeName( type ) + name );
			it != m_toMove.end() )
		{
			result = it->second.get();
		}

		return result;
	}

	void SkeletonAnimation::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SkeletonContext > skeletonContext{ result, CSCNSection::eSkeleton, CSCNSection::eScene };
		BlockParserContextT< SkeletonContext > animationContext{ result, CSCNSection::eSkeletonAnimation, CSCNSection::eSkeleton };
		BlockParserContextT< SkeletonContext > animationObjectContext{ result, CSCNSection::eSkeletonAnimationObject, CSCNSection::eSkeletonAnimation };
		BlockParserContextT< SkeletonContext > animationKeyframeContext{ result, CSCNSection::eSkeletonAnimationKeyframe, CSCNSection::eSkeletonAnimation };
		BlockParserContextT< SkeletonContext > animationKeyframeObjectContext{ result, CSCNSection::eSkeletonAnimationKeyframeObject, CSCNSection::eSkeletonAnimationKeyframe };

		skeletonContext.addPushParser( cuT( "animation" ), CSCNSection::eSkeletonAnimation, sklanm::parserRoot, { makeParameter< ParameterType::eName >() } );

		animationContext.addPushParser( cuT( "bone" ), CSCNSection::eSkeletonAnimationObject, sklanm::parserBone, { makeParameter< ParameterType::eName >() } );
		animationContext.addPushParser( cuT( "node" ), CSCNSection::eSkeletonAnimationObject, sklanm::parserNode, { makeParameter< ParameterType::eName >() } );
		animationContext.addPushParser( cuT( "keyframe" ), CSCNSection::eSkeletonAnimationKeyframe, sklanm::parserKeyframe );
		animationContext.addPopParser( cuT( "}" ), sklanm::parserEnd );

		animationObjectContext.addPushParser( cuT( "bone" ), CSCNSection::eSkeletonAnimationObject, sklanm::parserBoneRec, { makeParameter< ParameterType::eName >() } );
		animationObjectContext.addPushParser( cuT( "node" ), CSCNSection::eSkeletonAnimationObject, sklanm::parserNodeRec, { makeParameter< ParameterType::eName >() } );

		animationKeyframeContext.addParser( cuT( "index" ), sklanm::parserKeyframeIndex, { makeParameter< ParameterType::eUInt64 >() } );
		animationKeyframeContext.addPushParser( cuT( "node" ), CSCNSection::eSkeletonAnimationKeyframeObject, sklanm::parserKeyframeNode, { makeParameter< ParameterType::eName >() } );
		animationKeyframeContext.addPushParser( cuT( "bone" ), CSCNSection::eSkeletonAnimationKeyframeObject, sklanm::parserKeyframeBone, { makeParameter< ParameterType::eName >() } );
		animationKeyframeContext.addPopParser( cuT( "}" ), sklanm::parserKeyframeEnd );

		animationKeyframeObjectContext.addParser( cuT( "position" ), sklanm::parserKeyframeObjectPosition, { makeParameter< ParameterType::ePoint3F >() } );
		animationKeyframeObjectContext.addParser( cuT( "rotation" ), sklanm::parserKeyframeObjectRotation, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
		animationKeyframeObjectContext.addParser( cuT( "scale" ), sklanm::parserKeyframeObjectScale, { makeParameter< ParameterType::ePoint3F >() } );
		animationKeyframeObjectContext.addPopParser( cuT( "}" ), sklanm::parserKeyframeObjectEnd );
	}

	void SkeletonAnimation::doCloneInto( Animation & output )const
	{
		auto & skelOutput = static_cast< SkeletonAnimation & >( output );
		StringMap< SkeletonAnimationObject * > inserted;
		auto & skeleton = static_cast< Skeleton const & >( *skelOutput.getAnimable() );

		for ( auto object : m_rootObjects )
		{
			String name = sklanm::getMovingTypeName( object->getType() ) + object->getName();

			if ( object->getType() == SkeletonNodeType::eBone )
			{
				auto result = makeUnique< SkeletonAnimationBone >( skelOutput );
				auto node = skeleton.findNode( static_cast< SkeletonAnimationBone const & >( *object ).getBone()->getName() );
				result->setBone( static_cast< BoneNode & >( *node ) );
				skelOutput.m_rootObjects.push_back( result.get() );
				skelOutput.m_toMove.try_emplace( name, ptrRefCast< SkeletonAnimationObject >( result ) );
			}
			else
			{
				auto result = makeUnique< SkeletonAnimationNode >( skelOutput );
				auto node = skeleton.findNode( static_cast< SkeletonAnimationNode const & >( *object ).getNode()->getName() );
				result->setNode( *node );
				skelOutput.m_rootObjects.push_back( result.get() );
				skelOutput.m_toMove.try_emplace( name, ptrRefCast< SkeletonAnimationObject >( result ) );
			}
		}

		for ( auto & [name, object] : m_toMove )
		{
			if ( skelOutput.m_toMove.find( name ) == skelOutput.m_toMove.end() )
			{
				if ( object->getType() == SkeletonNodeType::eBone )
				{
					auto result = makeUnique< SkeletonAnimationBone >( skelOutput );
					auto node = skeleton.findNode( static_cast< SkeletonAnimationBone const & >( *object ).getBone()->getName() );
					result->setBone( static_cast< BoneNode & >( *node ) );
					skelOutput.m_toMove.try_emplace( name, ptrRefCast< SkeletonAnimationObject >( result ) );
				}
				else
				{
					auto result = makeUnique< SkeletonAnimationNode >( skelOutput );
					auto node = skeleton.findNode( static_cast< SkeletonAnimationNode const & >( *object ).getNode()->getName() );
					result->setNode( *node );
					skelOutput.m_toMove.try_emplace( name, ptrRefCast< SkeletonAnimationObject >( result ) );
				}
			}
		}
	}

	//*************************************************************************************************

	String getPrefix( SkeletonAnimationContext const & context )
	{
		return getPrefix( *context.skeleton );
	}

	Engine * getEngine( SkeletonAnimationContext const & context )
	{
		return getEngine( *context.skeleton );
	}

	//*************************************************************************************************
}
