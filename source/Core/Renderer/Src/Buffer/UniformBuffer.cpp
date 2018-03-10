/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "Buffer/UniformBuffer.hpp"

namespace renderer
{
	UniformBufferBase::UniformBufferBase( Device const & device
		, uint32_t count
		, uint32_t size
		, BufferTargets target
		, MemoryPropertyFlags flags )
		: m_device{ device }
		, m_count{ count }
		, m_size{ size }
	{
	}
}
