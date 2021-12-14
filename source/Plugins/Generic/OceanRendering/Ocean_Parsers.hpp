/*
See LICENSE file in root folder
*/
#ifndef __Ocean_Parsers_H___
#define __Ocean_Parsers_H___

#include "OceanUbo.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace ocean
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		OceanUboConfiguration config;
		castor3d::Parameters parameters;
		std::vector< castor3d::MaterialRPtr > materials;
		uint32_t wave{};
	};

	enum class OceanSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'W', 'A', 'V', 'R' ),
		eWaves = CU_MakeSectionName( 'W', 'A', 'V', 'S' ),
		eWave = CU_MakeSectionName( 'W', 'A', 'V', 'E' ),
	};

	CU_DeclareAttributeParser( parserOceanRendering )
	CU_DeclareAttributeParser( parserTessellationFactor )
	CU_DeclareAttributeParser( parserDampeningFactor )
	CU_DeclareAttributeParser( parserRefrDistortionFactor )
	CU_DeclareAttributeParser( parserRefrHeightFactor )
	CU_DeclareAttributeParser( parserRefrDistanceFactor )
	CU_DeclareAttributeParser( parserDepthSofteningDistance )
	CU_DeclareAttributeParser( parserFoamHeightStart )
	CU_DeclareAttributeParser( parserFoamFadeDistance )
	CU_DeclareAttributeParser( parserFoamTiling )
	CU_DeclareAttributeParser( parserFoamAngleExponent )
	CU_DeclareAttributeParser( parserFoamBrightness )
	CU_DeclareAttributeParser( parserNormalMapScrollSpeed )
	CU_DeclareAttributeParser( parserNormalMapScroll )
	CU_DeclareAttributeParser( parserSsrSettings )
	CU_DeclareAttributeParser( parserFoam )
	CU_DeclareAttributeParser( parserNormals1 )
	CU_DeclareAttributeParser( parserNormals2 )
	CU_DeclareAttributeParser( parserNoise )
	CU_DeclareAttributeParser( parserWaveRenderingEnd )
	CU_DeclareAttributeParser( parserWaves )
	CU_DeclareAttributeParser( parserWavesEnd )
	CU_DeclareAttributeParser( parserWave )
	CU_DeclareAttributeParser( parserWaveDirection )
	CU_DeclareAttributeParser( parserWaveSteepness )
	CU_DeclareAttributeParser( parserWaveLength )
	CU_DeclareAttributeParser( parserWaveAmplitude )
	CU_DeclareAttributeParser( parserWaveSpeed )
	CU_DeclareAttributeParser( parserWaveEnd )
}

#endif
