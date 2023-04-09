/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include "AtmosphereScatteringConfig.hpp"
#include "CloudsConfig.hpp"
#include "WeatherConfig.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace atmosphere_scattering
{
	static castor::String const PluginType = "atmospheric_scattering";
	static castor::String const PluginName = "Atmospheric Scattering";

	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		castor::Point2ui transmittanceDim{ 256u, 64u };
		uint32_t multiScatterDim{ 32u };
		uint32_t atmosphereVolumeDim{ 32u };
		castor::Point2ui skyViewDim{ 192u, 108u };
		uint32_t worleyDim{ 32u };
		uint32_t perlinWorleyDim{ 128u };
		uint32_t curlDim{ 128u };
		uint32_t weatherDim{ 1024u };
		AtmosphereScatteringConfig atmosphere{};
		WeatherConfig weather{};
		CloudsConfig clouds{};
		DensityProfileLayer * densityLayer{};
		AtmosphereBackgroundUPtr background;
	};

	enum class AtmosphereSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'A', 'T', 'S', 'C' ),
		eDensity = CU_MakeSectionName( 'A', 'T', 'D', 'S' ),
		eWeather = CU_MakeSectionName( 'A', 'T', 'W', 'T' ),
		eClouds = CU_MakeSectionName( 'A', 'T', 'C', 'L' ),
	};

	CU_DeclareAttributeParser( parserAtmosphereScattering )
	CU_DeclareAttributeParser( parserAtmosphereScatteringEnd )
	CU_DeclareAttributeParser( parserSunNode )
	CU_DeclareAttributeParser( parserPlanetNode )
	CU_DeclareAttributeParser( parserTransmittanceResolution )
	CU_DeclareAttributeParser( parserMultiScatterResolution )
	CU_DeclareAttributeParser( parserAtmosphereVolumeResolution )
	CU_DeclareAttributeParser( parserSkyViewResolution )
	CU_DeclareAttributeParser( parserSunIlluminance )
	CU_DeclareAttributeParser( parserSunIlluminanceScale )
	CU_DeclareAttributeParser( parserRayMarchMinSPP )
	CU_DeclareAttributeParser( parserRayMarchMaxSPP )
	CU_DeclareAttributeParser( parserMultipleScatteringFactor )
	CU_DeclareAttributeParser( parserSolarIrradiance )
	CU_DeclareAttributeParser( parserSunAngularRadius )
	CU_DeclareAttributeParser( parserAbsorptionExtinction )
	CU_DeclareAttributeParser( parserMaxSunZenithAngle )
	CU_DeclareAttributeParser( parserRayleighScattering )
	CU_DeclareAttributeParser( parserMieScattering )
	CU_DeclareAttributeParser( parserMiePhaseFunctionG )
	CU_DeclareAttributeParser( parserMieExtinction )
	CU_DeclareAttributeParser( parserBottomRadius )
	CU_DeclareAttributeParser( parserTopRadius )
	CU_DeclareAttributeParser( parserGroundAlbedo )
	CU_DeclareAttributeParser( parserMinRayleighDensity )
	CU_DeclareAttributeParser( parserMaxRayleighDensity )
	CU_DeclareAttributeParser( parserMinMieDensity )
	CU_DeclareAttributeParser( parserMaxMieDensity )
	CU_DeclareAttributeParser( parserMinAbsorptionDensity )
	CU_DeclareAttributeParser( parserMaxAbsorptionDensity )
	CU_DeclareAttributeParser( parserDensityLayerWidth )
	CU_DeclareAttributeParser( parserDensityExpTerm )
	CU_DeclareAttributeParser( parserDensityExpScale )
	CU_DeclareAttributeParser( parserDensityLinearTerm )
	CU_DeclareAttributeParser( parserDensityConstantTerm )
	CU_DeclareAttributeParser( parserDensityEnd )
	CU_DeclareAttributeParser( parserWeather )
	CU_DeclareAttributeParser( parserWeatherAmplitude )
	CU_DeclareAttributeParser( parserWeatherFrequency )
	CU_DeclareAttributeParser( parserWeatherScale )
	CU_DeclareAttributeParser( parserWeatherOctaves )
	CU_DeclareAttributeParser( parserWorleyResolution )
	CU_DeclareAttributeParser( parserPerlinWorleyResolution )
	CU_DeclareAttributeParser( parserCurlResolution )
	CU_DeclareAttributeParser( parserWeatherResolution )
	CU_DeclareAttributeParser( parserClouds )
	CU_DeclareAttributeParser( parserCloudsWindDirection )
	CU_DeclareAttributeParser( parserCloudsSpeed )
	CU_DeclareAttributeParser( parserCloudsCoverage )
	CU_DeclareAttributeParser( parserCloudsCrispiness )
	CU_DeclareAttributeParser( parserCloudsCurliness )
	CU_DeclareAttributeParser( parserCloudsDensity )
	CU_DeclareAttributeParser( parserCloudsAbsorption )
	CU_DeclareAttributeParser( parserCloudsInnerRadius )
	CU_DeclareAttributeParser( parserCloudsOuterRadius )
	CU_DeclareAttributeParser( parserCloudsTopColour )
	CU_DeclareAttributeParser( parserCloudsBottomColour )
	CU_DeclareAttributeParser( parserCloudsEnablePowder )
	CU_DeclareAttributeParser( parserCloudsTopOffset )
}

#endif
