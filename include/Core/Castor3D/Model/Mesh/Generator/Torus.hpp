/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Torus_H___
#define ___C3D_Torus_H___

#include "Castor3D/Model/Mesh/Generator/MeshGeneratorModule.hpp"
#include "Castor3D/Model/Mesh/MeshGenerator.hpp"

namespace castor3d
{
	class Torus
		: public MeshGenerator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Torus();
		/**
		 *\copydoc		castor3d::MeshGenerator::create
		 */
		C3D_API static MeshGeneratorUPtr create();

	private:
		C3D_API void doGenerate( Mesh & mesh
			, Parameters const & parameters )override;
	};
}

#endif
