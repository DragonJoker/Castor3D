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

		if ( !atmosphereContext.atmosphere->getNode() )
		{
			CU_ParsingError( cuT( "No node to attach the sun to..." ) );
		}
		else
		{
			atmosphereContext.config.multiScatteringLUTRes = float( atmosphereContext.multiScatter );
			atmosphereContext.atmosphere->setAtmosphereCfg( std::move( atmosphereContext.config ) );
			atmosphereContext.atmosphere->loadTransmittance( atmosphereContext.transmittance );
			atmosphereContext.atmosphere->loadMultiScatter( atmosphereContext.multiScatter );
			atmosphereContext.atmosphere->loadAtmosphereVolume( atmosphereContext.atmosphereVolume );
			atmosphereContext.atmosphere->loadSkyView( atmosphereContext.skyView );
			atmosphereContext.atmosphere->loadWorley( atmosphereContext.worley );
			atmosphereContext.atmosphere->loadPerlinWorley( atmosphereContext.perlinWorley );
			atmosphereContext.atmosphere->loadCurl( atmosphereContext.curl );
			atmosphereContext.atmosphere->loadWeather( atmosphereContext.weather );
			parsingContext.scene->setBackground( std::move( atmosphereContext.atmosphere ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserNode )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & parsingContext = castor3d::getSceneParserContext( context );
			auto & atmosphereContext = parser::getParserContext( context );
			castor::String name;

			if ( auto node = parsingContext.scene->findSceneNode( params[0]->get( name ) ).lock() )
			{
				atmosphereContext.atmosphere->setNode( *node );
			}
			else
			{
				CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTransmittanceResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.transmittance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMultiScatterResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.multiScatter );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAtmosphereVolumeResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.atmosphereVolume );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyViewResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.skyView );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSunIlluminance )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.sunIlluminance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSunIlluminanceScale )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.sunIlluminanceScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRayMarchMinSPP )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			uint32_t value;
			params[0]->get( value );
			atmosphereContext.config.rayMarchMinMaxSPP[0] = float( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRayMarchMaxSPP )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			uint32_t value;
			params[0]->get( value );
			atmosphereContext.config.rayMarchMinMaxSPP[1] = float( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMultipleScatteringFactor )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.multipleScatteringFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSolarIrradiance )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.solarIrradiance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSunAngularRadius )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.sunAngularRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAbsorptionExtinction )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.absorptionExtinction );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMaxSunZenithAngle )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			float value;
			params[0]->get( value );
			atmosphereContext.config.muSMin = float( castor::Angle::fromDegrees( value ).cos() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRayleighScattering )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.rayleighScattering );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMieScattering )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.mieScattering );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMiePhaseFunctionG )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.miePhaseFunctionG );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMieExtinction )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.mieExtinction );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBottomRadius )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.bottomRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTopRadius )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.topRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserGroundAlbedo )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.config.groundAlbedo );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMinRayleighDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.config.rayleighDensity[0];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMaxRayleighDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.config.rayleighDensity[1];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMinMieDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.config.mieDensity[0];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMaxMieDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.config.mieDensity[1];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMinAbsorptionDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.config.absorptionDensity[0];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMaxAbsorptionDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.config.absorptionDensity[1];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserDensityLayerWidth )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.densityLayer->layerWidth );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDensityExpTerm )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.densityLayer->expTerm );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDensityExpScale )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.densityLayer->expScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDensityLinearTerm )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.densityLayer->linearTerm );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDensityConstantTerm )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.densityLayer->constantTerm );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDensityEnd )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = nullptr;
	}
	CU_EndAttributePop()
}
