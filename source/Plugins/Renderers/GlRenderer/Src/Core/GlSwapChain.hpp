/**
*\file
*	Renderer.h
*\author
*	Sylvain Doremus
*/
#pragma once

#include "GlRendererPrerequisites.hpp"
#include "Core/GlRenderingResources.hpp"

#include <Core/SwapChain.hpp>

namespace gl_renderer
{
	class SwapChain
		: public renderer::SwapChain
	{
	public:
		/**
		*\brief
		*	Constructeur.
		*/
		SwapChain( Device const & device
			, renderer::Extent2D const & size );
		/**
		*\copydoc	renderer::SwapChain::reset
		*/
		void reset( renderer::Extent2D const & size )override;
		/**
		*\copydoc	renderer::SwapChain::createFrameBuffers
		*/
		renderer::FrameBufferPtrArray createFrameBuffers( renderer::RenderPass const & renderPass )const override;
		/**
		*\copydoc	renderer::SwapChain::createCommandBuffers
		*/
		renderer::CommandBufferPtrArray createCommandBuffers()const override;
		/**
		*\copydoc	renderer::SwapChain::getResources
		*/
		renderer::RenderingResources * getResources()override;
		/**
		*\copydoc	renderer::SwapChain::present
		*/
		void present( renderer::RenderingResources & resources )override;
		/**
		*\copydoc	renderer::SwapChain::createDepthStencil
		*/
		void createDepthStencil( renderer::Format format )override;
		/**
		*\brief
		*	D�finit la couleur de vidage de la swapchain.
		*\param[in] value
		*	La nouvelle valeur.
		*/
		inline void setClearColour( renderer::RgbaColour const & value )override
		{
			m_clearColour = value;
		}
		/**
		*\brief
		*	D�finit la couleur de vidage de la swapchain.
		*\param[in] value
		*	La nouvelle valeur.
		*/
		inline renderer::RgbaColour getClearColour()const override
		{
			return m_clearColour;
		}
		/**
		*\return
		*	Les dimensions de la swap chain.
		*/
		inline renderer::Extent2D getDimensions()const override
		{
			return m_dimensions;
		}
		/**
		*\return
		*	Les format des pixels de la swap chain.
		*/
		inline renderer::Format getFormat()const override
		{
			return m_format;
		}

	private:
		void doResetSwapChain();
		void doCreateBackBuffers();
		renderer::FrameBufferAttachmentArray doPrepareAttaches( uint32_t backBuffer
			, renderer::AttachmentDescriptionArray const & attaches )const;

	private:
		Device const & m_device;
		renderer::RgbaColour m_clearColour;
		renderer::Format m_format;
	};
}
