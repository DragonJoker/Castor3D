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
		atmosphereContext.background = std::make_unique< AtmosphereBackground >( *parsingContext.parser->getEngine()
			, *parsingContext.scene );
	}
	CU_EndAttributePush( AtmosphereSection::eRoot )

	CU_ImplementAttributeParser( parserAtmosphereScatteringEnd )
	{
		auto & parsingContext = castor3d::getSceneParserContext( context );
		auto & atmosphereContext = parser::getParserContext( context );

		if ( !atmosphereContext.background->getNode() )
		{
			CU_ParsingError( cuT( "No node to attach the sun to..." ) );
		}
		else
		{
			atmosphereContext.atmosphere.multiScatteringLUTRes = float( atmosphereContext.multiScatterDim );
			atmosphereContext.background->setAtmosphereCfg( std::move( atmosphereContext.atmosphere ) );
			atmosphereContext.background->setCloudsCfg( std::move( atmosphereContext.clouds ) );
			atmosphereContext.background->setWeatherCfg( std::move( atmosphereContext.weather ) );
			atmosphereContext.background->loadTransmittance( atmosphereContext.transmittanceDim );
			atmosphereContext.background->loadMultiScatter( atmosphereContext.multiScatterDim );
			atmosphereContext.background->loadAtmosphereVolume( atmosphereContext.atmosphereVolumeDim );
			atmosphereContext.background->loadSkyView( atmosphereContext.skyViewDim );
			atmosphereContext.background->loadWorley( atmosphereContext.worleyDim );
			atmosphereContext.background->loadPerlinWorley( atmosphereContext.perlinWorleyDim );
			atmosphereContext.background->loadCurl( atmosphereContext.curlDim );
			atmosphereContext.background->loadWeather( atmosphereContext.weatherDim );
			parsingContext.scene->setBackground( std::move( atmosphereContext.background ) );
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
				atmosphereContext.background->setNode( *node );
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
			params[0]->get( atmosphereContext.transmittanceDim );
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
			params[0]->get( atmosphereContext.multiScatterDim );
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
			params[0]->get( atmosphereContext.atmosphereVolumeDim );
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
			params[0]->get( atmosphereContext.skyViewDim );
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
			params[0]->get( atmosphereContext.atmosphere.sunIlluminance );
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
			params[0]->get( atmosphereContext.atmosphere.sunIlluminanceScale );
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
			atmosphereContext.atmosphere.rayMarchMinMaxSPP[0] = float( value );
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
			atmosphereContext.atmosphere.rayMarchMinMaxSPP[1] = float( value );
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
			params[0]->get( atmosphereContext.atmosphere.multipleScatteringFactor );
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
			params[0]->get( atmosphereContext.atmosphere.solarIrradiance );
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
			params[0]->get( atmosphereContext.atmosphere.sunAngularRadius );
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
			params[0]->get( atmosphereContext.atmosphere.absorptionExtinction );
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
			atmosphereContext.atmosphere.muSMin = float( castor::Angle::fromDegrees( value ).cos() );
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
			params[0]->get( atmosphereContext.atmosphere.rayleighScattering );
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
			params[0]->get( atmosphereContext.atmosphere.mieScattering );
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
			params[0]->get( atmosphereContext.atmosphere.miePhaseFunctionG );
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
			params[0]->get( atmosphereContext.atmosphere.mieExtinction );
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
			params[0]->get( atmosphereContext.atmosphere.bottomRadius );
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
			params[0]->get( atmosphereContext.atmosphere.topRadius );
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
			params[0]->get( atmosphereContext.atmosphere.groundAlbedo );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMinRayleighDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.atmosphere.rayleighDensity[0];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMaxRayleighDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.atmosphere.rayleighDensity[1];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMinMieDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.atmosphere.mieDensity[0];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMaxMieDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.atmosphere.mieDensity[1];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMinAbsorptionDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.atmosphere.absorptionDensity[0];
	}
	CU_EndAttributePush( AtmosphereSection::eDensity )

	CU_ImplementAttributeParser( parserMaxAbsorptionDensity )
	{
		auto & atmosphereContext = parser::getParserContext( context );
		atmosphereContext.densityLayer = &atmosphereContext.atmosphere.absorptionDensity[1];
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

	CU_ImplementAttributeParser( parserWeather )
	{
	}
	CU_EndAttributePush( AtmosphereSection::eWeather )

	CU_ImplementAttributeParser( parserWeatherAmplitude )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.weather.perlinAmplitude );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWeatherFrequency )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.weather.perlinFrequency );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWeatherScale )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.weather.perlinScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWeatherOctaves )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.weather.perlinOctaves );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWorleyResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.worleyDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPerlinWorleyResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.perlinWorleyDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCurlResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.curlDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWeatherResolution )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.weatherDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserClouds )
	{
	}
	CU_EndAttributePush( AtmosphereSection::eClouds )

	CU_ImplementAttributeParser( parserCloudsWindDirection )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.windDirection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsSpeed )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.cloudSpeed );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsCoverage )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.coverage );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsCrispiness )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.crispiness );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsCurliness )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.curliness );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsDensity )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.density );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsAbsorption )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.absorption );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsInnerRadius )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.sphereInnerRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsOuterRadius )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.sphereOuterRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsTopColour )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.cloudColorTop );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsBottomColour )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.cloudColorBottom );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsEnablePowder )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			bool value{};
			params[0]->get( value );
			atmosphereContext.clouds.enablePowder = value ? 1 : 0;;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCloudsTopOffset )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & atmosphereContext = parser::getParserContext( context );
			params[0]->get( atmosphereContext.clouds.cloudTopOffset );
		}
	}
	CU_EndAttribute()
}
