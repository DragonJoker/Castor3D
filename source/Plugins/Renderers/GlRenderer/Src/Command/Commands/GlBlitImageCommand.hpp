/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "GlCommandBase.hpp"

#include <Miscellaneous/ImageBlit.hpp>

namespace gl_renderer
{
	class BlitImageCommand
		: public CommandBase
	{
		struct Attachment
		{
			GlAttachmentPoint point;
			GLuint object;
			GlAttachmentType type;
		};

	public:
		BlitImageCommand( Device const & device
			, renderer::Texture const & srcImage
			, renderer::Texture const & dstImage
			, std::vector< renderer::ImageBlit > const & regions
			, renderer::Filter filter );
		~BlitImageCommand();

		void apply()const override;
		CommandPtr clone()const override;

	private:
		Texture const & m_srcTexture;
		Texture const & m_dstTexture;
		Attachment m_srcAttach;
		Attachment m_dstAttach;
		GLuint m_srcFbo;
		GLuint m_dstFbo;
		std::vector< renderer::ImageBlit > m_regions;
		GlFilter m_filter;
		GlImageAspectFlags m_mask;
		GLuint m_fbo;
	};
}
