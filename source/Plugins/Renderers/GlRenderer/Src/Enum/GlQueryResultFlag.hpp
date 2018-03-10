/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/QueryResultFlag.hpp>

namespace gl_renderer
{
	enum GlQueryResultFlag
	{
		GL_QUERY_RESULT = 0x8866,
		GL_QUERY_RESULT_NO_WAIT = 0x9194,
	};
	Utils_ImplementFlag( GlQueryResultFlag );
	std::string getName( GlQueryResultFlags value );
	/**
	*\brief
	*	Convertit un renderer::QueryResultFlags en GLuint.
	*\param[in] flags
	*	Le renderer::QueryResultFlags.
	*\return
	*	Le GLuint.
	*/
	GLuint convert( renderer::QueryResultFlags const & flags );
}
