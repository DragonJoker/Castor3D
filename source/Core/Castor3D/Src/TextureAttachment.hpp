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
	class C3D_API TextureAttachment
		:	public FrameBufferAttachment
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pTexture	The texture
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pTexture	La texture
		 */
		TextureAttachment( DynamicTextureSPtr p_pTexture );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextureAttachment();
		/**
		 *\~english
		 *\brief		Attaches the render buffer to the given frame buffer, at given attachment point
		 *\param[in]	p_eAttachment	The attachment point
		 *\param[in]	p_pFrameBuffer	The frame buffer
		 *\param[in]	p_eTarget		The dimension to which the texture must be attached
		 *\param[in]	p_iLayer		The associated layer, if p_eDimension equal eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true if OK
		 *\~french
		 *\brief		Attache la texture au tampon d'image donné, au point d'attache voulu
		 *\param[in]	p_eAttachment	Le point d'attache
		 *\param[in]	p_pFrameBuffer	Le tampon d'image
		 *\param[in]	p_eTarget		La dimension à laquelle la texture doit être attachée
		 *\param[in]	p_iLayer		La couche associée, si p_eDimension vaut eTEXTURE_TARGET_3D or eTEXTURE_TARGET_LAYER
		 *\return		\p true si tout s'est bien passé
		 */
		bool Attach( eATTACHMENT_POINT p_eAttachment, FrameBufferSPtr p_pFrameBuffer, eTEXTURE_TARGET p_eTarget, int p_iLayer = 0 );
		/**
		 *\~english
		 *\brief		Detaches the render buffer from the frame buffer
		 *\~french
		 *\brief		Détache la texture du tampon d'image
		 */
		void Detach();
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
			return m_pTexture.lock();
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
			return m_eAttachedTarget;
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
			return m_iAttachedLayer;
		}

	private:
		//!\~english The attached texture	\~french La texture attachée
		DynamicTextureWPtr m_pTexture;
		//!\~english The texture target type	\~french Le type de cible de la texture
		eTEXTURE_TARGET m_eAttachedTarget;
		//!\~english The attached layer	\~french La couche attachée
		int m_iAttachedLayer;
	};
}

#endif
