#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkVertexInputRate convert( renderer::VertexInputRate const & value )
	{
		switch ( value )
		{
		case renderer::VertexInputRate::eVertex:
			return VK_VERTEX_INPUT_RATE_VERTEX;

		case renderer::VertexInputRate::eInstance:
			return VK_VERTEX_INPUT_RATE_INSTANCE;

		default:
			assert( false );
			return VK_VERTEX_INPUT_RATE_VERTEX;
		}
	}
}
