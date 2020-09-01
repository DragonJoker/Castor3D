#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightVolumePassResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include <ashespp/Command/CommandBuffer.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	castor::String getTextureName( LpvTexture texture
		, std::string const & infix )
	{
		return cuT( "c3d_mapLpv" ) + infix + getName( texture );
	}

	castor::String getName( LpvTexture texture )
	{
		static std::array< String, size_t( LpvTexture::eCount ) > Values
		{
			{
				cuT( "R" ),
				cuT( "G" ),
				cuT( "B" ),
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getFormat( LpvTexture texture )
	{
		static std::array< VkFormat, size_t( LpvTexture::eCount ) > Values
		{
			{
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};
		return Values[size_t( texture )];
	}

	VkClearValue getClearValue( LpvTexture texture )
	{
		static float constexpr component = std::numeric_limits< float >::max();
		static auto const rgb32fMaxColor{ ashes::makeClearValue( VkClearColorValue{ component, component, component, component } ) };
		static std::array< VkClearValue, size_t( LpvTexture::eCount ) > Values
		{
			{
				opaqueBlackClearColor,
				opaqueBlackClearColor,
				opaqueBlackClearColor
			}
		};
		return Values[size_t( texture )];
	}

	VkImageUsageFlags getUsageFlags( LpvTexture texture )
	{
		static std::array< VkImageUsageFlags, size_t( LpvTexture::eCount ) > Values
		{
			{
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			}
		};
		return Values[size_t( texture )];
	}

	VkBorderColor getBorderColor( LpvTexture texture )
	{
		static std::array< VkBorderColor, size_t( LpvTexture::eCount ) > Values
		{
			{
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
				VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			}
		};
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	LightVolumePassResult::LightVolumePassResult( Engine & engine
		, castor::String const & prefix
		, uint32_t size )
		: GBufferT< LpvTexture >
		{
			engine,
			prefix + cuT( "LPVResult" ),
			{ nullptr, nullptr, nullptr },
			0u,
			{ size, size, size },
		}
	{
		initialise();
	}

	//*********************************************************************************************
}
