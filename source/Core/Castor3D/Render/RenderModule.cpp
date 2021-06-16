#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementExportedOwnedBy( castor3d::RenderSystem, RenderSystem )
CU_ImplementExportedOwnedBy( castor3d::RenderDevice, RenderDevice )

namespace castor3d
{
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

	castor::String getName( FrustumCorner value )
	{
		switch ( value )
		{
		case FrustumCorner::eFarLeftBottom:
			return cuT( "far_left_bottom" );
		case FrustumCorner::eFarLeftTop:
			return cuT( "far_left_top" );
		case FrustumCorner::eFarRightTop:
			return cuT( "far_right_top" );
		case FrustumCorner::eFarRightBottom:
			return cuT( "far_right_bottom" );
		case FrustumCorner::eNearLeftBottom:
			return cuT( "near_left_bottom" );
		case FrustumCorner::eNearLeftTop:
			return cuT( "near_left_top" );
		case FrustumCorner::eNearRightTop:
			return cuT( "near_right_top" );
		case FrustumCorner::eNearRightBottom:
			return cuT( "near_right_bottom" );
		default:
			CU_Failure( "Unsupported FrustumCorner" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( FrustumPlane value )
	{
		switch ( value )
		{
		case FrustumPlane::eNear:
			return cuT( "near" );
		case FrustumPlane::eFar:
			return cuT( "far" );
		case FrustumPlane::eLeft:
			return cuT( "left" );
		case FrustumPlane::eRight:
			return cuT( "right" );
		case FrustumPlane::eTop:
			return cuT( "top" );
		case FrustumPlane::eBottom:
			return cuT( "bottom" );
		default:
			CU_Failure( "Unsupported FrustumPlane" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( PickNodeType value )
	{
		switch ( value )
		{
		case PickNodeType::eNone:
			return cuT( "none" );
		case PickNodeType::eStatic:
			return cuT( "static" );
		case PickNodeType::eInstantiatedStatic:
			return cuT( "instantiated_static" );
		case PickNodeType::eSkinning:
			return cuT( "skinning" );
		case PickNodeType::eInstantiatedSkinning:
			return cuT( "instantiated_skinning" );
		case PickNodeType::eMorphing:
			return cuT( "morphing" );
		case PickNodeType::eBillboard:
			return cuT( "billboard" );
		default:
			CU_Failure( "Unsupported PickNodeType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( TargetType value )
	{
		switch ( value )
		{
		case TargetType::eWindow:
			return cuT( "window" );
		case TargetType::eTexture:
			return cuT( "texture" );
		default:
			CU_Failure( "Unsupported TargetType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( ViewportType value )
	{
		switch ( value )
		{
		case ViewportType::eOrtho:
			return cuT( "ortho" );
		case ViewportType::ePerspective:
			return cuT( "perspective" );
		case ViewportType::eFrustum:
			return cuT( "frustum" );
		default:
			CU_Failure( "Unsupported ViewportType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( RenderMode value )
	{
		switch ( value )
		{
		case RenderMode::eOpaqueOnly:
			return cuT( "opaque_only" );
		case RenderMode::eTransparentOnly:
			return cuT( "transparent_only" );
		case RenderMode::eBoth:
			return cuT( "both" );
		default:
			CU_Failure( "Unsupported RenderMode" );
			return castor::cuEmptyString;
		}
	}

	FilteredTextureFlags::const_iterator checkFlags( FilteredTextureFlags const & flags, TextureFlag flag )
	{
		auto it = std::find_if( flags.begin()
			, flags.end()
			, [flag]( FilteredTextureFlags::value_type const & lookup )
			{
				return checkFlag( lookup.second.flags, flag );
			} );
		return it;
	}

	TextureFlagsArray::const_iterator checkFlags( TextureFlagsArray const & flags, TextureFlag flag )
	{
		auto it = std::find_if( flags.begin()
			, flags.end()
			, [flag]( TextureFlagsId const & lookup )
			{
				return checkFlag( lookup.flags, flag );
			} );
		return it;
	}

	TextureFlags merge( TextureFlagsArray const & flags )
	{
		TextureFlags result = TextureFlag::eNone;

		for ( auto flag : flags )
		{
			result |= flag.flags;
		}

		return result;
	}

	FilteredTextureFlags filterTexturesFlags( TextureFlagsArray const & textures
		, TextureFlags mask )
	{
		FilteredTextureFlags result;
		uint32_t index = 0u;

		for ( auto & flagId : textures )
		{
			if ( ( flagId.flags & mask ) != 0 )
			{
				result.emplace( index, flagId );
			}

			++index;
		}

		return result;
	}

	bool operator==( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return lhs.colourBlendMode == rhs.colourBlendMode
			&& lhs.alphaBlendMode == rhs.alphaBlendMode
			&& lhs.passFlags == rhs.passFlags
			&& lhs.heightMapIndex == rhs.heightMapIndex
			&& lhs.programFlags == rhs.programFlags
			&& lhs.sceneFlags == rhs.sceneFlags
			&& lhs.topology == rhs.topology
			&& lhs.alphaFunc == rhs.alphaFunc
			&& lhs.blendAlphaFunc == rhs.blendAlphaFunc
			&& lhs.textures == rhs.textures
			&& lhs.texturesFlags == rhs.texturesFlags;
	}

	//*********************************************************************************************

	Texture::Texture()
		: handler{}
		, device{}
		, imageId{}
		, image{}
		, wholeViewId{}
		, targetViewId{}
		, wholeView{}
		, targetView{}
		, subViewsId{}
		, sampler{}
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
					: 0u ) )
			, ( size.depth > 1u
				? VK_IMAGE_TYPE_3D
				: VK_IMAGE_TYPE_2D )
			, retrieveFormat( device, format )
			, size
			, ( usageFlags
				| ( mipLevels > 1u
					? ( VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT )
					: 0u ) )
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

		if ( createSubviews )
		{
			auto sliceLayerCount = std::max( size.depth, layerCount );
			auto data = *wholeViewId.data;

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
		SamplerSPtr c3dSampler;

		if ( engine.getSamplerCache().has( name ) )
		{
			c3dSampler = engine.getSamplerCache().find( name );
		}
		else
		{
			c3dSampler = engine.getSamplerCache().add( name );
			c3dSampler->setMinFilter( VK_FILTER_LINEAR );
			c3dSampler->setMagFilter( VK_FILTER_LINEAR );
			c3dSampler->setMipFilter( VK_SAMPLER_MIPMAP_MODE_LINEAR );
			c3dSampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			c3dSampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			c3dSampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			c3dSampler->setBorderColour( borderColor );
			c3dSampler->initialise( device );
		}

		sampler = &c3dSampler->getSampler();
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
		return { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER
			, nullptr
			, srcAccessFlags
			, dstAccessMask
			, srcLayout
			, dstLayout
			, srcQueueFamily
			, dstQueueFamily
			, image
			, ( target
				? targetViewId.data->info.subresourceRange
				: wholeViewId.data->info.subresourceRange ) };
	}

	//*********************************************************************************************

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeLayoutTransition( image
			, range
			, srcLayout
			, dstLayout
			, ashes::getAccessMask( srcLayout )
			, ashes::getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return makeLayoutTransition( image
			, range
			, srcLayout
			, dstLayout
			, srcAccessFlags
			, ashes::getAccessMask( dstLayout )
			, srcQueueFamily
			, dstQueueFamily );
	}

	VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, VkAccessFlags dstAccessMask
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily )
	{
		return { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER
			, nullptr
			, srcAccessFlags
			, dstAccessMask
			, srcLayout
			, dstLayout
			, srcQueueFamily
			, dstQueueFamily
			, image
			, range };
	}

	//*********************************************************************************************
}
