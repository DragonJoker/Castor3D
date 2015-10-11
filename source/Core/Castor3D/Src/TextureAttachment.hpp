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
#ifndef ___C3D_TEXTURE_ATTACHMENT_H___
#define ___C3D_TEXTURE_ATTACHMENT_H___

#include "FrameBufferAttachment.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Description of an attachment between a frame buffer and a texture
	\~french
	\brief		Description d'une liaison entre un tampon d'image et une texture
	*/
	class TextureAttachment
		: public FrameBufferAttachment
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_texture	The texture
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_texture	La texture
		 */
		C3D_API TextureAttachment( DynamicTextureSPtr p_texture );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureAttachment();
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_attachment	The attachment point
		 *\param[in]	p_index			The attachment index
		 *\param[in]	p_frameBuffer	The frame buffer
		 *\param[in]	p_target		The dimension to which the texture must be attached
		 *\param[in]	p_layer			The associated layer, if p_eDimension equal eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache la texture au tampon d'image donné, au point d'attache voulu
		 *\param[in]	p_attachment	Le point d'attache
		 *\param[in]	p_index			L'index d'attache
		 *\param[in]	p_frameBuffer	Le tampon d'image
		 *\param[in]	p_target		La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_layer			La couche associée, si p_eDimension vaut eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_attachment, uint8_t p_index, FrameBufferSPtr p_frameBuffer, eTEXTURE_TARGET p_target, int p_layer = 0 );
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_attachment	The attachment point
		 *\param[in]	p_frameBuffer	The frame buffer
		 *\param[in]	p_target		The dimension to which the texture must be attached
		 *\param[in]	p_layer			The associated layer, if p_eDimension equal eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache la texture au tampon d'image donné, au point d'attache voulu
		 *\param[in]	p_attachment	Le point d'attache
		 *\param[in]	p_frameBuffer	Le tampon d'image
		 *\param[in]	p_target		La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_layer			La couche associée, si p_eDimension vaut eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Attach( eATTACHMENT_POINT p_attachment, FrameBufferSPtr p_frameBuffer, eTEXTURE_TARGET p_target, int p_layer = 0 );
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		Détache la texture du tampon d'image
		 */
		C3D_API void Detach();
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The texture
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La texture
		 */
		inline DynamicTextureSPtr GetTexture()const
		{
			return m_texture.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the attached dimension
		 *\return		The attached dimension
		 *\~french
		 *\brief		Récupère la dimension attachée
		 *\return		La dimension attachée
		 */
		inline eTEXTURE_TARGET GetAttachedTarget()const
		{
			return m_target;
		}
		/**
		 *\~english
		 *\brief		Retrieves the attached layer
		 *\return		The attached layer
		 *\~french
		 *\brief		Récupère la couche associée
		 *\return		La couche associée
		 */
		inline int GetAttachedLayer()const
		{
			return m_layer;
		}

	private:
		using FrameBufferAttachment::Attach;

		//!\~english The attached texture	\~french La texture attachée
		DynamicTextureWPtr m_texture;
		//!\~english The texture target type	\~french Le type de cible de la texture
		eTEXTURE_TARGET m_target;
		//!\~english The attached layer	\~french La couche attachée
		int m_layer;
	};
}

#endif
