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
				cuT( "Depth" ),
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
				VK_FORMAT_D32_SFLOAT,
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
		static auto const rgb32fMaxColor{ ashes::makeClearValue( VkClearColorValue{ component, component, component, component } ) };
		static std::array< VkClearValue, size_t( DsTexture::eCount ) > Values
		{
			{
				defaultClearDepthStencil,
				opaqueBlackClearColor,
				opaqueBlackClearColor,
				opaqueBlackClearColor,
				opaqueBlackClearColor,
				opaqueBlackClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( DsTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( DsTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
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

	OpaquePassResult::OpaquePassResult( crg::FrameGraph & graph
		, RenderDevice const & device
		, crg::ImageId const & depthTexture
		, crg::ImageId const & velocityTexture )
		: GBufferT< DsTexture >{ graph
			, device
			, cuT( "GPResult" )
			, { &depthTexture, nullptr, nullptr, nullptr, nullptr, &velocityTexture }
			, 0u
			, castor::Size{ getExtent( depthTexture ).width, getExtent( depthTexture ).height } }
	{
	}

	//*********************************************************************************************
}
