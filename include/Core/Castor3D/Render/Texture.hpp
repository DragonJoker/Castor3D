/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Texture_H___
#define ___C3D_Texture_H___

#include "RenderModule.hpp"

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>

namespace castor3d
{
	struct Texture
	{
		C3D_API Texture( Texture const & ) = delete;
		C3D_API Texture & operator=( Texture const & ) = delete;
		C3D_API Texture( Texture && rhs )noexcept;
		C3D_API Texture & operator=( Texture && rhs )noexcept;

		C3D_API Texture() = default;
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, castor::PixelFormat format
			, VkImageUsageFlags usageFlags
			, BorderColour borderColor = BorderColour::eFloatTransparentBlack
			, ComparisonFunc compareOp = ComparisonFunc::eNever
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, castor::PixelFormat format
			, VkImageUsageFlags usageFlags
			, FilterMode minFilter
			, FilterMode magFilter
			, MipmapMode mipFilter
			, WrapMode addressMode = WrapMode::eClampToEdge
			, BorderColour borderColor = BorderColour::eFloatTransparentBlack
			, ComparisonFunc compareOp = ComparisonFunc::eNever
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, castor::PixelFormat format
			, VkImageUsageFlags usageFlags
			, ashes::Sampler const * sampler
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, VkSampleCountFlagBits sampleCount
			, uint32_t mipLevels
			, castor::PixelFormat format
			, VkImageUsageFlags usageFlags
			, BorderColour borderColor = BorderColour::eFloatTransparentBlack
			, ComparisonFunc compareOp = ComparisonFunc::eNever
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, VkSampleCountFlagBits sampleCount
			, uint32_t mipLevels
			, castor::PixelFormat format
			, VkImageUsageFlags usageFlags
			, FilterMode minFilter
			, FilterMode magFilter
			, MipmapMode mipFilter
			, WrapMode addressMode = WrapMode::eClampToBorder
			, BorderColour borderColor = BorderColour::eFloatTransparentBlack
			, ComparisonFunc compareOp = ComparisonFunc::eNever
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, VkSampleCountFlagBits sampleCount
			, uint32_t mipLevels
			, castor::PixelFormat format
			, VkImageUsageFlags usageFlags
			, ashes::Sampler const * sampler
			, bool createSubviews = true );
		C3D_API ~Texture()noexcept;

		C3D_API void create();
		C3D_API void destroy()noexcept;

		operator bool()const noexcept
		{
			return resources != nullptr
				&& device != nullptr;
		}

		C3D_API VkImageMemoryBarrier makeGeneralLayout( ImageLayout srcLayout
			, AccessFlags dstAccessFlags
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferDestination( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferSource( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeShaderInputResource( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilReadOnly( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeColourAttachment( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilAttachment( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makePresentSource( ImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( ImageLayout sourceLayout
			, ImageLayout destinationLayout
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( ImageLayout srcLayout
			, ImageLayout dstLayout
			, AccessFlags srcAccessFlags
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( ImageLayout srcLayout
			, ImageLayout dstLayout
			, AccessFlags srcAccessFlags
			, AccessFlags dstAccessMask
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;

		uint32_t getMipLevels()const noexcept
		{
			return crg::getMipLevels( imageId );
		}

		castor::PixelFormat getFormat()const noexcept
		{
			return crg::getFormat( imageId );
		}

		VkExtent3D const & getExtent()const noexcept
		{
			return crg::getExtent( imageId );
		}

		crg::ResourcesCache * resources{};
		RenderDevice const * device{};
		crg::ImageId imageId{};
		ashes::ImagePtr image{};
		crg::ImageViewId wholeViewId{};
		crg::ImageViewId targetViewId{};
		crg::ImageViewId sampledViewId{};
		VkImageView wholeView{};
		VkImageView targetView{};
		VkImageView sampledView{};
		crg::ImageViewIdArray subViewsId{};
		castor::Vector< VkImageView > subViews{};
		ashes::Sampler const * sampler{};
	};

	struct IntermediateView
	{
		C3D_API IntermediateView() = default;

		IntermediateView( castor::String name
			, crg::ImageViewId const & viewId
			, ImageLayout layout
			, TextureFactors factors = {} )noexcept
			: name{ castor::move( name ) }
			, viewId{ viewId }
			, layout{ layout }
			, factors{ castor::move( factors ) }
		{
		}

		IntermediateView( castor::String name
			, Texture const & texture
			, ImageLayout layout
			, TextureFactors factors = {} )noexcept
			: IntermediateView{ castor::move( name )
				, texture.sampledViewId
				, layout
				, castor::move( factors ) }
		{
		}

		castor::String name{};
		crg::ImageViewId viewId{};
		ImageLayout layout{};
		TextureFactors factors{};
	};
}

#endif
