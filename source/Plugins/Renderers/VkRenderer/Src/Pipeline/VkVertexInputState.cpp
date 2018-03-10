#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineVertexInputStateCreateInfo convert( renderer::VertexInputState const & state
		, std::vector< VkVertexInputAttributeDescription > const & attributes
		, std::vector< VkVertexInputBindingDescription > const & bindings )
	{
		return VkPipelineVertexInputStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			static_cast< uint32_t >( bindings.size() ),
			bindings.data(),
			static_cast< uint32_t >( attributes.size() ),
			attributes.data()
		};
	}
}
