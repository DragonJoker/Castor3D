/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/GlRenderPass.hpp"

#include "RenderPass/GlFrameBuffer.hpp"
#include "Core/GlDevice.hpp"
#include "Command/GlCommandBuffer.hpp"
#include "RenderPass/GlRenderSubpass.hpp"
#include "Image/GlTexture.hpp"

namespace gl_renderer
{
	RenderPass::RenderPass( renderer::Device const & device
		, renderer::RenderPassAttachmentArray const & attaches
		, renderer::RenderSubpassPtrArray const & subpasses
		, renderer::RenderPassState const & initialState
		, renderer::RenderPassState const & finalState
		, renderer::SampleCountFlag samplesCount )
		: renderer::RenderPass{ device
			, attaches
			, subpasses
			, initialState
			, finalState
			, samplesCount }
	{
	}

	renderer::FrameBufferPtr RenderPass::createFrameBuffer( renderer::UIVec2 const & dimensions
		, renderer::FrameBufferAttachmentArray && textures )const
	{
		return std::make_shared< FrameBuffer >( *this
			, dimensions
			, std::move( textures ) );
	}
}
