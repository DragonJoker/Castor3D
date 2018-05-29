#include "RenderTechnique.hpp"

#include "Engine.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Camera.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Background/Background.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "ShadowMap/ShadowMapDirectional.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Technique/ForwardRenderTechniquePass.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Technique/Transparent/TransparentPass.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

using namespace castor;

#define DISPLAY_DEBUG_DEFERRED_BUFFERS 1
#define DISPLAY_DEBUG_WEIGHTED_BLEND_BUFFERS 0
#define DISPLAY_DEBUG_IBL_BUFFERS 0
#define DISPLAY_DEBUG_SHADOW_MAPS 0
#define DISPLAY_DEBUG_ENV_MAPS 0

#define USE_WEIGHTED_BLEND 1
#define DEBUG_FORWARD_RENDERING 1

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		std::map< double, LightSPtr > doSortLights( LightCache const & cache
			, LightType type
			, Camera const & camera )
		{
			auto lock = makeUniqueLock( cache );
			std::map< double, LightSPtr > lights;

			for ( auto & light : cache.getLights( type ) )
			{
				light->setShadowMap( nullptr );

				if ( light->isShadowProducer()
					&& ( light->getLightType() == LightType::eDirectional
						|| camera.isVisible( light->getBoundingBox()
							, light->getParent()->getDerivedTransformationMatrix() ) ) )
				{
					lights.emplace( point::distanceSquared( camera.getParent()->getDerivedPosition()
						, light->getParent()->getDerivedPosition() )
						, light );
				}
			}

			return lights;
		}

		void doPrepareShadowMap( LightCache const & cache
			, LightType type
			, Camera const & camera
			, ShadowMapUPtr & shadowMap
			, ShadowMapLightTypeArray & activeShadowMaps
			, RenderQueueArray & queues )
		{
			auto lights = doSortLights( cache, type, camera );

			if ( !lights.empty() )
			{
				auto it = lights.begin();
				it->second->setShadowMap( shadowMap.get() );
				activeShadowMaps[size_t( type )].emplace_back( std::ref( *shadowMap ) );
				shadowMap->update( camera
					, queues
					, *( it->second )
					, 0u );
			}
		}

		void doPrepareShadowMaps( LightCache const & cache
			, LightType type
			, Camera const & camera
			, RenderTechnique::ShadowMapArray & shadowMaps
			, ShadowMapLightTypeArray & activeShadowMaps
			, RenderQueueArray & queues )
		{
			auto lights = doSortLights( cache, type, camera );
			size_t count = std::min( shadowMaps.size(), lights.size() );
			auto it = lights.begin();

			for ( auto i = 0u; i < count; ++i )
			{
				auto & shadowMap = *shadowMaps[i];
				it->second->setShadowMap( &shadowMap );
				activeShadowMaps[size_t( type )].emplace_back( std::ref( shadowMap ) );
				shadowMap.update( camera
					, queues
					, *( it->second )
					, i );
				++it;
			}
		}

		TextureLayoutSPtr doCreateTexture( Engine & engine
			, Size const & size
			, renderer::Format format
			, renderer::ImageUsageFlags usage )
		{
			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eSampled | usage;

			auto result = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			result->getDefaultImage().initialiseSource();
			result->initialise();
			return result;
		}
	}

	//*************************************************************************************************

	RenderTechnique::RenderTechnique( String const & name
		, RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & parameters
		, SsaoConfig const & config )
		: OwnedBy< Engine >{ *renderSystem.getEngine() }
		, Named{ name }
		, m_renderTarget{ renderTarget }
		, m_renderSystem{ renderSystem }
#if DEBUG_FORWARD_RENDERING
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "opaque_pass" )
			, *renderTarget.getScene()
			, renderTarget.getCamera().get()
			, false
			, nullptr
			, config ) }
#else
		, m_opaquePass{ std::make_unique< OpaquePass >( *renderTarget.getScene()
			, renderTarget.getCamera().get()
			, config ) }
#endif
#if USE_WEIGHTED_BLEND
		, m_transparentPass{ std::make_unique< TransparentPass >( *renderTarget.getScene()
			, renderTarget.getCamera().get()
			, config ) }
#else
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "forward_transparent" )
			, *renderTarget.getScene()
			, renderTarget.getCamera().get()
			, false
			, false
			, false
			, nullptr
			, config ) }
