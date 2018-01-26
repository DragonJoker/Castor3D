/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/BufferTarget.hpp>

namespace gl_renderer
{
	enum GlBufferTarget
		: GLenum
	{
		GL_BUFFER_TARGET_ARRAY = 0x8892,
		GL_BUFFER_TARGET_ELEMENT_ARRAY = 0x8893,
		GL_BUFFER_TARGET_PIXEL_PACK = 0x88EB,
		GL_BUFFER_TARGET_PIXEL_UNPACK = 0x88EC,
		GL_BUFFER_TARGET_UNIFORM = 0x8A11,
		GL_BUFFER_TARGET_TEXTURE = 0x8C2A,
		GL_BUFFER_TARGET_COPY_READ = 0x8F36,
		GL_BUFFER_TARGET_COPY_WRITE = 0x8F37,
		GL_BUFFER_TARGET_DRAW_INDIRECT = 0x8F3F,
		GL_BUFFER_TARGET_SHADER_STORAGE = 0x90D2,
		GL_BUFFER_TARGET_DISPATCH_INDIRECT = 0x90EE,
	};
	std::string getName( GlBufferTarget value );

	/**
	*\brief
	*	Convertit un renderer::BufferTargets en GlBufferTarget.
	*\param[in] targets
	*	Le renderer::BufferTargets.
	*\return
	*	Le GlBufferTarget.
	*/
	GlBufferTarget convert( renderer::BufferTargets const & targets );
}
