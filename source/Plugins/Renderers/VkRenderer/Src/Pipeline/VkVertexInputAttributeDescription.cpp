#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkVertexInputAttributeDescription convert( renderer::VertexInputAttributeDescription const & desc )
	{
		return VkVertexInputAttributeDescription
		{
			desc.location,
			desc.binding,
			convert( desc.format ),
			desc.offset,
		};
	}
}