#endif
		, m_initialised{ false }
		, m_ssaoConfig{ config }
		, m_hdrConfigUbo{ *renderSystem.getEngine() }
	{
		m_pointShadowMaps.resize( shader::PointShadowMapCount );
		m_spotShadowMaps.resize( shader::SpotShadowMapCount );

		m_directionalShadowMap = std::make_unique< ShadowMapDirectional >( *renderTarget.getEngine()
			, *renderTarget.getScene() );
		m_allShadowMaps[size_t( LightType::eDirectional )].emplace_back( std::ref( *m_directionalShadowMap ) );

		for ( auto & shadowMap : m_pointShadowMaps )
		{
			shadowMap = std::make_unique< ShadowMapPoint >( *renderTarget.getEngine()
				, *renderTarget.getScene() );
			m_allShadowMaps[size_t( LightType::ePoint )].emplace_back( std::ref( *shadowMap ) );
		}

		for ( auto & shadowMap : m_spotShadowMaps )
		{
			shadowMap = std::make_unique< ShadowMapSpot >( *renderTarget.getEngine()
				, *renderTarget.getScene() );
			m_allShadowMaps[size_t( LightType::eSpot )].emplace_back( std::ref( *shadowMap ) );
		}
	}

	RenderTechnique::~RenderTechnique()
	{
		m_debugFrameBuffer.reset();
		m_debugRenderPass.reset();
		m_cbgCommandBuffer.reset();
		m_bgCommandBuffer.reset();
		m_bgFrameBuffer.reset();
		m_bgRenderPass.reset();
		m_signalFinished.reset();

		for ( auto & array : m_activeShadowMaps )
		{
			array.clear();
		}

		m_directionalShadowMap.reset();
		m_pointShadowMaps.clear();
		m_spotShadowMaps.clear();
		m_deferredRendering.reset();
		m_weightedBlendRendering.reset();
		m_transparentPass.reset();
		m_opaquePass.reset();
	}

	bool RenderTechnique::initialise()
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget.getSize();
			m_colourTexture = doCreateTexture( *getEngine()
				, m_size
				, renderer::Format::eR16G16B16A16_SFLOAT
				, renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eTransferDst );
			m_depthBuffer = doCreateTexture( *getEngine()
				, m_size
				, renderer::Format::eD24_UNORM_S8_UINT
				, renderer::ImageUsageFlag::eDepthStencilAttachment );
			m_signalFinished = getEngine()->getRenderSystem()->getCurrentDevice()->createSemaphore();
			m_hdrConfigUbo.initialise();

			doInitialiseShadowMaps();
			doInitialiseBackgroundPass();
			doInitialiseOpaquePass();
			doInitialiseTransparentPass();
			doInitialiseDebugPass();

			m_particleTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Particles" ), cuT( "Particles" ) );
			m_initialised = true;
		}

		return m_initialised;
	}

	void RenderTechnique::cleanup()
	{
		m_onBgChanged.disconnect();
		m_debugFrameBuffer.reset();
		m_debugRenderPass.reset();
		m_bgCommandBuffer.reset();
		m_cbgCommandBuffer.reset();
		m_bgFrameBuffer.reset();
		m_bgRenderPass.reset();
		m_particleTimer.reset();
		m_weightedBlendRendering.reset();
		m_deferredRendering.reset();
		doCleanupShadowMaps();
		m_hdrConfigUbo.cleanup();
		m_transparentPass->cleanup();
		m_opaquePass->cleanup();
		m_initialised = false;
		m_depthBuffer->cleanup();
		m_colourTexture->cleanup();
		m_depthBuffer.reset();
		m_colourTexture.reset();

		m_renderTarget.getScene()->getGeometryCache().cleanupUbos();
		m_renderTarget.getScene()->getBillboardPools().cleanupUbos();
		m_renderTarget.getScene()->getAnimatedObjectGroupCache().cleanupUbos();
	}

	void RenderTechnique::update( RenderQueueArray & queues )
	{
		m_opaquePass->update( queues );
		m_transparentPass->update( queues );
		doUpdateShadowMaps( queues );
		auto & maps = m_renderTarget.getScene()->getEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().update( queues );
		}
	}

	renderer::Semaphore const * RenderTechnique::render( Point2r const & jitter
		, renderer::SemaphoreCRefArray const & waitSemaphores
		, RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		auto & camera = *m_renderTarget.getCamera();
		scene.getLightCache().updateLightsTexture( camera );
		camera.resize( m_size );
		camera.update();
		scene.updateDeviceDependent( camera );

		// Update part
		doUpdateParticles( info );
		m_hdrConfigUbo.update( m_renderTarget.getHdrConfig() );
		auto * semaphore = doRenderBackground( waitSemaphores );
		semaphore = doRenderEnvironmentMaps( *semaphore );
		semaphore = doRenderShadowMaps( *semaphore );

#if !DEBUG_FORWARD_RENDERING

		m_deferredRendering->update( info
			, scene
			, camera
			//, m_activeShadowMaps
			, jitter );

#else

		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).update( info
			//, m_activeShadowMaps
			, jitter );
