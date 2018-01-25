/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/MemoryMapFlag.hpp>

namespace gl_renderer
{
	enum GlMemoryMapFlag
		: GLuint
	{
		GL_MEMORY_MAP_READ_BIT = 0x0001,
		GL_MEMORY_MAP_WRITE_BIT = 0x0002,
		GL_MEMORY_MAP_INVALIDATE_RANGE_BIT = 0x0004,
		GL_MEMORY_MAP_INVALIDATE_BUFFER_BIT = 0x0008,
		GL_MEMORY_MAP_FLUSH_EXPLICIT_BIT = 0x0010,
		GL_MEMORY_MAP_UNSYNCHRONIZED_BIT = 0x0020,
		GL_MEMORY_MAP_PERSISTENT_BIT = 0x00000040,
		GL_MEMORY_MAP_COHERENT_BIT = 0x00000080,
	};
	Utils_ImplementFlag( GlMemoryMapFlag )
	/**
	*\brief
	*	Convertit un renderer::MemoryMapFlags en GlMemoryMapFlags.
	*\param[in] format
	*	Le renderer::MemoryMapFlags.
	*\return
	*	Le GlMemoryMapFlags.
	*/
	GlMemoryMapFlags convert( renderer::MemoryMapFlags const & flags );
}
