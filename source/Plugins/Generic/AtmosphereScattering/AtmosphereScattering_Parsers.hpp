/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include "AtmosphereScatteringUbo.hpp"

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
		castor::Point2ui transmittance{ 256u, 64u };
		uint32_t multiScatter{ 32u };
		uint32_t atmosphereVolume{ 32u };
		castor::Point2ui skyView{ 192u, 108u };
		AtmosphereScatteringConfig config{};
		DensityProfileLayer * densityLayer{};
		std::unique_ptr< AtmosphereBackground > atmosphere;
	};

	enum class AtmosphereSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'A', 'T', 'S', 'C' ),
		eDensity = CU_MakeSectionName( 'A', 'T', 'D', 'S' ),
	};

	CU_DeclareAttributeParser( parserAtmosphereScattering )
	CU_DeclareAttributeParser( parserAtmosphereScatteringEnd )
	CU_DeclareAttributeParser( parserNode )
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
}

#endif