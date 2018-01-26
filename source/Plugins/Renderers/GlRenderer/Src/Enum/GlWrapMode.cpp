#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlWrapMode value )
	{
		switch ( value )
		{
		case gl_renderer::GL_WRAP_MODE_REPEAT:
			return "GL_REPEAT";

		case gl_renderer::GL_WRAP_MODE_MIRRORED_REPEAT:
			return "GL_MIRRORED_REPEAT";

		case gl_renderer::GL_WRAP_MODE_CLAMP_TO_EDGE:
			return "GL_CLAMP_TO_EDGE";

		case gl_renderer::GL_WRAP_MODE_CLAMP_TO_BORDER:
			return "GL_CLAMP_TO_BORDER";

		case gl_renderer::GL_WRAP_MODE_MIRROR_CLAMP_TO_EDGE:
			return "GL_CLAMP_TO_EDGE";

		default:
			assert( false && "Unsupported GlWrapMode" );
			return "GlWrapMode_UNKNOWN";
		}
	}

	GlWrapMode convert( renderer::WrapMode const & mode )
	{
		switch ( mode )
		{
		case renderer::WrapMode::eRepeat:
			return GL_WRAP_MODE_REPEAT;

		case renderer::WrapMode::eMirroredRepeat:
			return GL_WRAP_MODE_MIRRORED_REPEAT;

		case renderer::WrapMode::eClampToEdge:
			return GL_WRAP_MODE_CLAMP_TO_EDGE;

		case renderer::WrapMode::eClampToBorder:
			return GL_WRAP_MODE_CLAMP_TO_BORDER;

		case renderer::WrapMode::eMirrorClampToEdge:
			return GL_WRAP_MODE_MIRROR_CLAMP_TO_EDGE;

		default:
			assert( false );
			return GL_WRAP_MODE_REPEAT;
		}
	}
}
