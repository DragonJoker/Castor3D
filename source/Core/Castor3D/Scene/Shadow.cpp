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
		static CU_ImplementAttributeParserNewBlock( parserLightShadows, LightContext, ShadowContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else
			{
				if ( !blockContext->shadowConfig )
				{
					blockContext->shadowConfig = castor::makeUnique< ShadowConfig >();
				}

				newBlockContext->light = blockContext;
				newBlockContext->shadowConfig = blockContext->shadowConfig.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eShadows )

		static CU_ImplementAttributeParserBlock( parserLightShadowProducer, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( !params.empty() )
			{
				blockContext->shadowConfig = castor::makeUnique< ShadowConfig >();
				params[0]->get( blockContext->shadowConfig->enabled );
				blockContext->light->setShadowConfig( *blockContext->shadowConfig );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserLightGroupShadows, LightGroupContext, ShadowContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else
			{
				if ( !blockContext->shadowConfig )
				{
					blockContext->shadowConfig = castor::makeUnique< ShadowConfig >();
				}

				newBlockContext->lightGroup = blockContext;
				newBlockContext->shadowConfig = blockContext->shadowConfig.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eLightGroupShadows )

		static CU_ImplementAttributeParserBlock( parserLightGroupShadowProducer, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( !params.empty() )
			{
				blockContext->shadowConfig = castor::makeUnique< ShadowConfig >();
				params[0]->get( blockContext->shadowConfig->enabled );
				blockContext->light->setShadowConfig( *blockContext->shadowConfig );
			}
		}
		CU_EndAttribute()

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

		static CU_ImplementAttributeParserBlock( parserRawConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eRaw, blockContext )

		static CU_ImplementAttributeParserBlock( parserPcfConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::ePcf, blockContext )

		static CU_ImplementAttributeParserBlock( parserVsmConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eVsm, blockContext )

		static CU_ImplementAttributeParserBlock( parserGroupRawConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsRaw, blockContext )

		static CU_ImplementAttributeParserBlock( parserGroupPcfConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsPcf, blockContext )

		static CU_ImplementAttributeParserBlock( parserGroupVsmConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsVsm, blockContext )

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

	void ShadowConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		{
			BlockParserContextT< LightContext > lightContext{ result, CSCNSection::eLight, CSCNSection::eScene };
			BlockParserContextT< ShadowContext > shadowsContext{ result, CSCNSection::eShadows, CSCNSection::eLight };
			BlockParserContextT< ShadowContext > rawContext{ result, CSCNSection::eRaw, CSCNSection::eShadows };
			BlockParserContextT< ShadowContext > pcfContext{ result, CSCNSection::ePcf, CSCNSection::eShadows };
			BlockParserContextT< ShadowContext > vsmContext{ result, CSCNSection::eVsm, CSCNSection::eShadows };

			lightContext.addPushParser( cuT( "shadows" ), CSCNSection::eShadows, shdcfg::parserLightShadows );
			lightContext.addParser( cuT( "shadow_producer" ), shdcfg::parserLightShadowProducer, { makeParameter< ParameterType::eBool >() } );

			shadowsContext.addParser( cuT( "producer" ), shdcfg::parserProducer, { makeParameter< ParameterType::eBool >() } );
			shadowsContext.addParser( cuT( "filter" ), shdcfg::parserFilter, { makeParameter< ParameterType::eCheckedText, ShadowType >() } );
			shadowsContext.addParser( cuT( "global_illumination" ), shdcfg::parserGlobalIllumination, { makeParameter< ParameterType::eCheckedText, GlobalIlluminationType >() } );
			shadowsContext.addParser( cuT( "volumetric_steps" ), shdcfg::parserVolumetricSteps, { makeParameter< ParameterType::eUInt32 >() } );
			shadowsContext.addParser( cuT( "volumetric_scattering" ), shdcfg::parserVolumetricScatteringFactor, { makeParameter< ParameterType::eFloat >() } );
			shadowsContext.addPushParser( cuT( "raw_config" ), CSCNSection::eRaw, shdcfg::parserRawConfig );
			shadowsContext.addPushParser( cuT( "pcf_config" ), CSCNSection::ePcf, shdcfg::parserPcfConfig );
			shadowsContext.addPushParser( cuT( "vsm_config" ), CSCNSection::eVsm, shdcfg::parserVsmConfig );
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
		{
			BlockParserContextT< LightGroupContext > lightGroupContext{ result, CSCNSection::eLightGroup, CSCNSection::eScene };
			BlockParserContextT< ShadowContext > shadowsContext{ result, CSCNSection::eLightGroupShadows, CSCNSection::eLightGroup };
			BlockParserContextT< ShadowContext > rawContext{ result, CSCNSection::eLightGroupShadowsRaw, CSCNSection::eLightGroupShadows };
			BlockParserContextT< ShadowContext > pcfContext{ result, CSCNSection::eLightGroupShadowsPcf, CSCNSection::eLightGroupShadows };
			BlockParserContextT< ShadowContext > vsmContext{ result, CSCNSection::eLightGroupShadowsVsm, CSCNSection::eLightGroupShadows };

			lightGroupContext.addPushParser( cuT( "shadows" ), CSCNSection::eLightGroupShadows, shdcfg::parserLightGroupShadows );
			lightGroupContext.addParser( cuT( "shadow_producer" ), shdcfg::parserLightGroupShadowProducer, { makeParameter< ParameterType::eBool >() } );

			shadowsContext.addParser( cuT( "producer" ), shdcfg::parserProducer, { makeParameter< ParameterType::eBool >() } );
			shadowsContext.addParser( cuT( "filter" ), shdcfg::parserFilter, { makeParameter< ParameterType::eCheckedText, ShadowType >() } );
			shadowsContext.addParser( cuT( "global_illumination" ), shdcfg::parserGlobalIllumination, { makeParameter< ParameterType::eCheckedText, GlobalIlluminationType >() } );
			shadowsContext.addParser( cuT( "volumetric_steps" ), shdcfg::parserVolumetricSteps, { makeParameter< ParameterType::eUInt32 >() } );
			shadowsContext.addParser( cuT( "volumetric_scattering" ), shdcfg::parserVolumetricScatteringFactor, { makeParameter< ParameterType::eFloat >() } );
			shadowsContext.addPushParser( cuT( "raw_config" ), CSCNSection::eLightGroupShadowsRaw, shdcfg::parserGroupRawConfig );
			shadowsContext.addPushParser( cuT( "pcf_config" ), CSCNSection::eLightGroupShadowsPcf, shdcfg::parserGroupPcfConfig );
			shadowsContext.addPushParser( cuT( "vsm_config" ), CSCNSection::eLightGroupShadowsVsm, shdcfg::parserGroupVsmConfig );
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

		LpvConfig::addParsers( result );
		RsmConfig::addParsers( result );
	}
}
