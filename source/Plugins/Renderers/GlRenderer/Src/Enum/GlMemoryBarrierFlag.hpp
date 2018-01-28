/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Enum/PipelineStageFlag.hpp>

namespace gl_renderer
{
	enum GlMemoryBarrierFlag
		: GLbitfield
	{
		GL_MEMORY_BARRIER_VERTEX_ATTRIB_ARRAY = 0x00000001,
		GL_MEMORY_BARRIER_ELEMENT_ARRAY = 0x00000002,
		GL_MEMORY_BARRIER_UNIFORM = 0x00000004,
		GL_MEMORY_BARRIER_TEXTURE_FETCH = 0x00000008,
		GL_MEMORY_BARRIER_SHADER_IMAGE_ACCESS = 0x00000020,
		GL_MEMORY_BARRIER_COMMAND = 0x00000040,
		GL_MEMORY_BARRIER_PIXEL_BUFFER = 0x00000080,
		GL_MEMORY_BARRIER_TEXTURE_UPDATE = 0x00000100,
		GL_MEMORY_BARRIER_BUFFER_UPDATE = 0x00000200,
		GL_MEMORY_BARRIER_FRAMEBUFFER = 0x00000400,
		GL_MEMORY_BARRIER_TRANSFORM_FEEDBACK = 0x00000800,
		GL_MEMORY_BARRIER_ATOMIC_COUNTER = 0x00001000,
		GL_MEMORY_BARRIER_SHADER_STORAGE = 0x00002000,
		GL_MEMORY_BARRIER_CLIENT_MAPPED_BUFFER = 0x00004000,
		GL_MEMORY_BARRIER_QUERY_BUFFER = 0x00008000,
		GL_MEMORY_BARRIER_ALL = 0x0000FFFF
	};
	Utils_ImplementFlag( GlMemoryBarrierFlag );
	std::string getName( GlMemoryBarrierFlags value );
	/**
	*\brief
	*	Convertit un renderer::PipelineStageFlags en GlMemoryBarrierFlags.
	*\param[in] flags
	*	Le renderer::PipelineStageFlags.
	*\return
	*	Le GlMemoryBarrierFlags.
	*/
	GlMemoryBarrierFlags convert( renderer::PipelineStageFlags const & flags );
}
