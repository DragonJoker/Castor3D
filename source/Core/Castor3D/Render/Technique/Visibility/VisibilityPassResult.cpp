#include "Castor3D/Render/Technique/Visibility/VisibilityPassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

CU_ImplementCUSmartPtr( castor3d, VisibilityPassResult )

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( VbTexture texture )
	{
		return cuT( "c3d_map" ) + getTexName( texture );
	}

	castor::String getTexName( VbTexture texture )
	{
		static std::array< castor::String, size_t( VbTexture::eCount ) > Values
		{
			{
				cuT( "Depth" ),
				cuT( "Data" ),
			}
		};

		return Values[size_t( texture )];
	}
	
	VkFormat getFormat( RenderDevice const & device, VbTexture texture )
	{
		static std::array< VkFormat, size_t( VbTexture::eCount ) > Values
		{
			{
				device.selectSuitableDepthStencilFormat( VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT
					| VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
					| VK_FORMAT_FEATURE_TRANSFER_SRC_BIT ),
				VK_FORMAT_R32G32B32A32_UINT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( VbTexture texture )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static std::array< VkClearValue, size_t( VbTexture::eCount ) > Values
		{
			{
				makeClearValue( 1.0f, 0u ),
				transparentBlackClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( VbTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( VbTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( VbTexture texture )
	{
		static std::array< VkBorderColor, size_t( VbTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
				VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	VisibilityPassResult::VisibilityPassResult( crg::ResourceHandler & handler
		, RenderDevice const & device
		, TexturePtr depthTexture )
		: GBufferT< VbTexture >{ handler
			, device
			, cuT( "VBResult" )
			, { depthTexture, nullptr }
			, 0u
			, makeSize( getExtent( depthTexture->imageId ) ) }
	{
	}

	//*********************************************************************************************
}
