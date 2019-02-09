#include "GeometryPassResult.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		ashes::TexturePtr doCreateTexture( ashes::Device const & device
			, ashes::Format format
			, ashes::Extent3D const & size
			, bool isDepth )
		{
			ashes::ImageUsageFlags usage = isDepth
				? ashes::ImageUsageFlag::eDepthStencilAttachment | ashes::ImageUsageFlag::eTransferSrc
				: ashes::ImageUsageFlag::eColourAttachment;
			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = usage | ashes::ImageUsageFlag::eSampled;
			image.format = format;

			return device.createTexture( image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
		}
	}

	GeometryPassResult::GeometryPassResult( Engine & engine
		, ashes::Texture const & depthTexture
		, ashes::Texture const & velocityTexture )
		: m_engine{ engine }
	{
		auto & renderSystem = *engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		for ( auto i = uint32_t( DsTexture::eDepth ); i < uint32_t( DsTexture::eData5 ); i++ )
		{
			m_owned.emplace_back( doCreateTexture( device
				, getTextureFormat( DsTexture( i ) )
				, depthTexture.getDimensions()
				, i == uint32_t( DsTexture::eDepth ) ) );

			m_result[i] = m_owned.back().get();
		}

		m_result[uint32_t( DsTexture::eData5 )] = &velocityTexture;

		uint32_t index = 0u;
		ashes::ImageViewCreateInfo view{};
		view.viewType = ashes::TextureViewType::e2D;
		view.subresourceRange.baseArrayLayer = 0u;
		view.subresourceRange.baseMipLevel = 0u;
		view.subresourceRange.layerCount = 1u;
		view.subresourceRange.levelCount = 1u;

		for ( auto & texture : m_result )
		{
			view.format = texture->getFormat();
			view.subresourceRange.aspectMask = ashes::getAspectMask( view.format );

			if ( index == 0u )
			{
				m_depthStencilView = texture->createView( view );
				view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
			}

			m_samplableViews[index] = texture->createView( view );
			++index;
		}

		ashes::SamplerCreateInfo sampler{};
		sampler.addressModeU = ashes::WrapMode::eClampToEdge;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;
		sampler.anisotropyEnable = false;
		sampler.borderColor = ashes::BorderColour::eFloatOpaqueWhite;
		sampler.compareEnable = false;
		sampler.compareOp = ashes::CompareOp::eNever;
		sampler.minFilter = ashes::Filter::eLinear;
		sampler.magFilter = ashes::Filter::eLinear;
		sampler.mipmapMode = ashes::MipmapMode::eNone;
		sampler.maxAnisotropy = 0.0f;
		sampler.maxLod = 1.0f;
		sampler.minLod = 0.0f;
		sampler.mipLodBias = 0.0f;
		sampler.mipmapMode = ashes::MipmapMode::eNone;
		sampler.unnormalizedCoordinates = false;
		m_sampler = device.createSampler( sampler );
	}
}
