#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Shadow.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace c3d
{
	namespace lpvcfg
	{
		static CU_ImplementAttributeParserBlock( parserIndirectAttenuation, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else
			{
				params[0]->get( *blockContext->shadowConfig->lpvConfig.indirectAttenuation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTexelAreaModifier, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else
			{
				params[0]->get( *blockContext->shadowConfig->lpvConfig.texelAreaModifier );
			}
		}
		CU_EndAttribute()
	}

	void LpvConfig::accept( ConfigurationVisitorBase & visitor )
	{
		auto block = visitor.visit( cuT( "Light Propagation Volumes" ) );
		block.visit( cuT( "Indirect Attenuation" ), indirectAttenuation );
		block.visit( cuT( "Texel Area Modifier" ), texelAreaModifier );
	}

	void LpvConfig::addParsers( AttributeParsers & result
		, CSCNSection shadows, CSCNSection lightLpv
		, RawParserFunctionT< ShadowContext > parserConfig )
	{
		BlockParserContextT< ShadowContext > shadowContext{ result, shadows };
		BlockParserContextT< ShadowContext > lpvContext{ result, lightLpv, shadows };

		shadowContext.addPushParser( cuT( "lpv_config" ), lightLpv, c3d::move( parserConfig ) );
		lpvContext.addParser( cuT( "indirect_attenuation" ), lpvcfg::parserIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
		lpvContext.addParser( cuT( "texel_area_modifier" ), lpvcfg::parserTexelAreaModifier, { makeParameter< ParameterType::eFloat >() } );
		lpvContext.addDefaultPopParser();
	}
}
