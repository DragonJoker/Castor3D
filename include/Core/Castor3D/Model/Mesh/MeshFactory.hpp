/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshFactory_H___
#define ___C3D_MeshFactory_H___

#include "MeshModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace c3d
{
	class MeshFactory
		: public Factory< MeshGenerator, String, UniquePtr< MeshGenerator > >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API MeshFactory();
	};
}

#endif
