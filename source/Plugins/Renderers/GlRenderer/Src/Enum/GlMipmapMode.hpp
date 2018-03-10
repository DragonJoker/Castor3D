/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <Enum/MipmapMode.hpp>

namespace gl_renderer
{
	/**
	*\brief
	*	Convertit un renderer::MipmapMode en GlFilter.
	*\param[in] mode
	*	Le renderer::MipmapMode.
	*\return
	*	Le GlFilter.
	*/
	GLenum convert( renderer::MipmapMode const & mode );
}
