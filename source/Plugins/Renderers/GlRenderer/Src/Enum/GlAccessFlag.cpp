#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlAccessType value )
	{
		switch ( value )
		{
		case GL_ACCESS_TYPE_READ_ONLY:
			return "GL_ACCESS_TYPE_READ_ONLY";

		case GL_ACCESS_TYPE_WRITE_ONLY:
			return "GL_ACCESS_TYPE_WRITE_ONLY";

		case GL_ACCESS_TYPE_READ_WRITE:
			return "GL_ACCESS_TYPE_READ_WRITE";

		default:
			assert( false && "Unsupported GlAccessType" );
			return "GlAccessType_UNKNOWN";
		}
	}

	GLenum convert( renderer::AccessFlags const & flags )
	{
		GLenum result{ 0 };
		return result;
	}

	renderer::AccessFlags convertAccessFlags( GLenum flags )
	{
		renderer::AccessFlags result;
		return result;
	}
}
