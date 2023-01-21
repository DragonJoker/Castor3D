#include "Castor3D/Render/Prepass/PrepassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, PrepassResult )

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( PpTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	castor::String getTexName( PpTexture texture )
	{
		static std::array< castor::String, size_t( PpTexture::eCount ) > Values
		{
			{
				cuT( "Depth" ),
				cuT( "DepthObj" ),
			}
		};

		return Values[size_t( texture )];
	}
	
	VkFormat getFormat( RenderDevice const & device, PpTexture texture )
	{
		static std::array< VkFormat, size_t( PpTexture::eCount ) > Values
		{
			{
				device.selectSuitableDepthStencilFormat( getFeatureFlags( getUsageFlags( texture ) ) ),
				VK_FORMAT_R32G32B32A32_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( PpTexture texture )
	{
		static std::array< VkClearValue, size_t( PpTexture::eCount ) > Values
		{
			{
				defaultClearDepthStencil,
				makeClearValue( 1.0f, std::numeric_limits< float >::max(), 0.0f, 0.0f ),
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( PpTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( PpTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( PpTexture texture )
	{
		static std::array< VkBorderColor, size_t( PpTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	PrepassResult::PrepassResult( crg::ResourcesCache & handler
		, RenderDevice const & device
		, castor::Size const & size )
		: GBufferT< PpTexture >{ handler
			, device
			, cuT( "PPResult" )
			, { nullptr, nullptr }
			, 0u
			, size }
	{
	}

	//*********************************************************************************************
}
