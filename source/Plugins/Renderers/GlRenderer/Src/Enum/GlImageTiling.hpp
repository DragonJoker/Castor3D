/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/ImageTiling.hpp>

namespace gl_renderer
{
	enum GlImageTiling
	{
		GL_OPTIMAL_TILING_EXT = 0x9584,
		GL_LINEAR_TILING_EXT = 0x9585,
	};
	std::string getName( GlImageTiling value );
	/**
	*\brief
	*	Convertit un renderer::ImageTiling en GlImageTiling.
	*\param[in] value
	*	Le renderer::ImageTiling.
	*\return
	*	Le GlImageTiling.
	*/
	GlImageTiling convert( renderer::ImageTiling const & value );
}
