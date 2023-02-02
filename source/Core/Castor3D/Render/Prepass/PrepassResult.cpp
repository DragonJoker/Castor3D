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
				cuT( "DepthObj" ),
				cuT( "Visibility" ),
			}
		};

		return Values[size_t( texture )];
	}
	
	VkFormat getFormat( RenderDevice const & device, PpTexture texture )
	{
		static std::array< VkFormat, size_t( PpTexture::eCount ) > Values
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
		static std::array< VkClearValue, size_t( PpTexture::eCount ) > Values
		{
			{
				makeClearValue( 1.0f, std::numeric_limits< float >::max(), 0.0f, 0.0f ),
				makeClearValue( 0u, 0u, 0u, 0u ),
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( PpTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( PpTexture::eCount ) > Values
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
		static std::array< VkBorderColor, size_t( PpTexture::eCount ) > Values
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
		: GBufferT< PpTexture >{ resources
			, device
			, cuT( "PPResult" )
			, { nullptr, ( needsVisibility ?  nullptr : std::make_shared< Texture >() ) }
			, 0u
			, size }
	{
	}

	//*********************************************************************************************
}
