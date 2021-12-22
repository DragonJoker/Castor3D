/*
See LICENSE file in root folder
*/
#ifndef __Ocean_Parsers_H___
#define __Ocean_Parsers_H___

#include "OceanFFTUbo.hpp"
#include "OceanFFT.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace ocean_fft
{
	struct ParserContext
	{
		castor3d::Engine * engine{ nullptr };
		OceanUboConfiguration config{};
		OceanFFT::Config fftConfig{};
		castor3d::Parameters parameters;
		std::vector< castor3d::MaterialRPtr > materials;
	};

	enum class OceanSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'O', 'C', 'N', 'R' ),
		eFFT = CU_MakeSectionName( 'O', 'F', 'F', 'T' ),
	};

	CU_DeclareAttributeParser( parserOceanRendering )
	CU_DeclareAttributeParser( parserDepthSofteningDistance )
	CU_DeclareAttributeParser( parserRefrRatio )
	CU_DeclareAttributeParser( parserRefrDistortionFactor )
	CU_DeclareAttributeParser( parserRefrHeightFactor )
	CU_DeclareAttributeParser( parserRefrDistanceFactor )
	CU_DeclareAttributeParser( parserSsrSettings )
	CU_DeclareAttributeParser( parserDensity )
	CU_DeclareAttributeParser( parserFftConfig )
	CU_DeclareAttributeParser( parserOceanRenderingEnd )
	CU_DeclareAttributeParser( parserFftDisableRandomSeed )
	CU_DeclareAttributeParser( parserFftSize )
	CU_DeclareAttributeParser( parserFftHeightMapSamples )
	CU_DeclareAttributeParser( parserFftDisplacementDownsample )
	CU_DeclareAttributeParser( parserFftNormalMapFreqMod )
	CU_DeclareAttributeParser( parserFftMaxWaveLength )
	CU_DeclareAttributeParser( parserFftAmplitude )
	CU_DeclareAttributeParser( parserFftWindDirection )
	CU_DeclareAttributeParser( parserFftWindVelocity )
	CU_DeclareAttributeParser( parserFftPatchSize )
	CU_DeclareAttributeParser( parserFftBlocksCount )
	CU_DeclareAttributeParser( parserFftLOD0Distance )
	CU_DeclareAttributeParser( parserFftConfigEnd )
}

#endif