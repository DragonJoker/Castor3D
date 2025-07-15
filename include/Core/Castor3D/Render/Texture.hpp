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

namespace c3d
{
	struct TextureSamplerCreateInfo
	{
		BorderColour borderColor{ BorderColour::eFloatTransparentBlack };
		ComparisonFunc compareOp{ ComparisonFunc::eNever };
		WrapMode addressMode{ WrapMode::eClampToEdge };
		FilterMode minFilter{ FilterMode::eLinear };
		FilterMode magFilter{ FilterMode::eLinear };
		MipmapMode mipFilter{ MipmapMode::eLinear };
	};

	struct TextureSamplerInfo
	{
		TextureSamplerCreateInfo createInfo{};
		ashes::Sampler const * sampler{};
	};

	struct TextureCreateInfo
	{
		ImageCreateFlags createFlags;
		Extent3D extent;
		uint32_t layerCount;
		uint32_t mipLevels;
		PixelFormat format;
		ImageUsageFlags usageFlags;
		SampleCount sampleCount{ SampleCount::e1 };
	};

	struct Texture
	{
		C3D_API Texture( Texture const & ) = delete;
		C3D_API Texture & operator=( Texture const & ) = delete;
		C3D_API Texture( Texture && rhs )noexcept;
		C3D_API Texture & operator=( Texture && rhs )noexcept;

		C3D_API Texture() = default;
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, TextureCreateInfo const & imageInfo
			, TextureSamplerInfo const & samplerInfo
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

		PixelFormat getFormat()const noexcept
		{
			return crg::getFormat( imageId );
		}

		Extent3D const & getExtent()const noexcept
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
		Vector< VkImageView > subViews{};
		ashes::Sampler const * sampler{};
	};

	struct IntermediateView
	{
		C3D_API IntermediateView() = default;

		IntermediateView( String name
			, crg::ImageViewId const & viewId
			, ImageLayout layout
			, TextureFactors factors = {} )noexcept
			: name{ c3d::move( name ) }
			, viewId{ viewId }
			, layout{ layout }
			, factors{ c3d::move( factors ) }
		{
		}

		IntermediateView( String name
			, Texture const & texture
			, ImageLayout layout
			, TextureFactors factors = {} )noexcept
			: IntermediateView{ c3d::move( name )
				, texture.sampledViewId
				, layout
				, c3d::move( factors ) }
		{
		}

		String name{};
		crg::ImageViewId viewId{};
		ImageLayout layout{};
		TextureFactors factors{};
	};
}

#endif
