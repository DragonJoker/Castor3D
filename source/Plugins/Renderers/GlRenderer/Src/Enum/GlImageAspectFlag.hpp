/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/ImageAspectFlag.hpp>

namespace gl_renderer
{
	enum GlImageAspectFlag
		: GLenum
	{
		GL_DEPTH_BUFFER_BIT = 0x00000100,
		GL_STENCIL_BUFFER_BIT = 0x00000400,
		GL_COLOR_BUFFER_BIT = 0x00004000,
	};
	Utils_ImplementFlag( GlImageAspectFlag );
	/**
	*\brief
	*	Convertit un renderer::ImageAspectFlags en GlImageAspectFlags.
	*\param[in] flags
	*	Le renderer::ImageAspectFlags.
	*\return
	*	Le GlImageAspectFlags.
	*/
	GlImageAspectFlags convert( renderer::ImageAspectFlags const & flags );
}
