/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Miscellaneous/ImageBlit.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Commande de copie du contenu d'une image dans une autre.
	*/
	class BlitImageCommand
		: public CommandBase
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*\param[in] blitInfo
		*	Les informations de copie.
		*\param[in] src
		*	L'image source.
		*\param[in] dst
		*	L'image destination.
		*/
		BlitImageCommand( renderer::ImageBlit const & blitInfo
			, renderer::FrameBufferAttachment const & src
			, renderer::FrameBufferAttachment const & dst
			, renderer::Filter filter );
		~BlitImageCommand();

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Texture const & m_srcTexture;
		Texture const & m_dstTexture;
		FrameBuffer const & m_srcFrameBuffer;
		FrameBuffer const & m_dstFrameBuffer;
		renderer::ImageBlit m_blitInfo;
		GlFilter m_filter;
		GlImageAspectFlags m_mask;
		GLuint m_fbo;
	};
}
