#include "Castor3D/Render/Opaque/OpaquePassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, OpaquePassResult )

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( DsTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	castor::String getImageName( DsTexture texture )
	{
		return cuT( "c3d_img" ) + getTexName( texture );
	}

	castor::String getTexName( DsTexture texture )
	{
		static std::array< castor::String, size_t( DsTexture::eCount ) > Values
		{
			{
				cuT( "NmlOcc" ),
				cuT( "ColMtl" ),
				cuT( "SpcRgh" ),
				cuT( "EmsTrn" ),
				cuT( "ClrCot" ),
				cuT( "CrTsIr" ),
				cuT( "Sheen" ),
			}
		};

		return Values[size_t( texture )];
	}
	
	VkFormat getFormat( RenderDevice const & device, DsTexture texture )
	{
		static std::array< VkFormat, size_t( DsTexture::eCount ) > Values
		{
			{
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R8G8B8A8_UNORM,
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
		static std::array< VkClearValue, size_t( DsTexture::eCount ) > Values
		{
			{
				transparentBlackClearColor,
				transparentBlackClearColor,
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
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( DsTexture texture )
	{
		static std::array< VkBorderColor, size_t( DsTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
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

	OpaquePassResult::OpaquePassResult( crg::ResourcesCache & handler
		, RenderDevice const & device
		, TexturePtr nmlOccTexture )
		: GBufferT< DsTexture >{ handler
			, device
			, cuT( "GPResult" )
			, { nmlOccTexture, nullptr, nullptr, nullptr }
			, 0u
			, makeSize( getExtent( nmlOccTexture->imageId ) ) }
	{
	}

	//*********************************************************************************************
}
