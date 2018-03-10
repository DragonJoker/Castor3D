/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/PolygonMode.hpp>

namespace gl_renderer
{
	enum GlPolygonMode
		: GLenum
	{
		GL_POLYGON_MODE_POINT = 0x1B00,
		GL_POLYGON_MODE_LINE = 0x1B01,
		GL_POLYGON_MODE_FILL = 0x1B02,
	};
	std::string getName( GlPolygonMode value );
	/**
	*\brief
	*	Convertit un renderer::PolygonMode en GlPolygonMode.
	*\param[in] value
	*	Le renderer::PolygonMode.
	*\return
	*	Le GlPolygonMode.
	*/
	GlPolygonMode convert( renderer::PolygonMode const & value );
}
