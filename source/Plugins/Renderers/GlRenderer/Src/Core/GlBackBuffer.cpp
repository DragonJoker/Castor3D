/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/GlBackBuffer.hpp"

namespace gl_renderer
{
	BackBuffer::BackBuffer( renderer::Device const & device
		, renderer::TexturePtr && image
		, renderer::TextureViewPtr && view
		, uint32_t imageIndex )
		: renderer::BackBuffer{ device, std::move( image ), std::move( view ), imageIndex }
	{
	}
}
