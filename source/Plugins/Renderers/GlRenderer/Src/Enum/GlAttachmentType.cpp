#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlAttachmentType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_ATTACHMENT_TYPE_COLOR:
			return "GL_COLOR";

		case gl_renderer::GL_ATTACHMENT_TYPE_DEPTH:
			return "GL_DEPTH";

		case gl_renderer::GL_ATTACHMENT_TYPE_STENCIL:
			return "GL_STENCIL";

		case gl_renderer::GL_ATTACHMENT_TYPE_DEPTH_STENCIL:
			return "GL_DEPTH_STENCIL";

		default:
			assert( false && "Unsupported GlAttachmentType" );
			return "GlAttachmentType_UNKNOWN";
		}
	}
}