#endif
#if USE_WEIGHTED_BLEND

		m_weightedBlendRendering->update( info
			, scene
			, camera
			//, m_activeShadowMaps
			, jitter );

#endif

		getEngine()->getRenderSystem()->getCurrentDevice()->waitIdle();

		// Render part
		semaphore = doRenderOpaque( jitter, info, *semaphore );
		semaphore = doRenderTransparent( jitter, info, *semaphore );
		return semaphore;
	}

	bool RenderTechnique::writeInto( castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::debugDisplay( Size const & size )const
	{
		uint32_t index = 0u;

#if DISPLAY_DEBUG_DEFERRED_BUFFERS && !DEBUG_FORWARD_RENDERING

		m_deferredRendering->debugDisplay( *m_debugRenderPass
			, *m_debugFrameBuffer );

#endif
#if USE_WEIGHTED_BLEND && DISPLAY_DEBUG_WEIGHTED_BLEND_BUFFERS

		m_weightedBlendRendering->debugDisplay( *m_debugRenderPass
			, *m_debugFrameBuffer);

#endif
#if DISPLAY_DEBUG_IBL_BUFFERS

		m_frameBuffer.m_frameBuffer->bind();
		scene.getSkybox().getIbl().debugDisplay( size );
		m_frameBuffer.m_frameBuffer->unbind();

#endif
#if DISPLAY_DEBUG_ENV_MAPS

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			map.get().debugDisplay( *m_debugRenderPass
				, *m_debugFrameBuffer
				, size
				, index++ );
		}

		index = 0u;

#endif
#if DISPLAY_DEBUG_SHADOW_MAPS

		for ( auto & maps : m_activeShadowMaps )
		{
			for ( auto & map : maps )
			{
				map.get().debugDisplay( *m_debugRenderPass
					, *m_debugFrameBuffer
					, size
					, index++ );
			}
		}

#endif
	}

	void RenderTechnique::accept( RenderTechniqueVisitor & visitor )
	{
		ShaderProgramSPtr shaderProgram;

		if ( checkFlag( visitor.getPassFlags(), PassFlag::eAlphaBlending ) )
		{
#if DEBUG_FORWARD_RENDERING
			m_transparentPass->accept( visitor );
#else
			m_weightedBlendRendering->accept( visitor );
#endif
			shaderProgram = getEngine()->getShaderProgramCache().getAutomaticProgram( getTransparentPass()
				, visitor.getPassFlags()
				, visitor.getTextureFlags()
				, ProgramFlags{}
				, visitor.getSceneFlags()
				, visitor.getAlphaFunc()
				, false );
		}
		else
		{
#if DEBUG_FORWARD_RENDERING
			m_opaquePass->accept( visitor );
#else
			m_deferredRendering->accept( visitor );
#endif
		}
	}

	void RenderTechnique::doInitialiseShadowMaps()
	{
		m_directionalShadowMap->initialise();

		for ( auto & shadowMap : m_pointShadowMaps )
		{
			shadowMap->initialise();
		}

		for ( auto & shadowMap : m_spotShadowMaps )
		{
			shadowMap->initialise();
		}
	}

	void RenderTechnique::doInitialiseBackgroundPass()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_bgCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );
		m_cbgCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );

		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = m_depthBuffer->getPixelFormat();
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = m_colourTexture->getPixelFormat();
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };
		renderPass.subpasses[0].colorAttachments = { { 1u, renderer::ImageLayout::eColourAttachmentOptimal } };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_bgRenderPass = device.createRenderPass( renderPass );

		renderer::FrameBufferAttachmentArray attaches
		{
			{ *( m_bgRenderPass->getAttachments().begin() + 0u ), m_depthBuffer->getDefaultView() },
			{ *( m_bgRenderPass->getAttachments().begin() + 1u ), m_colourTexture->getDefaultView() },
		};
		m_bgFrameBuffer = m_bgRenderPass->createFrameBuffer( { m_depthBuffer->getDimensions().width, m_depthBuffer->getDimensions().height }
			, std::move( attaches ) );

		auto & background = m_renderTarget.getScene()->getBackground();
		auto prepareBackground = [&background, this]()
		{
			background.initialise( *m_bgRenderPass, m_hdrConfigUbo );
			background.prepareFrame( *m_bgCommandBuffer
				, Size{ m_colourTexture->getWidth(), m_colourTexture->getHeight() }
				, *m_bgRenderPass
				, *m_bgFrameBuffer );
		};
		prepareBackground();
		m_onBgChanged = background.onChanged.connect( [prepareBackground, this]( SceneBackground const & )
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, prepareBackground ) );
			} );
		auto & cbackground = m_renderTarget.getScene()->getColourBackground();
		auto prepareCBackground = [&cbackground, this]()
		{
			cbackground.initialise( *m_bgRenderPass, m_hdrConfigUbo );
			cbackground.prepareFrame( *m_cbgCommandBuffer
				, Size{ m_colourTexture->getWidth(), m_colourTexture->getHeight() }
				, *m_bgRenderPass
				, *m_bgFrameBuffer );
		};
		prepareCBackground();
		m_onCBgChanged = cbackground.onChanged.connect( [prepareCBackground, this]( SceneBackground const & )
			{
				getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender, prepareCBackground ) );
			} );
	}

	void RenderTechnique::doInitialiseOpaquePass()
	{
#if !DEBUG_FORWARD_RENDERING

		m_opaquePass->initialise( m_size );
		m_deferredRendering = std::make_unique< DeferredRendering >( *getEngine()
			, static_cast< OpaquePass & >( *m_opaquePass )
			, m_depthBuffer
			, m_renderTarget.getVelocity().getTexture()
			, m_colourTexture
			, m_renderTarget.getSize()
			, *m_renderTarget.getScene()
			, m_renderTarget.getCamera()->getViewport()
			, m_ssaoConfig );

#else

		m_opaquePass->initialise( m_size );
		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).initialiseRenderPass( m_colourTexture->getDefaultView()
			, m_depthBuffer->getDefaultView()
			, m_size
			, false );

