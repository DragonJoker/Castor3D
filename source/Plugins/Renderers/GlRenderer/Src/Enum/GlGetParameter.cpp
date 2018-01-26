#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlGetParameter value )
	{
		switch ( value )
		{
		case gl_renderer::GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT:
			return "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT";

		default:
			assert( false && "Unsupported GlGetParameter" );
			return "GlGetParameter_UNKNOWN";
		}
	}
}
