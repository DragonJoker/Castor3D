/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshFactory_H___
#define ___C3D_MeshFactory_H___

#include "MeshModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	class MeshFactory
		: public castor::Factory< MeshGenerator, castor::String >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API MeshFactory();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MeshFactory();
	};
}

#endif
