/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_FrameBufferAttachment_HPP___
#define ___Renderer_FrameBufferAttachment_HPP___
#pragma once

#include "Image/TextureView.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	An attach between a texture and a framebuffer.
	*\~french
	*\brief
	*	Une attache entre une image et un tampon d'image.
	*/
	class FrameBufferAttachment
	{
		friend class FrameBuffer;

	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param[in] attach
		*	The associated render pass attach.
		*\param[in] view
		*	The texture view.
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] attach
		*	L'attache de passe de rendu correspondante.
		*\param[in] view
		*	La vue sur la texture.
		*/
		FrameBufferAttachment( AttachmentDescription const & attach
			, TextureView const & view );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		virtual ~FrameBufferAttachment() = default;
		/**
		*\~english
		*\return
		*	The pixel format of the attachment.
		*\~french
		*\return
		*	Le format des pixels de l'attache.
		*/
		inline renderer::Format getFormat()const
		{
			return m_view.getFormat();
		}
		/**
		*\~english
		*\return
		*	The texture view.
		*\~french
		*\return
		*	La vue sur la texture.
		*/
		inline TextureView const & getView()const
		{
			return m_view;
		}
		/**
		*\~english
		*\return
		*	The texture.
		*\~french
		*\return
		*	La texture.
		*/
		inline Texture const & getTexture()const
		{
			return m_view.getTexture();
		}
		/**
		*\~english
		*\return
		*	The framebuffer.
		*\~french
		*\return
		*	Le tampon d'image.
		*/
		inline FrameBuffer const & getFrameBuffer()const
		{
			return *m_frameBuffer;
		}
		/**
		*\~english
		*\return
		*	The associated render pass attach.
		*\~french
		*\return
		*	L'attache de passe de rendu correspondante.
		*/
		inline AttachmentDescription const & getAttachment()const
		{
			return m_attach;
		}

	private:
		TextureView const & m_view;
		FrameBuffer const * m_frameBuffer;
		AttachmentDescription const & m_attach;
	};
}

#endif
