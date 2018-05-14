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
		renderer::RenderPassPtr doCreateRenderPass( Engine & engine
			, renderer::TextureView const & depthView
			, renderer::TextureView const & colourView )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();

			renderer::RenderPassCreateInfo createInfo{};
			createInfo.flags = 0u;

			createInfo.attachments.resize( 1u );
			createInfo.attachments[0].format = colourView.getFormat();
			createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
			createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = renderer::ImageLayout::eColourAttachmentOptimal;
			createInfo.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderer::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 1u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].colorAttachments = { { 0u, renderer::ImageLayout::eColourAttachmentOptimal } };

			createInfo.dependencies.resize( 2u );
			createInfo.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[0].dstSubpass = 0u;
			createInfo.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
			createInfo.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			createInfo.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			createInfo.dependencies[1].srcSubpass = 0u;
			createInfo.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
			createInfo.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
			createInfo.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( createInfo );
		}

		renderer::TexturePtr doCreateTexture( Engine & engine
			, Size const & size
			, WbTexture texture )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();

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

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, Size const & size
			, renderer::TextureView const & colourView )
		{
			renderer::FrameBufferAttachmentArray attaches
			{
				{ *( renderPass.getAttachments().begin() + 0u ), colourView },
			};
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
		, m_renderPass{ doCreateRenderPass( engine, *m_depthView, colourView ) }
		, m_weightedBlendPassResult{ { *m_depthView, *m_accumulationView, *m_revealageView, velocityTexture->getDefaultView() } }
		, m_frameBufferCB{ doCreateFrameBuffer( *m_renderPass, m_size, colourView ) }
		, m_finalCombinePass{ engine, m_size, m_transparentPass.getSceneUbo(), m_weightedBlendPassResult, *m_renderPass }
		, m_commandBuffer{ engine.getRenderSystem()->getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer() }
		, m_semaphore{ engine.getRenderSystem()->getCurrentDevice()->createSemaphore() }
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

	void WeightedBlendRendering::render( RenderInfo & info
		, Scene const & scene
		, renderer::Semaphore const & toWait )
	{
		if ( !m_frameBufferWB )
		{
			m_frameBufferWB = doCreateFrameBuffer( m_transparentPass.getRenderPass(), m_size, m_weightedBlendPassResult );
		}

		static renderer::DepthStencilClearValue const depthClear{ 1.0, 0 };
		static renderer::ClearColorValue const accumClear{ 0.0, 0.0, 0.0, 0.0 };
		static renderer::ClearColorValue const revealClear{ 1.0, 1.0, 1.0, 1.0 };
		static renderer::ClearColorValue const velocityClear{};
		m_engine.setPerObjectLighting( true );
		auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
		m_transparentPass.getTimer().start();

		// Accumulate blend.
		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			m_commandBuffer->resetQueryPool( m_transparentPass.getTimer().getQuery()
				, 0u
				, 2u );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_transparentPass.getTimer().getQuery()
				, 0u );
			m_commandBuffer->beginRenderPass( m_transparentPass.getRenderPass()
				, *m_frameBufferWB
				, { depthClear, accumClear, revealClear, velocityClear }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_transparentPass.getCommandBuffer() } );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_frameBufferCB
				, { accumClear }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_finalCombinePass.getCommandBuffer( scene.getFog().getType() ) } );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
				, m_transparentPass.getTimer().getQuery()
				, 1u );
			m_commandBuffer->end();
		}

		device.getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_semaphore
			, nullptr );
		device.getGraphicsQueue().waitIdle();
		m_transparentPass.getTimer().step();
		m_transparentPass.getTimer().stop();
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
}
