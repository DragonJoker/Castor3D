/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#include "VkRendererPrerequisites.hpp"

#include "Buffer/VkBuffer.hpp"

namespace vk_renderer
{
	VkBufferMemoryBarrier convert( renderer::BufferMemoryBarrier const & barrier )
	{
		return VkBufferMemoryBarrier
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			convert( barrier.getSrcAccessMask() ),
			convert( barrier.getDstAccessMask() ),
			barrier.getSrcQueueFamilyIndex(),
			barrier.getDstQueueFamilyIndex(),
			static_cast< Buffer const & >( barrier.getBuffer() ),
			barrier.getOffset(),
			barrier.getSize()
		};
	}

	renderer::BufferMemoryBarrier convert( Buffer const & bufferBase, VkBufferMemoryBarrier const & barrier )
	{
		return renderer::BufferMemoryBarrier
		{
			convertAccessFlags( barrier.srcAccessMask ),
			convertAccessFlags( barrier.dstAccessMask ),
			barrier.srcQueueFamilyIndex,
			barrier.dstQueueFamilyIndex,
			bufferBase,
			barrier.offset,
			barrier.size
		};
	}
}
