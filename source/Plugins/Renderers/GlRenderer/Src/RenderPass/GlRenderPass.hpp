/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "GlRendererPrerequisites.hpp"

#include <RenderPass/RenderPass.hpp>

namespace gl_renderer
{
	class RenderPass
		: public renderer::RenderPass
	{
	public:
		RenderPass( renderer::Device const & device
			, renderer::RenderPassCreateInfo && createInfo );
		/**
		*\copydoc	renderer::RenderPass::createFrameBuffer
		*/
		renderer::FrameBufferPtr createFrameBuffer( renderer::Extent2D const & dimensions
			, renderer::FrameBufferAttachmentArray && textures )const override;
	};
}
