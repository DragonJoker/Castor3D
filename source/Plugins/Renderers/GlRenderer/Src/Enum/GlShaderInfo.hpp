/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlShaderInfo
	{
		GL_INFO_COMPILE_STATUS = 0x8B81,
		GL_INFO_LINK_STATUS = 0x8B82,
		GL_INFO_VALIDATE_STATUS = 0x8B83,
		GL_INFO_LOG_LENGTH = 0x8B84,
		GL_INFO_ATTACHED_SHADERS = 0x8B85,
	};
	std::string getName( GlShaderInfo value );
}
