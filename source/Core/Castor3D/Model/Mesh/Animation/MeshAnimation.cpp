#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, MeshAnimation )

namespace c3d
{
	namespace mshanm
	{
		struct MeshAnimationContext
		{
			MeshContext * mesh{};
			MeshAnimationRPtr animation{};
			MeshMorphTargetUPtr keyframe;
			SubmeshRPtr submesh{};
			FloatArray weights;
		};

		static CU_ImplementAttributeParserNewBlock( parserRoot, MeshContext, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else
			{
				newBlockContext->mesh = blockContext;
				newBlockContext->animation = &blockContext->mesh->createAnimation( params[0]->get< String >() );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eMeshAnimation )

		static CU_ImplementAttributeParserBlock( parserKeyframe, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
		}
		CU_EndAttributePushBlock( CSCNSection::eMeshAnimationKeyframe, blockContext )

		static CU_ImplementAttributeParserBlock( parserKeyframeIndex, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else
				blockContext->keyframe = makeUnique< MeshMorphTarget >( *blockContext->animation
					, Milliseconds{ params[0]->get< uint64_t >() } );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeSubmesh, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else
			{
				if ( auto id = params[0]->get< uint32_t >();
					id >= blockContext->mesh->mesh->getSubmeshCount() )
					CU_ParsingError( cuT( "Submesh ID is out of bounds." ) );
				else
				{
					blockContext->submesh = blockContext->mesh->mesh->getSubmesh( id );
					if ( !blockContext->animation->hasChild( *blockContext->submesh ) )
						blockContext->animation->addChild( { *blockContext->animation, *blockContext->submesh } );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeWeights, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
			{
				blockContext->weights.clear();
				blockContext->weights.reserve( blockContext->submesh->getMorphTargetsCount() );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eMeshAnimationKeyframeWeights, blockContext )

		static CU_ImplementAttributeParserBlock( parserKeyframeTargetTargetWeight, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( blockContext->weights.size() >= blockContext->submesh->getMorphTargetsCount() )
				CU_ParsingWarning( cuT( "Too many morph target weights, ignoring the additional ones." ) );
			else
				params[0]->get( blockContext->weights.emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserKeyframeWeightsEnd, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( blockContext->weights.size() < blockContext->submesh->getMorphTargetsCount() )
				CU_ParsingError( cuT( "Not enough morph target weights specified." ) );
			else
				blockContext->keyframe->setTargetsWeights( *blockContext->submesh, move( blockContext->weights ) );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserKeyframeEnd, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->keyframe )
				CU_ParsingError( cuT( "No keyframe initialised." ) );
			else
				blockContext->animation->addKeyFrame( ptrRefCast< AnimationKeyFrame >( blockContext->keyframe ) );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserEnd, MeshAnimationContext )
		{
			if ( !blockContext->animation )
				CU_ParsingError( cuT( "No mesh animation initialised." ) );
			else if ( auto mesh = blockContext->mesh )
			{
				log::info << "Loaded morp animation [" << blockContext->animation->getName() << "]" << std::endl;
				*blockContext = {};
			}
			else
				CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		CU_EndAttributePop()
	}

	MeshAnimation::MeshAnimation( Mesh & mesh
		, String const & name )
		: Animation{ *mesh.getEngine()
			, AnimationType::eMesh
			, mesh
			, name }
	{
	}

	void MeshAnimation::addChild( MeshAnimationSubmesh object )
	{
		if ( !hasChild( object.getSubmesh() ) )
		{
			m_submeshes.push_back( c3d::move( object ) );
		}
	}

	bool MeshAnimation::hasChild( Submesh const & object )const noexcept
	{
		return m_submeshes.end() != std::find_if( m_submeshes.begin()
			, m_submeshes.end()
			, [&object]( MeshAnimationSubmesh const & lookup )
			{
				return &lookup.getSubmesh() == &object;
			} );
	}

	void MeshAnimation::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< MeshContext > meshContext{ result, CSCNSection::eMesh };
		BlockParserContextT< mshanm::MeshAnimationContext > animContext{ result, CSCNSection::eMeshAnimation, CSCNSection::eMesh };
		BlockParserContextT< mshanm::MeshAnimationContext > animationKeyframeContext{ result, CSCNSection::eMeshAnimationKeyframe, CSCNSection::eMeshAnimation };
		BlockParserContextT< mshanm::MeshAnimationContext > weightsContext{ result, CSCNSection::eMeshAnimationKeyframeWeights, CSCNSection::eMeshAnimationKeyframe };

		meshContext.addPushParser( cuT( "morph_animation" ), CSCNSection::eMeshAnimation, mshanm::parserRoot, { makeParameter< ParameterType::eName >() } );
		meshContext.addPushParser( cuT( "mesh_animation" ), CSCNSection::eMeshAnimation, mshanm::parserRoot, { makeParameter< ParameterType::eName >() } );

		animContext.addPushParser( cuT( "keyframe" ), CSCNSection::eMeshAnimationKeyframe, mshanm::parserKeyframe );
		animContext.addPopParser( cuT( "}" ), mshanm::parserEnd );

		animationKeyframeContext.addParser( cuT( "index" ), mshanm::parserKeyframeIndex, { makeParameter< ParameterType::eUInt64 >() } );
		animationKeyframeContext.addParser( cuT( "submesh" ), mshanm::parserKeyframeSubmesh, { makeParameter< ParameterType::eUInt32 >() } );
		animationKeyframeContext.addPushParser( cuT( "weights" ), CSCNSection::eMeshAnimationKeyframeWeights, mshanm::parserKeyframeWeights );
		animationKeyframeContext.addPopParser( cuT( "}" ), mshanm::parserKeyframeEnd );

		weightsContext.addParser( cuT( "weight" ), mshanm::parserKeyframeTargetTargetWeight, { makeParameter< ParameterType::eFloat >() } );
		weightsContext.addPopParser( cuT( "}" ), mshanm::parserKeyframeWeightsEnd );
	}

	void MeshAnimation::doCloneInto( Animation & output )const
	{
		auto & meshOutput = static_cast< MeshAnimation & >( output );
		auto & mesh = static_cast< Mesh const & >( *meshOutput.getAnimable() );

		for ( auto & animSubmesh : m_submeshes )
		{
			meshOutput.addChild( MeshAnimationSubmesh{ meshOutput
				, *mesh.getSubmesh( animSubmesh.getSubmesh().getId() ) } );
		}
	}
}
