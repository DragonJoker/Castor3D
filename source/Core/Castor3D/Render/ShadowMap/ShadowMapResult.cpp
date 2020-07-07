#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( LightType light
		, SmTexture texture )
	{
		return cuT( "c3d_" ) + getName( light ) + getName( texture );
	}

	castor::String getName( SmTexture texture )
	{
		static std::array< String, size_t( SmTexture::eCount ) > Values
		{
			{
				cuT( "Depth" ),
				cuT( "NormalDepth" ),
				cuT( "Variance" ),
				cuT( "Position" ),
				cuT( "Flux" ),
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getFormat( SmTexture texture )
	{
		static std::array< VkFormat, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_FORMAT_X8_D24_UNORM_PACK32,	// Depth
				VK_FORMAT_R32G32B32A32_SFLOAT,	// NormalLinear
				VK_FORMAT_R32G32_SFLOAT,		// Variance
				VK_FORMAT_R16G16B16A16_SFLOAT,	// Position
				VK_FORMAT_R16G16B16A16_SFLOAT,	// Flux
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( SmTexture texture )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static auto const rgb32fMaxColor{ ashes::makeClearValue( VkClearColorValue{ component, component, component, component } ) };
		static std::array< VkClearValue, size_t( SmTexture::eCount ) > Values
		{
			{
				defaultClearDepthStencil,						// Depth
				makeClearValue( component, 0.0f, 0.0f, 0.0f ),	// NormalLinear
				rgb32fMaxColor,									// Variance
				opaqueBlackClearColor,							// Position
				opaqueBlackClearColor							// Flux
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( SmTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,						// Depth
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,	// NormalLinear
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,	// Variance
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,	// Position
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,	// Flux
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( SmTexture texture )
	{
		static std::array< VkBorderColor, size_t( SmTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,	// Depth
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,	// NormalLinear
				VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,	// Variance
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,	// Position
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,	// Flux
			}
		};
		return Values[size_t( texture )];
	}

	uint32_t getMipLevels( SmTexture texture, castor::Size const & size )
	{
		return texture == SmTexture::eVariance
			? getMipLevels( VkExtent3D{ size.getWidth(), size.getHeight() } )
			: 1u;
	}

	//*********************************************************************************************

	ShadowMapResult::ShadowMapResult( Engine & engine
		, castor::String const & prefix
		, VkImageCreateFlags createFlags
		, castor::Size const & size
		, uint32_t layerCount )
		: GBufferT< SmTexture >
		{
			engine,
			prefix + cuT( "SmResult" ),
			{ nullptr, nullptr, nullptr, nullptr, nullptr },
			createFlags,
			size,
			layerCount,
		}
	{
	}

	//*********************************************************************************************
}
