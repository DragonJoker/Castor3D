/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/BlendOp.hpp>

namespace gl_renderer
{
	enum GlBlendOp
		: GLenum
	{
		GL_BLEND_OP_ADD = 0x8006,
		GL_BLEND_OP_MIN = 0x8007,
		GL_BLEND_OP_MAX = 0x8008,
		GL_BLEND_OP_SUBTRACT = 0x800A,
		GL_BLEND_OP_REVERSE_SUBTRACT = 0x800B,
	};
	/**
	*\brief
	*	Convertit un renderer::BlendOp en GlBlendOp.
	*\param[in] value
	*	Le renderer::BlendOp.
	*\return
	*	Le GlBlendOp.
	*/
	GlBlendOp convert( renderer::BlendOp const & value );
}
