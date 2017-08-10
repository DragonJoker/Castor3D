/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_TEXTURE_ATTACHMENT_H___
#define ___C3D_TEXTURE_ATTACHMENT_H___

#include "FrameBufferAttachment.hpp"

namespace castor3d
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
		C3D_API explicit TextureAttachment( TextureLayoutSPtr p_texture );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureAttachment();
		/**
		 *\copydoc		castor3d::FrameBufferAttachment::getBuffer
		 */
		C3D_API virtual castor::PxBufferBaseSPtr getBuffer()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The texture
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La texture
		 */
		inline TextureLayoutSPtr getTexture()const
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
		inline TextureType getTarget()const
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
		inline int getLayer()const
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
		inline void setTarget( TextureType p_target )
		{
			m_target = p_target;
		}
		/**
		 *\~english
		 *\brief		sets the attached layer.
		 *\param[in]	p_layer	The layer.
		 *\~french
		 *\brief		Définit la couche associée.
		 *\param[in]	p_layer	La couche.
		 */
		inline void setLayer( int p_layer )
		{
			m_layer = p_layer;
		}

	private:
		//!\~english The attached texture	\~french La texture attachée
		TextureLayoutWPtr m_texture;
		//!\~english The texture target type	\~french Le type de cible de la texture
		TextureType m_target;
		//!\~english The attached layer	\~french La couche attachée
		int m_layer;
	};
}

#endif
