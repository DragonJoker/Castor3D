/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/VkRenderPass.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"
#include "RenderPass/VkFrameBuffer.hpp"

#include <RenderPass/RenderSubpassState.hpp>

#include <algorithm>

namespace vk_renderer
{
	RenderPass::RenderPass( Device const & device
		, renderer::RenderPassCreateInfo && createInfo )
		: renderer::RenderPass{ device, createInfo }
		, m_device{ device }
		, m_dependencies{ convert< VkSubpassDependency >( createInfo.dependencies ) }
	{
		m_subpassInfos.reserve( createInfo.subpasses.size() );

		for ( auto & subpass : createInfo.subpasses )
		{
			m_subpassInfos.push_back(
			{
				convert< VkAttachmentReference >( subpass.inputAttachments ),
				convert< VkAttachmentReference >( subpass.colorAttachments ),
				convert< VkAttachmentReference >( subpass.resolveAttachments ),
				bool( subpass.depthStencilAttachment )
					? convert( subpass.depthStencilAttachment.value() )
					: VkAttachmentReference{ VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED }
			} );
			auto & info = m_subpassInfos.back();
			m_subpasses.push_back( convert( subpass
				, info.inputAttachments
				, info.colorAttachments
				, info.resolveAttachments
				, info.depthStencilAttachment ) );
		}

		auto attachments = convert< VkAttachmentDescription >( createInfo.attachments );
		auto renderPassInfo = convert( createInfo
			, attachments
			, m_subpasses
			, m_dependencies );
		DEBUG_DUMP( renderPassInfo );

		auto res = m_device.vkCreateRenderPass( m_device, &renderPassInfo, nullptr, &m_renderPass );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Render pass creation failed: " + getLastError() };
		}
	}

	RenderPass::~RenderPass()
	{
		m_device.vkDestroyRenderPass( m_device, m_renderPass, nullptr );
	}

	renderer::FrameBufferPtr RenderPass::createFrameBuffer( renderer::Extent2D const & dimensions
		, renderer::FrameBufferAttachmentArray && attaches )const
	{
		return std::make_unique< FrameBuffer >( m_device
			, *this
			, dimensions
			, std::move( attaches ) );
	}

	std::vector< VkClearValue > const & RenderPass::getClearValues (renderer::ClearValueArray const & clearValues)const
	{
		m_clearValues = convert< VkClearValue >( clearValues );
		return m_clearValues;
	}
}
