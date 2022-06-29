/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DiamondSquareTerrain_H___
#define ___C3D_DiamondSquareTerrain_H___

#include <Castor3D/Model/Mesh/MeshGenerator.hpp>

namespace diamond_square_terrain
{
	class Generator
		: public castor3d::MeshGenerator
	{
	public:
		Generator();
		/**
		*\copydoc		castor3d::MeshGenerator::create
		*/
		static castor3d::MeshGeneratorSPtr create();

	private:
		/**
		 *\copydoc		castor3d::MeshGenerator::doGenerate
		 */
		virtual void doGenerate( castor3d::Mesh & p_mesh
			, castor3d::Parameters const & p_parameters )override;

	public:
		static castor::String const Type;
		static castor::String const Name;
		static castor::String const ParamRoughness;
		static castor::String const ParamRandomSeed;
		static castor::String const ParamScale;
		static castor::String const ParamDetail;
		static castor::String const ParamMinY;
	};
}

#endif
