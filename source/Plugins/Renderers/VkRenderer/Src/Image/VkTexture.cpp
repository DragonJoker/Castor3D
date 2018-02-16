#include "Image/VkTexture.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Command/VkQueue.hpp"
#include "Core/VkDevice.hpp"
#include "Core/VkRenderer.hpp"
#include "Sync/VkImageMemoryBarrier.hpp"
#include "Image/VkImageSubresourceRange.hpp"
#include "Miscellaneous/VkMemoryStorage.hpp"
#include "Command/VkQueue.hpp"
#include "Image/VkTextureView.hpp"

namespace vk_renderer
{
	namespace
	{
		renderer::ImageMemoryBarrier doConvert( Texture const & texture
			, VkImageMemoryBarrier const & barrier )
		{
			return renderer::ImageMemoryBarrier
			{
				convertAccessFlags( barrier.srcAccessMask ),
				convertAccessFlags( barrier.dstAccessMask ),
				convertImageLayout( barrier.oldLayout ),
				convertImageLayout( barrier.newLayout ),
				barrier.srcQueueFamilyIndex,
				barrier.dstQueueFamilyIndex,
				texture,
				convert( barrier.subresourceRange )
			};
		}

		renderer::PixelFormat findSupportedFormat( Device const & device
			, const std::vector< renderer::PixelFormat > & candidates
			, VkImageTiling tiling
			, VkFormatFeatureFlags features
			, PhysicalDevice const & physicalDevice )
		{
			for ( renderer::PixelFormat format : candidates )
			{
				VkFormatProperties props;
				device.getRenderer().vkGetPhysicalDeviceFormatProperties( physicalDevice
					, convert( format )
					, &props );

				if ( tiling == VK_IMAGE_TILING_LINEAR && ( props.linearTilingFeatures & features ) == features )
				{
					return format;
				}

				if ( tiling == VK_IMAGE_TILING_OPTIMAL && ( props.optimalTilingFeatures & features ) == features )
				{
					return format;
				}
			}

			throw std::runtime_error( "failed to find supported format!" );
		}
	}

	Texture::Texture( Texture && rhs )
		: renderer::Texture{ std::move( rhs ) }
		, m_device{ rhs.m_device }
		, m_image{ rhs.m_image }
		, m_owner{ rhs.m_owner }
	{
		rhs.m_image = VK_NULL_HANDLE;
	}

	Texture & Texture::operator=( Texture && rhs )
	{
		renderer::Texture::operator=( std::move( rhs ) );

		if ( &rhs != this )
		{
			m_image = rhs.m_image;
			m_owner = rhs.m_owner;
			rhs.m_image = VK_NULL_HANDLE;
		}

		return *this;
	}

	Texture::Texture( Device const & device, renderer::ImageLayout initialLayout )
		: renderer::Texture{ device }
		, m_device{ device }
		, m_image{}
		, m_owner{ true }
	{
	}

	Texture::Texture( Device const & device
		, renderer::PixelFormat format
		, renderer::UIVec2 const & dimensions
		, VkImage image )
		: renderer::Texture{ device }
		, m_device{ device }
		, m_image{ image }
		, m_owner{ false }
	{
		if ( renderer::isDepthOrStencilFormat( format ) )
		{
			m_format = findSupportedFormat( m_device
				, { format }
				, VK_IMAGE_TILING_OPTIMAL
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				, m_device.getPhysicalDevice() );
		}
		else
		{
			m_format = format;
		}

		m_type = renderer::TextureType::e2D;
		m_size = renderer::UIVec3{ dimensions[0], dimensions[1], 0u };
		m_layerCount = 0u;
		m_samples = renderer::SampleCountFlag::e1;
	}

	Texture::Texture( Device const & device
		, renderer::PixelFormat format
		, renderer::UIVec2 const & dimensions
		, renderer::ImageUsageFlags usageFlags
		, renderer::ImageTiling tiling
		, renderer::MemoryPropertyFlags memoryFlags )
		: renderer::Texture{ device }
		, m_device{ device }
		, m_owner{ true }
	{
		if ( renderer::isDepthOrStencilFormat( format ) )
		{
			m_format = findSupportedFormat( m_device
				, { format }
				, VK_IMAGE_TILING_OPTIMAL
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				, m_device.getPhysicalDevice() );
		}
		else
		{
			m_format = format;
		}

		m_format = format;
		m_type = renderer::TextureType::e2D;
		m_size = renderer::UIVec3{ dimensions[0], dimensions[1], 0u };
		m_layerCount = 0u;
		m_samples = renderer::SampleCountFlag::e1;
		doSetImage2D( usageFlags
			, tiling
			, memoryFlags );
	}

