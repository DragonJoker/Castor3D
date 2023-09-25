#include "Castor3D/Render/Ssao/SsaoConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	namespace saocfg
	{
		static CU_ImplementAttributeParser( parserEnabled )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					bool value;
					params[0]->get( value );
					parsingContext.ssaoConfig.enabled = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserHighQuality )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					bool value;
					params[0]->get( value );
					parsingContext.ssaoConfig.highQuality = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserUseNormalsBuffer )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					bool value;
					params[0]->get( value );
					parsingContext.ssaoConfig.useNormalsBuffer = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRadius )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					float value;
					params[0]->get( value );
					parsingContext.ssaoConfig.radius = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserMinRadius )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					float value;
					params[0]->get( value );
					parsingContext.ssaoConfig.minRadius = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserBias )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					float value;
					params[0]->get( value );
					parsingContext.ssaoConfig.bias = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserIntensity )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					float value;
					params[0]->get( value );
					parsingContext.ssaoConfig.intensity = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserNumSamples )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					uint32_t value;
					params[0]->get( value );
					parsingContext.ssaoConfig.numSamples = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEdgeSharpness )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					float value;
					params[0]->get( value );
					parsingContext.ssaoConfig.edgeSharpness = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserBlurStepSize )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					uint32_t value;
					params[0]->get( value );
					parsingContext.ssaoConfig.blurStepSize = { value, parsingContext.ssaoConfig.blurStepSize.value().range() };
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserBlurHighQuality )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					bool value;
					params[0]->get( value );
					parsingContext.ssaoConfig.blurHighQuality = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserBlurRadius )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					uint32_t value;
					params[0]->get( value );
					parsingContext.ssaoConfig.blurRadius = { int32_t( value ), parsingContext.ssaoConfig.blurRadius.value().range() };
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserBendStepCount )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					uint32_t value;
					params[0]->get( value );
					parsingContext.ssaoConfig.bendStepCount = int32_t( value );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserBendStepSize )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				if ( params.empty() )
				{
					CU_ParsingError( cuT( "Missing parameter." ) );
				}
				else
				{
					float value;
					params[0]->get( value );
					parsingContext.ssaoConfig.bendStepSize = value;
				}
			}
			else
			{
				CU_ParsingError( cuT( "No render target initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserEnd )
		{
			auto & parsingContext = getParserContext( context );

			if ( parsingContext.renderTarget )
			{
				parsingContext.renderTarget->setSsaoConfig( std::move( parsingContext.ssaoConfig ) );
				parsingContext.ssaoConfig = {};
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
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "enabled" ), saocfg::parserEnabled, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "high_quality" ), saocfg::parserHighQuality, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "use_normals_buffer" ), saocfg::parserUseNormalsBuffer, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "radius" ), saocfg::parserRadius, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "min_radius" ), saocfg::parserMinRadius, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bias" ), saocfg::parserBias, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "intensity" ), saocfg::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "num_samples" ), saocfg::parserNumSamples, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "edge_sharpness" ), saocfg::parserEdgeSharpness, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_high_quality" ), saocfg::parserBlurHighQuality, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_step_size" ), saocfg::parserBlurStepSize, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "blur_radius" ), saocfg::parserBlurRadius, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bend_step_count" ), saocfg::parserBendStepCount, { makeParameter< ParameterType::eUInt32 >( makeRange( 1u, 60u ) ) } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "bend_step_size" ), saocfg::parserBendStepSize, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eSsao ), cuT( "}" ), saocfg::parserEnd );
	}
}
