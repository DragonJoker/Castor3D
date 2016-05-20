/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_CUBE_MAP_TEXTURE_H___
#define ___C3D_CUBE_MAP_TEXTURE_H___

#include "Texture/TextureLayout.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		13/05/2016
	\~english
	\brief		Cube map implementation.
	\remark		Special texture holding 6 faces of a cube texture.
	\~french
	\brief		Implémentation de cube map.
	\remark		Une texture spéciale contenant les textures de 6 faces d'un cube.
	*/
	class CubeMapTexture
		: public TextureLayout
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of eACCESS_TYPE).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 */
		C3D_API CubeMapTexture( RenderSystem & p_renderSystem, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~CubeMapTexture();
		/**
		 *\copydoc		Castor3D::Texture::Initialise
		 */
		C3D_API virtual bool Initialise();
		/**
		 *\copydoc		Castor3D::Texture::Cleanup
		 */
		C3D_API virtual void Cleanup();
	};
}

#endif
