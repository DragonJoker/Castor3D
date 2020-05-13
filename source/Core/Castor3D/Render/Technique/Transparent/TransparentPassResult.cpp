#include "Castor3D/Render/Technique/Transparent/TransparentPassResult.hpp"

#include "Castor3D/Material/Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	String getTextureName( WbTexture texture )
	{
		static std::array< String, size_t( WbTexture::eCount ) > Values
		{
			{
				"c3d_mapDepth",
				"c3d_mapAccumulation",
				"c3d_mapRevealage",
				"c3d_mapVelocity",
			}
		};

		return Values[size_t( texture )];
	}

	VkFormat getTextureFormat( WbTexture texture )
	{
		static std::array< VkFormat, size_t( WbTexture::eCount ) > Values
		{
			{
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
				VK_FORMAT_R16_SFLOAT,
				VK_FORMAT_R16G16B16A16_SFLOAT,
			}
		};

		CU_Require( texture != WbTexture::eDepth
			&& "You can't use this function for depth texture format" );
		return Values[size_t( texture )];
	}

	//*********************************************************************************************

	namespace
	{
		ashes::ImagePtr doCreateImage( RenderDevice const & device
			, VkFormat format
			, VkExtent3D const & size
			, std::string const & name )
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size.width, size.height, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT ),
			};
			return makeImage( device
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "WBResult" + name );
		}

		std::vector< ashes::ImagePtr > doCreateImages( Engine & engine
			, VkExtent3D const & size )
		{
			auto & device = getCurrentRenderDevice( engine );
			std::vector< ashes::ImagePtr > result;

			for ( auto i = 1u; i < uint32_t( WbTexture::eVelocity ); i++ )
			{
				result.emplace_back( doCreateImage( device
					, getTextureFormat( WbTexture( i ) )
					, size
					, getTextureName( WbTexture( i ) ) ) );
			}

			return result;
		}

		GBuffer::Textures doCreateTextures( Engine & engine
			, ashes::Image const & depthTexture
			, ashes::Image const & velocityTexture
			, std::vector< ashes::ImagePtr > const & owned )
		{
			GBuffer::Textures result;
			result.push_back( &depthTexture );

			for ( auto & image : owned )
			{
				result.push_back( image.get() );
			}

			result.push_back( &velocityTexture );
			return result;
		}
	}

	//*********************************************************************************************

	TransparentPassResult::TransparentPassResult( Engine & engine
		, ashes::Image const & depthTexture
		, ashes::Image const & velocityTexture )
		: GBuffer{ engine }
		, m_owned{ doCreateImages( engine, depthTexture.getDimensions() ) }
	{
		doInitialise( doCreateTextures( engine, depthTexture, velocityTexture, m_owned ) );
	}

	//*********************************************************************************************
}
