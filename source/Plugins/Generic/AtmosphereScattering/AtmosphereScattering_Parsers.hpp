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
	static castor::String const Transmittance = "transmittance";
	static castor::String const Inscatter = "inscatter";
	static castor::String const Irradiance = "irradiance";
	static castor::String const PluginType = "atmospheric_scattering";
	static castor::String const PluginName = "Atmospheric Scattering";

	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		AtmosphereScatteringUboConfiguration config{};
		castor::Path relImgPath;
		castor::Point2ui img2dDimensions;
		castor::Point3ui img3dDimensions;
		castor::PixelFormat imgFormat;
		castor3d::Parameters parameters;
	};

	enum class AtmosphereSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'A', 'T', 'S', 'C' ),
		eTransmittance = CU_MakeSectionName( 'A', 'S', 'T', 'R' ),
		eInscatter = CU_MakeSectionName( 'A', 'S', 'I', 'S' ),
		eIrradiance = CU_MakeSectionName( 'A', 'S', 'I', 'R' ),
	};

	CU_DeclareAttributeParser( parserAtmosphereScattering )
	CU_DeclareAttributeParser( parserTransmittance )
	CU_DeclareAttributeParser( parserTransmittanceImage )
	CU_DeclareAttributeParser( parserTransmittanceDimensions )
	CU_DeclareAttributeParser( parserTransmittanceFormat )
	CU_DeclareAttributeParser( parserTransmittanceEnd )
	CU_DeclareAttributeParser( parserInscatter )
	CU_DeclareAttributeParser( parserInscatterImage )
	CU_DeclareAttributeParser( parserInscatterDimensions )
	CU_DeclareAttributeParser( parserInscatterFormat )
	CU_DeclareAttributeParser( parserInscatterEnd )
	CU_DeclareAttributeParser( parserIrradiance )
	CU_DeclareAttributeParser( parserIrradianceImage )
	CU_DeclareAttributeParser( parserIrradianceDimensions )
	CU_DeclareAttributeParser( parserIrradianceFormat )
	CU_DeclareAttributeParser( parserIrradianceEnd )
	CU_DeclareAttributeParser( parserAtmosphereScatteringEnd )
}

#endif