#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, TransparentPassResult )

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( WbTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	castor::String getTexName( WbTexture texture )
	{
		static std::array< castor::String, size_t( WbTexture::eCount ) > Values
		{
			{
				"Depth",
				"Accumulation",
				"Revealage",
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getFormat( RenderDevice const & device
		, WbTexture texture )
	{
		static std::array< VkFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_FORMAT_D16_UNORM,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( WbTexture texture )
	{
		static std::array< VkClearValue, size_t( WbTexture::eCount ) > Values
		{
			{
				defaultClearDepthStencil,
				transparentBlackClearColor,
				opaqueWhiteClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( WbTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( WbTexture texture )
	{
		static std::array< VkBorderColor, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	TransparentPassResult::TransparentPassResult( crg::ResourceHandler & handler
		, RenderDevice const & device
		, TexturePtr depthTexture )
		: GBufferT< WbTexture >{ handler
			, device
			, cuT( "WBResult" )
			, { depthTexture, nullptr, nullptr }
			, 0u
			, makeSize( getExtent( depthTexture->imageId ) ) }
	{
	}

	//*********************************************************************************************
}
