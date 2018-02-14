#include "Shader/VkAttribute.hpp"

#include <Pipeline/VertexLayout.hpp>

namespace vk_renderer
{
	VkVertexInputAttributeDescription convert( renderer::Attribute const & value )
	{
		return VkVertexInputAttributeDescription
		{
			value.getLocation(),
			value.getLayout().getBindingSlot(),
			convert( value.getFormat() ),
			value.getOffset()
		};
	}
}
