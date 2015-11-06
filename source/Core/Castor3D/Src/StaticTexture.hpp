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
#ifndef ___C3D_STATIC_TEXTURE_H___
#define ___C3D_STATIC_TEXTURE_H___

#include "Texture.hpp"

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
		: public TextureBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le render system
		 */
		C3D_API StaticTexture( RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~StaticTexture();
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual bool Initialise( uint32_t p_index, uint8_t p_cpuAccess = 0xFF, uint8_t p_gpuAccess = 0xFF );
		/**
		 *\~english
		 *\brief		Defines the texture buffer and its dimensions, for a 3D texture or a texture array
		 *\param[in]	p_dimensions	The texture dimensions
		 *\param[in]	p_buffer		The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture, ainsi que ses dimensions, dans le cas de texture 2D ou tableau de textures
		 *\param[in]	p_dimensions	Les dimensions de la texture
		 *\param[in]	p_buffer		Le buffer
		 */
		C3D_API void SetImage( Castor::Point3ui const & p_dimensions, Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual bool BindAt( uint32_t p_index );
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual void UnbindFrom( uint32_t p_index );

		using TextureBase::SetImage;
	};
}

#endif
