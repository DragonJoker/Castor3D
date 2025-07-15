#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/RsmConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	namespace rsmcfg
	{
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

	void RsmConfig::addParsers( AttributeParsers & result
		, CSCNSection shadows, CSCNSection lightRsm
		, RawParserFunctionT< ShadowContext > parserConfig )
	{
		BlockParserContextT< ShadowContext > shadowContext{ result, shadows };
		BlockParserContextT< ShadowContext > rsmContext{ result, lightRsm, shadows };

		shadowContext.addPushParser( cuT( "rsm_config" ), lightRsm, c3d::move( parserConfig ) );
		rsmContext.addParser( cuT( "intensity" ), rsmcfg::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
		rsmContext.addParser( cuT( "max_radius" ), rsmcfg::parserMaxRadius, { makeParameter< ParameterType::eFloat >() } );
		rsmContext.addParser( cuT( "sample_count" ), rsmcfg::parserSampleCount, { makeParameter< ParameterType::eUInt32 >( makeRange( 20u, MaxRsmRange ) ) } );
		rsmContext.addDefaultPopParser();
	}
}
