#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, SceneNodeAnimation )

namespace c3d
{
	namespace nodanm
	{
		struct NodeAnimationContext
		{
			NodeContext * node;
			SceneNodeAnimationRPtr animation;
			SceneNodeAnimationKeyFrameUPtr keyframe;
		};

		static CU_ImplementAttributeParserNewBlock( parserRoot, NodeContext, NodeAnimationContext )
		{
			if ( !blockContext->scene )
				CU_ParsingError( cuT( "No scene initialised." ) );
			else
			{
				newBlockContext->node = blockContext;
				newBlockContext->animation = &blockContext->currentNode->createAnimation( params[0]->get< String >() );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eNodeAnimation )

		static CU_ImplementAttributeParserBlock( parserKeyframe, NodeAnimationContext )
		{
			if ( !blockContext->node )
				CU_ParsingError( cuT( "No node initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
		}
		CU_EndAttributePushBlock( CSCNSection::eNodeAnimationKeyframe, blockContext )

		static CU_ImplementAttributeParserBlock( parserKeyframeIndex, NodeAnimationContext )
		{
			if ( !blockContext->node )
				CU_ParsingError( cuT( "No node initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else
				blockContext->keyframe = makeUnique< SceneNodeAnimationKeyFrame >( *blockContext->animation
					, Milliseconds{ params[0]->get< uint64_t >() } );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframePosition, NodeAnimationContext )
		{
			if ( !blockContext->node )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				blockContext->keyframe->setPosition( params[0]->get< Point3f >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeRotation, NodeAnimationContext )
		{
			if ( !blockContext->node )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				blockContext->keyframe->setRotation( Quaternion::fromAxisAngle( params[0]->get< Point3f >()
					, Angle::fromDegrees( params[1]->get< float >() ) ) );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeScale, NodeAnimationContext )
		{
			if ( !blockContext->node )
				CU_ParsingError( cuT( "No skeleton initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				blockContext->keyframe->setScale( params[0]->get< Point3f >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeEnd, NodeAnimationContext )
		{
			if ( !blockContext->node )
				CU_ParsingError( cuT( "No node initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				blockContext->animation->addKeyFrame( ptrRefCast< AnimationKeyFrame >( blockContext->keyframe ) );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserEnd, NodeAnimationContext )
		{
			if ( !blockContext->node )
				CU_ParsingError( cuT( "No node initialised." ) );
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

	SceneNodeAnimation::SceneNodeAnimation( SceneNode & node
		, String const & name )
		: Animation{ *node.getEngine()
			, AnimationType::eSceneNode
			, node
			, name }
	{
	}

	void SceneNodeAnimation::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< NodeContext > nodeContext{ result, CSCNSection::eNode };
		BlockParserContextT< nodanm::NodeAnimationContext > animContext{ result, CSCNSection::eNodeAnimation, CSCNSection::eNode };
		BlockParserContextT< nodanm::NodeAnimationContext > animationKeyframeContext{ result, CSCNSection::eNodeAnimationKeyframe, CSCNSection::eNodeAnimation };

		nodeContext.addPushParser( cuT( "node_animation" ), CSCNSection::eNodeAnimation, nodanm::parserRoot, { makeParameter< ParameterType::eName >() } );

		animContext.addPushParser( cuT( "keyframe" ), CSCNSection::eNodeAnimationKeyframe, nodanm::parserKeyframe );
		animContext.addPopParser( cuT( "}" ), nodanm::parserEnd );

		animationKeyframeContext.addParser( cuT( "index" ), nodanm::parserKeyframeIndex, { makeParameter< ParameterType::eUInt64 >() } );
		animationKeyframeContext.addParser( cuT( "position" ), nodanm::parserKeyframePosition, { makeParameter< ParameterType::ePoint3F >() } );
		animationKeyframeContext.addParser( cuT( "rotation" ), nodanm::parserKeyframeRotation, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
		animationKeyframeContext.addParser( cuT( "scale" ), nodanm::parserKeyframeScale, { makeParameter< ParameterType::ePoint3F >() } );
		animationKeyframeContext.addPopParser( cuT( "}" ), nodanm::parserKeyframeEnd );
	}
}
