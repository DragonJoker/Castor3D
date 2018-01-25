#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkBufferCopy convert( renderer::BufferCopy const & value )
	{
		return VkBufferCopy
		{
			value.srcOffset,
			value.dstOffset,
			value.size,
		};
	}
}
