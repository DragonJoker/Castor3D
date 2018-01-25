#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkBufferImageCopy convert( renderer::BufferImageCopy const & value )
	{
		return VkBufferImageCopy
		{
			value.bufferOffset,
			value.bufferRowLength,
			value.bufferImageHeight,
			convert( value.imageSubresource ),
			{ value.imageOffset[0], value.imageOffset[1], value.imageOffset[2] },
			{ value.imageExtent[0], value.imageExtent[1], value.imageExtent[2] },
		};
	}
}
