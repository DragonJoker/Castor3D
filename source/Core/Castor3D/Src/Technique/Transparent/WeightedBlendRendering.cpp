#include "WeightedBlendRendering.hpp"

#include "Render/RenderInfo.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Texture/TextureLayout.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		renderer::TexturePtr doCreateTexture( Engine & engine
			, Size const & size
			, WbTexture texture )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );

			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = getTextureFormat( texture );
			image.imageType = renderer::TextureType::e2D;
			image.initialLayout = renderer::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;

			return device.createTexture( image, renderer::MemoryPropertyFlag::eDeviceLocal );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, Size const & size
			, WeightedBlendTextures const & wbResult )
		{
			renderer::FrameBufferAttachmentArray attaches;

			for ( uint32_t i = 0; i < uint32_t( WbTexture::eCount ); ++i )
			{
				attaches.emplace_back( *( renderPass.getAttachments().begin() + i ), wbResult[i] );
			}

			return renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
				, std::move( attaches ) );
		}

		renderer::TextureViewPtr doCreateDepthView( Engine & engine
			, renderer::TextureView const & depthView )
		{
			auto & depth = depthView.getTexture();
			renderer::ImageViewCreateInfo view{};
			view.format = depth.getFormat();
			view.viewType = renderer::TextureViewType::e2D;
			view.subresourceRange.aspectMask = renderer::ImageAspectFlag::eDepth;
			view.subresourceRange.baseArrayLayer = 0u;
			view.subresourceRange.layerCount = 1u;
			view.subresourceRange.baseMipLevel = 0u;
			view.subresourceRange.levelCount = 1u;
			return depth.createView( view );
		}
	}

	WeightedBlendRendering::WeightedBlendRendering( Engine & engine
		, TransparentPass & transparentPass
		, renderer::TextureView const & depthView
		, renderer::TextureView const & colourView
		, TextureLayoutSPtr velocityTexture
		, castor::Size const & size
		, Scene const & scene )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_size{ size }
		, m_depthView{ doCreateDepthView( engine, depthView ) }
		, m_accumulation{ doCreateTexture( engine, m_size, WbTexture::eAccumulation ) }
		, m_accumulationView{ m_accumulation->createView( renderer::TextureViewType::e2D, m_accumulation->getFormat() ) }
		, m_revealage{ doCreateTexture( engine, m_size, WbTexture::eRevealage ) }
		, m_revealageView{ m_revealage->createView( renderer::TextureViewType::e2D, m_revealage->getFormat() ) }
		, m_weightedBlendPassResult{ { *m_depthView, *m_accumulationView, *m_revealageView, velocityTexture->getDefaultView() } }
		, m_finalCombinePass{ engine, m_size, m_transparentPass.getSceneUbo(), m_weightedBlendPassResult, colourView }
	{
	}

	void WeightedBlendRendering::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, Point2r const & jitter )
	{
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getViewport().getProjection().getInverse();
		auto invViewProj = ( camera.getViewport().getProjection() * camera.getView() ).getInverse();

		m_transparentPass.getSceneUbo().update( camera, scene.getFog() );
		m_finalCombinePass.update( camera
			, invViewProj
			, invView
			, invProj );
		m_transparentPass.update( info
			, jitter );
	}

	renderer::Semaphore const & WeightedBlendRendering::render( RenderInfo & info
		, Scene const & scene
		, renderer::Semaphore const & toWait )
	{
		if ( !m_frameBuffer )
		{
			m_frameBuffer = doCreateFrameBuffer( m_transparentPass.getRenderPass()
				, m_size
				, m_weightedBlendPassResult );
		}

		m_engine.setPerObjectLighting( true );
		auto * result = &toWait;
		result = &m_transparentPass.render( *m_frameBuffer
			, *result );
		result = &m_finalCombinePass.render( scene.getFog().getType()
			, *result );
		return *result;
	}

	void WeightedBlendRendering::debugDisplay()
	{
		//auto count = 2;
		//int width = int( m_size.getWidth() ) / 6;
		//int height = int( m_size.getHeight() ) / 6;
		//int left = int( m_size.getWidth() ) - width;
		//int top = int( m_size.getHeight() ) - height;
		//auto size = Size( width, height );
		//auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		//auto index = 0;
		//context.renderDepth( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eDepth )]->getTexture() );
		//context.renderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eRevealage )]->getTexture() );
		//context.renderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eAccumulation )]->getTexture() );
	}

	void WeightedBlendRendering::accept( RenderTechniqueVisitor & visitor )
	{
		m_transparentPass.accept( visitor );
		m_finalCombinePass.accept( visitor );
	}
}
