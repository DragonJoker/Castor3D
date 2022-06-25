/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DiamondSquareTerrain_H___
#define ___C3D_DiamondSquareTerrain_H___

#include <Castor3D/Material/MaterialModule.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>

namespace diamond_square_terrain
{
	struct Biome
	{
		castor::String name{};
		castor::Point2f range{};
		uint32_t passIndex{};
	};
	using Biomes = std::vector< Biome >;

	class Generator
		: public castor3d::MeshGenerator
	{
	public:
		Generator();
		/**
		*\copydoc		castor3d::MeshGenerator::create
		*/
		static castor3d::MeshGeneratorSPtr create();

		void setBiomes( Biomes biomes )
		{
			m_biomes = std::move( biomes );
		}

	private:
		/**
		 *\copydoc		castor3d::MeshGenerator::doGenerate
		 */
		virtual void doGenerate( castor3d::Mesh & p_mesh
			, castor3d::Parameters const & p_parameters )override;

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const Biome;
		static castor::String const BiomeRange;
		static castor::String const BiomePassIndex;
		static castor::String const ParamRandomSeed;
		static castor::String const ParamHeightRange;
		static castor::String const ParamYMin;
		static castor::String const ParamYMax;
		static castor::String const ParamXzScale;
		static castor::String const ParamXScale;
		static castor::String const ParamZScale;
		static castor::String const ParamUvScale;
		static castor::String const ParamUScale;
		static castor::String const ParamVScale;
		static castor::String const ParamDetail;
		static castor::String const ParamGradient;
		static castor::String const ParamGradientFolder;
		static castor::String const ParamGradientRelative;
		static castor::String const ParamHeatOffset;
		static castor::String const ParamIsland;

	private:
		Biomes m_biomes;
	};
}

#endif
