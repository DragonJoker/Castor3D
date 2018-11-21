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
			, ashes::Format format
			, uint32_t mipLevels
			, bool isDepthStencil )
		{
			TextureLayoutSPtr result;
			ashes::ImageCreateInfo image{};
			image.flags = 0u;
			image.arrayLayers = 1u;
			image.extent.width = size[0];
			image.extent.height = size[1];
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = mipLevels;
			image.samples = ashes::SampleCountFlag::e1;
			image.sharingMode = ashes::SharingMode::eExclusive;
			image.tiling = ashes::ImageTiling::eOptimal;
			image.usage = ashes::ImageUsageFlag::eTransferSrc;
			
			if ( isDepthStencil )
			{
				image.usage |= ashes::ImageUsageFlag::eDepthStencilAttachment;

				if ( !ashes::isDepthOrStencilFormat( format ) )
				{
					image.usage |= ashes::ImageUsageFlag::eSampled;
				}
			}
			else
			{
				image.usage |= ashes::ImageUsageFlag::eColourAttachment
					| ashes::ImageUsageFlag::eSampled;
			}

			if ( mipLevels > 1 )
			{
				image.usage |= ashes::ImageUsageFlag::eTransferDst;
			}

			return std::make_shared< TextureLayout >( renderSystem
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
		}
	}

	PostEffectSurface::PostEffectSurface( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, Size const & size
		, ashes::Format format
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

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, Size const & size
		, ashes::Format colourFormat
		, ashes::Format depthFormat )
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

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, castor::Size const & size
		, TextureLayoutSPtr colourTexture )
	{
		return initialise( renderPass
			, size
			, colourTexture
			, nullptr );
	}

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, castor::Size const & size
		, TextureLayoutSPtr colourTexture
		, ashes::Format depthFormat )
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

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, castor::Size const & size
		, ashes::Format colourFormat
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

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, Size const & size
		, TextureLayoutSPtr colourTexture
		, TextureLayoutSPtr depthTexture )
	{
		this->size = size;
		ashes::FrameBufferAttachmentArray attaches;
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
			auto format = this->depthTexture->getPixelFormat();

			ashes::ImageViewCreateInfo view{};
			view.format = format;
			view.viewType = ashes::TextureViewType::e2D;
			auto & texture = this->depthTexture->getTexture();
			view.subresourceRange.levelCount = texture.getMipmapLevels();

			if ( isDepthStencilFormat( format ) )
			{
				view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
				depthView = texture.createView( view );
				view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eStencil;
				stencilView = texture.createView( view );
			}
			else if ( isDepthFormat( format ) )
			{
				view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
				depthView = texture.createView( view );
			}
			else if ( isStencilFormat( format ) )
			{
				view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eStencil;
				stencilView = texture.createView( view );
			}

			attaches.emplace_back( *( renderPass.getAttachments().begin() + index ), depthTexture->getDefaultView() );
		}

		frameBuffer = renderPass.createFrameBuffer( ashes::Extent2D{ size.getWidth(), size.getHeight() }
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
