/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/CompareOp.hpp>

namespace gl_renderer
{
	enum GlCompareOp
		: GLenum
	{
		GL_COMPARE_OP_NEVER = 0x0200,
		GL_COMPARE_OP_LESS = 0x0201,
		GL_COMPARE_OP_EQUAL = 0x0202,
		GL_COMPARE_OP_LEQUAL = 0x0203,
		GL_COMPARE_OP_GREATER = 0x0204,
		GL_COMPARE_OP_NOTEQUAL = 0x0205,
		GL_COMPARE_OP_GEQUAL = 0x0206,
		GL_COMPARE_OP_ALWAYS = 0x0207,
	};
	/**
	*\brief
	*	Convertit un renderer::CompareOp en GlCompareOp.
	*\param[in] value
	*	Le renderer::CompareOp.
	*\return
	*	Le GlCompareOp.
	*/
	GlCompareOp convert( renderer::CompareOp const & value );
}
