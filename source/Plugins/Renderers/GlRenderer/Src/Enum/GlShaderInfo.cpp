#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlShaderInfo value )
	{
		switch ( value )
		{
		case gl_renderer::GL_INFO_COMPILE_STATUS:
			return "GL_COMPILE_STATUS";

		case gl_renderer::GL_INFO_LINK_STATUS:
			return "GL_LINK_STATUS";

		case gl_renderer::GL_INFO_LOG_LENGTH:
			return "GL_INFO_LOG_LENGTH";

		case gl_renderer::GL_INFO_ATTACHED_SHADERS:
			return "GL_ATTACHED_SHADERS";

		default:
			assert( false && "Unsupported GlShaderInfo" );
			return "GlShaderInfo_UNKNOWN";
		}
	}
}
