#include "PostEffectSurface.hpp"

#include "Engine.hpp"
#include "Render/RenderTarget.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureLayoutSPtr doCreateTexture( RenderSystem & renderSystem
			, Size const & size
			, renderer::Format format
			, uint32_t mipLevels
			, bool isDepthStencil )
		{
			TextureLayoutSPtr result;
			renderer::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size[0];
			image.extent.height = size[1];
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = mipLevels;
			image.samples = renderer::SampleCountFlag::e1;
			image.sharingMode = renderer::SharingMode::eExclusive;
			image.tiling = renderer::ImageTiling::eOptimal;
			image.usage = renderer::ImageUsageFlag::eTransferSrc;
			
			if ( isDepthStencil )
			{
				image.usage |= renderer::ImageUsageFlag::eDepthStencilAttachment;

				if ( !renderer::isDepthOrStencilFormat( format ) )
				{
					image.usage |= renderer::ImageUsageFlag::eSampled;
				}
			}
			else
			{
				image.usage |= renderer::ImageUsageFlag::eColourAttachment
					| renderer::ImageUsageFlag::eSampled;
			}

			if ( mipLevels > 1 )
			{
				image.usage |= renderer::ImageUsageFlag::eTransferDst;
			}

			return std::make_shared< TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
		}
	}

	PostEffectSurface::PostEffectSurface( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool PostEffectSurface::initialise( renderer::RenderPass const & renderPass
		, Size const & size
		, renderer::Format format
		, uint32_t mipLevels )
	{
		return initialise( renderPass
			, size
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, format
				, mipLevels
				, false )
			, nullptr );
	}

	bool PostEffectSurface::initialise( renderer::RenderPass const & renderPass
		, Size const & size
		, renderer::Format colourFormat
		, renderer::Format depthFormat )
	{
		return initialise( renderPass
			, size
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, colourFormat
				, 1u
				, false )
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, depthFormat
				, 1u
				, true ) );
	}

	bool PostEffectSurface::initialise( renderer::RenderPass const & renderPass
		, castor::Size const & size
		, TextureLayoutSPtr colourTexture )
	{
		return initialise( renderPass
			, size
			, colourTexture
			, nullptr );
	}

	bool PostEffectSurface::initialise( renderer::RenderPass const & renderPass
		, castor::Size const & size
		, TextureLayoutSPtr colourTexture
		, renderer::Format depthFormat )
	{
		return initialise( renderPass
			, size
			, colourTexture
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, depthFormat
				, 1u
				, true ) );
	}

	bool PostEffectSurface::initialise( renderer::RenderPass const & renderPass
		, castor::Size const & size
		, renderer::Format colourFormat
		, TextureLayoutSPtr depthTexture )
	{
		return initialise( renderPass
			, size
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, colourFormat
				, 1u
				, false )
			, depthTexture );
	}

	bool PostEffectSurface::initialise( renderer::RenderPass const & renderPass
		, Size const & size
		, TextureLayoutSPtr colourTexture
		, TextureLayoutSPtr depthTexture )
	{
		this->size = size;
		renderer::FrameBufferAttachmentArray attaches;
		uint32_t index = 0u;

		if ( colourTexture )
		{
			this->colourTexture = colourTexture;
			this->colourTexture->initialise();
			attaches.emplace_back( *( renderPass.getAttachments().begin() + index ), colourTexture->getImage().getView() );
			++index;
		}

		if ( depthTexture )
		{
			this->depthTexture = depthTexture;
			this->depthTexture->initialise();
			attaches.emplace_back( *( renderPass.getAttachments().begin() + index ), depthTexture->getDefaultView() );
			++index;
		}

		frameBuffer = renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
		, std::move( attaches ) );
		return true;
	}

	void PostEffectSurface::cleanup()
	{
		frameBuffer.reset();

		if ( colourTexture )
		{
			colourTexture->cleanup();
			colourTexture.reset();
		}

		if ( depthTexture )
		{
			depthTexture->cleanup();
			depthTexture.reset();
		}
	}
}
