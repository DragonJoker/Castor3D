#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlClearTarget value )
	{
		switch ( value )
		{
		case gl_renderer::GL_CLEAR_TARGET_COLOR:
			return "GL_COLOR";

		case gl_renderer::GL_CLEAR_TARGET_DEPTH:
			return " GL_DEPTH";

		case gl_renderer::GL_CLEAR_TARGET_STENCIL:
			return "GL_STENCIL";

		case gl_renderer::GL_CLEAR_TARGET_DEPTH_STENCIL:
			return "GL_DEPTH_STENCIL";

		default:
			assert( false && "Unsupported GlClearTarget" );
			return "GlClearTarget_UNKNOWN";
		}
	}
}
