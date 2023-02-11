#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( LightType light
		, SmTexture texture )
	{
		return cuT( "c3d_" ) + getName( light ) + getTexName( texture );
	}

	castor::String getTexName( SmTexture texture )
	{
		static std::array< castor::String, size_t( SmTexture::eCount ) > Values
		{
			{
				cuT( "Depth" ),
				cuT( "Linear" ),
				cuT( "Variance" ),
				cuT( "Normal" ),
				cuT( "Position" ),
				cuT( "Flux" ),
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getFormat( RenderDevice const & device, SmTexture texture )
	{
		static std::array< VkFormat, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_FORMAT_D16_UNORM,							// Depth
				VK_FORMAT_R32_SFLOAT,							// Linear
				VK_FORMAT_R32G32_SFLOAT,						// Variance
				device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( getUsageFlags( SmTexture::eNormal ) ) ),	// Normal
				device.selectSmallestFormatRGBSFloatFormat( getFeatureFlags( getUsageFlags( SmTexture::ePosition ) ) ),	// Position
				device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( getUsageFlags( SmTexture::eFlux ) ) ),	// Flux
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( SmTexture texture )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static std::array< VkClearValue, size_t( SmTexture::eCount ) > Values
		{
			{
				defaultClearDepthStencil,		// Depth
				makeClearValue( component ),	// Linear
				opaqueWhiteClearColor,			// Variance
				transparentBlackClearColor,		// Normal
				transparentBlackClearColor,		// Position
				transparentBlackClearColor		// Flux
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( SmTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,	// Depth
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,			// Linear
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,			// Variance
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,			// Normal
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,			// Position
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,			// Flux
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( SmTexture texture )
	{
		static std::array< VkBorderColor, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,			// Depth
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,			// Linear
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,			// Variance
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,	// Normal
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,	// Position
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,	// Flux
			}
		};
		return Values[size_t( texture )];
	}

	uint32_t getMipLevels( RenderDevice const & device
		, SmTexture texture
		, castor::Size const & size )
	{
		return texture == SmTexture::eVariance
			? getMipLevels( VkExtent3D{ size.getWidth(), size.getHeight(), 1u }
				, getFormat( device, texture ) )
			: 1u;
	}

	//*********************************************************************************************

	ShadowMapResult::ShadowMapResult( crg::ResourcesCache & resources
		, RenderDevice const & device
		, castor::String const & prefix
		, VkImageCreateFlags createFlags
		, castor::Size const & size
		, uint32_t layerCount )
		: GBufferT< SmTexture >{ resources
			, device
			, prefix + cuT( "/SMRes" )
			, { nullptr, nullptr, nullptr, nullptr, nullptr }
			, createFlags
			, size
			, layerCount }
	{
	}

	//*********************************************************************************************
}
