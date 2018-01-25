/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/VkFrameBuffer.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Command/VkCommandPool.hpp"
#include "Core/VkDevice.hpp"
#include "Sync/VkFence.hpp"
#include "Sync/VkImageMemoryBarrier.hpp"
#include "Miscellaneous/VkMemoryStorage.hpp"
#include "Command/VkQueue.hpp"
#include "RenderPass/VkRenderPass.hpp"
#include "Image/VkTexture.hpp"
#include "Image/VkTextureView.hpp"

namespace vk_renderer
{
	namespace
	{
		TextureViewCRefArray convert( renderer::TextureViewCRefArray const & textures )
		{
			TextureViewCRefArray result;
			result.reserve( textures.size() );

			for ( auto & texture : textures )
			{
				result.emplace_back( static_cast< TextureView const & >( texture.get() ) );
			}

			return result;
		}
	}

	FrameBuffer::FrameBuffer( Device const & device
		, RenderPass const & renderPass
		, renderer::UIVec2 const & dimensions
		, renderer::TextureViewCRefArray const & textures )
		: renderer::FrameBuffer{ renderPass, dimensions, textures }
		, m_device{ device }
		, m_views{ convert( textures ) }
		, m_dimensions{ dimensions }
	{
		auto attachments = makeVkArray< VkImageView >( m_views );

		VkFramebufferCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0u,                                                 // flags
			renderPass,                                         // renderPass
			static_cast< uint32_t >( attachments.size() ),      // attachmentCount
			attachments.data(),                                 // pAttachments
			uint32_t( dimensions[0] ),                          // width
			uint32_t( dimensions[1] ),                          // height
			1u                                                  // layers
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateFramebuffer( device
			, &createInfo
			, nullptr
			, &m_framebuffer );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Frame buffer creation failed: " + getLastError() };
		}
	}

	FrameBuffer::~FrameBuffer()
	{
		if ( m_framebuffer )
		{
			vk::DestroyFramebuffer( m_device, m_framebuffer, nullptr );
		}
	}

	void FrameBuffer::download( renderer::Queue const & queue
		, uint32_t index
		, uint32_t xoffset
		, uint32_t yoffset
		, uint32_t width
		, uint32_t height
		, renderer::PixelFormat format
		, uint8_t * data )const noexcept
	{
		auto & attachView = m_views[index].get();
		auto & attachImage = attachView.getTexture();
		// Create the linear tiled destination image to copy to and to read the memory from
		Texture image{ m_device
			, format
			, renderer::UIVec2{ width, height }
			, renderer::ImageUsageFlag::eTransferDst
			, renderer::ImageTiling::eLinear
			, renderer::MemoryPropertyFlag::eHostVisible | renderer::MemoryPropertyFlag::eHostCoherent };
		TextureView view{ m_device
			, image
			, image.getType()
			, image.getFormat()
			, 0u
			, 1u
			, 0u
			, 1u };

		// Do the actual blit from the swapchain image to our host visible destination image
		CommandBuffer copyCmd{ m_device
			, static_cast< CommandPool const & >( m_device.getGraphicsCommandPool() )
			, true };
		copyCmd.begin();
		copyCmd.memoryBarrier( renderer::PipelineStageFlag::eTransfer
			, renderer::PipelineStageFlag::eTransfer
			, image.makeTransferDestination( view.getSubResourceRange() ) );
		copyCmd.memoryBarrier( renderer::PipelineStageFlag::eTransfer
			, renderer::PipelineStageFlag::eTransfer
			, attachImage.makeTransferSource( attachView.getSubResourceRange() ) );

		renderer::ImageCopy imageCopyRegion{};
		imageCopyRegion.srcSubresource.aspectMask = renderer::ImageAspectFlag::eColour;
		imageCopyRegion.srcSubresource.layerCount = 1;
		imageCopyRegion.dstSubresource.aspectMask = renderer::ImageAspectFlag::eColour;
		imageCopyRegion.dstSubresource.layerCount = 1;
		imageCopyRegion.extent[0] = width;
		imageCopyRegion.extent[1] = height;
		imageCopyRegion.extent[2] = 1;
		imageCopyRegion.srcOffset[0] = xoffset;
		imageCopyRegion.srcOffset[1] = yoffset;
		imageCopyRegion.srcOffset[2] = 0u;

		copyCmd.copyImage( imageCopyRegion
			, attachView
			, view );
		copyCmd.memoryBarrier( renderer::PipelineStageFlag::eTransfer
			, renderer::PipelineStageFlag::eTransfer
			, image.makeGeneralLayout( view.getSubResourceRange()
				, renderer::AccessFlag::eMemoryRead ) );

		copyCmd.end();

		{
			Fence fence{ m_device };
			queue.submit( copyCmd, &fence );
		}

		auto mapped = image.lock( 0u
			, uint32_t( VK_WHOLE_SIZE )
			, 0u );

		if ( mapped.data )
		{
			std::memcpy( data, mapped.data, mapped.size );
			image.unlock( uint32_t( VK_WHOLE_SIZE ), false );
		}
	}
}
