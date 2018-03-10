/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/ComponentSwizzle.hpp>

namespace gl_renderer
{
	/**
	*\~english
	*\brief
	*	Specifies how an image component is swizzled.
	*\~french
	*\brief
	*	Définit comment une composante d'une image est réarrangée.
	*/
	enum GlSwizzle
		: GLenum
	{
		GL_SWIZZLE_R = 0x8E42,
		GL_SWIZZLE_G = 0x8E43,
		GL_SWIZZLE_B = 0x8E44,
		GL_SWIZZLE_A = 0x8E45,
	};
	std::string getName( GlSwizzle value );
	/**
	*\~english
	*\brief
	*	Specifies how an image component is swizzled.
	*\~french
	*\brief
	*	Définit comment une composante d'une image est réarrangée.
	*/
	enum GlComponentSwizzle
		: GLenum
	{
		GL_COMPONENT_SWIZZLE_IDENTITTY = ~GLenum( 0 ),
		GL_COMPONENT_SWIZZLE_ZERO = 0,
		GL_COMPONENT_SWIZZLE_ONE = 1,
		GL_COMPONENT_SWIZZLE_RED = 0x1903,
		GL_COMPONENT_SWIZZLE_GREEN = 0x1904,
		GL_COMPONENT_SWIZZLE_BLUE = 0x1905,
		GL_COMPONENT_SWIZZLE_ALPHA = 0x1906,
	};
	std::string getName( GlComponentSwizzle value );
	/**
	*\brief
	*	Convertit un renderer::CompareOp en GlComponentSwizzle.
	*\param[in] value
	*	Le renderer::CompareOp.
	*\return
	*	Le GlComponentSwizzle.
	*/
	GlComponentSwizzle convert( renderer::ComponentSwizzle const & value );
}
