#include "WeightedBlendRendering.hpp"

#include "Render/RenderInfo.hpp"
#include "Texture/TextureLayout.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"

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

			createInfo.attachments.resize( 4u );
			createInfo.attachments[0].index = 0u;
			createInfo.attachments[0].format = depthView.getFormat();
			createInfo.attachments[0].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
			createInfo.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			createInfo.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

			createInfo.attachments[1].index = 1u;
			createInfo.attachments[1].format = getTextureFormat( WbTexture::eAccumulation );
			createInfo.attachments[1].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
			createInfo.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
			createInfo.attachments[1].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			createInfo.attachments[2].index = 2u;
			createInfo.attachments[2].format = getTextureFormat( WbTexture::eRevealage );
			createInfo.attachments[2].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[2].loadOp = renderer::AttachmentLoadOp::eClear;
			createInfo.attachments[2].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[2].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[2].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[2].initialLayout = renderer::ImageLayout::eUndefined;
			createInfo.attachments[2].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

			createInfo.attachments[3].index = 3u;
			createInfo.attachments[3].format = colourView.getFormat();
			createInfo.attachments[3].samples = renderer::SampleCountFlag::e1;
			createInfo.attachments[3].loadOp = renderer::AttachmentLoadOp::eLoad;
			createInfo.attachments[3].storeOp = renderer::AttachmentStoreOp::eStore;
			createInfo.attachments[3].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			createInfo.attachments[3].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			createInfo.attachments[3].initialLayout = renderer::ImageLayout::eUndefined;
			createInfo.attachments[3].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderer::AttachmentReference colourReference;
			colourReference.attachment = 0u;
			colourReference.layout = renderer::ImageLayout::eColourAttachmentOptimal;

			createInfo.subpasses.resize( 2u );
			createInfo.subpasses[0].flags = 0u;
			createInfo.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };
			createInfo.subpasses[0].colorAttachments =
			{
				{ 1u, renderer::ImageLayout::eColourAttachmentOptimal },
				{ 2u, renderer::ImageLayout::eColourAttachmentOptimal },
			};

			createInfo.subpasses[1].flags = 0u;
			createInfo.subpasses[1].colorAttachments = { { 3u, renderer::ImageLayout::eColourAttachmentOptimal } };

			createInfo.dependencies.resize( 3u );
			createInfo.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[0].dstSubpass = 0u;
			createInfo.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[0].srcAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			createInfo.dependencies[1].srcSubpass = 0u;
			createInfo.dependencies[1].dstSubpass = 1u;
			createInfo.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[1].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			createInfo.dependencies[2].srcSubpass = 1u;
			createInfo.dependencies[2].dstSubpass = renderer::ExternalSubpass;
			createInfo.dependencies[2].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			createInfo.dependencies[2].dstStageMask = renderer::PipelineStageFlag::eBottomOfPipe;
			createInfo.dependencies[2].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			createInfo.dependencies[2].dstAccessMask = renderer::AccessFlag::eMemoryRead;
			createInfo.dependencies[2].dependencyFlags = renderer::DependencyFlag::eByRegion;

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
			, WeightedBlendTextures const & wbResult
			, renderer::TextureView const & colourView )
		{
			renderer::FrameBufferAttachmentArray attaches
			{
				{ *( renderPass.getAttachments().begin() + 0u ), wbResult[0] },
				{ *( renderPass.getAttachments().begin() + 1u ), wbResult[1] },
				{ *( renderPass.getAttachments().begin() + 2u ), wbResult[2] },
				{ *( renderPass.getAttachments().begin() + 3u ), colourView },
			};
			return renderPass.createFrameBuffer( renderer::Extent2D{ size.getWidth(), size.getHeight() }
				, std::move( attaches ) );
		}
	}

	WeightedBlendRendering::WeightedBlendRendering( Engine & engine
		, TransparentPass & transparentPass
		, renderer::TextureView const & depthView
		, renderer::TextureView const & colourView
		, castor::Size const & size
		, Scene const & scene )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_size{ size }
		, m_accumulation{ doCreateTexture( engine, m_size, WbTexture::eAccumulation ) }
		, m_accumulationView{ m_accumulation->createView( renderer::TextureViewType::e2D, m_accumulation->getFormat() ) }
		, m_revealage{ doCreateTexture( engine, m_size, WbTexture::eRevealage ) }
		, m_revealageView{ m_revealage->createView( renderer::TextureViewType::e2D, m_revealage->getFormat() ) }
		, m_renderPass{ doCreateRenderPass( engine, depthView, colourView ) }
		, m_weightedBlendPassResult{ { depthView, *m_accumulationView, *m_revealageView } }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_size, m_weightedBlendPassResult, colourView ) }
		, m_finalCombinePass{ engine, m_size, m_transparentPass.getSceneUbo(), m_weightedBlendPassResult, *m_renderPass }
		, m_commandBuffer{ engine.getRenderSystem()->getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer() }
	{
	}

	void WeightedBlendRendering::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
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
			, shadowMaps
			, jitter );
	}

	void WeightedBlendRendering::render( RenderInfo & info
		, Scene const & scene
		, renderer::Semaphore const & toWait )
	{
		static renderer::ClearColorValue accumClear{ 0.0, 0.0, 0.0, 0.0 };
		static renderer::ClearColorValue revealClear{ 1.0, 1.0, 1.0, 1.0 };
		m_engine.setPerObjectLighting( true );

		// Accumulate blend.
		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit ) )
		{
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_frameBuffer
				, { { accumClear }, { revealClear } }
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_transparentPass.getCommandBuffer() } );
			m_commandBuffer->nextSubpass( renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_finalCombinePass.getCommandBuffer( scene.getFog().getType() ) } );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}

		m_engine.getRenderSystem()->getCurrentDevice()->getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_semaphore
			, nullptr );
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
