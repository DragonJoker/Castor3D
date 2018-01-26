#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlClipOrigin value )
	{
		switch ( value )
		{
		case gl_renderer::GL_LOWER_LEFT:
			return "GL_LOWER_LEFT";

		case gl_renderer::GL_UPPER_LEFT:
			return "GL_UPPER_LEFT";

		default:
			assert( false && "Unsupported GlClipOrigin" );
			return "GlClipOrigin_UNKNOWN";
		}
	}

	std::string getName( GlClipDepth value )
	{
		switch ( value )
		{
		case gl_renderer::GL_NEGATIVE_ONE_TO_ONE:
			return "GL_NEGATIVE_ONE_TO_ONE";

		case gl_renderer::GL_ZERO_TO_ONE:
			return "GL_ZERO_TO_ONE";

		default:
			assert( false && "Unsupported GlClipDepth" );
			return "GlClipDepth_UNKNOWN";
		}
	}
}
