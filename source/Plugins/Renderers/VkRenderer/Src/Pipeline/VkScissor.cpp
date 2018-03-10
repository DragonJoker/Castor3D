/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkRect2D convert( renderer::Scissor const & scissor )
	{
		return VkRect2D
		{
			scissor.getOffset().x,
			scissor.getOffset().y,
			scissor.getSize().width,
			scissor.getSize().height
		};
	}
}
