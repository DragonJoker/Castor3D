#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSubpassDescriptionFlags convert( renderer::SubpassDescriptionFlags const & flags )
	{
		return VkSubpassDescriptionFlags{ 0 };
	}

	renderer::SubpassDescriptionFlags convertSubpassDescriptionFlags( VkSubpassDescriptionFlags const & flags )
	{
		return renderer::SubpassDescriptionFlags{ 0 };
	}
}
