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

#define C3D_DisplayDebugDeferredBuffers 0
#define C3D_DisplayDebugWeightedBlendedBuffers 0
#define C3D_DisplayDebugIBLBuffers 0
#define C3D_DisplayDebugShadowMaps 0
#define C3D_DisplayDebugEnvironmentMaps 0

#define C3D_UseWeightedBlendedRendering 1
#define C3D_UseDeferredRendering 1

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
			size_t count = std::min( shadowMap->getCount(), uint32_t( lights.size() ) );

			if ( count > 0 )
			{
				uint32_t index = 0u;
				auto lightIt = lights.begin();
				activeShadowMaps[size_t( type )].emplace_back( std::ref( *shadowMap ), UInt32Array{} );
				auto & active = activeShadowMaps[size_t( type )].back();

				for ( auto i = 0u; i < count; ++i )
				{
					lightIt->second->setShadowMap( shadowMap.get(), index );
					active.second.push_back( index );
					shadowMap->update( camera
						, queues
						, *( lightIt->second )
						, index );
					++index;
					++lightIt;
				}
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
				activeShadowMaps[size_t( type )].emplace_back( std::ref( shadowMap ), 0u );
				shadowMap.update( camera
					, queues
					, *( it->second )
					, i );
				++it;
			}
		}

		TextureLayoutSPtr doCreateTexture( Engine & engine
			, Size const & size
			, ashes::Format format
			, ashes::ImageUsageFlags usage )
		{
			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.getWidth();
			image.extent.height = size.getHeight();
			image.extent.depth = 1u;
			image.format = format;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = usage | ashes::ImageUsageFlag::eSampled;

			auto result = std::make_shared< TextureLayout >( *engine.getRenderSystem()
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
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
		, m_matrixUbo{ *renderSystem.getEngine() }
#if C3D_UseDeferredRendering
		, m_opaquePass{ std::make_unique< OpaquePass >( m_matrixUbo
			, renderTarget.getCuller()
			, config ) }
#else
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "opaque_pass" )
			, m_matrixUbo
			, renderTarget.getCuller()
			, false
			, nullptr
			, config ) }
#endif
#if C3D_UseWeightedBlendedRendering
		, m_transparentPass{ std::make_unique< TransparentPass >( m_matrixUbo
			, renderTarget.getCuller()
			, config ) }
#else
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "transparent_pass" )
			, m_matrixUbo
			, renderTarget.getCuller()
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
		m_directionalShadowMap = std::make_unique< ShadowMapDirectional >( *renderTarget.getEngine()
			, *renderTarget.getScene()
			, renderTarget.getScene()->getDirectionalShadowCascades() );
		m_allShadowMaps[size_t( LightType::eDirectional )].emplace_back( std::ref( *m_directionalShadowMap ), UInt32Array{} );
		m_spotShadowMap = std::make_unique< ShadowMapSpot >( *renderTarget.getEngine()
			, *renderTarget.getScene() );
		m_allShadowMaps[size_t( LightType::eSpot )].emplace_back( std::ref( *m_spotShadowMap ), UInt32Array{} );
		m_pointShadowMap = std::make_unique< ShadowMapPoint >( *renderTarget.getEngine()
			, *renderTarget.getScene() );
		m_allShadowMaps[size_t( LightType::ePoint )].emplace_back( std::ref( *m_pointShadowMap ), UInt32Array{} );
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
		m_pointShadowMap.reset();
		m_spotShadowMap.reset();
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
				, ashes::Format::eR16G16B16A16_SFLOAT
				, ( ashes::ImageUsageFlag::eColourAttachment
					| ashes::ImageUsageFlag::eTransferDst ) );
			m_depthBuffer = doCreateTexture( *getEngine()
				, m_size
				, ashes::Format::eD24_UNORM_S8_UINT
				, ashes::ImageUsageFlag::eDepthStencilAttachment );
			m_signalFinished = getCurrentDevice( *this ).createSemaphore();
			m_hdrConfigUbo.initialise();
			m_matrixUbo.initialise();

			auto & device = getCurrentDevice( *this );
			m_stagingBuffer = std::make_unique< ashes::StagingBuffer >( device
				, ashes::BufferTarget::eTransferSrc
				, m_matrixUbo.getUbo().getAlignedSize() );
			m_uploadCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer();

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
		m_matrixUbo.cleanup();
		m_uploadCommandBuffer.reset();
		m_stagingBuffer.reset();
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
		m_renderTarget.update();
		m_opaquePass->update( queues );
		m_transparentPass->update( queues );
		doUpdateShadowMaps( queues );
		auto & maps = m_renderTarget.getScene()->getEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().update( queues );
		}
	}

	ashes::Semaphore const & RenderTechnique::render( Point2r const & jitter
		, ashes::SemaphoreCRefArray const & waitSemaphores
		, RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		auto & camera = *m_renderTarget.getCamera();
		scene.getLightCache().updateLightsTexture( camera );
		scene.updateDeviceDependent( camera );

		// Update part
		doUpdateParticles( info );
		auto jitterProjSpace = jitter * 2.0_r;
		jitterProjSpace[0] /= camera.getWidth();
		jitterProjSpace[1] /= camera.getHeight();
		m_matrixUbo.update( camera.getView()
			, camera.getProjection()
			, jitterProjSpace
			, *m_stagingBuffer
			, *m_uploadCommandBuffer );
		m_hdrConfigUbo.update( m_renderTarget.getHdrConfig() );
		auto * semaphore = &doRenderBackground( waitSemaphores );
		semaphore = &doRenderEnvironmentMaps( *semaphore );
		semaphore = &doRenderShadowMaps( *semaphore );

#if C3D_UseDeferredRendering

		m_deferredRendering->update( info
			, scene
			, camera
			, jitter );

#else

		static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).update( info
			, jitter );

