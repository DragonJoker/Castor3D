#include "Castor3D/Render/Texture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/ResourceHandler.hpp>

#include <ashespp/Image/Image.hpp>

CU_ImplementSmartPtr( castor3d, Texture )

namespace castor3d
{
	//*********************************************************************************************

	namespace texture
	{
		static castor::PixelFormat getDepthFormat( RenderDevice const & device
			, castor::PixelFormat format )
		{
			castor::Vector< castor::PixelFormat > depthFormats
			{
				format,
				castor::PixelFormat::eD24_UNORM_S8_UINT,
				castor::PixelFormat::eD16_UNORM_S8_UINT,
			};
			return device.selectSuitableFormat( depthFormats
				, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		static castor::PixelFormat retrieveFormat( RenderDevice const & device
			, castor::PixelFormat format )
		{
			return isDepthOrStencilFormat( format )
				? getDepthFormat( device, format )
				: format;
		}

		static ashes::Sampler const * getSampler( RenderDevice const & device
			, FilterMode minFilter
			, FilterMode magFilter
			, MipmapMode mipFilter
			, WrapMode addressMode = WrapMode::eClampToEdge
			, BorderColour borderColor = BorderColour::eFloatTransparentBlack
			, ComparisonFunc compareOp = ComparisonFunc::eNever )
		{
			auto & engine = *device.renderSystem.getEngine();
			Sampler const * c3dSampler{};

			if ( auto splName = getSamplerName( compareOp
					, minFilter
					, magFilter
					, mipFilter
					, addressMode
					, addressMode
					, addressMode
					, borderColor );
				engine.hasSampler( splName ) )
			{
				c3dSampler = engine.findSampler( splName );
			}
			else
			{
				auto created = engine.createSampler( splName, engine );
				created->setMinFilter( minFilter );
				created->setMagFilter( magFilter );
				created->setMipFilter( mipFilter );
				created->setWrapS( addressMode );
				created->setWrapT( addressMode );
				created->setWrapR( addressMode );
				created->setBorderColour( borderColor );

				if ( compareOp != ComparisonFunc::eNever )
				{
					created->enableCompare( true );
					created->setCompareOp( compareOp );
				}

				created->initialise( device );
				c3dSampler = engine.addSampler( splName, created, false );
			}

			return &c3dSampler->getSampler();
		}

		static bool isTexture1D( VkExtent3D const & extent )
		{
			return extent.height == 1
				&& extent.width > 1;
		}
	}

	//*********************************************************************************************

	Texture::Texture( Texture && rhs )noexcept
		: resources{ castor::move( rhs.resources ) }
		, device{ castor::move( rhs.device ) }
		, imageId{ castor::move( rhs.imageId ) }
		, image{ castor::move( rhs.image ) }
		, wholeViewId{ castor::move( rhs.wholeViewId ) }
		, targetViewId{ castor::move( rhs.targetViewId ) }
		, sampledViewId{ castor::move( rhs.sampledViewId ) }
		, wholeView{ castor::move( rhs.wholeView ) }
		, targetView{ castor::move( rhs.targetView ) }
		, sampledView{ castor::move( rhs.sampledView ) }
		, subViewsId{ castor::move( rhs.subViewsId ) }
		, sampler{ castor::move( rhs.sampler ) }
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
		resources = castor::move( rhs.resources );
		device = castor::move( rhs.device );
		imageId = castor::move( rhs.imageId );
		image = castor::move( rhs.image );
		wholeViewId = castor::move( rhs.wholeViewId );
		targetViewId = castor::move( rhs.targetViewId );
		sampledViewId = castor::move( rhs.sampledViewId );
		wholeView = castor::move( rhs.wholeView );
		targetView = castor::move( rhs.targetView );
		sampledView = castor::move( rhs.sampledView );
		subViewsId = castor::move( rhs.subViewsId );
		sampler = castor::move( rhs.sampler );

		rhs.device = nullptr;
		rhs.resources = nullptr;
		rhs.image = nullptr;
		rhs.wholeView = VkImageView{};
		rhs.targetView = VkImageView{};
		rhs.sampledView = VkImageView{};

		return *this;
	}

	Texture::Texture( RenderDevice const & device
		, crg::ResourcesCache & resources
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, uint32_t mipLevels
		, castor::PixelFormat format
		, VkImageUsageFlags usageFlags
		, BorderColour borderColor
		, ComparisonFunc compareOp
		, bool createSubviews )
		: Texture{ device
			, resources
			, name
			, createFlags
			, size
			, layerCount
			, VK_SAMPLE_COUNT_1_BIT
			, mipLevels
			, format
			, usageFlags
			, texture::getSampler( device
				, FilterMode::eLinear
				, FilterMode::eLinear
				, MipmapMode::eLinear
				, WrapMode::eClampToEdge
				, borderColor
				, compareOp )
			, createSubviews }
	{
	}

	Texture::Texture( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
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
		, WrapMode addressMode
		, BorderColour borderColor
		, ComparisonFunc compareOp
		, bool createSubviews )
		: Texture{ pdevice
			, presources
			, name
			, createFlags
			, size
			, layerCount
			, VK_SAMPLE_COUNT_1_BIT
			, mipLevels
			, format
			, usageFlags
			, texture::getSampler( pdevice
				, minFilter
				, magFilter
				, mipFilter
				, addressMode
				, borderColor
				, compareOp )
			, createSubviews }
	{
	}
	
