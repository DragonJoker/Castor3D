/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GenerateBiomes_H___
#define ___C3D_GenerateBiomes_H___

#include "DiamondSquareTerrainPrerequisites.hpp"

#include <Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp>

namespace diamond_square_terrain
{
	void generateBiomes( std::default_random_engine engine
		, uint32_t max
		, uint32_t size
		, float heatOffset
		, float zeroPoint
		, Matrix const & heightMap
		, Biomes const & biomes
		, castor3d::TriFaceMapping const & faces
		, castor3d::Submesh & submesh );
}

#endif
