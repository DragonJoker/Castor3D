#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlBaseType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_BYTE:
			return "GL_BYTE";

		case gl_renderer::GL_UNSIGNED_BYTE:
			return "GL_UNSIGNED_BYTE";

		case gl_renderer::GL_SHORT:
			return "GL_SHORT";

		case gl_renderer::GL_UNSIGNED_SHORT:
			return "GL_UNSIGNED_SHORT";

		case gl_renderer::GL_INT:
			return "GL_INT";

		case gl_renderer::GL_UNSIGNED_INT:
			return "GL_UNSIGNED_INT";

		case gl_renderer::GL_FLOAT:
			return "GL_FLOAT";

		default:
			assert( false && "Unsupported GlBaseType" );
			return "GlBaseType_UNKNOWN";
		}
	}
}
