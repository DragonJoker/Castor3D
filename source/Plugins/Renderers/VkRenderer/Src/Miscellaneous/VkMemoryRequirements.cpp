#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkMemoryRequirements convert( renderer::MemoryRequirements const & value )
	{
		return VkMemoryRequirements
		{
			value.size,
			value.alignment,
			value.memoryTypeBits,
		};
	}

	renderer::MemoryRequirements convert( VkMemoryRequirements const & value )
	{
		return renderer::MemoryRequirements
		{
			renderer::ResourceType( 0u ),
			value.size,
			value.alignment,
			value.memoryTypeBits,
		};
	}
}
