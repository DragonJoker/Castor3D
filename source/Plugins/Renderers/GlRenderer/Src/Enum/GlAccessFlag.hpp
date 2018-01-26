/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/AccessFlag.hpp>

namespace gl_renderer
{
	enum GlAccessType
		: GLenum
	{
		GL_ACCESS_TYPE_READ_ONLY = 0x88B8,
		GL_ACCESS_TYPE_WRITE_ONLY = 0x88B9,
		GL_ACCESS_TYPE_READ_WRITE = 0x88BA,
	};
	std::string getName( GlAccessType value );
	/**
	*\brief
	*	Convertit un renderer::AccessFlags en VkAccessFlags.
	*\param[in] flags
	*	Le renderer::AccessFlags.
	*\return
	*	Le VkAccessFlags.
	*/
	GLenum convert( renderer::AccessFlags const & flags );
	/**
	*\brief
	*	Convertit un VkAccessFlags en renderer::AccessFlags.
	*\param[in] flags
	*	Le VkAccessFlags.
	*\return
	*	Le renderer::AccessFlags.
	*/
	renderer::AccessFlags convertAccessFlags( GLenum flags );
}
