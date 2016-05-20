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
#ifndef ___C3D_STATIC_TEXTURE_H___
#define ___C3D_STATIC_TEXTURE_H___

#include "Texture/TextureLayout.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Static texture class
	\remark		A static texture deletes it's buffer from ram when initialised
	\~french
	\brief		Class de texture statique
	\remark		Une texture statique supprime son buffer de la ram quand elle est initialisée
	*/
	class StaticTexture
		: public TextureLayout
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of eACCESS_TYPE).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_type			Le type de texture.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 */
		C3D_API StaticTexture( RenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~StaticTexture();
	};
}

#endif
