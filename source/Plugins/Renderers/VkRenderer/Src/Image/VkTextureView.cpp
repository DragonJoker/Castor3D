#include "Image/VkTextureView.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Core/VkDevice.hpp"
#include "Sync/VkImageMemoryBarrier.hpp"
#include "Image/VkImageSubresourceRange.hpp"
#include "Core/VkRenderingResources.hpp"
#include "Image/VkTexture.hpp"

namespace vk_renderer
{
	TextureView::TextureView( Device const & device
		, Texture const & image
		, renderer::TextureType type
		, renderer::PixelFormat format
		, uint32_t baseMipLevel
		, uint32_t levelCount
		, uint32_t baseArrayLayer
		, uint32_t layerCount
		, renderer::ImageLayout initialLayout
		, renderer::AccessFlags initialAccessMask )
		: renderer::TextureView{ device, image, type, format, baseMipLevel, levelCount, baseArrayLayer, layerCount }
		, m_device{ device }
		, m_currentLayout{ initialLayout }
		, m_currentAccessMask{ initialAccessMask }
	{
		VkImageViewCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			nullptr,
			0,                                              // flags
			static_cast< Texture const & >( getTexture() ), // image
			convert( type ),                                // viewType
			convert( getFormat() ),                         // format
			{                                               // components
				VK_COMPONENT_SWIZZLE_IDENTITY,                  // r
				VK_COMPONENT_SWIZZLE_IDENTITY,                  // g
				VK_COMPONENT_SWIZZLE_IDENTITY,                  // b
				VK_COMPONENT_SWIZZLE_IDENTITY                   // a
			},
			convert( getSubResourceRange() )
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateImageView( m_device
			, &createInfo
			, nullptr
			, &m_view );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image view creation failed: " + getLastError() };
		}
	}

	TextureView::~TextureView()
	{
		if ( m_view )
		{
			vk::DestroyImageView( m_device
				, m_view
				, nullptr );
		}
	}

	renderer::ImageMemoryBarrier TextureView::makeGeneralLayout( renderer::AccessFlags accessFlags )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eGeneral
			, VK_QUEUE_FAMILY_IGNORED
			, accessFlags );
	}

	renderer::ImageMemoryBarrier TextureView::makeTransferDestination()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferDstOptimal
			, VK_QUEUE_FAMILY_IGNORED
			, renderer::AccessFlag::eTransferWrite );
	}

	renderer::ImageMemoryBarrier TextureView::makeTransferSource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferSrcOptimal
			, VK_QUEUE_FAMILY_IGNORED
			, renderer::AccessFlag::eTransferRead );
	}

	renderer::ImageMemoryBarrier TextureView::makeShaderInputResource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eShaderReadOnlyOptimal
			, VK_QUEUE_FAMILY_IGNORED
			, renderer::AccessFlag::eShaderRead );
	}

	renderer::ImageMemoryBarrier TextureView::makeDepthStencilReadOnly()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilReadOnlyOptimal
			, m_device.getGraphicsQueue().getFamilyIndex()
			, renderer::AccessFlag::eShaderRead );
	}

	renderer::ImageMemoryBarrier TextureView::makeColourAttachment()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eColourAttachmentOptimal
			, m_device.getGraphicsQueue().getFamilyIndex()
			, renderer::AccessFlag::eColourAttachmentWrite );
	}

	renderer::ImageMemoryBarrier TextureView::makeDepthStencilAttachment()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilAttachmentOptimal
			, m_device.getGraphicsQueue().getFamilyIndex()
			, renderer::AccessFlag::eDepthStencilAttachmentWrite );
	}

	renderer::ImageMemoryBarrier TextureView::makePresentSource()const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::ePresentSrc
			, m_device.getPresentQueue().getFamilyIndex()
			, renderer::AccessFlag::eMemoryRead );
	}

	renderer::ImageMemoryBarrier TextureView::doMakeLayoutTransition( renderer::ImageLayout layout
		, uint32_t queueFamily
		, renderer::AccessFlags dstAccessMask )const
	{
		// On fait passer le layout de l'image à un autre layout, via une barrière.
		renderer::ImageMemoryBarrier transitionBarrier
		{
			m_currentAccessMask,                     // srcAccessMask
			dstAccessMask,                           // dstAccessMask
			m_currentLayout,                         // oldLayout
			layout,                                  // newLayout
			queueFamily == VK_QUEUE_FAMILY_IGNORED   // srcQueueFamilyIndex
			? VK_QUEUE_FAMILY_IGNORED
			: m_currentQueueFamily,
			queueFamily,                             // dstQueueFamilyIndex
			getTexture(),                            // image
			getSubResourceRange()                    // subresourceRange
		};
		DEBUG_DUMP( convert( transitionBarrier ) );
		m_currentAccessMask = dstAccessMask;
		m_currentLayout = layout;
		m_currentQueueFamily = queueFamily == VK_QUEUE_FAMILY_IGNORED
			? m_currentQueueFamily
			: queueFamily;
		return transitionBarrier;
	}
}
