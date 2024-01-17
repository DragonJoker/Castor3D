#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/Light/Light.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

namespace castor3d
{
	namespace hdrcfg
	{
		static CU_ImplementAttributeParserBlock( parserHdrConfig, CameraContext )
		{
		}
		CU_EndAttributePushBlock( CSCNSection::eHdrConfig, blockContext )

		static CU_ImplementAttributeParserBlock( parserExponent, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->hdrConfig.exposure );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserGamma, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->hdrConfig.gamma );
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
		addParserT( result, CSCNSection::eCamera, CSCNSection::eHdrConfig, cuT( "hdr_config" ), hdrcfg::parserHdrConfig );
		addParserT( result, CSCNSection::eHdrConfig, cuT( "exposure" ), hdrcfg::parserExponent, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eHdrConfig, cuT( "gamma" ), hdrcfg::parserGamma, { makeParameter< ParameterType::eFloat >() } );
		addParserT( result, CSCNSection::eHdrConfig, CSCNSection::eCamera, cuT( "}" ), parserDefaultEnd );
	}
}
