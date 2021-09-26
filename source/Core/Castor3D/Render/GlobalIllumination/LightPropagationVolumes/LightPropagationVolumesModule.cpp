#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"

CU_ImplementCUSmartPtr( castor3d, LightPropagationVolumes )
CU_ImplementCUSmartPtr( castor3d, LightPropagationVolumesG )
CU_ImplementCUSmartPtr( castor3d, LayeredLightPropagationVolumes )
CU_ImplementCUSmartPtr( castor3d, LayeredLightPropagationVolumesG )

namespace castor3d
{
	castor::String getTextureName( LpvTexture texture
		, std::string const & infix )
	{
		return cuT( "c3d_mapLpv" ) + infix + getName( texture );
	}

	castor::String getName( LpvTexture texture )
	{
		static std::array< castor::String, size_t( LpvTexture::eCount ) > Values
		{
			{
				cuT( "R" ),
				cuT( "G" ),
				cuT( "B" ),
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getFormat( LpvTexture texture )
	{
		static std::array< VkFormat, size_t( LpvTexture::eCount ) > Values
		{
			{
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( LpvTexture texture )
	{
		static std::array< VkClearValue, size_t( LpvTexture::eCount ) > Values
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
		static std::array< VkImageUsageFlags, size_t( LpvTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( LpvTexture texture )
	{
		static std::array< VkBorderColor, size_t( LpvTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			}
		};
		return Values[size_t( texture )];
	}

}
