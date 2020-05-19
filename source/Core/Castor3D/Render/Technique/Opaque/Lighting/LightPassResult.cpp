#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( LpTexture texture )
	{
		return cuT( "c3d_mapLight" ) + getName( texture );
	}

	castor::String getName( LpTexture texture )
	{
		static std::array< String, size_t( LpTexture::eCount ) > Values
		{
			{
				cuT( "Depth" ),
				cuT( "Diffuse" ),
				cuT( "Specular" ),
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getFormat( LpTexture texture )
	{
		static std::array< VkFormat, size_t( LpTexture::eCount ) > Values
		{
			{
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( LpTexture texture )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static auto const rgb32fMaxColor{ ashes::makeClearValue( VkClearColorValue{ component, component, component, component } ) };
		static std::array< VkClearValue, size_t( LpTexture::eCount ) > Values
		{
			{
				defaultClearDepthStencil,
				opaqueBlackClearColor,
				opaqueBlackClearColor
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( LpTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( LpTexture texture )
	{
		static std::array< VkBorderColor, size_t( LpTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	LightPassResult::LightPassResult( Engine & engine
		, castor::Size const & size )
		: GBufferT< LpTexture >
		{
			engine,
			cuT( "LPResult" ),
			{ nullptr, nullptr, nullptr },
			0u,
			{ size.getWidth(), size.getHeight() },
			1u,
		}
	{
	}

	//*********************************************************************************************
}
