/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/MemoryPropertyFlag.hpp>

namespace gl_renderer
{
	enum GlMemoryPropertyFlag
		: GLuint
	{
		GL_MEMORY_PROPERTY_READ_BIT = 0x0001,
		GL_MEMORY_PROPERTY_WRITE_BIT = 0x0002,
		GL_MEMORY_PROPERTY_PERSISTENT_BIT = 0x00000040,
		GL_MEMORY_PROPERTY_COHERENT_BIT = 0x00000080,
		GL_MEMORY_PROPERTY_DYNAMIC_STORAGE_BIT = 0x0100,
	};
	Utils_ImplementFlag( GlMemoryPropertyFlag )
	/**
	*\brief
	*	Convertit un renderer::MemoryPropertyFlags en GlMemoryPropertyFlags.
	*\param[in] format
	*	Le renderer::MemoryPropertyFlags.
	*\return
	*	Le GlMemoryPropertyFlags.
	*/
	GlMemoryPropertyFlags convert( renderer::MemoryPropertyFlags const & flags );
}
