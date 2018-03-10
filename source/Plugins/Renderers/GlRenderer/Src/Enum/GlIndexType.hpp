/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <Enum/IndexType.hpp>

namespace gl_renderer
{
	enum GlIndexType
		: GLenum
	{
		GL_INDEX_TYPE_UINT16 = 0x1403,
		GL_INDEX_TYPE_UINT32 = 0x1405,
	};
	std::string getName( GlIndexType value );
	/**
	*\brief
	*	Convertit un renderer::IndexType en GlIndexType.
	*\param[in] type
	*	Le renderer::IndexType.
	*\return
	*	Le GlIndexType.
	*/
	GlIndexType convert( renderer::IndexType const & type );
}
