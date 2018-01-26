/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/QueryType.hpp>

namespace gl_renderer
{
	enum GlQueryType
		: GLenum
	{
		GL_QUERY_TYPE_SAMPLES_PASSED = 0x8914,
		GL_QUERY_TYPE_PRIMITIVES_GENERATED = 0x8C87,
		GL_QUERY_TYPE_TIMESTAMP = 0x8E28,
	};
	std::string getName( GlQueryType value );
	/**
	*\brief
	*	Convertit un renderer::QueryType en GlQueryType.
	*\param[in] value
	*	Le renderer::QueryType.
	*\return
	*	Le GlQueryType.
	*/
	GlQueryType convert( renderer::QueryType const & value );
}