#endif
	}

	void RenderTechnique::doInitialiseTransparentPass()
	{
#if USE_WEIGHTED_BLEND
		static_cast< TransparentPass & >( *m_transparentPass ).setDepthFormat( m_depthBuffer->getPixelFormat() );
		m_transparentPass->initialise( m_size );
		m_weightedBlendRendering = std::make_unique< WeightedBlendRendering >( *getEngine()
			, static_cast< TransparentPass & >( *m_transparentPass )
			, m_depthBuffer->getDefaultView()
			, m_colourTexture->getDefaultView()
			, m_renderTarget.getVelocity().getTexture()
			, m_renderTarget.getSize()
			, *m_renderTarget.getScene() );
#else
		m_transparentPass->initialise( m_size );
#endif
	}

	void RenderTechnique::doInitialiseDebugPass()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_colourTexture->getPixelFormat();
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eColourAttachmentOptimal;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].colorAttachments = { { 0u, renderer::ImageLayout::eColourAttachmentOptimal } };

		m_debugRenderPass = device.createRenderPass( renderPass );

		renderer::FrameBufferAttachmentArray attaches
		{
			{ *( m_bgRenderPass->getAttachments().begin() + 0u ), m_colourTexture->getDefaultView() },
		};
		m_debugFrameBuffer = m_debugRenderPass->createFrameBuffer( { m_colourTexture->getDimensions().width, m_colourTexture->getDimensions().height }
			, std::move( attaches ) );
	}

	void RenderTechnique::doCleanupShadowMaps()
	{
		m_directionalShadowMap->cleanup();

		for ( auto & shadowMap : m_pointShadowMaps )
		{
			shadowMap->cleanup();
		}

		for ( auto & shadowMap : m_spotShadowMaps )
		{
			shadowMap->cleanup();
		}
	}

	void RenderTechnique::doUpdateShadowMaps( RenderQueueArray & queues )
	{
		auto & scene = *m_renderTarget.getScene();
		auto & camera = *m_renderTarget.getCamera();
		auto & cache = scene.getLightCache();

		for ( auto & array : m_activeShadowMaps )
		{
			array.clear();
		}

		doPrepareShadowMap( cache
			, LightType::eDirectional
			, camera
			, m_directionalShadowMap
			, m_activeShadowMaps
			, queues );
		doPrepareShadowMaps( cache
			, LightType::ePoint
			, camera
			, m_pointShadowMaps
			, m_activeShadowMaps
			, queues );
		doPrepareShadowMaps( cache
			, LightType::eSpot
			, camera
			, m_spotShadowMaps
			, m_activeShadowMaps
			, queues );
	}
	
	void RenderTechnique::doUpdateParticles( RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		auto & cache = scene.getParticleSystemCache();
		auto lock = makeUniqueLock( cache );

		if ( !cache.isEmpty() )
		{
			if ( m_particleTimer->getCount() < cache.getObjectCount() )
			{
				m_particleTimer->updateCount( 2u * cache.getObjectCount() );
			}

			m_particleTimer->start();
			uint32_t index = 0u;

			for ( auto & particleSystem : cache )
			{
				particleSystem.second->update( *m_particleTimer, index );
				m_particleTimer->notifyPassRender( index++ );
				m_particleTimer->notifyPassRender( index++ );
				info.m_particlesCount += particleSystem.second->getParticlesCount();
			}

			m_particleTimer->stop();
		}
	}

	renderer::Semaphore const * RenderTechnique::doRenderShadowMaps( renderer::Semaphore const & semaphore )
	{
		renderer::Semaphore const * result = &semaphore;

		for ( auto & array : m_activeShadowMaps )
		{
			for ( auto & shadowMap : array )
			{
				shadowMap.get().render( *result );
				result = &shadowMap.get().getSemaphore();
			}
		}

		return result;
	}

	renderer::Semaphore const * RenderTechnique::doRenderEnvironmentMaps( renderer::Semaphore const & semaphore )
	{
		renderer::Semaphore const * result = &semaphore;
		auto & scene = *m_renderTarget.getScene();
		auto & maps = scene.getEnvironmentMaps();

		for ( auto & map : maps )
		{
			result = &map.get().render( *result );
		}

		return result;
	}

	renderer::Semaphore const * RenderTechnique::doRenderBackground( renderer::SemaphoreCRefArray const & semaphores )
	{
		if ( m_renderTarget.getScene()->getFog().getType() != FogType::eDisabled )
		{
			auto & background = m_renderTarget.getScene()->getColourBackground();
			auto & bgSemaphore = background.getSemaphore();
			background.start();
			background.notifyPassRender();
			getEngine()->getRenderSystem()->getCurrentDevice()->getGraphicsQueue().submit( { *m_cbgCommandBuffer }
				, semaphores
				, renderer::PipelineStageFlagsArray( semaphores.size(), renderer::PipelineStageFlag::eColourAttachmentOutput )
				, { bgSemaphore }
				, nullptr );
			background.stop();
			return &bgSemaphore;
		}

		auto & background = m_renderTarget.getScene()->getBackground();
		auto & bgSemaphore = background.getSemaphore();
		background.start();
		background.notifyPassRender();
		getEngine()->getRenderSystem()->getCurrentDevice()->getGraphicsQueue().submit( { *m_bgCommandBuffer }
			, semaphores
			, renderer::PipelineStageFlagsArray( semaphores.size(), renderer::PipelineStageFlag::eColourAttachmentOutput )
			, { bgSemaphore }
			, nullptr );
		background.stop();
		return &bgSemaphore;

	}

	renderer::Semaphore const * RenderTechnique::doRenderOpaque( Point2r const & jitter
		, RenderInfo & info
		, renderer::Semaphore const & semaphore )
	{
		renderer::Semaphore const * result = &semaphore;

		if ( m_opaquePass->hasNodes() )
		{
			auto & scene = *m_renderTarget.getScene();
			auto & camera = *m_renderTarget.getCamera();

#if DEBUG_FORWARD_RENDERING

			result = &static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).render( *result );

#else

			result = &m_deferredRendering->render( info, scene, camera, *result );

#endif
		}

		return result;
	}

	renderer::Semaphore const * RenderTechnique::doRenderTransparent( Point2r const & jitter
		, RenderInfo & info
		, renderer::Semaphore const & semaphore )
	{
		renderer::Semaphore const * result = &semaphore;

		if ( m_transparentPass->hasNodes() )
		{
			auto & scene = *m_renderTarget.getScene();
			auto & camera = *m_renderTarget.getCamera();

#if USE_WEIGHTED_BLEND

			m_weightedBlendRendering->render( info, scene, *result );
			result = &m_weightedBlendRendering->getSemaphore();

#else

			getEngine()->setPerObjectLighting( true );
			m_transparentPass->renderShadowMaps();
			camera.apply();
			m_frameBuffer.m_frameBuffer->bind( FrameBufferTarget::eDraw );
			m_frameBuffer.m_frameBuffer->setDrawBuffers();
			m_transparentPass->render( info
				, scene.hasShadows()
				, m_activeShadowMaps );
			m_frameBuffer.m_frameBuffer->unbind();

#endif
			getEngine()->getRenderSystem()->getCurrentDevice()->waitIdle();
		}

		return result;
	}
}
