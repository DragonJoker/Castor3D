#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkVertexInputBindingDescription convert( renderer::VertexInputBindingDescription const & desc )
	{
		return VkVertexInputBindingDescription
		{
			desc.binding,
			desc.stride,
			convert( desc.inputRate )
		};
	}
}
