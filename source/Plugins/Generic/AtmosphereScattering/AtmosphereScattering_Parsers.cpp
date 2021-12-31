#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
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

		template< size_t CountT >
		static void parseFilePath( castor::FileParserContext & context
			, castor::Path relative
			, castor::String const & prefix
			, castor::Point< uint32_t, CountT > const & dimensions
			, castor::PixelFormat format )
		{
			castor::Path folder;
			auto & atmosphereContext = getParserContext( context );

			if ( castor::File::fileExists( context.file.getPath() / relative ) )
			{
				folder = context.file.getPath();
			}
			else if ( !castor::File::fileExists( relative ) )
			{
				CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			if ( !relative.empty() )
			{
				atmosphereContext.parameters.add( prefix + "Folder", folder );
				atmosphereContext.parameters.add( prefix + "Relative", relative );
				atmosphereContext.parameters.add( prefix + "Dimensions", dimensions );
				atmosphereContext.parameters.add( prefix + "Format", format );
			}
		}
	}

	CU_ImplementAttributeParser( parserAtmosphereScattering )
	{
	}
	CU_EndAttributePush( AtmosphereSection::eRoot )

	CU_ImplementAttributeParser( parserAtmosphereScatteringEnd )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.parameters.add( PluginType, atmosphereContext.config );
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
		parser::parseFilePath( context
			, atmosphereContext.relImgPath
			, Transmittance
			, atmosphereContext.img2dDimensions
			, atmosphereContext.imgFormat );
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
		parser::parseFilePath( context
			, atmosphereContext.relImgPath
			, Inscatter
			, atmosphereContext.img3dDimensions
			, atmosphereContext.imgFormat );
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
		parser::parseFilePath( context
			, atmosphereContext.relImgPath
			, Irradiance
			, atmosphereContext.img2dDimensions
			, atmosphereContext.imgFormat );
	}
	CU_EndAttributePop()
}
