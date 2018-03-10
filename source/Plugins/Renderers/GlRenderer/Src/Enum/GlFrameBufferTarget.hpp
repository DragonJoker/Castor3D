/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

namespace gl_renderer
{
	enum GlFrameBufferTarget
		: GLenum
	{
		GL_FRAMEBUFFER = 0x8D40,
		GL_READ_FRAMEBUFFER = 0x8CA8,
		GL_DRAW_FRAMEBUFFER = 0x8CA9,
	};
	std::string getName( GlFrameBufferTarget value );
}
