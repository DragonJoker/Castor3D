#include "Image/VkTexture.hpp"

#include "Command/VkCommandBuffer.hpp"
#include "Core/VkDevice.hpp"
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

		renderer::PixelFormat findSupportedFormat( const std::vector< renderer::PixelFormat > & candidates
			, VkImageTiling tiling
			, VkFormatFeatureFlags features
			, PhysicalDevice const & physicalDevice )
		{
			for ( renderer::PixelFormat format : candidates )
			{
				VkFormatProperties props;
				vk::GetPhysicalDeviceFormatProperties( physicalDevice
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
		, m_currentLayout{ rhs.m_currentLayout }
		, m_currentAccessMask{ rhs.m_currentAccessMask }
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
			m_currentLayout = rhs.m_currentLayout;
			m_currentAccessMask = rhs.m_currentAccessMask;
			rhs.m_image = VK_NULL_HANDLE;
		}

		return *this;
	}

	Texture::Texture( Device const & device, renderer::ImageLayout initialLayout )
		: renderer::Texture{ device }
		, m_device{ device }
		, m_image{}
		, m_owner{ true }
		, m_currentLayout{ initialLayout }
		, m_currentAccessMask{ 0 }
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
		, m_currentLayout{ renderer::ImageLayout::ePresentSrc }
		, m_currentAccessMask{ renderer::AccessFlag::eMemoryRead }
	{
		if ( renderer::isDepthFormat( format )
			|| renderer::isDepthStencilFormat( format ) )
		{
			m_format = findSupportedFormat( { format }
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
		, m_currentLayout{ renderer::ImageLayout::eUndefined }
		, m_currentAccessMask{ 0 }
	{
		if ( renderer::isDepthFormat( format )
			|| renderer::isDepthStencilFormat( format ) )
		{
			m_format = findSupportedFormat( { format }
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
			vk::DestroyImage( m_device
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
		vk::GetImageSubresourceLayout( m_device, m_image, &subResource, &subResourceLayout );

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
		//m_texture->bind( 0 );
		//m_texture->generateMipmaps();
		//m_texture->unbind( 0 );
	}

	renderer::ImageMemoryBarrier Texture::makeGeneralLayout( renderer::ImageSubresourceRange const & range, renderer::AccessFlags accessFlags )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eGeneral
			, VK_QUEUE_FAMILY_IGNORED
			, accessFlags
			, range );
	}

	renderer::ImageMemoryBarrier Texture::makeTransferDestination( renderer::ImageSubresourceRange const & range )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferDstOptimal
			, VK_QUEUE_FAMILY_IGNORED
			, renderer::AccessFlag::eTransferWrite
			, range );
	}

	renderer::ImageMemoryBarrier Texture::makeTransferSource( renderer::ImageSubresourceRange const & range )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eTransferSrcOptimal
			, VK_QUEUE_FAMILY_IGNORED
			, renderer::AccessFlag::eTransferRead
			, range );
	}

	renderer::ImageMemoryBarrier Texture::makeShaderInputResource( renderer::ImageSubresourceRange const & range )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eShaderReadOnlyOptimal
			, VK_QUEUE_FAMILY_IGNORED
			, renderer::AccessFlag::eShaderRead
			, range );
	}

	renderer::ImageMemoryBarrier Texture::makeDepthStencilReadOnly( renderer::ImageSubresourceRange const & range )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilReadOnlyOptimal
			, m_device.getGraphicsQueue().getFamilyIndex()
			, renderer::AccessFlag::eShaderRead
			, range );
	}

	renderer::ImageMemoryBarrier Texture::makeColourAttachment( renderer::ImageSubresourceRange const & range )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eColourAttachmentOptimal
			, m_device.getGraphicsQueue().getFamilyIndex()
			, renderer::AccessFlag::eColourAttachmentWrite
			, range );
	}

	renderer::ImageMemoryBarrier Texture::makeDepthStencilAttachment( renderer::ImageSubresourceRange const & range )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::eDepthStencilAttachmentOptimal
			, m_device.getGraphicsQueue().getFamilyIndex()
			, renderer::AccessFlag::eDepthStencilAttachmentWrite
			, range );
	}

	renderer::ImageMemoryBarrier Texture::makePresentSource( renderer::ImageSubresourceRange const & range )const
	{
		return doMakeLayoutTransition( renderer::ImageLayout::ePresentSrc
			, m_device.getPresentQueue().getFamilyIndex()
			, renderer::AccessFlag::eMemoryRead
			, range );
	}

	renderer::ImageMemoryBarrier Texture::doMakeLayoutTransition( renderer::ImageLayout layout
		, uint32_t queueFamily
		, renderer::AccessFlags dstAccessMask
		, renderer::ImageSubresourceRange const & range )const
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
			*this,                                   // image
			range                                    // subresourceRange
		};
		DEBUG_DUMP( convert( transitionBarrier ) );
		m_currentAccessMask = dstAccessMask;
		m_currentLayout = layout;
		m_currentQueueFamily = queueFamily == VK_QUEUE_FAMILY_IGNORED
			? m_currentQueueFamily
			: queueFamily;
		return transitionBarrier;
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
			1,                                                    // mipLevels
			m_layerCount ? m_layerCount : 1u,                     // arrayLayers
			convert( m_samples ),                                 // samples
			convert( tiling ),                                    // tiling
			convert( usageFlags ),                                // usage
			VK_SHARING_MODE_EXCLUSIVE,                            // sharingMode
			0,                                                    // queueFamilyIndexCount
			nullptr,                                              // pQueueFamilyIndices
			convert( m_currentLayout )                            // initialLayout
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateImage( m_device
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
		res = vk::BindImageMemory( m_device
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
		if ( ( renderer::isDepthFormat( m_format )
			|| renderer::isDepthStencilFormat( m_format ) )
			&& checkFlag( usageFlags, renderer::ImageUsageFlag::eDepthStencilAttachment ) )
		{
			m_format = findSupportedFormat( { m_format }
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
			1,                                                    // mipLevels
			m_layerCount ? m_layerCount : 1u,                     // arrayLayers
			convert( m_samples ),                                 // samples
			convert( tiling ),                                    // tiling
			convert( usageFlags ),                                // usage
			VK_SHARING_MODE_EXCLUSIVE,                            // sharingMode
			0,                                                    // queueFamilyIndexCount
			nullptr,                                              // pQueueFamilyIndices
			convert( m_currentLayout )                            // initialLayout
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateImage( m_device
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
		res = vk::BindImageMemory( m_device
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
			1u,                                                   // mipLevels
			1u,                                                   // arrayLayers
			convert( m_samples ),                                 // samples
			convert( tiling ),                                    // tiling
			convert( usageFlags ),                                // usage
			VK_SHARING_MODE_EXCLUSIVE,                            // sharingMode
			0,                                                    // queueFamilyIndexCount
			nullptr,                                              // pQueueFamilyIndices
			convert( m_currentLayout )                            // initialLayout
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreateImage( m_device
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
		res = vk::BindImageMemory( m_device
			, m_image
			, *m_storage
			, 0 );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Image storage binding failed: " + getLastError() };
		}
	}
}
