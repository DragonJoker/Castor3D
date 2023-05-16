#include "Castor3D/Render/Opaque/Lighting/LightPassResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( LpTexture texture )
	{
		return cuT( "c3d_mapLight" ) + getTexName( texture );
	}

	castor::String getTexName( LpTexture texture )
	{
		static std::array< castor::String, size_t( LpTexture::eCount ) > Values
		{
			{
				cuT( "Diffuse" ),
				cuT( "Specular" ),
				cuT( "Scattering" ),
				cuT( "IndirectDiffuse" ),
				cuT( "IndirectSpecular" ),
			}
		};
		return Values[size_t( texture )];
	}

	VkFormat getFormat( RenderDevice const & device, LpTexture texture )
	{
		static std::array< VkFormat, size_t( LpTexture::eCount ) > Values
		{
			{
				device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( getUsageFlags( LpTexture::eDiffuse ) ) ),
				device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( getUsageFlags( LpTexture::eSpecular ) ) ),
				device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( getUsageFlags( LpTexture::eScattering ) ) ),
				device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( getUsageFlags( LpTexture::eIndirectDiffuse ) ) ),
				device.selectSmallestFormatRGBUFloatFormat( getFeatureFlags( getUsageFlags( LpTexture::eIndirectSpecular ) ) ),
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( LpTexture texture )
	{
		static std::array< VkClearValue, size_t( LpTexture::eCount ) > Values
		{
			{
				opaqueBlackClearColor,
				opaqueBlackClearColor,
				opaqueBlackClearColor,
				opaqueBlackClearColor,
				opaqueBlackClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( LpTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( LpTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( LpTexture texture )
	{
		static std::array< VkBorderColor, size_t( LpTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	LightPassResult::LightPassResult( crg::ResourcesCache & handler
		, RenderDevice const & device
		, castor::Size const & size )
		: GBufferT< LpTexture >{ handler
			, device
			, cuT( "LPResult" )
			, { nullptr, nullptr, nullptr, nullptr }
			, 0u
			, size }
	{
	}

	//*********************************************************************************************
}
