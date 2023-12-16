/*
See LICENSE file in root folder
*/
#ifndef __C3DAS_AtmosphereScattering_Parsers_H___
#define __C3DAS_AtmosphereScattering_Parsers_H___

#include "DiamondSquareTerrain.hpp"

#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace diamond_square_terrain
{
	struct TerrainContext
	{
		castor3d::SceneRPtr scene{};
		castor3d::MeshRPtr mesh{};
		castor3d::Parameters parameters{};
		Biomes biomes{};
		Biome biome{};
	};

	enum class DiamondSquareSection
		: uint32_t
	{
		eRoot = CU_MakeSectionName( 'D', 'M', 'S', 'Q' ),
		eBiome = CU_MakeSectionName( 'D', 'S', 'B', 'M' ),
	};

	CU_DeclareAttributeParserBlock( parserDiamondSquareTerrain, castor3d::MeshContext )
	CU_DeclareAttributeParserBlock( parserDiamondSquareTerrainEnd, TerrainContext )
	CU_DeclareAttributeParserBlock( parserRandomSeed, TerrainContext )
	CU_DeclareAttributeParserBlock( parserIsland, TerrainContext )
	CU_DeclareAttributeParserBlock( parserXzScale, TerrainContext )
	CU_DeclareAttributeParserBlock( parserUvScale, TerrainContext )
	CU_DeclareAttributeParserBlock( parserHeightRange, TerrainContext )
	CU_DeclareAttributeParserBlock( parserDetail, TerrainContext )
	CU_DeclareAttributeParserBlock( parserGradient, TerrainContext )
	CU_DeclareAttributeParserBlock( parserHeatOffset, TerrainContext )
	CU_DeclareAttributeParserBlock( parserBiome, TerrainContext )
	CU_DeclareAttributeParserBlock( parserBiomeEnd, TerrainContext )
	CU_DeclareAttributeParserBlock( parserBiomeRange, TerrainContext )
	CU_DeclareAttributeParserBlock( parserBiomeLowSteepness, TerrainContext )
	CU_DeclareAttributeParserBlock( parserBiomeMedSteepness, TerrainContext )
	CU_DeclareAttributeParserBlock( parserBiomeHigSteepness, TerrainContext )
}

#endif
