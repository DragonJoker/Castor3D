/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlClearTarget
		: GLenum
	{
		GL_CLEAR_TARGET_COLOR = 0x1800,
		GL_CLEAR_TARGET_DEPTH = 0x1801,
		GL_CLEAR_TARGET_STENCIL = 0x1802,
		GL_CLEAR_TARGET_DEPTH_STENCIL = 0x84F9,
	};
	std::string getName( GlClearTarget value );
}
