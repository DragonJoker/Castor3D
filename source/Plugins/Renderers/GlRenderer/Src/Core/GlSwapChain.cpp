#include "Core/GlSwapChain.hpp"

#include "Command/GlCommandBuffer.hpp"
#include "Core/GlBackBuffer.hpp"
#include "Core/GlDevice.hpp"
#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "Sync/GlSemaphore.hpp"
#include "Image/GlTexture.hpp"
#include "Image/GlTextureView.hpp"

#include <iostream>

namespace gl_renderer
{
	SwapChain::SwapChain( Device const & device
		, renderer::Extent2D const & size )
		: renderer::SwapChain{ device, size }
		, m_device{ device }
	{
		m_format = renderer::Format::eR8G8B8A8_UNORM;
		m_renderingResources.emplace_back( std::make_unique< RenderingResources >( device ) );
		doCreateBackBuffers();
	}

	void SwapChain::reset( renderer::Extent2D const & size )
	{
		m_dimensions = size;
		doResetSwapChain();
	}

	renderer::FrameBufferPtrArray SwapChain::createFrameBuffers( renderer::RenderPass const & renderPass )const
	{
		renderer::FrameBufferPtrArray result;
		result.emplace_back( std::make_unique< FrameBuffer >( renderPass, m_dimensions ) );
		return result;
	}

	renderer::CommandBufferPtrArray SwapChain::createCommandBuffers()const
	{
		renderer::CommandBufferPtrArray result;
		result.emplace_back( std::make_unique< CommandBuffer >( static_cast< Device const & >( m_device )
			, m_device.getGraphicsCommandPool()
			, true ) );
		return result;
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

	void SwapChain::createDepthStencil( renderer::Format format )
	{
		auto texture = std::make_unique< Texture >( m_device
			, format
			, m_dimensions );
		m_depthStencilView = std::make_unique< TextureView >( m_device
			, *texture );
		m_depthStencil = std::move( texture );
	}

	void SwapChain::doResetSwapChain()
	{
		m_device.waitIdle();
		doCreateBackBuffers();
		onReset();
	}

	void SwapChain::doCreateBackBuffers()
	{
		m_backBuffers.clear();
		auto texture = std::make_unique< Texture >( m_device
			, m_format
			, m_dimensions );
		auto view = std::make_unique< TextureView >( m_device
			, *texture );
		m_backBuffers.emplace_back( std::make_unique< BackBuffer >( m_device
			, std::move( texture )
			, std::move( view )
			, 0u ) );
	}
}
