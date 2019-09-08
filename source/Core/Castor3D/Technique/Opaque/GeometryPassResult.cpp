#include "Castor3D/Technique/Opaque/GeometryPassResult.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Technique/Opaque/LightPass.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		ashes::ImagePtr doCreateTexture( RenderDevice const & device
			, VkFormat format
			, VkExtent3D const & size
			, bool isDepth
			, std::string const & name )
		{
			VkImageUsageFlags usage = isDepth
				? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
				: VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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
				usage | VK_IMAGE_USAGE_SAMPLED_BIT,
			};
			return makeImage( device
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "GPResult_" + name );
		}
	}

	GeometryPassResult::GeometryPassResult( Engine & engine
		, ashes::Image const & depthTexture
		, ashes::Image const & velocityTexture )
		: m_engine{ engine }
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );

		m_result[uint32_t( DsTexture::eDepth )] = &depthTexture;

		for ( auto i = uint32_t( DsTexture::eData1 ); i < uint32_t( DsTexture::eData5 ); i++ )
		{
			m_owned.emplace_back( doCreateTexture( device
				, getTextureFormat( DsTexture( i ) )
				, depthTexture.getDimensions()
				, i == uint32_t( DsTexture::eDepth )
				, getTextureName( DsTexture( i ) ) ) );

			m_result[i] = m_owned.back().get();
		}

		m_result[uint32_t( DsTexture::eData5 )] = &velocityTexture;

		uint32_t index = 0u;
		ashes::ImageViewCreateInfo view
		{
			0u,
			VK_NULL_HANDLE,
			VK_IMAGE_VIEW_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			VkComponentMapping{},
			{ 0u, 0u, 1u, 0u, 1u }
		};

		for ( auto & texture : m_result )
		{
			view->format = texture->getFormat();
			view->subresourceRange.aspectMask = ashes::getAspectMask( view->format );

			if ( index == 0u )
			{
				m_depthStencilView = texture->createView( view );
				view->subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			}

			m_samplableViews[index] = texture->createView( view );
			++index;
		}

		ashes::SamplerCreateInfo sampler
		{
			0u,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VK_FALSE,
			0.0f,
			VK_FALSE,
			VK_COMPARE_OP_NEVER,
			0.0f,
			1.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			VK_FALSE,
		};
		m_sampler = device->createSampler( std::move( sampler ) );
	}
}
