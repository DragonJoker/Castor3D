#include "Castor3D/Render/RenderModule.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ashespp/Image/Image.hpp>

#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementExportedOwnedBy( castor3d::RenderSystem, RenderSystem )
CU_ImplementExportedOwnedBy( castor3d::RenderDevice, RenderDevice )

namespace castor3d
{
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
		case ViewportType::eUndefined:
			return cuT( "undefined" );
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

	ashes::Image makeImage( ashes::Device const & device
		, VkImage image
		, crg::ImageId data )
	{
		return ashes::Image{ device, image, data.data->info };
	}

	ashes::Image makeImage( ashes::Device const & device
		, Texture const & texture )
	{
		return makeImage( device
			, texture.image
			, texture.imageId );
	}

	ashes::ImageView makeImageView( ashes::Device const & device
		, ashes::Image const & image
		, VkImageView view
		, crg::ImageViewId data )
	{
		return ashes::ImageView{ data.data->info, view, &image };
	}

	ashes::ImageView makeTargetImageView( ashes::Device const & device
		, ashes::Image const & image
		, Texture const & texture )
	{
		return makeImageView( device
			, image
			, texture.targetView
			, texture.targetViewId );
	}

	ashes::ImageView makeSampledImageView( ashes::Device const & device
		, ashes::Image const & image
		, Texture const & texture )
	{
		return makeImageView( device
			, image
			, texture.sampledView
			, texture.sampledViewId );
	}

	ashes::ImageView makeWholeImageView( ashes::Device const & device
		, ashes::Image const & image
		, Texture const & texture )
	{
		return makeImageView( device
			, image
			, texture.wholeView
			, texture.wholeViewId );
	}

	bool operator==( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return lhs.colourBlendMode == rhs.colourBlendMode
			&& lhs.alphaBlendMode == rhs.alphaBlendMode
			&& lhs.passFlags == rhs.passFlags
			&& lhs.renderPassType == rhs.renderPassType
			&& lhs.passType == rhs.passType
			&& lhs.heightMapIndex == rhs.heightMapIndex
			&& lhs.programFlags == rhs.programFlags
			&& lhs.sceneFlags == rhs.sceneFlags
			&& lhs.topology == rhs.topology
			&& lhs.patchVertices == rhs.patchVertices
			&& lhs.alphaFunc == rhs.alphaFunc
			&& lhs.blendAlphaFunc == rhs.blendAlphaFunc
			&& lhs.textures == rhs.textures
			&& lhs.texturesFlags == rhs.texturesFlags;
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

	//*************************************************************************************************

	uint32_t getSafeBandsSize( castor::Size const & size )
	{
		return castor::getNextPowerOfTwo( std::min( size.getWidth()
			, size.getHeight() ) / 10u );
	}
	
	uint32_t getSafeBandSize( castor::Size const & size )
	{
		return getSafeBandsSize( size ) / 2u;
	}

	castor::Size getSafeBandedSize( castor::Size const & size )
	{
		auto bandsSize = getSafeBandsSize( size );
		return { size.getWidth() + bandsSize
			, size.getHeight() + bandsSize };
	}

	VkExtent3D getSafeBandedExtent3D( castor::Size const & size )
	{
		return makeExtent3D( getSafeBandedSize( size ) );
	}

	castor::Angle getSafeBandedFovY( castor::Angle const & fovY
		, castor::Size const & size )
	{
		auto bandsSize = double( getSafeBandsSize( size ) );
		auto ratio = bandsSize / size.getHeight();
		return fovY + ( fovY * ratio );
	}

	float getSafeBandedAspect( float aspect
		, castor::Size const & size )
	{
		auto bandsSize = double( getSafeBandsSize( size ) );
		return float( ( aspect * float( size.getHeight() ) + bandsSize ) / ( float( size.getHeight() ) + bandsSize ) );
	}

	//*********************************************************************************************
}
