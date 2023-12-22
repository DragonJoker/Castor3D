#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	namespace saocfg
	{
		static CU_ImplementAttributeParserBlock( parserSsao, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eSsao, blockContext )

		static CU_ImplementAttributeParserBlock( parserEnabled, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.enabled );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserHighQuality, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.highQuality );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUseNormalsBuffer, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.useNormalsBuffer );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRadius, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.radius );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMinRadius, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.minRadius );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBias, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.bias );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserIntensity, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.intensity );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNumSamples, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.numSamples );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEdgeSharpness, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.edgeSharpness );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBlurStepSize, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					blockContext->ssaoConfig.blurStepSize = { params[0]->get< uint32_t >()
						, blockContext->ssaoConfig.blurStepSize.value().range() };
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBlurHighQuality, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.blurHighQuality );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBlurRadius, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					blockContext->ssaoConfig.blurRadius = { params[0]->get< uint32_t >()
						, blockContext->ssaoConfig.blurRadius.value().range() };
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBendStepCount, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					blockContext->ssaoConfig.bendStepCount = { params[0]->get< uint32_t >()
						, blockContext->ssaoConfig.bendStepCount.range() };
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBendStepSize, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					params[0]->get( blockContext->ssaoConfig.bendStepSize );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, TargetContext )
		{
			if ( blockContext->renderTarget )
			{
				blockContext->renderTarget->setSsaoConfig( std::move( blockContext->ssaoConfig ) );
				blockContext->ssaoConfig = {};
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttributePop()
	}

	void SsaoConfig::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "SSAO" ) );
		visitor.visit( cuT( "Enable SSAO" ), enabled );
		visitor.visit( cuT( "High Quality" ), highQuality );
		visitor.visit( cuT( "Normals Buffer" ), useNormalsBuffer );
		visitor.visit( cuT( "Radius" ), radius );
		visitor.visit( cuT( "Bias" ), bias );
		visitor.visit( cuT( "Intensity" ), intensity );
		visitor.visit( cuT( "Samples" ), numSamples );
		visitor.visit( cuT( "Edge Sharpness" ), edgeSharpness );
		visitor.visit( cuT( "Blur High Quality" ), blurHighQuality );
		visitor.visit( cuT( "Blur Step Size" ), blurStepSize );
		visitor.visit( cuT( "Blur Radius" ), blurRadius );
		visitor.visit( cuT( "Bend Step Count" ), bendStepCount );
		visitor.visit( cuT( "Bend Step Size" ), bendStepSize );
	}

	void SsaoConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		addParserT( result, CSCNSection::eRenderTarget, CSCNSection::eSsao, cuT( "ssao" ), saocfg::parserSsao );
		addParserT( result, CSCNSection::eSsao, cuT( "enabled" ), saocfg::parserEnabled, { makeParameter< ParameterType::eBool >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "high_quality" ), saocfg::parserHighQuality, { makeParameter< ParameterType::eBool >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "use_normals_buffer" ), saocfg::parserUseNormalsBuffer, { makeParameter< ParameterType::eBool >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "radius" ), saocfg::parserRadius, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "min_radius" ), saocfg::parserMinRadius, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "bias" ), saocfg::parserBias, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "intensity" ), saocfg::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "num_samples" ), saocfg::parserNumSamples, { makeParameter< ParameterType::eUInt32 >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "edge_sharpness" ), saocfg::parserEdgeSharpness, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "blur_high_quality" ), saocfg::parserBlurHighQuality, { makeParameter< ParameterType::eBool >() } );
		addParserT( result, CSCNSection::eSsao, cuT( "blur_step_size" ), saocfg::parserBlurStepSize, { makeParameter< ParameterType::eUInt32 >( makeRange( 1u, 60u ) ) } );
		addParserT( result, CSCNSection::eSsao, cuT( "blur_radius" ), saocfg::parserBlurRadius, { makeParameter< ParameterType::eUInt32 >( makeRange( 1u, 6u ) ) } );
		addParserT( result, CSCNSection::eSsao, cuT( "bend_step_count" ), saocfg::parserBendStepCount, { makeParameter< ParameterType::eUInt32 >( makeRange( 1u, 60u ) ) } );
		addParserT( result, CSCNSection::eSsao, cuT( "bend_step_size" ), saocfg::parserBendStepSize, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eSsao, CSCNSection::eRenderTarget, cuT( "}" ), saocfg::parserEnd );
	}
}
