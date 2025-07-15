/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DiamondSquareTerrain_H___
#define ___C3D_DiamondSquareTerrain_H___

#include "DiamondSquareTerrainPrerequisites.hpp"

#include <Castor3D/Material/MaterialModule.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>

namespace diamond_square_terrain
{
	class Generator
		: public c3d::MeshGenerator
	{
	public:
		Generator();
		/**
		*\copydoc		c3d::MeshGenerator::create
		*/
		static c3d::MeshGeneratorUPtr create();

		void setBiomes( Biomes biomes )
		{
			m_biomes = c3d::move( biomes );
		}

	private:
		/**
		 *\copydoc		c3d::MeshGenerator::doGenerate
		 */
		virtual void doGenerate( c3d::Mesh & mesh
			, c3d::Parameters const & parameters )override;

	public:
		static c3d::MbString const Name;
		static c3d::String const Type;
		static c3d::String const Biome;
		static c3d::String const BiomeRange;
		static c3d::String const BiomeLowSteepness;
		static c3d::String const BiomeMediumSteepness;
		static c3d::String const BiomeHighSteepness;
		static c3d::String const ParamRandomSeed;
		static c3d::String const ParamHeightRange;
		static c3d::String const ParamYMin;
		static c3d::String const ParamYMax;
		static c3d::String const ParamXzScale;
		static c3d::String const ParamXScale;
		static c3d::String const ParamZScale;
		static c3d::String const ParamUvScale;
		static c3d::String const ParamUScale;
		static c3d::String const ParamVScale;
		static c3d::String const ParamDetail;
		static c3d::String const ParamGradient;
		static c3d::String const ParamGradientFolder;
		static c3d::String const ParamGradientRelative;
		static c3d::String const ParamHeatOffset;
		static c3d::String const ParamIsland;

	private:
		Biomes m_biomes;
	};
}

#endif
