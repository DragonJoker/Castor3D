/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlFenceWaitFlag
	{
		GL_WAIT_FLAG_SYNC_GPU_COMMANDS_COMPLETE = 0x9117,
		GL_WAIT_FLAG_SYNC_FLUSH_COMMANDS_BIT = 0x00000001,
	};
	std::string getName( GlFenceWaitFlag value );
}
