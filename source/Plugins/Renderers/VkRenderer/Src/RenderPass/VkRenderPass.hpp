/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <RenderPass/RenderPass.hpp>
#include <RenderPass/AttachmentDescription.hpp>
#include <RenderPass/RenderSubpassState.hpp>

namespace vk_renderer
{
	class RenderPass
		: public renderer::RenderPass
	{
	public:
		RenderPass( Device const & device
			, renderer::RenderPassCreateInfo && createInfo );
		~RenderPass();
		/**
		*\copydoc	renderer::RenderPass::createFrameBuffer
		*/
		renderer::FrameBufferPtr createFrameBuffer( renderer::Extent2D const & dimensions
			, renderer::FrameBufferAttachmentArray && textures )const override;
		std::vector< VkClearValue > const & getClearValues( renderer::ClearValueArray const & clearValues )const;
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkRenderPass.
		*\~english
		*\brief
		*	VkRenderPass implicit cast operator.
		*/
		inline operator VkRenderPass const &( )const
		{
			return m_renderPass;
		}

	private:
		struct Subpass
		{
			std::vector< VkAttachmentReference > inputAttachments;
			std::vector< VkAttachmentReference > colorAttachments;
			std::vector< VkAttachmentReference > resolveAttachments;
			VkAttachmentReference depthStencilAttachment;
		};

	private:
		Device const & m_device;
		VkRenderPass m_renderPass{};
		std::vector< Subpass > m_subpassInfos;
		std::vector< VkSubpassDescription > m_subpasses;
		std::vector< VkSubpassDependency > m_dependencies;
		mutable std::vector< VkClearValue > m_clearValues;
	};
}
