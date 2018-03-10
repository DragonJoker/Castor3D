/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Core/GlConnection.hpp"

#include "Core/GlRenderer.hpp"

#include <Core/PlatformWindowHandle.hpp>

namespace gl_renderer
{
	Connection::Connection( renderer::Renderer const & renderer
		, uint32_t deviceIndex
		, renderer::WindowHandle && handle )
		: renderer::Connection{ renderer
			, deviceIndex
			, std::move( handle ) }
	{
	}
}