#endif
#if C3D_UseWeightedBlendedRendering

		m_weightedBlendRendering->update( info
			, scene
			, camera
			, jitter );

#endif

		// Render part
		semaphore = &doRenderOpaque( jitter, info, *semaphore );
		semaphore = &doRenderTransparent( jitter, info, *semaphore );
		return *semaphore;
	}

	bool RenderTechnique::writeInto( castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::debugDisplay( Size const & size )const
	{
		uint32_t index = 0u;

#if C3D_UseDeferredRendering && C3D_DisplayDebugDeferredBuffers

		m_deferredRendering->debugDisplay( *m_debugRenderPass
			, *m_debugFrameBuffer );

#endif
#if C3D_UseWeightedBlendedRendering && C3D_DisplayDebugWeightedBlendedBuffers

		m_weightedBlendRendering->debugDisplay( *m_debugRenderPass
			, *m_debugFrameBuffer);

#endif
#if C3D_DisplayDebugIBLBuffers

		m_frameBuffer.m_frameBuffer->bind();
		scene.getSkybox().getIbl().debugDisplay( size );
		m_frameBuffer.m_frameBuffer->unbind();

#endif
#if C3D_DisplayDebugEnvironmentMaps

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			map.get().debugDisplay( *m_debugRenderPass
				, *m_debugFrameBuffer
				, size
				, index++ );
		}

		index = 0u;

#endif
#if C3D_DisplayDebugShadowMaps

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
		if ( checkFlag( visitor.getPassFlags(), PassFlag::eAlphaBlending ) )
		{
#if C3D_UseWeightedBlendedRendering
			m_weightedBlendRendering->accept( visitor );
#else
			m_transparentPass->accept( visitor );
#endif
		}
		else
		{
#if C3D_UseDeferredRendering
			m_deferredRendering->accept( visitor );
#else
			m_opaquePass->accept( visitor );
#endif
		}
	}

	void RenderTechnique::doInitialiseShadowMaps()
	{
		m_directionalShadowMap->initialise();
		m_spotShadowMap->initialise();
		m_pointShadowMap->initialise();
	}

	void RenderTechnique::doInitialiseBackgroundPass()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		m_bgCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );
		m_cbgCommandBuffer = device.getGraphicsCommandPool().createCommandBuffer( true );

		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = m_depthBuffer->getPixelFormat();
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = m_colourTexture->getPixelFormat();
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].depthStencilAttachment = { 0u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };
		renderPass.subpasses[0].colorAttachments = { { 1u, ashes::ImageLayout::eColourAttachmentOptimal } };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eMemoryRead;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite | ashes::AccessFlag::eColourAttachmentRead;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eMemoryRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eBottomOfPipe;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_bgRenderPass = device.createRenderPass( renderPass );

		ashes::FrameBufferAttachmentArray attaches
		{
			{ *( m_bgRenderPass->getAttachments().begin() + 0u ), m_depthBuffer->getDefaultView() },
			{ *( m_bgRenderPass->getAttachments().begin() + 1u ), m_colourTexture->getDefaultView() },
		};
		m_bgFrameBuffer = m_bgRenderPass->createFrameBuffer( { m_depthBuffer->getDimensions().width, m_depthBuffer->getDimensions().height }
			, std::move( attaches ) );

		auto & background = *m_renderTarget.getScene()->getBackground();
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
#if C3D_UseDeferredRendering

		m_opaquePass->initialise( m_size );
		m_deferredRendering = std::make_unique< DeferredRendering >( *getEngine()
			, static_cast< OpaquePass & >( *m_opaquePass )
			, m_depthBuffer
			, m_renderTarget.getVelocity().getTexture()
			, m_colourTexture
			, m_renderTarget.getSize()
			, *m_renderTarget.getScene()
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
#if C3D_UseWeightedBlendedRendering

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
		static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).initialiseRenderPass( m_colourTexture->getDefaultView()
			, m_depthBuffer->getDefaultView()
			, m_size
			, false );

