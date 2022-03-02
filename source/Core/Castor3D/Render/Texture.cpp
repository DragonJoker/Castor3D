#include "Castor3D/Render/Texture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/ResourceHandler.hpp>

#include <ashespp/Image/Image.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		VkFormat getDepthFormat( RenderDevice const & device
			, VkFormat format )
		{
			std::vector< VkFormat > depthFormats
			{
				format,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM_S8_UINT,
			};
			return device.selectSuitableFormat( depthFormats
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		VkFormat retrieveFormat( RenderDevice const & device
			, VkFormat format )
		{
			return ashes::isDepthOrStencilFormat( format )
				? getDepthFormat( device, format )
				: format;
		}
	}

	//*********************************************************************************************

	Texture::Texture()
		: handler{}
		, device{}
		, imageId{}
		, image{}
		, wholeViewId{}
		, targetViewId{}
		, sampledViewId{}
		, wholeView{}
		, targetView{}
		, sampledView{}
		, subViewsId{}
		, sampler{}
	{
	}

	Texture::Texture( Texture && rhs )
		: handler{ std::move( rhs.handler ) }
		, device{ std::move( rhs.device ) }
		, imageId{ std::move( rhs.imageId ) }
		, image{ std::move( rhs.image ) }
		, wholeViewId{ std::move( rhs.wholeViewId ) }
		, targetViewId{ std::move( rhs.targetViewId ) }
		, sampledViewId{ std::move( rhs.sampledViewId ) }
		, wholeView{ std::move( rhs.wholeView ) }
		, targetView{ std::move( rhs.targetView ) }
		, sampledView{ std::move( rhs.sampledView ) }
		, subViewsId{ std::move( rhs.subViewsId ) }
		, sampler{ std::move( rhs.sampler ) }
	{
		rhs.device = nullptr;
		rhs.handler = nullptr;
		rhs.image = nullptr;
		rhs.wholeView = nullptr;
		rhs.targetView = nullptr;
		rhs.sampledView = nullptr;
	}

	Texture & Texture::operator=( Texture && rhs )
	{
		handler = std::move( rhs.handler );
		device = std::move( rhs.device );
		imageId = std::move( rhs.imageId );
		image = std::move( rhs.image );
		wholeViewId = std::move( rhs.wholeViewId );
		targetViewId = std::move( rhs.targetViewId );
		sampledViewId = std::move( rhs.sampledViewId );
		wholeView = std::move( rhs.wholeView );
		targetView = std::move( rhs.targetView );
		sampledView = std::move( rhs.sampledView );
		subViewsId = std::move( rhs.subViewsId );
		sampler = std::move( rhs.sampler );

		rhs.device = nullptr;
		rhs.handler = nullptr;
		rhs.image = nullptr;
		rhs.wholeView = nullptr;
		rhs.targetView = nullptr;
		rhs.sampledView = nullptr;

		return *this;
	}

	Texture::Texture( RenderDevice const & device
		, crg::ResourceHandler & handler
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, uint32_t mipLevels
		, VkFormat format
		, VkImageUsageFlags usageFlags
		, VkBorderColor const & borderColor
		, bool createSubviews )
		: Texture{ device
			, handler
			, name
			, createFlags
			, size
			, layerCount
			, mipLevels
			, format
			, usageFlags
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, borderColor
			, createSubviews }
	{
	}

	Texture::Texture( RenderDevice const & device
		, crg::ResourceHandler & handler
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
		, VkSamplerAddressMode addressMode
		, VkBorderColor const & borderColor
		, bool createSubviews )
		: handler{ &handler }
		, device{ &device }
	{
		mipLevels = std::max( 1u, mipLevels );
		layerCount = ( size.depth > 1u ? 1u : layerCount );
		imageId = handler.createImageId( crg::ImageData{ name
			, ( createFlags
				| ( size.depth > 1u
					? VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
					: VkImageCreateFlagBits{} ) )
			, ( size.depth > 1u
				? VK_IMAGE_TYPE_3D
				: VK_IMAGE_TYPE_2D )
			, retrieveFormat( device, format )
			, size
			, ( usageFlags
				| ( mipLevels > 1u
					? ( VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT )
					: VkImageUsageFlags{} ) )
			, mipLevels
			, layerCount } );
		wholeViewId = handler.createViewId( crg::ImageViewData{ name + "Whole"
			, imageId
			, 0u
			, ( size.depth > 1u
				? VK_IMAGE_VIEW_TYPE_3D
				: ( layerCount > 1u
					? ( ashes::checkFlag( createFlags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT )
						? ( layerCount > 6u
							? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
							: VK_IMAGE_VIEW_TYPE_CUBE )
						: VK_IMAGE_VIEW_TYPE_2D_ARRAY )
					: VK_IMAGE_VIEW_TYPE_2D ) )
			, format
			, { ashes::getAspectMask( format ), 0u, mipLevels, 0u, layerCount } } );

		if ( wholeViewId.data->info.viewType == VK_IMAGE_VIEW_TYPE_3D )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			createInfo.name = name + "Target";
			createInfo.info.subresourceRange.baseArrayLayer = 0u;
			createInfo.info.subresourceRange.layerCount = createInfo.image.data->info.extent.depth;
			createInfo.info.subresourceRange.baseMipLevel = 0u;
			createInfo.info.subresourceRange.levelCount = 1u;
			targetViewId = handler.createViewId( createInfo );
		}
		else if ( wholeViewId.data->info.subresourceRange.levelCount == wholeViewId.data->image.data->info.mipLevels )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.name = name + "Target";
			createInfo.info.subresourceRange.baseMipLevel = 0u;
			createInfo.info.subresourceRange.levelCount = 1u;
			targetViewId = handler.createViewId( createInfo );
		}
		else
		{
			targetViewId = wholeViewId;
		}

		if ( ashes::isDepthStencilFormat( wholeViewId.data->image.data->info.format ) )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.name = name + "Sampled";
			createInfo.info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			sampledViewId = handler.createViewId( createInfo );
		}
		else
		{
			sampledViewId = wholeViewId;
		}

		if ( createSubviews )
		{
			auto sliceLayerCount = std::max( size.depth, layerCount );

			for ( uint32_t index = 0u; index < sliceLayerCount; ++index )
			{
				subViewsId.push_back( handler.createViewId( crg::ImageViewData{ name + "Sub" + std::to_string( index )
					, imageId
					, 0u
					, VK_IMAGE_VIEW_TYPE_2D
					, format
					, { ashes::getAspectMask( format ), 0u, 1u, index, 1u } } ) );
			}
		}

		auto & engine = *device.renderSystem.getEngine();
		SamplerResPtr c3dSampler;
		auto splName = getSamplerName( minFilter
			, magFilter
			, mipFilter
			, addressMode
			, addressMode
			, addressMode );

		if ( engine.getSamplerCache().has( splName ) )
		{
			c3dSampler = engine.getSamplerCache().find( splName );
		}
		else
		{
			auto created = castor::makeResource< Sampler, castor::String >( splName, engine );
			created->setMinFilter( minFilter );
			created->setMagFilter( magFilter );
			created->setMipFilter( mipFilter );
			created->setWrapS( addressMode );
			created->setWrapT( addressMode );
			created->setWrapR( addressMode );
			created->setBorderColour( borderColor );
			created->initialise( device );
			c3dSampler = engine.getSamplerCache().add( splName, created, false );
		}

		sampler = &c3dSampler.lock()->getSampler();
	}

	Texture::~Texture()
	{
		CU_Require( image == nullptr && wholeView == nullptr );
	}

	void Texture::create()
	{
		if ( !device )
		{
			return;
		}

		auto & context = device->makeContext();
		image = handler->createImage( context, imageId );
		wholeView = handler->createImageView( context, wholeViewId );

		if ( wholeViewId != targetViewId )
		{
			targetView = handler->createImageView( context, targetViewId );
		}
		else
		{
			targetView = wholeView;
		}

		if ( wholeViewId != sampledViewId )
		{
			sampledView = handler->createImageView( context, sampledViewId );
		}
		else
		{
			sampledView = wholeView;
		}
	}

	void Texture::destroy()
	{
		if ( device && handler )
		{
			auto & context = device->makeContext();

			if ( wholeViewId != sampledViewId )
			{
				handler->destroyImageView( context, sampledViewId );
				sampledView = nullptr;
			}

			if ( wholeViewId != targetViewId )
			{
				handler->destroyImageView( context, targetViewId );
				targetView = nullptr;
			}

			handler->destroyImageView( context, wholeViewId );
			wholeView = nullptr;
			handler->destroyImage( context, imageId );
			image = nullptr;
		}
	}

	VkImageMemoryBarrier Texture::makeGeneralLayout( VkImageLayout srcLayout
		, VkAccessFlags dstAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_GENERAL
			, ashes::getAccessMask( srcLayout )
			, dstAccessFlags
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeTransferDestination( VkImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeTransferSource( VkImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeShaderInputResource( VkImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeDepthStencilReadOnly( VkImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeColourAttachment( VkImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeDepthStencilAttachment( VkImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makePresentSource( VkImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeLayoutTransition( VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, dstLayout
			, ashes::getAccessMask( srcLayout )
			, ashes::getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier Texture::makeLayoutTransition( VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, dstLayout
			, srcAccessFlags
			, ashes::getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier Texture::makeLayoutTransition( VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, VkAccessFlags dstAccessMask
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeVkStruct< VkImageMemoryBarrier >( srcAccessFlags
			, dstAccessMask
			, srcLayout
			, dstLayout
			, srcQueueFamily
			, dstQueueFamily
			, image
			, ( target
				? targetViewId.data->info.subresourceRange
				: sampledViewId.data->info.subresourceRange ) );
	}

	//*********************************************************************************************
}
