#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlTweak value )
	{
		switch ( value )
		{
		case gl_renderer::GL_CULL_FACE:
			return "GL_CULL_FACE";

		case gl_renderer::GL_DEPTH_TEST:
			return "GL_DEPTH_TEST";

		case gl_renderer::GL_STENCIL_TEST:
			return "GL_STENCIL_TEST";

		case gl_renderer::GL_BLEND:
			return "GL_BLEND";

		case gl_renderer::GL_POLYGON_OFFSET_POINT:
			return "GL_POLYGON_OFFSET_POINT";

		case gl_renderer::GL_POLYGON_OFFSET_LINE:
			return "GL_POLYGON_OFFSET_LINE";

		case gl_renderer::GL_MULTISAMPLE:
			return "GL_MULTISAMPLE";

		case gl_renderer::GL_SAMPLE_ALPHA_TO_COVERAGE:
			return "GL_SAMPLE_ALPHA_TO_COVERAGE";

		case gl_renderer::GL_SAMPLE_ALPHA_TO_ONE:
			return "GL_SAMPLE_ALPHA_TO_ONE";

		case gl_renderer::GL_POLYGON_OFFSET_FILL:
			return "GL_POLYGON_OFFSET_FILL";

		case gl_renderer::GL_DEPTH_CLAMP:
			return "GL_DEPTH_CLAMP";

		case gl_renderer::GL_RASTERIZER_DISCARD:
			return "GL_RASTERIZER_DISCARD";

		default:
			assert( false && "Unsupported GlTweak" );
			return "GlTweak_UNKNOWN";
		}
	}
}
