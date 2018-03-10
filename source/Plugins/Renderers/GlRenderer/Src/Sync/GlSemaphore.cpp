/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Sync/GlSemaphore.hpp"

#include "Core/GlDevice.hpp"

namespace gl_renderer
{
	Semaphore::Semaphore( renderer::Device const & device )
		: renderer::Semaphore{ device }
	{
	}
}
