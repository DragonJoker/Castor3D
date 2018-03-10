#include "VkRendererPrerequisites.hpp"

#include "Buffer/VkBuffer.hpp"

namespace vk_renderer
{
	VkDescriptorBufferInfo convert( renderer::DescriptorBufferInfo const & value )
	{
		return VkDescriptorBufferInfo
		{
			static_cast< Buffer const & >( value.buffer.get() ),
			value.offset,
			value.range,
		};
	}
}
