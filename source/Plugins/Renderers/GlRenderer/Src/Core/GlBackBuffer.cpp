/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Core/GlBackBuffer.hpp"

namespace gl_renderer
{
	BackBuffer::BackBuffer( renderer::Device const & device
		, renderer::SwapChain const & swapChain
		, uint32_t imageIndex )
		: renderer::BackBuffer{ device, swapChain, imageIndex }
	{
	}
}
