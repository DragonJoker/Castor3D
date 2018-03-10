#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkRenderPassCreateFlags convert( renderer::RenderPassCreateFlags const & flags )
	{
		return VkRenderPassCreateFlags{ 0 };
	}

	renderer::RenderPassCreateFlags convertRenderPassCreateFlags( VkRenderPassCreateFlags const & flags )
	{
		return renderer::RenderPassCreateFlags{ 0 };
	}
}
