/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include "DiamondSquareTerrain.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace diamond_square_terrain
{
	struct ParserContext
	{
		castor3d::Engine * engine{};
		castor3d::Parameters parameters{};
		Biomes biomes;
		Biome biome;
	};

	enum class DiamondSquareSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'D', 'M', 'S', 'Q' ),
		eBiome = CU_MakeSectionName( 'D', 'S', 'B', 'M' ),
	};

	CU_DeclareAttributeParser( parserDiamondSquareTerrain )
	CU_DeclareAttributeParser( parserDiamondSquareTerrainEnd )
	CU_DeclareAttributeParser( parserRandomSeed )
	CU_DeclareAttributeParser( parserIsland )
	CU_DeclareAttributeParser( parserXzScale )
	CU_DeclareAttributeParser( parserUvScale )
	CU_DeclareAttributeParser( parserHeightRange )
	CU_DeclareAttributeParser( parserDetail )
	CU_DeclareAttributeParser( parserGradient )
	CU_DeclareAttributeParser( parserHeatOffset )
	CU_DeclareAttributeParser( parserBiome )
	CU_DeclareAttributeParser( parserBiomeEnd )
	CU_DeclareAttributeParser( parserBiomeRange )
	CU_DeclareAttributeParser( parserBiomeLowSteepness )
	CU_DeclareAttributeParser( parserBiomeMedSteepness )
	CU_DeclareAttributeParser( parserBiomeHigSteepness )
}

#endif
