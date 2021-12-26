#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, OpaquePassResult )

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( DsTexture texture )
	{
		return cuT( "c3d_map" ) + getName( texture );
	}

	castor::String getName( DsTexture texture )
	{
		static std::array< castor::String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "Data0" ),
				cuT( "Data1" ),
				cuT( "Data2" ),
				cuT( "Data3" ),
				cuT( "Data4" ),
				cuT( "Data5" ),
			}
		};

		return Values[size_t( texture )];
	}
	
	VkFormat getFormat( DsTexture texture )
	{
		static std::array< VkFormat, size_t( DsTexture::eCount ) > Values
		{
			{
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( DsTexture texture )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static std::array< VkClearValue, size_t( DsTexture::eCount ) > Values
		{
			{
				makeClearValue( 1.0f, component, 0.0f, 0.0f ),
				transparentBlackClearColor,
				transparentBlackClearColor,
				transparentBlackClearColor,
				transparentBlackClearColor,
				transparentBlackClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( DsTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( DsTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( DsTexture texture )
	{
		static std::array< VkBorderColor, size_t( DsTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	OpaquePassResult::OpaquePassResult( crg::ResourceHandler & handler
		, RenderDevice const & device
		, TexturePtr depthTexture
		, TexturePtr data1Texture
		, TexturePtr velocityTexture )
		: GBufferT< DsTexture >{ handler
			, device
			, cuT( "GPResult" )
			, { depthTexture, data1Texture, nullptr, nullptr, nullptr, velocityTexture }
			, 0u
			, makeSize( getExtent( depthTexture->imageId ) ) }
	{
	}

	//*********************************************************************************************
}
