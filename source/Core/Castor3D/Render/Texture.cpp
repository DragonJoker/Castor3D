#include "Castor3D/Render/Texture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <RenderGraph/FramePassGroup.hpp>
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
			, TextureSamplerCreateInfo const & createInfo )
		{
			auto & engine = c3d::getEngine( device );
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

		static crg::ImageViewId makeTargetViewId( crg::ResourceHandler & handler
			, crg::ImageViewId wholeViewId
			, std::string const & mbName )
		{
			if ( wholeViewId.data->info.viewType == ImageViewType::e3D )
			{
				auto createInfo = *wholeViewId.data;
				createInfo.info.viewType = ImageViewType::e2DArray;
				createInfo.name = mbName + "Target";
				createInfo.info.subresourceRange.baseArrayLayer = 0u;
				createInfo.info.subresourceRange.layerCount = createInfo.image.data->info.extent.depth;
				createInfo.info.subresourceRange.baseMipLevel = 0u;
				createInfo.info.subresourceRange.levelCount = 1u;
				return handler.createViewId( createInfo );
			}

			if ( wholeViewId.data->info.subresourceRange.levelCount == wholeViewId.data->image.data->info.mipLevels )
			{
				auto createInfo = *wholeViewId.data;
				createInfo.name = mbName + "Target";
				createInfo.info.subresourceRange.baseMipLevel = 0u;
				createInfo.info.subresourceRange.levelCount = 1u;
				return handler.createViewId( createInfo );
			}

			return wholeViewId;
		}

		static crg::ImageViewId makeSampledViewId( crg::ResourceHandler & handler
			, crg::ImageViewId wholeViewId
			, std::string const & mbName )
		{
			if ( isDepthStencilFormat( wholeViewId.data->image.data->info.format ) )
			{
				auto createInfo = *wholeViewId.data;
				createInfo.name = mbName + "Sampled";
				createInfo.info.subresourceRange.aspectMask = ImageAspectFlags::eDepth;
				return handler.createViewId( createInfo );
			}
			if ( isStencilFormat( wholeViewId.data->image.data->info.format ) )
			{
				auto createInfo = *wholeViewId.data;
				createInfo.name = mbName + "Sampled";
				createInfo.info.subresourceRange.aspectMask = ImageAspectFlags::eStencil;
				return handler.createViewId( createInfo );
			}

			return wholeViewId;
		}
	}

	//*********************************************************************************************

	Texture::Texture( Texture && rhs )noexcept
		: resources{ c3d::move( rhs.resources ) }
		, device{ c3d::move( rhs.device ) }
		, imageId{ c3d::move( rhs.imageId ) }
		, image{ c3d::move( rhs.image ) }
		, sampler{ c3d::move( rhs.sampler ) }
		, m_wholeViewId{ c3d::move( rhs.m_wholeViewId ) }
		, m_targetViewId{ c3d::move( rhs.m_targetViewId ) }
		, m_sampledViewId{ c3d::move( rhs.m_sampledViewId ) }
		, m_wholeView{ c3d::move( rhs.m_wholeView ) }
		, m_targetView{ c3d::move( rhs.m_targetView ) }
		, m_sampledView{ c3d::move( rhs.m_sampledView ) }
		, m_attach{ c3d::move( rhs.m_attach ) }
		, m_layers{ c3d::move( rhs.m_layers ) }
		, m_ownImage{ rhs.m_ownImage }
	{
		rhs.device = {};
		rhs.resources = {};
		rhs.image = {};
		rhs.sampler = {};
		rhs.m_ownImage = {};
	}

	Texture & Texture::operator=( Texture && rhs )noexcept
	{
		resources = c3d::move( rhs.resources );
		device = c3d::move( rhs.device );
		imageId = c3d::move( rhs.imageId );
		image = c3d::move( rhs.image );
		sampler = c3d::move( rhs.sampler );
		m_wholeViewId = c3d::move( rhs.m_wholeViewId );
		m_targetViewId = c3d::move( rhs.m_targetViewId );
		m_sampledViewId = c3d::move( rhs.m_sampledViewId );
		m_wholeView = c3d::move( rhs.m_wholeView );
		m_targetView = c3d::move( rhs.m_targetView );
		m_sampledView = c3d::move( rhs.m_sampledView );
		m_attach = c3d::move( rhs.m_attach );
		m_layers = c3d::move( rhs.m_layers );
		m_ownImage = rhs.m_ownImage;

		rhs.device = {};
		rhs.resources = {};
		rhs.image = {};
		rhs.sampler = {};
		rhs.m_ownImage = {};

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
			: texture::getSampler( pdevice, samplerInfo.createInfo ) ) }
		, m_ownImage{ true }
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
		m_wholeViewId = handler.createViewId( crg::ImageViewData{ mbName + "Whole"
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
		m_targetViewId = texture::makeTargetViewId( handler, m_wholeViewId, mbName );
		m_sampledViewId = texture::makeSampledViewId( handler, m_wholeViewId, mbName );

		if ( createSubviews )
		{
			auto sliceLayerCount = std::max( imageInfo.extent.depth, layerCount );

			for ( uint32_t layer = 0u; layer < sliceLayerCount; ++layer )
			{
				auto & layerViews = m_layers.emplace_back();
				auto layerName = mbName + "Layer" + string::toMbString( layer );
				layerViews.wholeViewId = handler.createViewId( crg::ImageViewData{ layerName
					, imageId
					, ImageViewCreateFlags::eNone
					, ( isTexture1D ? ImageViewType::e1D : ImageViewType::e2D )
					, imageInfo.format
					, { getAspectMask( imageInfo.format ), 0u, imageInfo.mipLevels, layer, 1u } } );
				layerViews.targetViewId = texture::makeTargetViewId( handler, layerViews.wholeViewId, mbName );
				layerViews.sampledViewId = texture::makeSampledViewId( handler, layerViews.wholeViewId, mbName );

				for ( uint32_t level = 0u; level < imageInfo.mipLevels; ++level )
				{
					auto & mipViews = layerViews.mipViews.emplace_back();
					mipViews.targetViewId = handler.createViewId( crg::ImageViewData{ layerName + "Mip" + string::toMbString( level )
						, imageId
						, ImageViewCreateFlags::eNone
						, ( isTexture1D ? ImageViewType::e1D : ImageViewType::e2D )
						, imageInfo.format
						, { getAspectMask( imageInfo.format ), level, 1u, layer, 1u } } );
					mipViews.sampledViewId = texture::makeSampledViewId( handler, mipViews.targetViewId, mbName );
				}
			}
		}
	}

	Texture::Texture( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, crg::ImageViewId view )
		: Texture{ pdevice, presources, view, view, view }
	{
	}

	Texture::Texture( RenderDevice const & pdevice
		, crg::ResourcesCache & presources
		, crg::ImageViewId wholeView
		, crg::ImageViewId targetView
		, crg::ImageViewId sampledView )
		: resources{ &presources }
		, device{ &pdevice }
		, imageId{ wholeView.data->image }
		, m_wholeViewId{ wholeView }
		, m_targetViewId{ targetView }
		, m_sampledViewId{ sampledView }
		, m_ownImage{ false }
	{
		auto & context = device->makeContext();
		image = makeRawUnique< ashes::Image >( **device
			, imageId.data->name
			, resources->createImage( context, imageId )
			, ashes::ImageCreateInfo{ convert( imageId.data->info ) } );
		m_wholeView = resources->createImageView( context, m_wholeViewId );

		if ( m_wholeViewId != m_targetViewId )
			m_targetView = resources->createImageView( context, m_targetViewId );
		else
			m_targetView = m_wholeView;

		if ( m_wholeViewId != m_sampledViewId )
			m_sampledView = resources->createImageView( context, m_sampledViewId );
		else
			m_sampledView = m_wholeView;
	}

	Texture::~Texture()noexcept
	{
		CU_Require( !m_ownImage || ( image == nullptr && m_wholeView == nullptr ) );
	}

	void Texture::create()
	{
		if ( !device || !resources || image || !m_ownImage )
			return;

		auto & context = device->makeContext();
		
		image = makeRawUnique< ashes::Image >( **device
			, imageId.data->name
			, resources->createImage( context, imageId )
			, ashes::ImageCreateInfo{ convert( imageId.data->info ) } );
		m_wholeView = resources->createImageView( context, m_wholeViewId );

		if ( m_wholeViewId != m_targetViewId )
			m_targetView = resources->createImageView( context, m_targetViewId );
		else
			m_targetView = m_wholeView;

		if ( m_wholeViewId != m_sampledViewId )
			m_sampledView = resources->createImageView( context, m_sampledViewId );
		else
			m_sampledView = m_wholeView;

		for ( auto & layerViews : m_layers )
		{
			layerViews.wholeView = resources->createImageView( context, layerViews.wholeViewId );

			if ( layerViews.wholeViewId != layerViews.targetViewId )
				layerViews.targetView = resources->createImageView( context, layerViews.targetViewId );
			else
				layerViews.targetView = layerViews.wholeView;

			if ( layerViews.wholeViewId != layerViews.sampledViewId )
				layerViews.sampledView = resources->createImageView( context, layerViews.sampledViewId );
			else
				layerViews.sampledView = layerViews.wholeView;

			for ( auto & mipViews : layerViews.mipViews )
			{
				mipViews.targetView = resources->createImageView( context, mipViews.targetViewId );

				if ( mipViews.targetViewId != mipViews.sampledViewId )
					mipViews.sampledView = resources->createImageView( context, mipViews.sampledViewId );
				else
					mipViews.sampledView = mipViews.targetView;
			}
		}
	}

	void Texture::destroy()noexcept
	{
		if ( !device || !resources || !m_ownImage )
			return;

		for ( auto & layerViews : m_layers )
		{
			if ( layerViews.wholeViewId != layerViews.sampledViewId )
			{
				resources->destroyImageView( layerViews.sampledViewId );
				layerViews.sampledView = VkImageView{};
			}

			if ( layerViews.wholeViewId != layerViews.targetViewId )
			{
				resources->destroyImageView( layerViews.targetViewId );
				layerViews.targetView = VkImageView{};
			}

			resources->destroyImageView( layerViews.wholeViewId );

			for ( auto & mipViews : layerViews.mipViews )
			{
				if ( mipViews.targetViewId != mipViews.sampledViewId )
				{
					resources->destroyImageView( mipViews.sampledViewId );
					mipViews.sampledView = VkImageView{};
				}

				resources->destroyImageView( mipViews.targetViewId );
			}
		}

		m_layers.clear();

		if ( m_wholeViewId != m_sampledViewId )
		{
			resources->destroyImageView( m_sampledViewId );
			m_sampledView = VkImageView{};
		}

		if ( m_wholeViewId != m_targetViewId )
		{
			resources->destroyImageView( m_targetViewId );
			m_targetView = VkImageView{};
		}

		resources->destroyImageView( m_wholeViewId );
		m_wholeView = VkImageView{};
		resources->destroyImage( imageId );
		image = nullptr;
	}

	crg::Attachment const * Texture::mergeLayerAttachments( crg::FramePassGroup & graph )const
	{
		crg::AttachmentArray attachs;
		for ( auto & layerViews : m_layers )
			attachs.push_back( layerViews.attach );
		return graph.mergeAttachments( attachs );
	}

	crg::Attachment const * Texture::getSampledLastAttach( uint32_t layerIndex, uint32_t mipLevel )const
	{
		auto result = getLastAttach( layerIndex, mipLevel );
		if ( result && result->imageAttach.view() != getSampledViewId( layerIndex, mipLevel )
			&& getTargetViewId( layerIndex, mipLevel ) != getSampledViewId( layerIndex, mipLevel ) )
		{
			auto [it, inserted] = m_cache.try_emplace( ( ( layerIndex + 1u ) << 16u ) + ( mipLevel + 1u ), nullptr );
			if ( inserted )
				it->second = c3d::makeRawUnique< crg::Attachment >( getSampledViewId( layerIndex, mipLevel ), *result );
			return it->second.get();
		}
		return result;
	}

	crg::Attachment const * Texture::getSampledLastAttach( uint32_t layerIndex )const
	{
		auto result = getLastAttach( layerIndex );
		if ( result && result->imageAttach.view() != getSampledViewId( layerIndex )
			&& getTargetViewId( layerIndex ) != getSampledViewId( layerIndex ) )
		{
			auto [it, inserted] = m_cache.try_emplace( ( ( layerIndex + 1u ) << 16u ), nullptr );
			if ( inserted )
				it->second = c3d::makeRawUnique< crg::Attachment >( getSampledViewId( layerIndex ), *result );
			return it->second.get();
		}
		return result;
	}

	crg::Attachment const * Texture::getSampledLastAttach()const
	{
		auto result = getLastAttach();
		if ( result && result->imageAttach.view() != getSampledViewId()
			&& getTargetViewId() != getSampledViewId() )
		{
			auto [it, inserted] = m_cache.try_emplace( 0u, nullptr );
			if ( inserted )
				it->second = c3d::makeRawUnique< crg::Attachment >( getSampledViewId(), *result );
			return it->second.get();
		}
		return result;
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
				? m_targetViewId.data->info.subresourceRange
				: m_sampledViewId.data->info.subresourceRange ) );
	}

	//*********************************************************************************************
}
