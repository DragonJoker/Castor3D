#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlFrameBufferTarget value )
	{
		switch ( value )
		{
		case gl_renderer::GL_FRAMEBUFFER:
			return "GL_FRAMEBUFFER";

		case gl_renderer::GL_READ_FRAMEBUFFER:
			return "GL_READ_FRAMEBUFFER";

		case gl_renderer::GL_DRAW_FRAMEBUFFER:
			return "GL_DRAW_FRAMEBUFFER";

		default:
			assert( false && "Unsupported GlFrameBufferTarget" );
			return "GlFrameBufferTarget_UNKNOWN";
		}
	}
}
