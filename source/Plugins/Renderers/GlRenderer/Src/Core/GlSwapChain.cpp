#include "Core/GlSwapChain.hpp"

#include "Command/GlCommandBuffer.hpp"
#include "Core/GlDevice.hpp"
#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "Sync/GlSemaphore.hpp"
#include "Image/GlTexture.hpp"

#include <iostream>

namespace gl_renderer
{
	SwapChain::SwapChain( renderer::Device const & device
		, renderer::UIVec2 const & size )
		: renderer::SwapChain{ device, size }
	{
		m_renderingResources.emplace_back( std::make_unique< RenderingResources >( device ) );
	}

	void SwapChain::reset( renderer::UIVec2 const & size )
	{
		m_dimensions = size;
		doResetSwapChain();
	}

	renderer::FrameBufferPtrArray SwapChain::createFrameBuffers( renderer::RenderPass const & renderPass )const
	{
		return renderer::FrameBufferPtrArray
		{
			std::make_shared< FrameBuffer >( renderPass, m_dimensions )
		};
	}

	renderer::CommandBufferPtrArray SwapChain::createCommandBuffers()const
	{
		renderer::CommandBufferPtrArray result;
		result.emplace_back( std::make_unique< CommandBuffer >( static_cast< Device const & >( m_device )
			, m_device.getGraphicsCommandPool()
			, true ) );
		return result;
	}

	void SwapChain::preRenderCommands( uint32_t index
		, renderer::CommandBuffer const & commandBuffer )const
	{
	}

	void SwapChain::postRenderCommands( uint32_t index
		, renderer::CommandBuffer const & commandBuffer )const
	{
	}

	renderer::RenderingResources * SwapChain::getResources()
	{
		auto & resources = *m_renderingResources[m_resourceIndex];

		if ( resources.waitRecord( renderer::FenceTimeout ) )
		{
			uint32_t backBuffer{ 0u };
			resources.setBackBuffer( backBuffer );
			return &resources;
		}

		std::cerr << "Can't render" << std::endl;
		return nullptr;
	}

	void SwapChain::present( renderer::RenderingResources & resources )
	{
		static_cast< Device const & >( m_device ).swapBuffers();
		resources.setBackBuffer( ~0u );
	}

	void SwapChain::doResetSwapChain()
	{
		m_device.waitIdle();
		onReset();
	}
}
