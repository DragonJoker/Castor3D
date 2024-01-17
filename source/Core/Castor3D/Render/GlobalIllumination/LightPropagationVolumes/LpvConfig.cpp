#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace castor3d
{
	namespace lpvcfg
	{
		static CU_ImplementAttributeParserBlock( parserGlobalIndirectAttenuation, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No Scene initialised." ) );
			}
			else
			{
				float value{ 0u };
				params[0]->get( value );
				blockContext->scene->setLpvIndirectAttenuation( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserConfig, LightContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLpv, blockContext )

		static CU_ImplementAttributeParserBlock( parserIndirectAttenuation, LightContext )
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

		static CU_ImplementAttributeParserBlock( parserTexelAreaModifier, LightContext )
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

	void LpvConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< SceneContext > sceneContext{ result, CSCNSection::eScene };
		BlockParserContextT< LightContext > shadowContext{ result, CSCNSection::eShadows, CSCNSection::eLight };
		BlockParserContextT< LightContext > lpvContext{ result, CSCNSection::eLpv, CSCNSection::eShadows };

		sceneContext.addParser( cuT( "lpv_indirect_attenuation" ), lpvcfg::parserGlobalIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
		shadowContext.addPushParser( cuT( "lpv_config" ), CSCNSection::eLpv, lpvcfg::parserConfig );
		lpvContext.addParser( cuT( "indirect_attenuation" ), lpvcfg::parserIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
		lpvContext.addParser( cuT( "texel_area_modifier" ), lpvcfg::parserTexelAreaModifier, { makeParameter< ParameterType::eFloat >() } );
		lpvContext.addDefaultPopParser();
	}
}
