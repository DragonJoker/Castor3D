#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlPrimitiveTopology convert( renderer::PrimitiveTopology const & topology )
	{
		switch ( topology )
		{
		case renderer::PrimitiveTopology::ePointList:
			return GL_PRIMITIVE_TOPOLOGY_POINT_LIST;

		case renderer::PrimitiveTopology::eLineList:
			return GL_PRIMITIVE_TOPOLOGY_LINE_LIST;

		case renderer::PrimitiveTopology::eLineStrip:
			return GL_PRIMITIVE_TOPOLOGY_LINE_STRIP;

		case renderer::PrimitiveTopology::eTriangleList:
			return GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		case renderer::PrimitiveTopology::eTriangleStrip:
			return GL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

		case renderer::PrimitiveTopology::eTriangleFan:
			return GL_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

		case renderer::PrimitiveTopology::eLineListWithAdjacency:
			return GL_PRIMITIVE_TOPOLOGY_LINE_LIST_ADJACENCY;

		case renderer::PrimitiveTopology::eLineStripWithAdjacency:
			return GL_PRIMITIVE_TOPOLOGY_LINE_STRIP_ADJACENCY;

		case renderer::PrimitiveTopology::eTriangleListWithAdjacency:
			return GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_ADJACENCY;

		case renderer::PrimitiveTopology::eTriangleStripWithAdjacency:
			return GL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_ADJACENCY;

		case renderer::PrimitiveTopology::ePatchList:
			return GL_PRIMITIVE_TOPOLOGY_PATCH_LIST;

		default:
			assert( false );
			return GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}
}
