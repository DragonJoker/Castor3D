#include "GeometryPassResult.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		renderer::TexturePtr doCreateTexture( renderer::Device const & device
			, renderer::Format format
			, renderer::Extent3D const & size )
		{
			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;
			image.format = format;

			return device.createTexture( image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
		}
	}

	GeometryPassResult::GeometryPassResult( Engine & engine
		, renderer::Texture const & depthTexture
		, renderer::Texture const & velocityTexture )
		: m_engine{ engine }
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		m_result[uint32_t( DsTexture::eDepth )] = &depthTexture;

		for ( auto i = uint32_t( DsTexture::eData1 ); i < uint32_t( DsTexture::eData5 ); i++ )
		{
			m_owned.emplace_back( doCreateTexture( device
				, getTextureFormat( DsTexture( i ) )
				, depthTexture.getDimensions() ) );

			m_result[i] = m_owned.back().get();
		}

		m_result[uint32_t( DsTexture::eData5 )] = &velocityTexture;

		uint32_t index = 0u;
		renderer::ImageViewCreateInfo view{};
		view.viewType = renderer::TextureViewType::e2D;
		view.subresourceRange.baseArrayLayer = 0u;
		view.subresourceRange.baseMipLevel = 0u;
		view.subresourceRange.layerCount = 1u;
		view.subresourceRange.levelCount = 1u;

		for ( auto & texture : m_result )
		{
			view.format = texture->getFormat();
			view.subresourceRange.aspectMask = renderer::getAspectMask( view.format );

			if ( index == 0u )
			{
				m_depthStencilView = texture->createView( view );
				view.subresourceRange.aspectMask = renderer::ImageAspectFlag::eDepth;
			}

			m_samplableViews[index] = texture->createView( view );
			++index;
		}

		renderer::SamplerCreateInfo sampler{};
		sampler.addressModeU = renderer::WrapMode::eClampToEdge;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;
		sampler.anisotropyEnable = false;
		sampler.borderColor = renderer::BorderColour::eFloatOpaqueWhite;
		sampler.compareEnable = false;
		sampler.compareOp = renderer::CompareOp::eNever;
		sampler.minFilter = renderer::Filter::eLinear;
		sampler.magFilter = renderer::Filter::eLinear;
		sampler.mipmapMode = renderer::MipmapMode::eNone;
		sampler.maxAnisotropy = 0.0f;
		sampler.maxLod = 1.0f;
		sampler.minLod = 0.0f;
		sampler.mipLodBias = 0.0f;
		sampler.mipmapMode = renderer::MipmapMode::eNone;
		sampler.unnormalizedCoordinates = false;
		m_sampler = device.createSampler( sampler );
	}
}
