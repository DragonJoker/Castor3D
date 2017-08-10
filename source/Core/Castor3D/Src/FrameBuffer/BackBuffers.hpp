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
#ifndef ___C3D_BACK_BUFFERS_H___
#define ___C3D_BACK_BUFFERS_H___

#include "FrameBuffer.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		20/10/2015
	\~english
	\brief		Back buffers class.
	\remark		Render window default back buffers.
	\~french
	\brief		Classe de tampons d'image de fenêtre.
	\remark		Tampons de rendu d'une fenêtre.
	*/
	class BackBuffers
		: public FrameBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit BackBuffers( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~BackBuffers();
		/**
		 *\~english
		 *\brief		Initialises the internal buffer, in RAM memory.
		 *\param[in]	p_size		The buffer dimensions.
		 *\param[in]	p_format	The buffer pixel format.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Initialise le tampon interne, en mémoire RAM.
		 *\param[in]	p_size		Les dimensions du tampon.
		 *\param[in]	p_format	Le format des pixels du tampon.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( castor::Size const & p_size, castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\param[in]	p_buffer	The frame buffer to bind.
		 *\param[in]	p_target	The frame buffer binding target.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\param[in]	p_buffer	Le tampon d'image à activer.
		 *\param[in]	p_target	La cible d'activation du tampon d'image.
		 */
		C3D_API virtual void bind( WindowBuffer p_buffer, FrameBufferTarget p_target )const = 0;
		/**
		 *\~english
		 *\brief		Creates an attachment to a render buffer
		 *\param[in]	p_renderBuffer	The render buffer
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à un tampon de rendu
		 *\param[in]	p_renderBuffer	Le tampon de rendu
		 *\return		L'attache créée
		 */
		C3D_API RenderBufferAttachmentSPtr createAttachment( RenderBufferSPtr p_renderBuffer )override
		{
			return nullptr;
		}
		/**
		 *\~english
		 *\brief		Creates an attachment to a texture
		 *\param[in]	p_texture	The texture
		 *\return		The created attachment
		 *\~french
		 *\brief		Crée une attache à une texture
		 *\param[in]	p_texture	La texture
		 *\return		L'attache créée
		 */
		C3D_API TextureAttachmentSPtr createAttachment( TextureLayoutSPtr p_texture )override
		{
			return nullptr;
		}
		/**
		 *\~english
		 *\brief		Creates an attachment to a cube texture's face.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_face		The face.
		 *\return		The created attachment.
		 *\~french
		 *\brief		Crée une attache à une face d'une texture cube.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_face		La face.
		 *\return		L'attache créée.
		 */
		C3D_API TextureAttachmentSPtr createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face )override
		{
			return nullptr;
		}
		/**
		 *\~english
		 *\brief		Creates an attachment to a cube texture's face.
		 *\param[in]	p_texture	The texture.
		 *\param[in]	p_face		The face.
		 *\return		The created attachment.
		 *\~french
		 *\brief		Crée une attache à une face d'une texture cube.
		 *\param[in]	p_texture	La texture.
		 *\param[in]	p_face		La face.
		 *\return		L'attache créée.
		 */
		C3D_API TextureAttachmentSPtr createAttachment( TextureLayoutSPtr p_texture, CubeMapFace p_face, uint32_t p_mipLevel )override
		{
			return nullptr;
		}
		/**
		 *\~english
		 *\brief		Checks if the FBO is complete
		 *\return		\p false if the buffer is in error if there is an attachment missing
		 *\~french
		 *\brief		Vérifie si le FBO est complet
		 *\return		\p false si le tampon est en erreur ou s'il manque une attache.
		 */
		C3D_API virtual bool isComplete()const
		{
			return true;
		}

	private:
		using FrameBuffer::initialise;
		using FrameBuffer::bind;
		using FrameBuffer::blitInto;
		using FrameBuffer::setDrawBuffers;
		using FrameBuffer::setDrawBuffer;
		using FrameBuffer::attach;
		using FrameBuffer::detachAll;
		using FrameBuffer::create;
		using FrameBuffer::setReadBuffer;
		using FrameBuffer::createColourRenderBuffer;
		using FrameBuffer::createDepthStencilRenderBuffer;
		using FrameBuffer::isComplete;
		using FrameBuffer::downloadBuffer;

	private:
		/**
		 *\copydoc		castor3d::FrameBuffer::doBind
		 */
		C3D_API void doBind( castor3d::FrameBufferTarget p_target )const override;
		/**
		 *\copydoc		castor3d::FrameBuffer::doUnbind
		 */
		C3D_API void doUnbind()const override;

	private:
		castor::PxBufferBaseSPtr m_buffer;
	};
}

#endif
