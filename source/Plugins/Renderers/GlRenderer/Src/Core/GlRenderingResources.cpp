#include "Core/GlRenderingResources.hpp"

#include "Command/GlCommandBuffer.hpp"
#include "Core/GlDevice.hpp"
#include "Sync/GlFence.hpp"
#include "Sync/GlSemaphore.hpp"

namespace gl_renderer
{
	RenderingResources::RenderingResources( renderer::Device const & device )
		: renderer::RenderingResources{ device }
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
