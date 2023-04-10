#include "Castor3D/Render/Transparent/TransparentPassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

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
		static std::array< castor::String, size_t( WbTexture::eCount ) > Values
		{
			{
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
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
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
