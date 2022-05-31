/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MeshPreparer_H___
#define ___C3D_MeshPreparer_H___

#include "Castor3D/Model/Mesh/MeshModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"

namespace castor3d
{
	class MeshPreparer
	{
	public:
		/**
		 *\~english
		 *\brief		Prepares a Mesh for render.
		 *\param[out]	mesh		The mesh to prepare.
		 *\param[in]	parameters	Prepare configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Prépare un Mesh pour le rendu.
		 *\param[out]	mesh		Le mesh à préparer.
		 *\param[in]	parameters	Paramètres de configuration de la préparation.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool prepare( Mesh & mesh
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Prepares a Mesh for render.
		 *\param[out]	submesh		The mesh to prepare.
		 *\param[in]	parameters	Prepare configuration parameters.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Prépare un Mesh pour le rendu.
		 *\param[out]	submesh		Le mesh à préparer.
		 *\param[in]	parameters	Paramètres de configuration de la préparation.
		 *\return		\p false si un problème quelconque est survenu.
		 */
		C3D_API static bool prepare( Submesh & submesh
			, Parameters const & parameters );
	};
}

#endif
