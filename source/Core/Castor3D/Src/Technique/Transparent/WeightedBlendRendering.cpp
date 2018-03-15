#include "WeightedBlendRendering.hpp"

#include "Render/RenderInfo.hpp"
#include "Texture/TextureLayout.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassState.hpp>
#include <RenderPass/RenderSubpass.hpp>
#include <RenderPass/RenderSubpassState.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		renderer::RenderPassPtr doCreateRenderPass( Engine & engine
			, renderer::TextureView & depthView
			, renderer::TextureView & colourView )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			std::vector< renderer::PixelFormat > formats
			{
				depthView.getFormat(),
				getTextureFormat( WbTexture::eAccumulation ),
				getTextureFormat( WbTexture::eRevealage ),
				colourView.getFormat()
			};
			renderer::RenderPassAttachmentArray attaches
			{
				renderer::RenderPassAttachment::createDepthStencilAttachment( depthView.getFormat(), false ),
				renderer::RenderPassAttachment::createColourAttachment( 0u, getTextureFormat( WbTexture::eAccumulation ), true ),
				renderer::RenderPassAttachment::createColourAttachment( 1u, getTextureFormat( WbTexture::eRevealage ), true ),
				renderer::RenderPassAttachment::createColourAttachment( 2u, colourView.getFormat(), false ),
			};
			renderer::ImageLayoutArray const initialLayouts
			{
				renderer::ImageLayout::eDepthStencilAttachmentOptimal,
				renderer::ImageLayout::eShaderReadOnlyOptimal,
				renderer::ImageLayout::eShaderReadOnlyOptimal,
				renderer::ImageLayout::eColourAttachmentOptimal,
			};
			renderer::ImageLayoutArray const finalLayouts
			{
				renderer::ImageLayout::eDepthStencilAttachmentOptimal,
				renderer::ImageLayout::eShaderReadOnlyOptimal,
				renderer::ImageLayout::eShaderReadOnlyOptimal,
				renderer::ImageLayout::eColourAttachmentOptimal,
			};
			renderer::RenderSubpassPtrArray subpasses;
			subpasses.emplace_back( device.createRenderSubpass( { attaches[0], attaches[1], attaches[2] }
				, { renderer::PipelineStageFlag::eColourAttachmentOutput, renderer::AccessFlag::eColourAttachmentWrite } ) );
			subpasses.emplace_back( device.createRenderSubpass( { attaches[3] }
				, { renderer::PipelineStageFlag::eColourAttachmentOutput, renderer::AccessFlag::eColourAttachmentWrite } ) );
			return device.createRenderPass( attaches
				, std::move( subpasses )
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, initialLayouts }
				, renderer::RenderPassState{ renderer::PipelineStageFlag::eColourAttachmentOutput
					, renderer::AccessFlag::eColourAttachmentWrite
					, finalLayouts } );
		}

		renderer::TexturePtr doCreateTexture( Engine & engine
			, Size const & size
			, WbTexture texture )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			auto result = device.createTexture();
			result->setImage( getTextureFormat( texture )
				, renderer::Extent3D{ size.getWidth(), size.getHeight(), 1u }
			, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled );
			return result;
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
		, renderer::TextureView & depthView
		, renderer::TextureView & colourView
		, castor::Size const & size
		, Scene const & scene )
		: m_engine{ engine }
		, m_transparentPass{ transparentPass }
		, m_size{ size }
		, m_accumulation{ doCreateTexture( engine, m_size, WbTexture::eAccumulation ) }
		, m_accumulationView{ m_accumulation->createView( renderer::TextureViewType::e2D, m_accumulation->getFormat() ) }
		, m_revealage{ doCreateTexture( engine, m_size, WbTexture::eRevealage ) }
		, m_revealageView{ m_revealage->createView( renderer::TextureViewType::e2D, m_accumulation->getFormat() ) }
		, m_renderPass{ doCreateRenderPass( engine, depthView, colourView ) }
		, m_weightedBlendPassResult{ { depthView, *m_accumulationView, *m_revealageView } }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_size, m_weightedBlendPassResult, colourView ) }
		, m_finalCombinePass{ engine, m_size, m_transparentPass.getSceneUbo(), m_weightedBlendPassResult, *m_renderPass }
		, m_commandBuffer{ engine.getRenderSystem()->getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer() }
	{
	}

	void WeightedBlendRendering::update( Scene const & scene
		, Camera const & camera )
	{
		m_transparentPass.getSceneUbo().update( camera, scene.getFog() );
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getViewport().getProjection().getInverse();
		auto invViewProj = ( camera.getViewport().getProjection() * camera.getView() ).getInverse();

		m_finalCombinePass.update( camera
			, invViewProj
			, invView
			, invProj );
	}

	void WeightedBlendRendering::render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter
		, TextureUnit const & velocity )
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
			m_transparentPass.render( info
				, shadowMaps
				, jitter );
			m_commandBuffer->nextSubpass( renderer::SubpassContents::eSecondaryCommandBuffers );
			m_commandBuffer->executeCommands( { m_finalCombinePass.getCommandBuffer( scene.getFog().getType() ) } );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->end();
		}
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