	Texture::Texture( RenderDevice const & device
		, crg::ResourcesCache & resources
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, uint32_t mipLevels
		, castor::PixelFormat format
		, VkImageUsageFlags usageFlags
		, ashes::Sampler const * sampler
		, bool createSubviews )
		: Texture{ device
			, resources
			, name
			, createFlags
			, size
			, layerCount
			, VK_SAMPLE_COUNT_1_BIT
			, mipLevels
			, format
			, usageFlags
			, sampler
			, createSubviews }
	{
	}

	Texture::Texture( RenderDevice const & device
		, crg::ResourcesCache & resources
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, VkSampleCountFlagBits sampleCount
		, uint32_t mipLevels
		, castor::PixelFormat format
		, VkImageUsageFlags usageFlags
		, BorderColour borderColor
		, ComparisonFunc compareOp
		, bool createSubviews )
		: Texture{ device
			, resources
			, name
			, createFlags
			, size
			, layerCount
			, sampleCount
			, mipLevels
			, format
			, usageFlags
			, texture::getSampler( device
				, FilterMode::eLinear
				, FilterMode::eLinear
				, MipmapMode::eLinear
				, WrapMode::eClampToEdge
				, borderColor
				, compareOp )
			, createSubviews }
	{
	}

	Texture::Texture( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
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
		, WrapMode addressMode
		, BorderColour borderColor
		, ComparisonFunc compareOp
		, bool createSubviews )
		: Texture{ pdevice
			, presources
			, name
			, createFlags
			, size
			, layerCount
			, sampleCount
			, mipLevels
			, format
			, usageFlags
			, texture::getSampler( pdevice
				, minFilter
				, magFilter
				, mipFilter
				, addressMode
				, borderColor
				, compareOp )
			, createSubviews }
	{
	}

	Texture::Texture( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, castor::String const & name
		, VkImageCreateFlags createFlags
		, VkExtent3D const & size
		, uint32_t layerCount
		, VkSampleCountFlagBits sampleCount
		, uint32_t mipLevels
		, castor::PixelFormat format
		, VkImageUsageFlags usageFlags
		, ashes::Sampler const * psampler
		, bool createSubviews )
		: resources{ &presources }
		, device{ &pdevice }
		, sampler{ psampler }
	{
		auto & handler = resources->getHandler();
		mipLevels = std::max( 1u, mipLevels );
		if ( size.depth > 1u )
			layerCount = 1u;
		auto mbName = castor::toUtf8( name );
		bool isTexture1D = texture::isTexture1D( size );
		imageId = handler.createImageId( crg::ImageData{ mbName
			, ( createFlags
				| ( size.depth > 1u
					? VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
					: VkImageCreateFlagBits{} ) )
			, ( size.depth > 1u
				? ImageType::e3D
				: ( isTexture1D
					? ImageType::e1D
					: ImageType::e2D ) )
			, texture::retrieveFormat( *device, format )
			, size
			, ( usageFlags
				| ( mipLevels > 1u
					? ( VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT )
					: VkImageUsageFlags{} ) )
			, mipLevels
			, layerCount
			, sampleCount } );
		wholeViewId = handler.createViewId( crg::ImageViewData{ mbName + "Whole"
			, imageId
			, 0u
			, ( size.depth > 1u
				? ImageViewType::e3D
				: ( isTexture1D
					? ( layerCount > 1u
						? ImageViewType::e1DArray
						: ImageViewType::e1D )
					: ( layerCount > 1u
						? ( ashes::checkFlag( createFlags, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT )
							? ( layerCount > 6u
								? ImageViewType::eCubeArray
								: ImageViewType::eCube )
							: ImageViewType::e2DArray )
						: ImageViewType::e2D ) ) )
			, format
			, { ashes::getAspectMask( convert( format ) ), 0u, mipLevels, 0u, layerCount } } );

		if ( wholeViewId.data->info.viewType == VK_IMAGE_VIEW_TYPE_3D )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
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

		if ( ashes::isDepthStencilFormat( wholeViewId.data->image.data->info.format ) )
		{
			auto createInfo = *wholeViewId.data;
			createInfo.name = mbName + "Sampled";
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
				subViewsId.push_back( handler.createViewId( crg::ImageViewData{ mbName + "Sub" + castor::string::toMbString( index )
					, imageId
					, 0u
					, ( isTexture1D ? ImageViewType::e1D : ImageViewType::e2D )
					, format
					, { ashes::getAspectMask( convert( format ) ), 0u, 1u, index, 1u } } ) );
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
		
		image = castor::make_unique< ashes::Image >( **device
			, imageId.data->name
			, resources->createImage( context, imageId )
			, ashes::ImageCreateInfo{ imageId.data->info } );
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
		return makeVkStruct< VkImageMemoryBarrier >( convert( srcAccessFlags )
			, convert( dstAccessMask )
			, convert( srcLayout )
			, convert( dstLayout )
			, srcQueueFamily
			, dstQueueFamily
			, *image
			, ( target
				? targetViewId.data->info.subresourceRange
				: sampledViewId.data->info.subresourceRange ) );
	}

	//*********************************************************************************************
}
