#include "Castor3D/Render/PostEffect/PostEffectSurface.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <ashespp/RenderPass/RenderPass.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureLayoutSPtr doCreateTexture( RenderSystem & renderSystem
			, Size const & size
			, VkFormat format
			, uint32_t mipLevels
			, bool isDepthStencil
			, castor::String name )
		{
			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size[0], size[1], 1u },
				mipLevels,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			};

			if ( isDepthStencil )
			{
				image->usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

				if ( !ashes::isDepthOrStencilFormat( format ) )
				{
					image->usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
				}
			}
			else
			{
				image->usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT;
			}

			if ( mipLevels > 1 )
			{
				image->usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}

			return std::make_shared< TextureLayout >( renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, std::move( name ) );
		}
	}

	PostEffectSurface::PostEffectSurface( Engine & engine
		, castor::String debugName )
		: OwnedBy< Engine >{ engine }
		, m_debugName{ std::move( debugName ) }
	{
	}

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, Size const & size
		, VkFormat format
		, uint32_t mipLevels )
	{
		return initialise( renderPass
			, size
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, format
				, mipLevels
				, false
				, m_debugName + cuT( "_Colour" ) )
			, nullptr );
	}

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, Size const & size
		, VkFormat colourFormat
		, VkFormat depthFormat )
	{
		return initialise( renderPass
			, size
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, colourFormat
				, 1u
				, false
				, m_debugName + cuT( "_Colour" ) )
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, depthFormat
				, 1u
				, true
				, m_debugName + cuT( "_Depth" ) ) );
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
		, VkFormat depthFormat )
	{
		return initialise( renderPass
			, size
			, colourTexture
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, depthFormat
				, 1u
				, true
				, m_debugName + cuT( "_Depth" ) ) );
	}

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, castor::Size const & size
		, VkFormat colourFormat
		, TextureLayoutSPtr depthTexture )
	{
		return initialise( renderPass
			, size
			, doCreateTexture( *getEngine()->getRenderSystem()
				, size
				, colourFormat
				, 1u
				, false
				, m_debugName + cuT( "_Colour" ) )
			, depthTexture );
	}

	bool PostEffectSurface::initialise( ashes::RenderPass const & renderPass
		, Size const & size
		, TextureLayoutSPtr colourTexture
		, TextureLayoutSPtr depthTexture )
	{
		this->size = size;
		ashes::ImageViewCRefArray attaches;

		if ( colourTexture )
		{
			this->colourTexture = colourTexture;
			this->colourTexture->initialise();
			attaches.emplace_back( colourTexture->getDefaultView().getView() );
		}

		if ( depthTexture )
		{
			this->depthTexture = depthTexture;
			this->depthTexture->initialise();
			auto & texture = this->depthTexture->getTexture();
			auto format = texture.getFormat();
			auto aspectMask = ashes::getAspectMask( format );
			ashes::ImageViewCreateInfo view
			{
				0u,
				texture,
				VK_IMAGE_VIEW_TYPE_2D,
				format,
				VkComponentMapping{},
				{
					aspectMask,
					0u,
					texture.getMipmapLevels(),
					0u,
					1u
				},
			};

			if ( ashes::isDepthStencilFormat( format ) )
			{
				view->subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				depthView = texture.createView( m_debugName + "Depth", view );
				view->subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
				stencilView = texture.createView( m_debugName + "Stencil", view );
			}
			else if ( ashes::isDepthFormat( format ) )
			{
				view->subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				depthView = texture.createView( m_debugName, view );
			}
			else if ( ashes::isStencilFormat( format ) )
			{
				view->subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
				stencilView = texture.createView( m_debugName, view );
			}

			attaches.emplace_back( depthTexture->getDefaultView().getView() );
		}

		frameBuffer = renderPass.createFrameBuffer( m_debugName
			, VkExtent2D{ size.getWidth(), size.getHeight() }
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
