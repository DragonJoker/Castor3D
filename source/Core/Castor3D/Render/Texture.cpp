#include "Castor3D/Render/Texture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/ResourceHandler.hpp>

#include <ashespp/Image/Image.hpp>

CU_ImplementSmartPtr( c3d, Texture )

namespace c3d
{
	//*********************************************************************************************

	namespace texture
	{
		static PixelFormat getDepthFormat( RenderDevice const & device
			, PixelFormat format )
		{
			Vector< PixelFormat > depthFormats
			{
				format,
				PixelFormat::eD24_UNORM_S8_UINT,
				PixelFormat::eD16_UNORM_S8_UINT,
			};
			return device.selectSuitableFormat( depthFormats
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		static PixelFormat retrieveFormat( RenderDevice const & device
			, PixelFormat format )
		{
			return isDepthOrStencilFormat( format )
				? getDepthFormat( device, format )
				: format;
		}

		static ashes::Sampler const * getSampler( RenderDevice const & device
			, TextureSamplerCreateInfo createInfo )
		{
			auto & engine = *device.renderSystem.getEngine();
			Sampler const * c3dSampler{};

			if ( auto splName = getSamplerName( createInfo.compareOp
					, createInfo.minFilter
					, createInfo.magFilter
					, createInfo.mipFilter
					, createInfo.addressMode
					, createInfo.addressMode
					, createInfo.addressMode
					, createInfo.borderColor );
				engine.hasSampler( splName ) )
			{
				c3dSampler = engine.findSampler( splName );
			}
			else
			{
				auto created = engine.createSampler( splName, engine );
				created->setMinFilter( createInfo.minFilter );
				created->setMagFilter( createInfo.magFilter );
				created->setMipFilter( createInfo.mipFilter );
				created->setWrapS( createInfo.addressMode );
				created->setWrapT( createInfo.addressMode );
				created->setWrapR( createInfo.addressMode );
				created->setBorderColour( createInfo.borderColor );

				if ( createInfo.compareOp != ComparisonFunc::eNever )
				{
					created->enableCompare( true );
					created->setCompareOp( createInfo.compareOp );
				}

				created->initialise( device );
				c3dSampler = engine.addSampler( splName, created, false );
			}

			return &c3dSampler->getSampler();
		}

		static bool isTexture1D( Extent3D const & extent )
		{
			return extent.height == 1
				&& extent.width > 1;
		}
	}

	//*********************************************************************************************

	Texture::Texture( Texture && rhs )noexcept
		: resources{ c3d::move( rhs.resources ) }
		, device{ c3d::move( rhs.device ) }
		, imageId{ c3d::move( rhs.imageId ) }
		, image{ c3d::move( rhs.image ) }
		, wholeViewId{ c3d::move( rhs.wholeViewId ) }
		, targetViewId{ c3d::move( rhs.targetViewId ) }
		, sampledViewId{ c3d::move( rhs.sampledViewId ) }
		, wholeView{ c3d::move( rhs.wholeView ) }
		, targetView{ c3d::move( rhs.targetView ) }
		, sampledView{ c3d::move( rhs.sampledView ) }
		, subViewsId{ c3d::move( rhs.subViewsId ) }
		, sampler{ c3d::move( rhs.sampler ) }
	{
		rhs.device = nullptr;
		rhs.resources = nullptr;
		rhs.image = nullptr;
		rhs.wholeView = VkImageView{};
		rhs.targetView = VkImageView{};
		rhs.sampledView = VkImageView{};
	}

	Texture & Texture::operator=( Texture && rhs )noexcept
	{
		resources = c3d::move( rhs.resources );
		device = c3d::move( rhs.device );
		imageId = c3d::move( rhs.imageId );
		image = c3d::move( rhs.image );
		wholeViewId = c3d::move( rhs.wholeViewId );
		targetViewId = c3d::move( rhs.targetViewId );
		sampledViewId = c3d::move( rhs.sampledViewId );
		wholeView = c3d::move( rhs.wholeView );
		targetView = c3d::move( rhs.targetView );
		sampledView = c3d::move( rhs.sampledView );
		subViewsId = c3d::move( rhs.subViewsId );
		sampler = c3d::move( rhs.sampler );

		rhs.device = nullptr;
		rhs.resources = nullptr;
		rhs.image = nullptr;
		rhs.wholeView = VkImageView{};
		rhs.targetView = VkImageView{};
		rhs.sampledView = VkImageView{};

		return *this;
	}

	Texture::Texture( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, String const & name
		, TextureCreateInfo const & imageInfo
		, TextureSamplerInfo const & samplerInfo
		, bool createSubviews )
		: resources{ &presources }
		, device{ &pdevice }
		, sampler{ ( samplerInfo.sampler
			? samplerInfo.sampler
			: texture::getSampler( pdevice, c3d::move( samplerInfo.createInfo ) ) ) }
	{
		auto & handler = resources->getHandler();
		auto mipLevels = std::max( 1u, imageInfo.mipLevels );
		auto layerCount = ( imageInfo.extent.depth > 1u ? 1u : imageInfo.layerCount );
		auto mbName = toUtf8( name );
		bool isTexture1D = texture::isTexture1D( imageInfo.extent );
		imageId = handler.createImageId( crg::ImageData{ mbName
			, ( imageInfo.createFlags
				| ( imageInfo.extent.depth > 1u
					? ImageCreateFlags::e2DArrayCompatible
					: ImageCreateFlags::eNone ) )
			, ( imageInfo.extent.depth > 1u
				? ImageType::e3D
				: ( isTexture1D
					? ImageType::e1D
					: ImageType::e2D ) )
			, texture::retrieveFormat( *device, imageInfo.format )
			, imageInfo.extent
			, ( imageInfo.usageFlags
				| ( mipLevels > 1u
					? ( ImageUsageFlags::eTransferSrc | ImageUsageFlags::eTransferDst )
					: ImageUsageFlags::eNone ) )
			, mipLevels
			, layerCount
			, imageInfo.sampleCount } );
		wholeViewId = handler.createViewId( crg::ImageViewData{ mbName + "Whole"
			, imageId
			, ImageViewCreateFlags::eNone
			, ( imageInfo.extent.depth > 1u
				? ImageViewType::e3D
				: ( isTexture1D
					? ( layerCount > 1u
						? ImageViewType::e1DArray
						: ImageViewType::e1D )
					: ( layerCount > 1u
						? ( ashes::checkFlag( imageInfo.createFlags, ImageCreateFlags::eCubeCompatible )
							? ( layerCount > 6u
								? ImageViewType::eCubeArray
								: ImageViewType::eCube )
							: ImageViewType::e2DArray )
						: ImageViewType::e2D ) ) )
			, imageInfo.format
			, { getAspectMask( imageInfo.format ), 0u, mipLevels, 0u, layerCount } } );

		if ( wholeViewId.data->info.viewType == ImageViewType::e3D )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.info.viewType = ImageViewType::e2DArray;
			createInfo.name = mbName + "Target";
			createInfo.info.subresourceRange.baseArrayLayer = 0u;
			createInfo.info.subresourceRange.layerCount = createInfo.image.data->info.extent.depth;
			createInfo.info.subresourceRange.baseMipLevel = 0u;
			createInfo.info.subresourceRange.levelCount = 1u;
			targetViewId = handler.createViewId( createInfo );
		}
		else if ( wholeViewId.data->info.subresourceRange.levelCount == wholeViewId.data->image.data->info.mipLevels )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.name = mbName + "Target";
			createInfo.info.subresourceRange.baseMipLevel = 0u;
			createInfo.info.subresourceRange.levelCount = 1u;
			targetViewId = handler.createViewId( createInfo );
		}
		else
		{
			targetViewId = wholeViewId;
		}