	Texture::~Texture()
	{
		if ( m_owner )
		{
			m_device.vkDestroyImage( m_device
				, m_image
				, nullptr );
		}
	}

	renderer::Texture::Mapped Texture::lock( uint32_t offset
		, uint32_t size
		, VkMemoryMapFlags flags )const
	{
		renderer::Texture::Mapped mapped{};
		VkImageSubresource subResource{};
		subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		VkSubresourceLayout subResourceLayout;
		m_device.vkGetImageSubresourceLayout( m_device, m_image, &subResource, &subResourceLayout );

		mapped.data = m_storage->lock( offset
			, size
			, flags );

		if ( mapped.data )
		{
			mapped.arrayPitch = subResourceLayout.arrayPitch;
			mapped.depthPitch = subResourceLayout.depthPitch;
			mapped.rowPitch = subResourceLayout.rowPitch;
			mapped.size = subResourceLayout.size;
			mapped.data += subResourceLayout.offset;
		}

		return mapped;
	}

	void Texture::unlock( uint32_t size
		, bool modified )const
	{
		m_storage->unlock( size
			, modified );
	}

	renderer::TextureViewPtr Texture::createView( renderer::TextureType type
		, renderer::PixelFormat format
		, uint32_t baseMipLevel
		, uint32_t levelCount
		, uint32_t baseArrayLayer
		, uint32_t layerCount )const
	{
		return std::make_shared< TextureView >( m_device
			, *this
			, type
			, format
			, baseMipLevel
			, levelCount
			, baseArrayLayer
			, layerCount );
	}

	void Texture::generateMipmaps()const
	{
		auto const width = int32_t( getDimensions()[0] );
		auto const height = int32_t( getDimensions()[1] );
		auto commandBuffer = m_device.getGraphicsCommandPool().createCommandBuffer();
		auto & vkCommandBuffer = static_cast< CommandBuffer const & >( *commandBuffer );

		if ( commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			TextureView srcView{ m_device, *this, getType(), getFormat(), 0, 1u, 0u, 1u };
			vkCommandBuffer.memoryBarrier( convert( renderer::PipelineStageFlag::eFragmentShader )
				, convert( renderer::PipelineStageFlag::eTopOfPipe )
				, srcView.makeTransferSource() );

			for ( uint32_t i = 1; i < m_mipmapLevels; ++i )
			{
				TextureView dstView{ m_device, *this, getType(), getFormat(), i, 1u, 0u, 1u };
				vkCommandBuffer.memoryBarrier( convert( renderer::PipelineStageFlag::eFragmentShader )
					, convert( renderer::PipelineStageFlag::eTopOfPipe )
					, dstView.makeTransferDestination() );

				int32_t const mipWidth = width >> i;
				int32_t const mipHeight = height >> i;

				VkImageBlit imageBlit = {};
				imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.srcSubresource.baseArrayLayer = 0;
				imageBlit.srcSubresource.layerCount = 1;
				imageBlit.srcSubresource.mipLevel = 0;
				imageBlit.srcOffsets[0] = { 0, 0, 0 };
				imageBlit.srcOffsets[1] = { width, height, 1 };

				imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				imageBlit.dstSubresource.baseArrayLayer = 0;
				imageBlit.dstSubresource.layerCount = 1;
				imageBlit.dstSubresource.mipLevel = i;
				imageBlit.dstOffsets[0] = { 0, 0, 0 };
				imageBlit.dstOffsets[1] = { mipWidth, mipHeight, 1 };

				m_device.vkCmdBlitImage( vkCommandBuffer
					, m_image
					, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
					, m_image
					, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, 1
					, &imageBlit
					, VK_FILTER_LINEAR );
			}

			commandBuffer->end();
			m_device.getGraphicsQueue().submit( *commandBuffer, nullptr );
			m_device.waitIdle();
		}
	}

