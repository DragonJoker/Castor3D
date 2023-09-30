#include "Castor3D/Scene/Shadow.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

CU_ImplementSmartPtr( castor3d, ShadowConfig )

namespace castor3d
{
	namespace shdcfg
	{
		static CU_ImplementAttributeParser( parserShadows )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( !parsingContext.shadowConfig )
			{
				parsingContext.shadowConfig = castor::makeUnique< ShadowConfig >();
			}
		}
		CU_EndAttributePush( CSCNSection::eShadows )

		static CU_ImplementAttributeParser( parserLightProducer )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( !params.empty() )
			{
				parsingContext.shadowConfig = castor::makeUnique< ShadowConfig >();
				params[0]->get( parsingContext.shadowConfig->enabled );
				parsingContext.light->setShadowConfig( *parsingContext.shadowConfig );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserProducer )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				params[0]->get( parsingContext.shadowConfig->enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserFilter )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.shadowConfig->filterType = ShadowType( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserGlobalIllumination )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.shadowConfig->globalIllumination = GlobalIlluminationType( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserVolumetricSteps )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				params[0]->get( parsingContext.shadowConfig->volumetricSteps );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserVolumetricScatteringFactor )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				params[0]->get( parsingContext.shadowConfig->volumetricScattering );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRawConfig )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePush( CSCNSection::eRaw )

		static CU_ImplementAttributeParser( parserPcfConfig )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePush( CSCNSection::ePcf )

		static CU_ImplementAttributeParser( parserVsmConfig )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePush( CSCNSection::eVsm )

		static CU_ImplementAttributeParser( parserEnd )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else
			{
				parsingContext.light->setShadowConfig( std::move( *parsingContext.shadowConfig ) );
				parsingContext.shadowConfig = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParser( parserRawMinOffset )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( parsingContext.shadowConfig->rawOffsets[0] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserRawMaxSlopeOffset )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( parsingContext.shadowConfig->rawOffsets[1] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPcfMinOffset )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( parsingContext.shadowConfig->pcfOffsets[0] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPcfMaxSlopeOffset )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( parsingContext.shadowConfig->pcfOffsets[1] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPcfFilterSize )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.shadowConfig->pcfFilterSize = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserPcfSampleCount )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.shadowConfig->pcfSampleCount = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserVsmMinVariance )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( parsingContext.shadowConfig->vsmMinVariance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserVsmLightBleedingReduction )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( parsingContext.shadowConfig->vsmLightBleedingReduction );
			}
		}
		CU_EndAttribute()
	}

	void ShadowConfig::accept( ConfigurationVisitorBase & visitor
		, LightType lightType )
	{
		auto baseBlock = visitor.visit( cuT( "Shadows" ) );
		baseBlock.visit( cuT( "Type" )
			, filterType
			, castor::StringArray{ "None", "RAW", "PCF", "VSM" }
			, ConfigurationVisitorBase::OnEnumValueChangeT< ShadowType >( [this]( ShadowType oldV, ShadowType newV )
			{
				filterType = newV;
			} ) );

		if ( lightType == castor3d::LightType::eDirectional )
		{
			baseBlock.visit( cuT( "Volumetric Steps" ), volumetricSteps );
			baseBlock.visit( cuT( "Volumetric Scattering Factor" ), volumetricScattering );
		}
		{
			auto block = baseBlock.visit( cuT( "Raw:" ) );
			block.visit( cuT( "Raw Min. Offset" ), rawOffsets[0] );
			block.visit( cuT( "Raw Max. Slope Offset" ), rawOffsets[1] );
		}
		{
			auto block = baseBlock.visit( cuT( "PCF: " ) );
			block.visit( cuT( "PCF Min. Offset" ), pcfOffsets[0] );
			block.visit( cuT( "PCF Max. Slope Offset" ), pcfOffsets[1] );
			block.visit( cuT( "PCF Filter Size" ), pcfFilterSize );
			block.visit( cuT( "PCF Sample Count" ), pcfSampleCount );
		}
		{
			auto block = baseBlock.visit( cuT( "VSM:" ) );
			block.visit( cuT( "Min. Variance" ), vsmMinVariance );
			block.visit( cuT( "Light Bleeding Reduction" ), vsmLightBleedingReduction );
		}
		if ( lightType == castor3d::LightType::eDirectional )
		{
			baseBlock.visit( cuT( "GI Type" )
				, globalIllumination
				, castor::StringArray{ "None", "LPV", "LPV (Geometry)", "Layered LPV", "Layered LPV (Geometry)" }
				, ConfigurationVisitorBase::OnEnumValueChangeT< GlobalIlluminationType >( [this]( GlobalIlluminationType oldV, GlobalIlluminationType newV )
				{
					globalIllumination = GlobalIlluminationType( newV );
				} ) );
		}
		else
		{
			baseBlock.visit( cuT( "GI Type" )
				, globalIllumination
				, castor::StringArray{ "None", "LPV", "LPV (Geometry)" }
				, ConfigurationVisitorBase::OnEnumValueChangeT< GlobalIlluminationType >( [this]( GlobalIlluminationType oldV, GlobalIlluminationType newV )
				{
					globalIllumination = GlobalIlluminationType( newV );
				} ) );
		}
		lpvConfig.accept( *baseBlock );
	}

	void ShadowConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		addParser( result, uint32_t( CSCNSection::eLight ), cuT( "shadows" ), shdcfg::parserShadows );
		addParser( result, uint32_t( CSCNSection::eLight ), cuT( "shadow_producer" ), shdcfg::parserLightProducer, { makeParameter< ParameterType::eBool >() } );

		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "producer" ), shdcfg::parserProducer, { makeParameter< ParameterType::eBool >() } );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "filter" ), shdcfg::parserFilter, { makeParameter< ParameterType::eCheckedText, ShadowType >() } );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "global_illumination" ), shdcfg::parserGlobalIllumination, { makeParameter< ParameterType::eCheckedText, GlobalIlluminationType >() } );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "volumetric_steps" ), shdcfg::parserVolumetricSteps, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "volumetric_scattering" ), shdcfg::parserVolumetricScatteringFactor, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "raw_config" ), shdcfg::parserRawConfig );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "pcf_config" ), shdcfg::parserPcfConfig );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "vsm_config" ), shdcfg::parserVsmConfig );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "}" ), shdcfg::parserEnd );

		addParser( result, uint32_t( CSCNSection::eRaw ), cuT( "min_offset" ), shdcfg::parserRawMinOffset, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eRaw ), cuT( "max_slope_offset" ), shdcfg::parserRawMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );

		addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "min_offset" ), shdcfg::parserPcfMinOffset, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "max_slope_offset" ), shdcfg::parserPcfMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "filter_size" ), shdcfg::parserPcfFilterSize, { makeParameter< ParameterType::eUInt32 >() } );
		addParser( result, uint32_t( CSCNSection::ePcf ), cuT( "sample_count" ), shdcfg::parserPcfSampleCount, { makeParameter< ParameterType::eUInt32 >() } );

		addParser( result, uint32_t( CSCNSection::eVsm ), cuT( "min_variance" ), shdcfg::parserVsmMinVariance, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eVsm ), cuT( "light_bleeding_reduction" ), shdcfg::parserVsmLightBleedingReduction, { makeParameter< ParameterType::eFloat >() } );
	}
}
