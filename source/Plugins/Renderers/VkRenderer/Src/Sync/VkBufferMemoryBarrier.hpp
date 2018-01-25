/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Sync/BufferMemoryBarrier.hpp>

namespace vk_renderer
{
	class Buffer;
	/**
	*\brief
	*	Convertit un renderer::BufferMemoryBarrier en VkBufferMemoryBarrier.
	*\param[in] flags
	*	Le renderer::BufferMemoryBarrier.
	*\return
	*	Le VkBufferMemoryBarrier.
	*/
	VkBufferMemoryBarrier convert( renderer::BufferMemoryBarrier const & barrier );
	/**
	*\brief
	*	Convertit un VkBufferMemoryBarrier en renderer::BufferMemoryBarrier.
	*\param[in] flags
	*	Le VkBufferMemoryBarrier.
	*\return
	*	Le renderer::BufferMemoryBarrier.
	*/
	renderer::BufferMemoryBarrier convert( Buffer const & bufferBase, VkBufferMemoryBarrier const & barrier );
}
