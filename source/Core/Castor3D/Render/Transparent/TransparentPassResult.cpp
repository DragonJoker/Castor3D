#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

CU_ImplementSmartPtr( castor3d, TransparentPassResult )

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( WbTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	castor::String getTexName( WbTexture texture )
	{
		static castor::Array< castor::String, size_t( WbTexture::eCount ) > Values
		{
			{
				cuT( "Accumulation" ),
				cuT( "Revealage" ),
			}
		};

		return Values[size_t( texture )];
	}

	castor::PixelFormat getFormat( RenderDevice const &, WbTexture texture )
	{
		static castor::Array< castor::PixelFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				castor::PixelFormat::eR16G16B16A16_SFLOAT,
				castor::PixelFormat::eR16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( WbTexture texture )
	{
		static castor::Array< VkClearValue, size_t( WbTexture::eCount ) > Values
		{
			{
				transparentBlackClearColor,
				opaqueWhiteClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( WbTexture texture )
	{
		static castor::Array< VkImageUsageFlags, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	BorderColour getBorderColor( WbTexture texture )
	{
		static castor::Array< BorderColour, size_t( WbTexture::eCount ) > Values
		{
			{
				BorderColour::eFloatTransparentBlack,
				BorderColour::eFloatOpaqueWhite,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	TransparentPassResult::TransparentPassResult( crg::ResourcesCache & handler
		, RenderDevice const & device
		, castor::Size const & size )
		: GBufferT< WbTexture >{ handler
			, device
			, cuT( "WBResult" )
			, { nullptr, nullptr }
			, 0u
			, size }
	{
	}

	//*********************************************************************************************
}
