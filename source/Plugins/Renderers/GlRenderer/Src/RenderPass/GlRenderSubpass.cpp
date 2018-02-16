/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/GlRenderSubpass.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	RenderSubpass::RenderSubpass( renderer::Device const & device
		, renderer::RenderPassAttachmentArray const & attaches
		, renderer::RenderSubpassState const & neededState )
		: renderer::RenderSubpass{ device, attaches, neededState }
		, m_attaches{ attaches }
	{
	}
}
