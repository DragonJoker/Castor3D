#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace castor3d
{
	namespace rsmcfg
	{
		static CU_ImplementAttributeParserBlock( parserConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eRsm, blockContext )

		static CU_ImplementAttributeParserBlock( parserGroupConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsRsm, blockContext )

		static CU_ImplementAttributeParserBlock( parserIntensity, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else
			{
				params[0]->get( *blockContext->shadowConfig->rsmConfig.intensity );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxRadius, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else
			{
				params[0]->get( *blockContext->shadowConfig->rsmConfig.maxRadius );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSampleCount, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else
			{
				*blockContext->shadowConfig->rsmConfig.sampleCount = params[0]->get< uint32_t >();
			}
		}
		CU_EndAttribute()
	}

	void RsmConfig::accept( ConfigurationVisitorBase & visitor )
	{
		auto block = visitor.visit( cuT( "Reflective Shadow Maps" ) );
		block.visit( cuT( "RSM Intensity" ), intensity );
		block.visit( cuT( "RSM Max Radius" ), maxRadius );
		block.visit( cuT( "RSM Sample Count" ), sampleCount );
	}

	void RsmConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		{
			BlockParserContextT< ShadowContext > shadowContext{ result, CSCNSection::eShadows };
			BlockParserContextT< ShadowContext > rsmContext{ result, CSCNSection::eRsm, CSCNSection::eShadows };

			shadowContext.addPushParser( cuT( "rsm_config" ), CSCNSection::eRsm, rsmcfg::parserConfig );
			rsmContext.addParser( cuT( "intensity" ), rsmcfg::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
			rsmContext.addParser( cuT( "max_radius" ), rsmcfg::parserMaxRadius, { makeParameter< ParameterType::eFloat >() } );
			rsmContext.addParser( cuT( "sample_count" ), rsmcfg::parserSampleCount, { makeParameter< ParameterType::eUInt32 >( castor::makeRange( 20u, MaxRsmRange ) ) } );
			rsmContext.addDefaultPopParser();
		}
		{
			BlockParserContextT< ShadowContext > shadowContext{ result, CSCNSection::eLightGroupShadows };
			BlockParserContextT< ShadowContext > rsmContext{ result, CSCNSection::eLightGroupShadowsRsm, CSCNSection::eLightGroupShadows };

			shadowContext.addPushParser( cuT( "rsm_config" ), CSCNSection::eLightGroupShadowsRsm, rsmcfg::parserGroupConfig );
			rsmContext.addParser( cuT( "intensity" ), rsmcfg::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
			rsmContext.addParser( cuT( "max_radius" ), rsmcfg::parserMaxRadius, { makeParameter< ParameterType::eFloat >() } );
			rsmContext.addParser( cuT( "sample_count" ), rsmcfg::parserSampleCount, { makeParameter< ParameterType::eUInt32 >( castor::makeRange( 20u, MaxRsmRange ) ) } );
			rsmContext.addDefaultPopParser();
		}
	}
}
