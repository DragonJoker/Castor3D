#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlFenceWaitFlag value )
	{
		switch ( value )
		{
		case gl_renderer::GL_WAIT_FLAG_SYNC_GPU_COMMANDS_COMPLETE:
			return "GL_SYNC_GPU_COMMANDS_COMPLETE";

		case gl_renderer::GL_WAIT_FLAG_SYNC_FLUSH_COMMANDS_BIT:
			return "GL_SYNC_FLUSH_COMMANDS_BIT";

		default:
			assert( false && "Unsupported GlFenceWaitFlag" );
			return "GlFenceWaitFlag_UNKNOWN";
		}
	}
}