	void Texture::doSetImage1D( renderer::ImageUsageFlags usageFlags
		, renderer::ImageTiling tiling
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		assert( m_owner );
		VkImageCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,                                                    // flags
			VK_IMAGE_TYPE_1D,                                     // imageType
			convert( m_format ),                                  // format
			{                                                     // extent
				uint32_t( m_size[0] ),                                // width
				1u,                                                   // height
				1u                                                    // depth
			},
			m_mipmapLevels,                                       // mipLevels
			m_layerCount ? m_layerCount : 1u,                     // arrayLayers
			convert( m_samples ),                                 // samples
			convert( tiling ),                                    // tiling
			convert( usageFlags ),                                // usage
			VK_SHARING_MODE_EXCLUSIVE,                            // sharingMode
			0,                                                    // queueFamilyIndexCount
			nullptr,                                              // pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED                             // initialLayout
		};
		DEBUG_DUMP( createInfo );
		auto res = m_device.vkCreateImage( m_device
			, &createInfo
			, nullptr
			, &m_image );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image creation failed: " + getLastError() };
		}

		m_storage = std::make_unique< ImageStorage >( m_device
			, m_image
			, convert( memoryFlags ) );
		res = m_device.vkBindImageMemory( m_device
			, m_image
			, *m_storage
			, 0 );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image storage binding failed: " + getLastError() };
		}
	}

	void Texture::doSetImage2D( renderer::ImageUsageFlags usageFlags
		, renderer::ImageTiling tiling
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		if ( renderer::isDepthOrStencilFormat( m_format )
			&& checkFlag( usageFlags, renderer::ImageUsageFlag::eDepthStencilAttachment ) )
		{
			m_format = findSupportedFormat( m_device
				, { m_format }
				, convert( tiling )
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				, m_device.getPhysicalDevice() );
		}

		assert( m_owner );
		VkImageCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,                                                    // flags
			VK_IMAGE_TYPE_2D,                                     // imageType
			convert( m_format ),                                  // format
			{                                                     // extent
				uint32_t( m_size[0] ),                                // width
				uint32_t( m_size[1] ),                                // height
				1                                                     // depth
			},
			m_mipmapLevels,                                       // mipLevels
			m_layerCount ? m_layerCount : 1u,                     // arrayLayers
			convert( m_samples ),                                 // samples
			convert( tiling ),                                    // tiling
			convert( usageFlags ),                                // usage
			VK_SHARING_MODE_EXCLUSIVE,                            // sharingMode
			0,                                                    // queueFamilyIndexCount
			nullptr,                                              // pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED                             // initialLayout
		};
		DEBUG_DUMP( createInfo );
		auto res = m_device.vkCreateImage( m_device
			, &createInfo
			, nullptr
			, &m_image );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image creation failed: " + getLastError() };
		}

		m_storage = std::make_unique< ImageStorage >( m_device
			, m_image
			, convert( memoryFlags ) );
		res = m_device.vkBindImageMemory( m_device
			, m_image
			, *m_storage
			, 0 );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image storage binding failed: " + getLastError() };
		}
	}

	void Texture::doSetImage3D( renderer::ImageUsageFlags usageFlags
		, renderer::ImageTiling tiling
		, renderer::MemoryPropertyFlags memoryFlags )
	{
		assert( m_owner );
		VkImageCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			nullptr,
			0,                                                    // flags
			VK_IMAGE_TYPE_3D,                                     // imageType
			convert( m_format ),                                  // format
			{                                                     // extent
				uint32_t( m_size[0] ),                                // width
				uint32_t( m_size[1] ),                                // height
				uint32_t( m_size[2] ),                                // depth
			},
			m_mipmapLevels,                                       // mipLevels
			1u,                                                   // arrayLayers
			convert( m_samples ),                                 // samples
			convert( tiling ),                                    // tiling
			convert( usageFlags ),                                // usage
			VK_SHARING_MODE_EXCLUSIVE,                            // sharingMode
			0,                                                    // queueFamilyIndexCount
			nullptr,                                              // pQueueFamilyIndices
			VK_IMAGE_LAYOUT_UNDEFINED                             // initialLayout
		};
		DEBUG_DUMP( createInfo );
		auto res = m_device.vkCreateImage( m_device
			, &createInfo
			, nullptr
			, &m_image );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image creation failed: " + getLastError() };
		}

		m_storage = std::make_unique< ImageStorage >( m_device
			, m_image
			, convert( memoryFlags ) );
		res = m_device.vkBindImageMemory( m_device
			, m_image
			, *m_storage
			, 0 );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image storage binding failed: " + getLastError() };
		}
	}
}
