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
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkBorderColor const & borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK
			, VkCompareOp compareOp = VK_COMPARE_OP_NEVER
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkFilter minFilter
			, VkFilter magFilter
			, VkSamplerMipmapMode mipFilter
			, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VkBorderColor const & borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK
			, VkCompareOp compareOp = VK_COMPARE_OP_NEVER
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, VkFormat format
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
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkBorderColor const & borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK
			, VkCompareOp compareOp = VK_COMPARE_OP_NEVER
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, VkSampleCountFlagBits sampleCount
			, uint32_t mipLevels
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkFilter minFilter
			, VkFilter magFilter
			, VkSamplerMipmapMode mipFilter
			, VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VkBorderColor const & borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK
			, VkCompareOp compareOp = VK_COMPARE_OP_NEVER
			, bool createSubviews = true );
		C3D_API Texture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, VkSampleCountFlagBits sampleCount
			, uint32_t mipLevels
			, VkFormat format
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

		C3D_API VkImageMemoryBarrier makeGeneralLayout( VkImageLayout srcLayout
			, VkAccessFlags dstAccessFlags
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferDestination( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferSource( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeShaderInputResource( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilReadOnly( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeColourAttachment( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilAttachment( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makePresentSource( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImageLayout sourceLayout
			, VkImageLayout destinationLayout
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImageLayout srcLayout
			, VkImageLayout dstLayout
			, VkAccessFlags srcAccessFlags
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImageLayout srcLayout
			, VkImageLayout dstLayout
			, VkAccessFlags srcAccessFlags
			, VkAccessFlags dstAccessMask
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;

		uint32_t getMipLevels()const noexcept
		{
			return imageId.data->info.mipLevels;
		}

		VkFormat getFormat()const noexcept
		{
			return imageId.data->info.format;
		}

		VkExtent3D const & getExtent()const noexcept
		{
			return imageId.data->info.extent;
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
			, VkImageLayout layout
			, TextureFactors factors = {} )noexcept
			: name{ castor::move( name ) }
			, viewId{ viewId }
			, layout{ layout }
			, factors{ castor::move( factors ) }
		{
		}

		IntermediateView( castor::String name
			, Texture const & texture
			, VkImageLayout layout
			, TextureFactors factors = {} )noexcept
			: IntermediateView{ castor::move( name )
				, texture.sampledViewId
				, layout
				, castor::move( factors ) }
		{
		}

		castor::String name{};
		crg::ImageViewId viewId{};
		VkImageLayout layout{};
		TextureFactors factors{};
	};
}

#endif
