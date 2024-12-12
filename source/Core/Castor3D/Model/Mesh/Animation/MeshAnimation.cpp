#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, MeshAnimation )

namespace castor3d
{
	namespace mshanm
	{
		static CU_ImplementAttributeParserBlock( parserMeshMorphTargetWeight, MeshContext )
		{
			if ( !blockContext->morphAnimation )
			{
				CU_ParsingError( cuT( "No Morph Animation initialised." ) );
			}
			else if ( params.size() < 3u )
			{
				CU_ParsingError( cuT( "Invalid parameters." ) );
			}
			else if ( auto mesh = blockContext->mesh )
			{
				float timeIndex{};
				params[0]->get( timeIndex );
				uint32_t targetIndex{};
				params[1]->get( targetIndex );
				float targetWeight{};
				params[2]->get( targetWeight );
				auto & animation = *blockContext->morphAnimation;

				for ( auto const & submesh : *mesh )
				{
					MeshAnimationSubmesh animSubmesh{ animation, *submesh };
					animation.addChild( castor::move( animSubmesh ) );
					auto time = castor::Milliseconds{ uint64_t( timeIndex * 1000 ) };
					auto kfit = animation.find( time );
					castor3d::MeshMorphTarget * kf{};

					if ( kfit == animation.end() )
					{
						auto keyFrame = castor::makeUnique< MeshMorphTarget >( animation, time );
						kf = keyFrame.get();
						animation.addKeyFrame( castor::ptrRefCast< AnimationKeyFrame >( keyFrame ) );
					}
					else
					{
						kf = &static_cast< MeshMorphTarget & >( **kfit );
					}

					kf->setTargetWeight( *submesh, targetIndex, targetWeight );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshMorphAnimationEnd, MeshContext )
		{
			if ( !blockContext->morphAnimation )
			{
				CU_ParsingError( cuT( "No Morph Animation initialised." ) );
			}
			else if ( auto mesh = blockContext->mesh )
			{
				log::info << "Loaded morp animation [" << blockContext->morphAnimation->getName() << "]" << std::endl;
				mesh->addAnimation( castor::ptrRefCast< Animation >( blockContext->morphAnimation ) );
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttributePop()
	}

	MeshAnimation::MeshAnimation( Mesh & mesh
		, castor::String const & name )
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
			m_submeshes.push_back( castor::move( object ) );
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

	void MeshAnimation::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< MeshContext > context{ result, CSCNSection::eMorphAnimation, CSCNSection::eMesh };

		context.addParser( cuT( "target_weight" ), mshanm::parserMeshMorphTargetWeight, ParserParameterArray{ makeParameter< ParameterType::eFloat >(), makeParameter< ParameterType::eUInt32 >(), makeParameter< ParameterType::eFloat >() } );
		context.addPopParser( cuT( "}" ), mshanm::parserMeshMorphAnimationEnd );
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
