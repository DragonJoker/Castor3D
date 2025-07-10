#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"

CU_ImplementSmartPtr( castor3d, LightPropagationVolumes )
CU_ImplementSmartPtr( castor3d, LightPropagationVolumesG )
CU_ImplementSmartPtr( castor3d, LayeredLightPropagationVolumes )
CU_ImplementSmartPtr( castor3d, LayeredLightPropagationVolumesG )

namespace castor3d
{
	castor::String getTextureName( LpvTexture texture
		, castor::String const & infix )
	{
		return cuT( "c3d_mapLpv" ) + infix + getTexName( texture );
	}

	castor::String getTexName( LpvTexture texture )
	{
		static castor::Array< castor::String, size_t( LpvTexture::eCount ) > Values
		{
			{
				cuT( "R" ),
				cuT( "G" ),
				cuT( "B" ),
			}
		};

		return Values[size_t( texture )];
	}

	castor::PixelFormat getFormat( RenderDevice const &, LpvTexture texture )
	{
		static castor::Array< castor::PixelFormat, size_t( LpvTexture::eCount ) > Values
		{
			{
				castor::PixelFormat::eR16G16B16A16_SFLOAT,
				castor::PixelFormat::eR16G16B16A16_SFLOAT,
				castor::PixelFormat::eR16G16B16A16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( LpvTexture texture )
	{
		static castor::Array< VkClearValue, size_t( LpvTexture::eCount ) > Values
		{
			{
				transparentBlackClearColor,
				transparentBlackClearColor,
				transparentBlackClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( LpvTexture texture )
	{
		static castor::Array< VkImageUsageFlags, size_t( LpvTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	BorderColour getBorderColor( LpvTexture texture )
	{
		static castor::Array< BorderColour, size_t( LpvTexture::eCount ) > Values
		{
			{
				BorderColour::eFloatOpaqueBlack,
				BorderColour::eFloatOpaqueBlack,
				BorderColour::eFloatOpaqueBlack,
			}
		};
		return Values[size_t( texture )];
	}

}
