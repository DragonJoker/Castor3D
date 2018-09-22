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
		ashes::TexturePtr doCreateTexture( Engine & engine
			, Size const & size
			, WbTexture texture )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );

			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = getTextureFormat( texture );
			image.imageType = ashes::TextureType::e2D;
			image.initialLayout = ashes::ImageLayout::eUndefined;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled;

			return device.createTexture( image, ashes::MemoryPropertyFlag::eDeviceLocal );
		}

		ashes::TextureViewPtr doCreateDepthView( Engine & engine
			, ashes::TextureView const & depthView )
		{
			auto & depth = depthView.getTexture();
			ashes::ImageViewCreateInfo view{};
			view.format = depth.getFormat();
			view.viewType = ashes::TextureViewType::e2D;
			view.subresourceRange.aspectMask = ashes::ImageAspectFlag::eDepth;
			view.subresourceRange.baseArrayLayer = 0u;
			view.subresourceRange.layerCount = 1u;
			view.subresourceRange.baseMipLevel = 0u;
			view.subresourceRange.levelCount = 1u;
			return depth.createView( view );
		}
	}

	WeightedBlendRendering::WeightedBlendRendering( Engine & engine
		, TransparentPass & transparentPass
		, ashes::TextureView const & depthView
		, ashes::TextureView const & colourView
		, TextureLayoutSPtr velocityTexture
		, castor::Size const & size
		, Scene const & scene )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_size{ size }
		, m_depthView{ doCreateDepthView( engine, depthView ) }
		, m_accumulation{ doCreateTexture( engine, m_size, WbTexture::eAccumulation ) }
		, m_accumulationView{ m_accumulation->createView( ashes::TextureViewType::e2D, m_accumulation->getFormat() ) }
		, m_revealage{ doCreateTexture( engine, m_size, WbTexture::eRevealage ) }
		, m_revealageView{ m_revealage->createView( ashes::TextureViewType::e2D, m_revealage->getFormat() ) }
		, m_weightedBlendPassResult{ { *m_depthView, *m_accumulationView, *m_revealageView, velocityTexture->getDefaultView() } }
		, m_finalCombinePass{ engine, m_size, m_transparentPass.getSceneUbo(), m_weightedBlendPassResult, colourView }
	{
		m_transparentPass.initialiseRenderPass( m_weightedBlendPassResult );
		m_transparentPass.initialise( m_size );
	}

	void WeightedBlendRendering::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, Point2r const & jitter )
	{
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getProjection().getInverse();
		auto invViewProj = ( camera.getProjection() * camera.getView() ).getInverse();

		m_transparentPass.getSceneUbo().update( &camera, scene.getFog() );
		m_finalCombinePass.update( camera
			, invViewProj
			, invView
			, invProj );
		m_transparentPass.update( info
			, jitter );
	}

	ashes::Semaphore const & WeightedBlendRendering::render( RenderInfo & info
		, Scene const & scene
		, ashes::Semaphore const & toWait )
	{
		m_engine.setPerObjectLighting( true );
		auto * result = &toWait;
		result = &m_transparentPass.render( *result );
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
