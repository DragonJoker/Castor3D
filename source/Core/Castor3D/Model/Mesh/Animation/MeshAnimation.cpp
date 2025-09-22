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

		static CU_ImplementAttributeParserNewBlock( parserMorphAnimation, MeshContext, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			else
			{
				newBlockContext->mesh = blockContext;
				newBlockContext->animation = &blockContext->mesh->createAnimation( params[0]->get< String >() );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eMorphAnimation )

		static CU_ImplementAttributeParserBlock( parserMorphTargetWeight, MeshAnimationContext )
		{
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else if ( params.size() < 3u )
				CU_ParsingError( cuT( "Invalid parameters." ) );
			else
			{
				float timeIndex{};
				params[0]->get( timeIndex );
				uint32_t targetIndex{};
				params[1]->get( targetIndex );
				float targetWeight{};
				params[2]->get( targetWeight );
				auto & animation = *blockContext->animation;

				for ( auto const & submesh : *blockContext->mesh->mesh )
				{
					MeshAnimationSubmesh animSubmesh{ animation, *submesh };
					animation.addChild( c3d::move( animSubmesh ) );
					auto time = Milliseconds{ uint64_t( timeIndex * 1000 ) };
					auto kfit = animation.find( time );
					MeshMorphTarget * kf{};

					if ( kfit == animation.end() )
					{
						auto keyFrame = makeUnique< MeshMorphTarget >( animation, time );
						kf = keyFrame.get();
						animation.addKeyFrame( ptrRefCast< AnimationKeyFrame >( keyFrame ) );
					}
					else
					{
						kf = &static_cast< MeshMorphTarget & >( **kfit );
					}

					kf->setTargetWeight( *submesh, targetIndex, targetWeight );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserMeshAnimation, MeshContext, MeshAnimationContext )
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
				blockContext->keyframe->setTargetsWeights( *blockContext->submesh, c3d::move( blockContext->weights ) );
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
			if ( !blockContext->mesh )
				CU_ParsingError( cuT( "No mesh initialised." ) );
			else if ( !blockContext->animation )
				CU_ParsingError( cuT( "No animation initialised." ) );
			else
			{
				log::info << "Loaded morp animation [" << blockContext->animation->getName() << "]" << std::endl;
				*blockContext = {};
			}
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
		BlockParserContextT< mshanm::MeshAnimationContext > morphContext{ result, CSCNSection::eMorphAnimation, CSCNSection::eMesh };
		BlockParserContextT< mshanm::MeshAnimationContext > animContext{ result, CSCNSection::eMeshAnimation, CSCNSection::eMesh };
		BlockParserContextT< mshanm::MeshAnimationContext > animationKeyframeContext{ result, CSCNSection::eMeshAnimationKeyframe, CSCNSection::eMeshAnimation };
		BlockParserContextT< mshanm::MeshAnimationContext > weightsContext{ result, CSCNSection::eMeshAnimationKeyframeWeights, CSCNSection::eMeshAnimationKeyframe };

		meshContext.addPushParser( cuT( "morph_animation" ), CSCNSection::eMorphAnimation, mshanm::parserMorphAnimation, { makeParameter< ParameterType::eName >() } );
		meshContext.addPushParser( cuT( "mesh_animation" ), CSCNSection::eMeshAnimation, mshanm::parserMeshAnimation, { makeParameter< ParameterType::eName >() } );

		morphContext.addParser( cuT( "target_weight" ), mshanm::parserMorphTargetWeight, ParserParameterArray{ makeParameter< ParameterType::eFloat >(), makeParameter< ParameterType::eUInt32 >(), makeParameter< ParameterType::eFloat >() } );
		morphContext.addPopParser( cuT( "}" ), mshanm::parserEnd );

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
