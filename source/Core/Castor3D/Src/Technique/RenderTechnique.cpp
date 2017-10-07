#include "RenderTechnique.hpp"

#include "Engine.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Camera.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Skybox.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "ShadowMap/ShadowMapDirectional.hpp"
#include "ShadowMap/ShadowMapPoint.hpp"
#include "ShadowMap/ShadowMapSpot.hpp"

#include "Technique/RenderTechniquePass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Technique/ForwardRenderTechniquePass.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Technique/Transparent/TransparentPass.hpp"

#include <GlslSource.hpp>

#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;

#define DISPLAY_DEBUG_DEFERRED_BUFFERS 1
#define DISPLAY_DEBUG_WEIGHTED_BLEND_BUFFERS 1
#define DISPLAY_DEBUG_IBL_BUFFERS 0
#define DISPLAY_DEBUG_SHADOW_MAPS 1
#define DISPLAY_DEBUG_ENV_MAPS 0

#define USE_WEIGHTED_BLEND 1
#define DEBUG_FORWARD_RENDERING 0

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		void doPrepareShadowMaps( LightCache const & cache
			, LightType type
			, Camera const & camera
			, RenderTechnique::ShadowMapArray & shadowMaps
			, ShadowMapLightTypeArray & activeShadowMaps
			, RenderQueueArray & queues )
		{
			auto lock = makeUniqueLock( cache );
			std::map< double, LightSPtr > lights;

			for ( auto & light : cache.getLights( type ) )
			{
				light->setShadowMap( nullptr );

				if ( light->isShadowProducer()
					&& ( light->getLightType() == LightType::eDirectional
						|| camera.isVisible( light->getCubeBox(), light->getParent()->getDerivedTransformationMatrix() ) ) )
				{
					lights.emplace( point::distanceSquared( camera.getParent()->getDerivedPosition()
						, light->getParent()->getDerivedPosition() )
						, light );
				}
			}

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
	}

	//*************************************************************************************************

	RenderTechnique::TechniqueFbo::TechniqueFbo( RenderTechnique & technique )
		: m_technique{ technique }
	{
	}

	bool RenderTechnique::TechniqueFbo::initialise( Size size )
	{
		m_colourTexture = m_technique.getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGBA16F32F
			, size );
		m_colourTexture->getImage().initialiseSource();
		size = m_colourTexture->getDimensions();

		bool result = m_colourTexture->initialise();

		if ( result )
		{
			m_frameBuffer = m_technique.getEngine()->getRenderSystem()->createFrameBuffer();
			m_depthBuffer = m_technique.getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eD24S8
				, size );
			m_depthBuffer->getImage().initialiseSource();
			result = m_depthBuffer->initialise();
		}

		if ( result )
		{
			m_colourAttach = m_frameBuffer->createAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );
			result = m_frameBuffer->initialise();
		}

		if ( result )
		{
			m_frameBuffer->bind();
			m_frameBuffer->attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->getType() );
			m_frameBuffer->attach( AttachmentPoint::eDepthStencil, m_depthAttach, m_depthBuffer->getType() );
			m_frameBuffer->setDrawBuffer( m_colourAttach );
			result = m_frameBuffer->isComplete();
			m_frameBuffer->unbind();
		}

		return result;
	}

	void RenderTechnique::TechniqueFbo::cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->bind();
			m_frameBuffer->detachAll();
			m_frameBuffer->unbind();
			m_frameBuffer->cleanup();
			m_colourTexture->cleanup();
			m_depthBuffer->cleanup();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
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
		, m_frameBuffer{ *this }
		, m_ssaoConfig{ config }
	{
		m_directionalShadowMaps.resize( 1u );
		m_pointShadowMaps.resize( shader::PointShadowMapCount );
		m_spotShadowMaps.resize( shader::SpotShadowMapCount );

		for ( auto & shadowMap : m_directionalShadowMaps )
		{
			shadowMap = std::make_unique< ShadowMapDirectional >( *renderTarget.getEngine()
				, *renderTarget.getScene() );
		}

		for ( auto & shadowMap : m_pointShadowMaps )
		{
			shadowMap = std::make_unique< ShadowMapPoint >( *renderTarget.getEngine()
				, *renderTarget.getScene() );
		}

		for ( auto & shadowMap : m_spotShadowMaps )
		{
			shadowMap = std::make_unique< ShadowMapSpot >( *renderTarget.getEngine()
				, *renderTarget.getScene() );
		}
	}

	RenderTechnique::~RenderTechnique()
	{
		m_transparentPass.reset();
		m_opaquePass.reset();
	}

	bool RenderTechnique::initialise( uint32_t & index )
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget.getSize();
			m_initialised = m_frameBuffer.initialise( m_size );

			if ( m_initialised )
			{
				doInitialiseShadowMaps();
				m_opaquePass->initialise( m_size );
				m_transparentPass->initialise( m_size );
			}

			if ( m_initialised )
			{
#if !DEBUG_FORWARD_RENDERING
				m_deferredRendering = std::make_unique< DeferredRendering >( *getEngine()
					, static_cast< OpaquePass & >( *m_opaquePass )
					, *m_frameBuffer.m_frameBuffer
					, *m_frameBuffer.m_depthAttach
					, m_renderTarget.getSize()
					, *m_renderTarget.getScene()
					, m_ssaoConfig );
#endif
#if USE_WEIGHTED_BLEND
				m_weightedBlendRendering = std::make_unique< WeightedBlendRendering >( *getEngine()
					, static_cast< TransparentPass & >( *m_transparentPass )
					, *m_frameBuffer.m_frameBuffer
					, m_renderTarget.getSize()
					, *m_renderTarget.getScene() );
#endif
			}

			m_particleTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Particles" ), cuT( "Particles" ) );
			m_postFxTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Post effects" ), cuT( "Post effects" ) );
			ENSURE( m_initialised );
		}

		return m_initialised;
	}

	void RenderTechnique::cleanup()
	{
		m_particleTimer.reset();
		m_postFxTimer.reset();
		m_weightedBlendRendering.reset();
		m_deferredRendering.reset();
		doCleanupShadowMaps();
		m_transparentPass->cleanup();
		m_opaquePass->cleanup();
		m_initialised = false;
		m_frameBuffer.cleanup();
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

	void RenderTechnique::render( Point2r const & jitter
		, TextureUnit const & velocity
		, RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		scene.getLightCache().updateLights();
		m_renderSystem.pushScene( &scene );
		auto & camera = *m_renderTarget.getCamera();
		camera.resize( m_size );
		camera.update();

		// Update part
		doRenderEnvironmentMaps();
		doRenderShadowMaps();
		doUpdateParticles( info );

		// Render part
		doRenderOpaque( jitter
			, velocity
			, info );
		scene.renderBackground( getSize(), camera );
		doRenderTransparent( jitter
			, velocity
			, info );
		doApplyPostEffects();

		m_renderSystem.popScene();
	}

	bool RenderTechnique::writeInto( castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::debugDisplay( Size const & size )const
	{
		uint32_t index = 0u;

#if DISPLAY_DEBUG_DEFERRED_BUFFERS && !DEBUG_FORWARD_RENDERING

		m_deferredRendering->debugDisplay();

#endif
#if USE_WEIGHTED_BLEND && DISPLAY_DEBUG_WEIGHTED_BLEND_BUFFERS

		m_weightedBlendRendering->debugDisplay();

#endif
#if DISPLAY_DEBUG_IBL_BUFFERS

		m_frameBuffer.m_frameBuffer->bind();
		scene.getSkybox().getIbl().debugDisplay( size );
		m_frameBuffer.m_frameBuffer->unbind();

#endif
#if DISPLAY_DEBUG_ENV_MAPS

		index = 0u;

		for ( auto & map : m_renderTarget.getScene()->getEnvironmentMaps() )
		{
			map.get().debugDisplay( size, index++ );
		}

#endif
#if DISPLAY_DEBUG_SHADOW_MAPS

		index = 0u;

		for ( auto & maps : m_activeShadowMaps )
		{
			for ( auto & map : maps )
			{
				map.get().debugDisplay( size, index++ );
			}
		}

#endif
	}

	void RenderTechnique::doInitialiseShadowMaps()
	{
		for ( auto & shadowMap : m_directionalShadowMaps )
		{
			shadowMap->initialise();
		}

		for ( auto & shadowMap : m_pointShadowMaps )
		{
			shadowMap->initialise();
		}

		for ( auto & shadowMap : m_spotShadowMaps )
		{
			shadowMap->initialise();
		}
	}

	void RenderTechnique::doCleanupShadowMaps()
	{
		for ( auto & shadowMap : m_directionalShadowMaps )
		{
			shadowMap->cleanup();
		}

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

		doPrepareShadowMaps( cache
			, LightType::eDirectional
			, camera
			, m_directionalShadowMaps
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

	void RenderTechnique::doRenderShadowMaps()
	{
		getEngine()->getMaterialCache().getPassBuffer().bind();

		for ( auto & array : m_activeShadowMaps )
		{
			for ( auto & shadowMap : array )
			{
				shadowMap.get().render();
			}
		}
	}

	void RenderTechnique::doRenderEnvironmentMaps()
	{
		auto & scene = *m_renderTarget.getScene();
		auto & maps = scene.getEnvironmentMaps();
		getEngine()->getMaterialCache().getPassBuffer().bind();

		for ( auto & map : maps )
		{
			map.get().render();
		}
	}

	void RenderTechnique::doRenderOpaque( Point2r const & jitter
		, TextureUnit const & velocity
		, RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		auto & camera = *m_renderTarget.getCamera();
		getEngine()->getMaterialCache().getPassBuffer().bind();

#if DEBUG_FORWARD_RENDERING

		getEngine()->setPerObjectLighting( true );
		camera.apply();
		m_frameBuffer.m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->setDrawBuffers();
		m_frameBuffer.m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->render( info
			, m_activeShadowMaps );

#else

		m_deferredRendering->render( info
			, scene
			, camera
			, m_activeShadowMaps
			, jitter
			, velocity );

#endif
	}

	void RenderTechnique::doUpdateParticles( RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		m_particleTimer->start();
		scene.getParticleSystemCache().forEach( [this, &info]( ParticleSystem & particleSystem )
		{
			particleSystem.update();
			info.m_particlesCount += particleSystem.getParticlesCount();
		} );
		m_particleTimer->stop();
	}

	void RenderTechnique::doRenderTransparent( Point2r const & jitter
		, TextureUnit const & velocity
		, RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		auto & camera = *m_renderTarget.getCamera();
		getEngine()->getMaterialCache().getPassBuffer().bind();

#if USE_WEIGHTED_BLEND
#	if !DEBUG_FORWARD_RENDERING

		m_deferredRendering->blitDepthInto( m_weightedBlendRendering->getFbo() );

#	endif

		m_weightedBlendRendering->render( info
			, scene
			, camera
			, m_activeShadowMaps
			, jitter
			, velocity );

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
	}

	void RenderTechnique::doApplyPostEffects()
	{
		m_postFxTimer->start();

		for ( auto effect : m_renderTarget.getPostEffects() )
		{
			effect->apply( *m_frameBuffer.m_frameBuffer );
		}

		m_postFxTimer->stop();
	}
}
