/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/GlRenderSubpass.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	RenderSubpass::RenderSubpass( renderer::Device const & device
		, std::vector< renderer::PixelFormat > const & formats
		, renderer::RenderSubpassState const & neededState )
		: renderer::RenderSubpass{ device, formats, neededState }
	{
	}
}
