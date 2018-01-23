/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Command/CommandPool.hpp"

#include "Command/CommandBuffer.hpp"

namespace renderer
{
	CommandPool::CommandPool( Device const & device
		, uint32_t queueFamilyIndex
		, CommandPoolCreateFlags flags )
		: m_device{ device }
	{
	}
}
