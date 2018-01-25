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
		, uint32_t layerCount )
		: renderer::TextureView{ device, image, type, format, baseMipLevel, levelCount, baseArrayLayer, layerCount }
		, m_device{ device }
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
}
