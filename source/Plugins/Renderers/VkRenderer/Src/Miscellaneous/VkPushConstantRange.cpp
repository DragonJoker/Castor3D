#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPushConstantRange convert( renderer::PushConstantRange const & value )
	{
		return VkPushConstantRange
		{
			convert( value.stageFlags ),
			value.offset,
			value.size,
		};
	}
}
