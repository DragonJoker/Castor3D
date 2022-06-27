/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GenerateHeightMap_H___
#define ___C3D_GenerateHeightMap_H___

#include "DiamondSquareTerrainPrerequisites.hpp"

namespace diamond_square_terrain
{
	void generateHeightMap( bool island
		, std::default_random_engine engine
		, uint32_t max
		, uint32_t size
		, Matrix & heightMap );
}

#endif
