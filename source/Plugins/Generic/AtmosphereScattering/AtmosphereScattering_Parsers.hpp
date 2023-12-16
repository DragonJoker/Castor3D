/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include "AtmosphereScatteringConfig.hpp"
#include "CloudsConfig.hpp"
#include "WeatherConfig.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace atmosphere_scattering
{
	static castor::String const PluginType = "atmospheric_scattering";
	static castor::String const PluginName = "Atmospheric Scattering";

	struct AtmosphereContext
	{
		castor3d::SceneRPtr scene{};
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
		AtmosphereBackgroundUPtr background{};
	};

	enum class AtmosphereSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'A', 'T', 'S', 'C' ),
		eDensity = CU_MakeSectionName( 'A', 'T', 'D', 'S' ),
		eWeather = CU_MakeSectionName( 'A', 'T', 'W', 'T' ),
		eClouds = CU_MakeSectionName( 'A', 'T', 'C', 'L' ),
	};

	CU_DeclareAttributeParserBlock( parserAtmosphereScattering, castor3d::SceneContext )
	CU_DeclareAttributeParserBlock( parserAtmosphereScatteringEnd, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserSunNode, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserPlanetNode, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserTransmittanceResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMultiScatterResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserAtmosphereVolumeResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserSkyViewResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserSunIlluminance, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserSunIlluminanceScale, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserRayMarchMinSPP, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserRayMarchMaxSPP, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMultipleScatteringFactor, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserSolarIrradiance, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserSunAngularRadius, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserAbsorptionExtinction, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMaxSunZenithAngle, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserRayleighScattering, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMieScattering, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMiePhaseFunctionG, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMieExtinction, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserBottomRadius, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserTopRadius, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserGroundAlbedo, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMinRayleighDensity, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMaxRayleighDensity, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMinMieDensity, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMaxMieDensity, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMinAbsorptionDensity, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserMaxAbsorptionDensity, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserDensityLayerWidth, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserDensityExpTerm, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserDensityExpScale, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserDensityLinearTerm, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserDensityConstantTerm, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserDensityEnd, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserWeather, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserWeatherAmplitude, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserWeatherFrequency, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserWeatherScale, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserWeatherOctaves, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserWorleyResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserPerlinWorleyResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCurlResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserWeatherResolution, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserClouds, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsWindDirection, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsSpeed, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsCoverage, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsCrispiness, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsCurliness, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsDensity, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsAbsorption, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsInnerRadius, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsOuterRadius, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsTopColour, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsBottomColour, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsEnablePowder, AtmosphereContext )
	CU_DeclareAttributeParserBlock( parserCloudsTopOffset, AtmosphereContext )
}

#endif
