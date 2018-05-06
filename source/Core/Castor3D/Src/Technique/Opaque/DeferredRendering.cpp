#include "DeferredRendering.hpp"

#include "Render/RenderPassTimer.hpp"
#include "Scene/Background/Background.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Texture/Sampler.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name )
		{
			SamplerSPtr result;
			auto & cache = engine.getSamplerCache();

			if ( cache.has( name ) )
			{
				result = cache.find( name );
			}
			else
			{
				result = engine.getSamplerCache().add( cuT( "TextureProjection" ) );
				result->setMinFilter( renderer::Filter::eNearest );
				result->setMagFilter( renderer::Filter::eNearest );
				result->setWrapS( renderer::WrapMode::eClampToEdge );
				result->setWrapT( renderer::WrapMode::eClampToEdge );
				result->setWrapR( renderer::WrapMode::eClampToEdge );
				result->initialise();
			}

			return result;
		}
	}

	//*********************************************************************************************

	DeferredRendering::DeferredRendering( Engine & engine
		, OpaquePass & opaquePass
		, TextureLayoutSPtr depthTexture
		, TextureLayoutSPtr velocityTexture
		, TextureLayoutSPtr resultTexture
		, Size const & size
		, Scene & scene
		, Viewport const & viewport
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_ssaoConfig{ config }
		, m_opaquePass{ opaquePass }
		, m_size{ size }
		, m_gpInfoUbo{ engine }
		, m_geometryPassResult{ engine, depthTexture->getTexture(), velocityTexture->getTexture() }
	{
		auto & renderSystem = *engine.getRenderSystem();
		m_opaquePass.initialiseRenderPass( m_geometryPassResult );

		m_lightingPass = std::make_unique< LightingPass >( engine
			, m_size
			, scene
			, m_geometryPassResult
			, m_opaquePass
			, depthTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo );
		m_subsurfaceScattering = std::make_unique< SubsurfaceScatteringPass >( engine
			, m_gpInfoUbo
			, m_opaquePass.getSceneUbo()
			, m_size
			, m_geometryPassResult
			, m_lightingPass->getDiffuse() );
		m_reflection.emplace_back( std::make_unique< ReflectionPass >( engine
			, scene
			, m_geometryPassResult
			, m_lightingPass->getDiffuse().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, m_ssaoConfig
			, viewport ) );
		m_reflection.emplace_back( std::make_unique< ReflectionPass >( engine
			, scene
			, m_geometryPassResult
			, m_subsurfaceScattering->getResult().getTexture()->getDefaultView()
			, m_lightingPass->getSpecular().getTexture()->getDefaultView()
			, resultTexture->getDefaultView()
			, m_opaquePass.getSceneUbo()
			, m_gpInfoUbo
			, m_ssaoConfig
			, viewport ) );

		m_nodesCommands = renderSystem.getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer();
		m_fence = renderSystem.getCurrentDevice()->createFence( renderer::FenceCreateFlag::eSignaled );
	}

	DeferredRendering::~DeferredRendering()
	{
		m_results.clear();
		m_reflection.clear();
		m_subsurfaceScattering.reset();
		m_lightingPass.reset();
	}

	void DeferredRendering::update( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, ShadowMapLightTypeArray & shadowMaps
		, Point2r const & jitter )
	{
		auto invView = camera.getView().getInverse().getTransposed();
		auto invProj = camera.getViewport().getProjection().getInverse();
		auto invViewProj = ( camera.getViewport().getProjection() * camera.getView() ).getInverse();
		m_opaquePass.getSceneUbo().update( scene, camera );
		m_gpInfoUbo.update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		m_opaquePass.update( info
			, shadowMaps
			, jitter );
		m_reflection[0]->update( camera );
		m_reflection[1]->update( camera );
	}

	renderer::Semaphore const & DeferredRendering::render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera
		, renderer::Semaphore const & toWait )
	{
		renderer::Semaphore const * result = &toWait;
		m_engine.setPerObjectLighting( false );
		m_opaquePass.getTimer().start();
		auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
		static renderer::ClearValueArray const clearValues
		{
			renderer::DepthStencilClearValue{ 1.0, 0 },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
			renderer::ClearColorValue{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		m_fence->reset();

		if ( m_nodesCommands->begin() )
		{
			m_nodesCommands->resetQueryPool( m_opaquePass.getTimer().getQuery()
				, 0u
				, 2u );
			m_nodesCommands->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_opaquePass.getTimer().getQuery()
				, 0u );
			m_nodesCommands->beginRenderPass( m_opaquePass.getRenderPass()
				, m_opaquePass.getFrameBuffer()
				, clearValues
				, renderer::SubpassContents::eSecondaryCommandBuffers );
			m_nodesCommands->executeCommands( { m_opaquePass.getCommandBuffer() } );
			m_nodesCommands->endRenderPass();
			m_nodesCommands->writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
				, m_opaquePass.getTimer().getQuery()
				, 1u );
			m_nodesCommands->end();
			device.getGraphicsQueue().submit( { *m_nodesCommands }
				, { *result }
				, { renderer::PipelineStageFlag::eColourAttachmentOutput }
				, { m_opaquePass.getSemaphore() }
				, m_fence.get() );
			m_fence->wait( renderer::FenceTimeout );
			device.getGraphicsQueue().waitIdle();
			m_opaquePass.getTimer().step();
			result = &m_opaquePass.getSemaphore();
		}

		result = m_lightingPass->render( scene
			, camera
			, m_geometryPassResult
			, *result
			, info );

		if ( scene.needsSubsurfaceScattering() )
		{
			m_fence->reset();
			device.getGraphicsQueue().submit( m_subsurfaceScattering->getCommandBuffer()
				, *result
				, renderer::PipelineStageFlag::eColourAttachmentOutput
				, m_subsurfaceScattering->getSemaphore()
				, m_fence.get() );
			result = &m_subsurfaceScattering->getSemaphore();
			m_fence->wait( renderer::FenceTimeout );
			m_reflection[1]->render( *result );
			result = &m_reflection[1]->getSemaphore();
		}
		else
		{
			m_reflection[0]->render( *result );
			result = &m_reflection[0]->getSemaphore();
		}

		m_opaquePass.getTimer().stop();
		return *result;
	}

	void DeferredRendering::debugDisplay( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer )const
	{
		//auto count = 8 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		//int width = int( m_size.getWidth() ) / count;
		//int height = int( m_size.getHeight() ) / count;
		//auto size = Size( width, height );
		//auto & context = *m_engine.getRenderSystem()->getCurrentContext();
		//auto index = 0;
		//context.renderDepth( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData1 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData2 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData3 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData4 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData5 )]->getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getDiffuse().getTexture() );
		//context.renderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->getSpecular().getTexture() );

		//if ( m_ssaoConfig.m_enabled )
		//{
		//	context.renderTexture( Position{ width * ( index++ ), 0 }, size, m_reflection->getSsao() );
		//}

		//m_subsurfaceScattering->debugDisplay( m_size );
	}
}
