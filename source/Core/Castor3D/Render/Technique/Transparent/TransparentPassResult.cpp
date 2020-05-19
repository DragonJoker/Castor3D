#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( WbTexture texture )
	{
		return cuT( "c3d_map" ) + getName( texture );
	}

	String getName( WbTexture texture )
	{
		static std::array< String, size_t( WbTexture::eCount ) > Values
		{
			{
				"Depth",
				"Accumulation",
				"Revealage",
				"Velocity",
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getFormat( WbTexture texture )
	{
		static std::array< VkFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( WbTexture texture )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static auto const rgb32fMaxColor{ ashes::makeClearValue( VkClearColorValue{ component, component, component, component } ) };
		static std::array< VkClearValue, size_t( WbTexture::eCount ) > Values
		{
			{
				defaultClearDepthStencil,
				transparentBlackClearColor,
				opaqueWhiteClearColor,
				transparentBlackClearColor,
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( WbTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
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
				VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	TransparentPassResult::TransparentPassResult( Engine & engine
		, TextureUnit const & depthTexture
		, TextureUnit const & velocityTexture )
		: GBufferT< WbTexture >
		{
			engine,
			cuT( "WBResult" ),
			{ &depthTexture, nullptr, nullptr, &velocityTexture },
			0u,
			{ depthTexture.getTexture()->getDimensions().width, depthTexture.getTexture()->getDimensions().height },
			1u,
		}
	{
		initialise();
	}

	//*********************************************************************************************
}
