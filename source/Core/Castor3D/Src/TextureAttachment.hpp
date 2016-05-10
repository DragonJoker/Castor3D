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
		 *\copydoc		Castor3D::FrameBufferAttachment::GetBuffer
		 */
		C3D_API virtual Castor::PxBufferBaseSPtr GetBuffer()const;
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
		 *\brief		Retrieves the attached target dimension
		 *\return		The target dimension
		 *\~french
		 *\brief		Récupère la dimension cible attachée
		 *\return		La dimension cible
		 */
		inline eTEXTURE_TARGET GetTarget()const
		{
			return m_target;
		}
		/**
		 *\~english
		 *\brief		Retrieves the attached layer
		 *\return		The layer
		 *\~french
		 *\brief		Récupère la couche associée
		 *\return		La couche
		 */
		inline int GetLayer()const
		{
			return m_layer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the attached target dimension.
		 *\param[in]	p_target	The target dimension.
		 *\~french
		 *\brief		Récupère la dimension cible attachée.
		 *\param[in]	p_target	La dimension cible.
		 */
		inline void SetTarget( eTEXTURE_TARGET p_target )
		{
			m_target = p_target;
		}
		/**
		 *\~english
		 *\brief		Sets the attached layer.
		 *\param[in]	p_layer	The layer.
		 *\~french
		 *\brief		Définit la couche associée.
		 *\param[in]	p_layer	La couche.
		 */
		inline void SetLayer( int p_layer )
		{
			m_layer = p_layer;
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
