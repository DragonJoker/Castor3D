#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LpvConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	namespace lpvcfg
	{
		static CU_ImplementAttributeParser( parserGlobalIndirectAttenuation )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.scene )
			{
				CU_ParsingError( cuT( "No Scene initialised." ) );
			}
			else
			{
				float value{ 0u };
				params[0]->get( value );
				parsingContext.scene->setLpvIndirectAttenuation( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserConfig )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePush( CSCNSection::eLpv )

		static CU_ImplementAttributeParser( parserIndirectAttenuation )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else
			{
				params[0]->get( *parsingContext.shadowConfig->lpvConfig.indirectAttenuation );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTexelAreaModifier )
		{
			auto & parsingContext = getParserContext( context );

			if ( !parsingContext.shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
			else
			{
				params[0]->get( *parsingContext.shadowConfig->lpvConfig.texelAreaModifier );
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
		addParser( result, uint32_t( CSCNSection::eScene ), cuT( "lpv_indirect_attenuation" ), lpvcfg::parserGlobalIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eShadows ), cuT( "lpv_config" ), lpvcfg::parserConfig );
		addParser( result, uint32_t( CSCNSection::eLpv ), cuT( "indirect_attenuation" ), lpvcfg::parserIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eLpv ), cuT( "texel_area_modifier" ), lpvcfg::parserTexelAreaModifier, { makeParameter< ParameterType::eFloat >() } );
	}
}
