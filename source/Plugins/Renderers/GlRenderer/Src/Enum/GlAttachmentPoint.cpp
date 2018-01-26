#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlAttachmentPoint value )
	{
		switch ( value )
		{
		case gl_renderer::GL_ATTACHMENT_POINT_DEPTH_STENCIL:
			return "GL_DEPTH_STENCIL_ATTACHMENT";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR0:
			return "GL_COLOR_ATTACHMENT0";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR1:
			return "GL_COLOR_ATTACHMENT1";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR2:
			return "GL_COLOR_ATTACHMENT2";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR3:
			return "GL_COLOR_ATTACHMENT3";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR4:
			return "GL_COLOR_ATTACHMENT4";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR5:
			return "GL_COLOR_ATTACHMENT5";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR6:
			return "GL_COLOR_ATTACHMENT6";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR7:
			return "GL_COLOR_ATTACHMENT7";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR8:
			return "GL_COLOR_ATTACHMENT8";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR9:
			return "GL_COLOR_ATTACHMENT9";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR10:
			return "GL_COLOR_ATTACHMENT10";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR11:
			return "GL_COLOR_ATTACHMENT11";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR12:
			return "GL_COLOR_ATTACHMENT12";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR13:
			return "GL_COLOR_ATTACHMENT13";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR14:
			return "GL_COLOR_ATTACHMENT14";

		case gl_renderer::GL_ATTACHMENT_POINT_COLOR15:
			return "GL_COLOR_ATTACHMENT15";

		case gl_renderer::GL_ATTACHMENT_POINT_DEPTH:
			return "GL_DEPTH_ATTACHMENT";

		case gl_renderer::GL_ATTACHMENT_POINT_STENCIL:
			return "GL_STENCIL_ATTACHMENT";

		default:
			assert( false && "Unsupported GlAttachmentPoint" );
			return "GlAttachmentPoint_UNKNOWN";
		}
	}
}
