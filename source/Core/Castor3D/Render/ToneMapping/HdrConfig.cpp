#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParser.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace castor3d
{
	namespace hdrcfg
	{
		static CU_ImplementAttributeParser( parserExponent )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.hdrConfig.exposure );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserGamma )
		{
			auto & parsingContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( parsingContext.hdrConfig.gamma );
			}
		}
		CU_EndAttribute()
	}

	void HdrConfig::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( cuT( "High Dynamic Range" ) );
		visitor.visit( cuT( "Exposure" ), exposure );
		visitor.visit( cuT( "Gamma Correction" ), gamma );
	}

	void HdrConfig::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		addParser( result, uint32_t( CSCNSection::eHdrConfig ), cuT( "exposure" ), hdrcfg::parserExponent, { makeParameter< ParameterType::eFloat >() } );
		addParser( result, uint32_t( CSCNSection::eHdrConfig ), cuT( "gamma" ), hdrcfg::parserGamma, { makeParameter< ParameterType::eFloat >() } );
	}
}
