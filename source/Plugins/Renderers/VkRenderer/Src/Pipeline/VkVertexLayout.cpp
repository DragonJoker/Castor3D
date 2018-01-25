#include "Pipeline/VkVertexLayout.hpp"

#include "Shader/VkAttribute.hpp"

namespace vk_renderer
{
	VertexLayout::VertexLayout( uint32_t bindingSlot
		, uint32_t stride )
		: renderer::VertexLayout{ bindingSlot, stride }
		, m_bindingDescription
		{
			bindingSlot,                 // binding
			stride,                      // stride
			VK_VERTEX_INPUT_RATE_VERTEX  // inputRate
		}
	{
	}

	renderer::AttributeBase VertexLayout::createAttribute( uint32_t location
		, renderer::AttributeFormat format
		, uint32_t offset )
	{
		m_attributes.emplace_back( *this
			, format
			, location
			, offset );
		assert( m_bindingDescription.stride >= offset + getSize( format ) );
		return m_attributes.back();
	}
}
