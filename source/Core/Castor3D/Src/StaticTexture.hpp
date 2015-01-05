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

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API StaticTexture
		:	public TextureBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		StaticTexture( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~StaticTexture();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_uiIndex		The texture index
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_uiIndex		L'index de la texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise( uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Defines the texture buffer and its dimensions, for a 3D texture or a texture array
		 *\param[in]	p_dimensions	The texture dimensions
		 *\param[in]	p_pBuffer		The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture, ainsi que ses dimensions, dans le cas de texture 2D ou tableau de textures
		 *\param[in]	p_dimensions	Les dimensions de la texture
		 *\param[in]	p_pBuffer		Le buffer
		 */
		void SetImage( Castor::Point3ui const & p_dimensions, Castor::PxBufferBaseSPtr p_pBuffer );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_uiIndex	The texture index
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_uiIndex	L'index de texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind();
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_uiIndex	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_uiIndex	L'index de texture
		 */
		virtual void Unbind();

		using TextureBase::SetImage;
	};
}

#pragma warning( pop )

#endif
