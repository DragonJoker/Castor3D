#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSubpassDependency convert( renderer::SubpassDependency const & value )
	{
		return VkSubpassDependency
		{
			value.srcSubpass,
			value.dstSubpass,
			convert( value.srcStageMask ),
			convert( value.dstStageMask ),
			convert( value.srcAccessMask ),
			convert( value.dstAccessMask ),
			convert( value.dependencyFlags )
		};
	}
}