		if ( isDepthStencilFormat( wholeViewId.data->image.data->info.format ) )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.name = mbName + "Sampled";
			createInfo.info.subresourceRange.aspectMask = ImageAspectFlags::eDepth;
			sampledViewId = handler.createViewId( createInfo );
		}
		else
		{
			sampledViewId = wholeViewId;
		}

		if ( createSubviews )
		{
			auto sliceLayerCount = std::max( imageInfo.extent.depth, layerCount );

			for ( uint32_t index = 0u; index < sliceLayerCount; ++index )
			{
				subViewsId.push_back( handler.createViewId( crg::ImageViewData{ mbName + "Sub" + string::toMbString( index )
					, imageId
					, ImageViewCreateFlags::eNone
					, ( isTexture1D ? ImageViewType::e1D : ImageViewType::e2D )
					, imageInfo.format
					, { getAspectMask( imageInfo.format ), 0u, 1u, index, 1u } } ) );
			}
		}
	}

	Texture::~Texture()noexcept
	{
		CU_Require( image == nullptr && wholeView == nullptr );
	}

	void Texture::create()
	{
		if ( !device || !resources || image )
		{
			return;
		}

		auto & context = device->makeContext();
		
		image = makeRawUnique< ashes::Image >( **device
			, imageId.data->name
			, resources->createImage( context, imageId )
			, ashes::ImageCreateInfo{ convert( imageId.data->info ) } );
		wholeView = resources->createImageView( context, wholeViewId );

		if ( wholeViewId != targetViewId )
		{
			targetView = resources->createImageView( context, targetViewId );
		}
		else
		{
			targetView = wholeView;
		}

		if ( wholeViewId != sampledViewId )
		{
			sampledView = resources->createImageView( context, sampledViewId );
		}
		else
		{
			sampledView = wholeView;
		}

		for ( auto subViewId : subViewsId )
		{
			subViews.push_back( resources->createImageView( context, subViewId ) );
		}
	}

	void Texture::destroy()noexcept
	{
		if ( !device || !resources )
		{
			return;
		}

		for ( auto subViewId : subViewsId )
		{
			resources->destroyImageView( subViewId );
		}

		subViewsId.clear();

		if ( wholeViewId != sampledViewId )
		{
			resources->destroyImageView( sampledViewId );
			sampledView = VkImageView{};
		}

		if ( wholeViewId != targetViewId )
		{
			resources->destroyImageView( targetViewId );
			targetView = VkImageView{};
		}

		resources->destroyImageView( wholeViewId );
		wholeView = VkImageView{};
		resources->destroyImage( imageId );
		image = nullptr;
	}

	VkImageMemoryBarrier Texture::makeGeneralLayout( ImageLayout srcLayout
		, AccessFlags dstAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::eGeneral
			, getAccessMask( srcLayout )
			, dstAccessFlags
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeTransferDestination( ImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::eTransferDst
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeTransferSource( ImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::eTransferSrc
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeShaderInputResource( ImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::eShaderReadOnly
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeDepthStencilReadOnly( ImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::eDepthStencilReadOnly
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeColourAttachment( ImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::eColorAttachment
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeDepthStencilAttachment( ImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::eDepthStencilAttachment
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makePresentSource( ImageLayout srcLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, ImageLayout::ePresentSrc
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeLayoutTransition( ImageLayout srcLayout
		, ImageLayout dstLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, dstLayout
			, getAccessMask( srcLayout )
			, getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeLayoutTransition( ImageLayout srcLayout
		, ImageLayout dstLayout
		, AccessFlags srcAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeLayoutTransition( srcLayout
			, dstLayout
			, srcAccessFlags
			, getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily
			, target );
	}

	VkImageMemoryBarrier Texture::makeLayoutTransition( ImageLayout srcLayout
		, ImageLayout dstLayout
		, AccessFlags srcAccessFlags
		, AccessFlags dstAccessMask
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily
		, bool target )const
	{
		return makeVkStruct< VkImageMemoryBarrier >( getAccessFlags( srcAccessFlags )
			, getAccessFlags( dstAccessMask )
			, convert( srcLayout )
			, convert( dstLayout )
			, srcQueueFamily
			, dstQueueFamily
			, *image
			, convert( target
				? targetViewId.data->info.subresourceRange
				: sampledViewId.data->info.subresourceRange ) );
	}

	//*********************************************************************************************
}
