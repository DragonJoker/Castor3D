#include "Castor3D/Scene/Shadow.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/LightGroup.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, ShadowConfig )

namespace castor3d
{
	namespace shdcfg
	{
		static CU_ImplementAttributeParserBlock( parserProducer, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				params[0]->get( blockContext->shadowConfig->enabled );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFilter, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value;
				params[0]->get( value );
				blockContext->shadowConfig->filterType = ShadowType( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserGlobalIllumination, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				uint32_t value;
				params[0]->get( value );
				blockContext->shadowConfig->globalIllumination = GlobalIlluminationType( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVolumetricSteps, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				params[0]->get( blockContext->shadowConfig->volumetricSteps );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVolumetricScatteringFactor, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( !params.empty() )
			{
				params[0]->get( blockContext->shadowConfig->volumetricScattering );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, ShadowContext )
		{
			if ( blockContext->light )
			{
				blockContext->light->light->setShadowConfig( castor::move( *blockContext->shadowConfig ) );
			}
			else if ( blockContext->lightGroup )
			{
				blockContext->lightGroup->light->setShadowConfig( castor::move( *blockContext->shadowConfig ) );
			}
			else
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserRawMinOffset, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->shadowConfig->rawOffsets[0] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRawMaxSlopeOffset, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->shadowConfig->rawOffsets[1] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPcfMinOffset, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->shadowConfig->pcfOffsets[0] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPcfMaxSlopeOffset, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->shadowConfig->pcfOffsets[1] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPcfFilterSize, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
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
				blockContext->shadowConfig->pcfFilterSize = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPcfSampleCount, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
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
				blockContext->shadowConfig->pcfSampleCount = value;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVsmMinVariance, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->shadowConfig->vsmMinVariance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVsmLightBleedingReduction, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->shadowConfig->vsmLightBleedingReduction );
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
			, castor::StringArray{ cuT( "None" ), cuT( "RAW" ), cuT( "PCF" ), cuT( "VSM" ) }
			, ConfigurationVisitorBase::OnEnumValueChangeT< ShadowType >( [this]( ShadowType, ShadowType newV )
			{
				filterType = newV;
			} ) );
		if ( lightType == castor3d::LightType::eDirectional )
		{
			baseBlock.visit( cuT( "GI Type" )
				, globalIllumination
				, castor::StringArray{ cuT( "None" ), cuT( "RSM" ), cuT( "LPV" ), cuT( "LPV (Geometry)" ), cuT( "Layered LPV" ), cuT( "Layered LPV (Geometry)" ) }
				, ConfigurationVisitorBase::OnEnumValueChangeT< GlobalIlluminationType >( [this]( GlobalIlluminationType, GlobalIlluminationType newV )
				{
					globalIllumination = GlobalIlluminationType( newV );
				} ) );
		}
		else
		{
			baseBlock.visit( cuT( "GI Type" )
				, globalIllumination
				, castor::StringArray{ cuT( "None" ), cuT( "RSM" ), cuT( "LPV" ), cuT( "LPV (Geometry)" ) }
				, ConfigurationVisitorBase::OnEnumValueChangeT< GlobalIlluminationType >( [this]( GlobalIlluminationType, GlobalIlluminationType newV )
				{
					globalIllumination = GlobalIlluminationType( newV );
				} ) );
		}

		if ( lightType == castor3d::LightType::eDirectional )
		{
			baseBlock.visit( cuT( "Volumetric Steps" ), volumetricSteps );
			baseBlock.visit( cuT( "Volumetric Scattering Factor" ), volumetricScattering );
		}
		{
			auto block = baseBlock.visit( cuT( "Raw" ) );
			block.visit( cuT( "Raw Min. Offset" ), rawOffsets[0] );
			block.visit( cuT( "Raw Max. Slope Offset" ), rawOffsets[1] );
		}
		{
			auto block = baseBlock.visit( cuT( "Percentage Closer Filtering" ) );
			block.visit( cuT( "PCF Min. Offset" ), pcfOffsets[0] );
			block.visit( cuT( "PCF Max. Slope Offset" ), pcfOffsets[1] );
			block.visit( cuT( "PCF Filter Size" ), pcfFilterSize );
			block.visit( cuT( "PCF Sample Count" ), pcfSampleCount );
		}
		{
			auto block = baseBlock.visit( cuT( "Variance Shadow Maps" ) );
			block.visit( cuT( "Min. Variance" ), vsmMinVariance );
			block.visit( cuT( "Light Bleeding Reduction" ), vsmLightBleedingReduction );
		}
		rsmConfig.accept( *baseBlock );
		lpvConfig.accept( *baseBlock );
	}

	void ShadowConfig::addParsers( castor::AttributeParsers & result
		, CSCNSection light, CSCNSection shadows
		, CSCNSection shadowsRaw, CSCNSection shadowsPcf, CSCNSection shadowsVsm
		, castor::RawParserFunctionT< void > parserShadows, castor::RawParserFunctionT< void > parserShadowProducer
		, castor::RawParserFunctionT< ShadowContext > parserRawConfig, castor::RawParserFunctionT< ShadowContext > parserPcfConfig, castor::RawParserFunctionT< ShadowContext > parserVsmConfig )
	{
		using namespace castor;
		BlockParserContextT< LightContext > lightContext{ result, light, CSCNSection::eScene };
		BlockParserContextT< ShadowContext > shadowsContext{ result, shadows, light };
		BlockParserContextT< ShadowContext > rawContext{ result, shadowsRaw, shadows };
		BlockParserContextT< ShadowContext > pcfContext{ result, shadowsPcf, shadows };
		BlockParserContextT< ShadowContext > vsmContext{ result, shadowsVsm, shadows };

		lightContext.addPushParser( cuT( "shadows" ), shadows, castor::move( parserShadows ) );
		lightContext.addParser( cuT( "shadow_producer" ), castor::move( parserShadowProducer ), { makeParameter< ParameterType::eBool >() } );

		shadowsContext.addParser( cuT( "producer" ), shdcfg::parserProducer, { makeParameter< ParameterType::eBool >() } );
		shadowsContext.addParser( cuT( "filter" ), shdcfg::parserFilter, { makeParameter< ParameterType::eCheckedText, ShadowType >() } );
		shadowsContext.addParser( cuT( "global_illumination" ), shdcfg::parserGlobalIllumination, { makeParameter< ParameterType::eCheckedText, GlobalIlluminationType >() } );
		shadowsContext.addParser( cuT( "volumetric_steps" ), shdcfg::parserVolumetricSteps, { makeParameter< ParameterType::eUInt32 >() } );
		shadowsContext.addParser( cuT( "volumetric_scattering" ), shdcfg::parserVolumetricScatteringFactor, { makeParameter< ParameterType::eFloat >() } );
		shadowsContext.addPushParser( cuT( "raw_config" ), shadowsRaw, parserRawConfig );
		shadowsContext.addPushParser( cuT( "pcf_config" ), shadowsPcf, parserPcfConfig );
		shadowsContext.addPushParser( cuT( "vsm_config" ), shadowsVsm, parserVsmConfig );
		shadowsContext.addPopParser( cuT( "}" ), shdcfg::parserEnd );

		rawContext.addParser( cuT( "min_offset" ), shdcfg::parserRawMinOffset, { makeParameter< ParameterType::eFloat >() } );
		rawContext.addParser( cuT( "max_slope_offset" ), shdcfg::parserRawMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );
		rawContext.addDefaultPopParser();

		pcfContext.addParser( cuT( "min_offset" ), shdcfg::parserPcfMinOffset, { makeParameter< ParameterType::eFloat >() } );
		pcfContext.addParser( cuT( "max_slope_offset" ), shdcfg::parserPcfMaxSlopeOffset, { makeParameter< ParameterType::eFloat >() } );
		pcfContext.addParser( cuT( "filter_size" ), shdcfg::parserPcfFilterSize, { makeParameter< ParameterType::eUInt32 >() } );
		pcfContext.addParser( cuT( "sample_count" ), shdcfg::parserPcfSampleCount, { makeParameter< ParameterType::eUInt32 >() } );
		pcfContext.addDefaultPopParser();

		vsmContext.addParser( cuT( "min_variance" ), shdcfg::parserVsmMinVariance, { makeParameter< ParameterType::eFloat >() } );
		vsmContext.addParser( cuT( "light_bleeding_reduction" ), shdcfg::parserVsmLightBleedingReduction, { makeParameter< ParameterType::eFloat >() } );
		vsmContext.addDefaultPopParser();
	}
}
