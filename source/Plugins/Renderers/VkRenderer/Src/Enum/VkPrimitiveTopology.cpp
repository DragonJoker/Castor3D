#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPrimitiveTopology convert( renderer::PrimitiveTopology const & topology )
	{
		switch ( topology )
		{
		case renderer::PrimitiveTopology::ePointList:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

		case renderer::PrimitiveTopology::eLineList:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

		case renderer::PrimitiveTopology::eLineStrip:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

		case renderer::PrimitiveTopology::eTriangleList:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		case renderer::PrimitiveTopology::eTriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

		case renderer::PrimitiveTopology::eTriangleFan:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

		case renderer::PrimitiveTopology::eLineListWithAdjacency:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;

		case renderer::PrimitiveTopology::eLineStripWithAdjacency:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;

		case renderer::PrimitiveTopology::eTriangleListWithAdjacency:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

		case renderer::PrimitiveTopology::eTriangleStripWithAdjacency:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

		case renderer::PrimitiveTopology::ePatchList:
			return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

		default:
			assert( false );
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}
}
