#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlPolygonMode convert( renderer::PolygonMode const & value )
	{
		switch ( value )
		{
		case renderer::PolygonMode::eFill:
			return GL_POLYGON_MODE_FILL;

		case renderer::PolygonMode::eLine:
			return GL_POLYGON_MODE_LINE;

		case renderer::PolygonMode::ePoint:
			return GL_POLYGON_MODE_POINT;

		default:
			assert( false && "Unsupported polygon mode" );
			return GL_POLYGON_MODE_FILL;
		}
	}
}
