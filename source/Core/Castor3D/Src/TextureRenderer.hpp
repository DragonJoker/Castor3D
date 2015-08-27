/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CU_TEXTURE_RENDERER_H___
#define ___CU_TEXTURE_RENDERER_H___

#include "Castor3DPrerequisites.hpp"
#include "Renderer.hpp"

namespace Castor3D
{
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\~english
	\brief Texture unit renderer
	\~french
	\brief Renderer d'unité de texture
	*/
	class C3D_API TextureRenderer
		:	public Renderer< TextureUnit, TextureRenderer >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param		p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param		p_pRenderSystem	Le render system
		 */
		TextureRenderer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextureRenderer();
		/**
		 *\~english
		 *\brief		Applies multitexturing parameters
		 *\return		\p true if ok
		 *\~french
		 *\brief		Applique les paramètres de multitexturing
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Render() = 0;
		/**
		 *\~english
		 *\brief		Removes multitexturing parameters
		 *\~french
		 *\brief		Enlève les paramètres de multitexturing
		 */
		virtual void EndRender() = 0;
	};
}

#endif
