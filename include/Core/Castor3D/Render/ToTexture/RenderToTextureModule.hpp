/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderToTextureModule_H___
#define ___C3D_RenderToTextureModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name ToTexture */
	//@{

	/**
	*\~english
	*\brief
	*	Class used to render an equirectangular texture into a cube.
	*\~french
	*\brief
	*	Classe utilisée pour rendre une texture équirectangulaire dans un cube.
	*/
	class EquirectangularToCube;
	/**
	*\~english
	*\brief
	*	Class used to render a texture into a cube.
	*\~french
	*\brief
	*	Classe utilisée pour rendre une texture dans un cube.
	*/
	class RenderCube;
	/**
	*\~english
	*\brief
	*	Renders a 3D texture into a 2D texture.
	*\~french
	*\brief
	*	Dessine une texture 3D dans une texture 2D.
	*/
	class Texture3DTo2D;

	CU_DeclareSmartPtr( EquirectangularToCube );
	CU_DeclareSmartPtr( RenderCube );

	CU_DeclareCUSmartPtr( castor3d, Texture3DTo2D, C3D_API );

	//@}
	//@}
}

#endif
