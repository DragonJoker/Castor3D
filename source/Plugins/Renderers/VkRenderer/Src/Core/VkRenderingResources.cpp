#include "Core/VkRenderingResources.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Sync/VkFence.hpp"
#include "Sync/VkSemaphore.hpp"

namespace vk_renderer
{
	RenderingResources::RenderingResources( Device const & device )
		: renderer::RenderingResources{ device }
		, m_device{ device }
	{
		m_commandBuffer = m_device.getGraphicsCommandPool().createCommandBuffer();
		m_imageAvailableSemaphore = std::make_unique< Semaphore >( m_device );
		m_finishedRenderingSemaphore = std::make_unique< Semaphore >( m_device );
		m_fence = std::make_unique< Fence >( m_device, renderer::FenceCreateFlag::eSignaled );
	}

	bool RenderingResources::waitRecord( uint32_t timeout )
	{
		bool res = m_fence->wait( timeout ) == renderer::WaitResult::eSuccess;

		if ( res )
		{
			m_fence->reset();
		}

		return res;
	}
}
