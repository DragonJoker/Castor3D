/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PrepassModule_H___
#define ___C3D_PrepassModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Prepass */
	//@{

	/**
	*\~english
	*\brief
	*	Deferred lighting Render technique pass.
	*\~french
	*\brief
	*	Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class DepthPass;
	/**
	*\~english
	*\brief
	*	The render nodes pass writing visibility buffer.
	*\~french
	*\brief
	*	La passe de noeuds de rendu générant le buffer de visibilité.
	*/
	class VisibilityPass;

	CU_DeclareCUSmartPtr( castor3d, DepthPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, VisibilityPass, C3D_API );

	//@}
	//@}
}

#endif
