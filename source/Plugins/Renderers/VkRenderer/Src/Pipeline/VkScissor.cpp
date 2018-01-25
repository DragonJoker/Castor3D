/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkRect2D convert( renderer::Scissor const & scissor )
	{
		return VkRect2D
		{
			scissor.getOffset()[0],
			scissor.getOffset()[1],
			uint32_t( scissor.getSize()[0] ),
			uint32_t( scissor.getSize()[1] )
		};
	}
}
