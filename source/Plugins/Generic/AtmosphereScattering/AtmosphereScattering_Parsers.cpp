#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace atmosphere_scattering
{
	namespace parser
	{
		static ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( PluginType ) );
		}

		static std::pair< castor::Path, castor::Path > parseFilePath( castor::FileParserContext & context
			, castor::Path relative )
		{
			castor::Path folder;

			if ( castor::File::fileExists( context.file.getPath() / relative ) )
			{
				folder = context.file.getPath();
			}
			else if ( !castor::File::fileExists( relative ) )
			{
				CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			return { folder, relative };
		}
	}

	CU_ImplementAttributeParser( parserAtmosphereScattering )
	{
		auto & parsingContext = castor3d::getSceneParserContext( context );
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.atmosphere = std::make_unique< AtmosphereBackground >( *parsingContext.parser->getEngine()
			, *parsingContext.scene );
	}
	CU_EndAttributePush( AtmosphereSection::eRoot )

	CU_ImplementAttributeParser( parserAtmosphereScatteringEnd )
	{
		auto & parsingContext = castor3d::getSceneParserContext( context );
		auto & atmosphereContext = parser::getParserContext( context );
		parsingContext.scene->setBackground( std::move( atmosphereContext.atmosphere ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserTransmittance )
	{
	}
	CU_EndAttributePush( AtmosphereSection::eTransmittance )

	CU_ImplementAttributeParser( parserTransmittanceImage )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.relImgPath );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTransmittanceDimensions )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.img2dDimensions );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTransmittanceFormat )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.imgFormat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTransmittanceEnd )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		auto [folder, relative] = parser::parseFilePath( context
			, atmosphereContext.relImgPath );

		if ( !relative.empty() )
		{
			atmosphereContext.atmosphere->loadTransmittance( folder
				, relative
				, atmosphereContext.img2dDimensions
				, atmosphereContext.imgFormat );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserInscatter )
	{
	}
	CU_EndAttributePush( AtmosphereSection::eInscatter )

	CU_ImplementAttributeParser( parserInscatterImage )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.relImgPath );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserInscatterDimensions )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.img3dDimensions );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserInscatterFormat )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.imgFormat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserInscatterEnd )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		auto [folder, relative] = parser::parseFilePath( context
			, atmosphereContext.relImgPath );

		if ( !relative.empty() )
		{
			atmosphereContext.atmosphere->loadInscaterring( folder
				, relative
				, atmosphereContext.img3dDimensions
				, atmosphereContext.imgFormat );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserIrradiance )
	{
	}
	CU_EndAttributePush( AtmosphereSection::eIrradiance )

	CU_ImplementAttributeParser( parserIrradianceImage )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.relImgPath );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserIrradianceDimensions )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.img2dDimensions );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserIrradianceFormat )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.imgFormat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserIrradianceEnd )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		auto [folder, relative] = parser::parseFilePath( context
			, atmosphereContext.relImgPath );

		if ( !relative.empty() )
		{
			atmosphereContext.atmosphere->loadIrradiance( folder
				, relative
				, atmosphereContext.img2dDimensions
				, atmosphereContext.imgFormat );
		}
	}
	CU_EndAttributePop()
}
