/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkViewport convert( renderer::Viewport const & viewport )
	{
		return VkViewport
		{
			float( viewport.getOffset().x ),
			float( viewport.getOffset().y ),
			float( viewport.getSize().width ),
			float( viewport.getSize().height ),
			viewport.getDepthBounds()[0],
			viewport.getDepthBounds()[1]
		};
	}
}