#endif
	}

	void RenderTechnique::doInitialiseDebugPass()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_colourTexture->getPixelFormat();
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eLoad;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eColourAttachmentOptimal;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].colorAttachments = { { 0u, ashes::ImageLayout::eColourAttachmentOptimal } };

		m_debugRenderPass = device.createRenderPass( renderPass );

		ashes::FrameBufferAttachmentArray attaches
		{
			{ *( m_bgRenderPass->getAttachments().begin() + 0u ), m_colourTexture->getDefaultView() },
		};
		m_debugFrameBuffer = m_debugRenderPass->createFrameBuffer( { m_colourTexture->getDimensions().width, m_colourTexture->getDimensions().height }
			, std::move( attaches ) );
	}

	void RenderTechnique::doCleanupShadowMaps()
	{
		m_directionalShadowMap->cleanup();
		m_spotShadowMap->cleanup();
		m_pointShadowMap->cleanup();
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
		doPrepareShadowMap( cache
			, LightType::ePoint
			, camera
			, m_pointShadowMap
			, m_activeShadowMaps
			, queues );
		doPrepareShadowMap( cache
			, LightType::eSpot
			, camera
			, m_spotShadowMap
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

			auto timerBlock = m_particleTimer->start();
			uint32_t index = 0u;

			for ( auto & particleSystem : cache )
			{
				particleSystem.second->update( *m_particleTimer, index );
				m_particleTimer->notifyPassRender( index++ );
				m_particleTimer->notifyPassRender( index++ );
				info.m_particlesCount += particleSystem.second->getParticlesCount();
			}
		}
	}

	ashes::Semaphore const & RenderTechnique::doRenderShadowMaps( ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		for ( auto & array : m_activeShadowMaps )
		{
			for ( auto & shadowMap : array )
			{
				for ( auto & index : shadowMap.second )
				{
					result = &shadowMap.first.get().render( *result, index );
				}
			}
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderEnvironmentMaps( ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			result = &map.get().render( *result );
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderBackground( ashes::SemaphoreCRefArray const & semaphores )
	{
		auto const & queue = getCurrentDevice( *this ).getGraphicsQueue();
		ashes::PipelineStageFlagsArray const stages( semaphores.size(), ashes::PipelineStageFlag::eColourAttachmentOutput );

		if ( m_renderTarget.getScene()->getFog().getType() != FogType::eDisabled )
		{
			auto & background = m_renderTarget.getScene()->getColourBackground();
			auto & bgSemaphore = background.getSemaphore();
			auto timerBlock = background.start();
			background.notifyPassRender();
			queue.submit( { *m_cbgCommandBuffer }
				, semaphores
				, stages
				, { bgSemaphore }
				, nullptr );
			return bgSemaphore;
		}

		auto & background = *m_renderTarget.getScene()->getBackground();
		auto & bgSemaphore = background.getSemaphore();
		auto timerBlock = background.start();
		background.notifyPassRender();
		queue.submit( { *m_bgCommandBuffer }
			, semaphores
			, stages
			, { bgSemaphore }
			, nullptr );
		return bgSemaphore;

	}

	ashes::Semaphore const & RenderTechnique::doRenderOpaque( Point2r const & jitter
		, RenderInfo & info
		, ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		if ( m_opaquePass->hasNodes() )
		{
			auto & scene = *m_renderTarget.getScene();
			auto & camera = *m_renderTarget.getCamera();

#if C3D_UseDeferredRendering
			result = &m_deferredRendering->render( info, scene, camera, *result );
#else
			result = &static_cast< ForwardRenderTechniquePass & >( *m_opaquePass ).render( *result );
#endif
		}

		return *result;
	}

	ashes::Semaphore const & RenderTechnique::doRenderTransparent( Point2r const & jitter
		, RenderInfo & info
		, ashes::Semaphore const & semaphore )
	{
		ashes::Semaphore const * result = &semaphore;

		if ( m_transparentPass->hasNodes() )
		{
			auto & scene = *m_renderTarget.getScene();

#if C3D_UseWeightedBlendedRendering
			result = &m_weightedBlendRendering->render( info, scene, *result );
#else
			result = &static_cast< ForwardRenderTechniquePass & >( *m_transparentPass ).render( *result );
#endif
		}

		return *result;
	}
}
