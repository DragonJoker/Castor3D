#include "Castor3D/Render/Prepass/PrepassResult.hpp"

#include "Castor3D/Render/RenderDevice.hpp"

CU_ImplementSmartPtr( castor3d, PrepassResult )

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( PpTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	castor::String getTexName( PpTexture texture )
	{
		static castor::Array< castor::String, size_t( PpTexture::eCount ) > Values
		{
			{
				cuT( "DepthObj" ),
				cuT( "Visibility" ),
			}
		};

		return Values[size_t( texture )];
	}
	
	VkFormat getFormat( RenderDevice const &, PpTexture texture )
	{
		static castor::Array< VkFormat, size_t( PpTexture::eCount ) > Values
		{
			{
				VK_FORMAT_R32G32B32A32_SFLOAT,
				VK_FORMAT_R32G32_UINT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( PpTexture texture )
	{
		static castor::Array< VkClearValue, size_t( PpTexture::eCount ) > Values
		{
			{
				transparentBlackClearColor,
				makeClearValue( 0u, 0u, 0u, 0u ),
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( PpTexture texture )
	{
		static castor::Array< VkImageUsageFlags, size_t( PpTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( PpTexture texture )
	{
		static castor::Array< VkBorderColor, size_t( PpTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
				VK_BORDER_COLOR_INT_OPAQUE_BLACK,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	PrepassResult::PrepassResult( crg::ResourcesCache & resources
		, RenderDevice const & device
		, castor::Size const & size
		, bool needsVisibility )
		: TextureHolder{ needsVisibility ? nullptr : castor::makeUnique< Texture >() }
		, GBufferT< PpTexture >{ resources
			, device
			, cuT( "PPResult" )
			, { nullptr, TextureHolder::getData().get() }
			, 0u
			, size }
	{
	}

	//*********************************************************************************************
}
