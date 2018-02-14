#include "Pipeline/VkVertexLayout.hpp"

#include "Shader/VkAttribute.hpp"

namespace vk_renderer
{
	VertexLayout::VertexLayout( uint32_t bindingSlot
		, uint32_t stride
		, renderer::VertexInputRate inputRate )
		: renderer::VertexLayout{ bindingSlot, stride, inputRate }
		, m_bindingDescription
		{
			bindingSlot,
			stride,
			convert( inputRate )
		}
	{
	}
}
