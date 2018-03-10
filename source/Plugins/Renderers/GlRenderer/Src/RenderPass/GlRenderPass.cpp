/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/GlRenderPass.hpp"

#include "RenderPass/GlFrameBuffer.hpp"
#include "Core/GlDevice.hpp"
#include "Command/GlCommandBuffer.hpp"
#include "Image/GlTexture.hpp"

namespace gl_renderer
{
	RenderPass::RenderPass( renderer::Device const & device
		, renderer::RenderPassCreateInfo && createInfo )
		: renderer::RenderPass{ device, createInfo }
	{
	}

	renderer::FrameBufferPtr RenderPass::createFrameBuffer( renderer::Extent2D const & dimensions
		, renderer::FrameBufferAttachmentArray && textures )const
	{
		return std::make_unique< FrameBuffer >( *this
			, dimensions
			, std::move( textures ) );
	}
}
