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
#include "Shader/PassBuffer.hpp"
#include "ShadowMap/ShadowMapPass.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Technique/ForwardRenderTechniquePass.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Technique/Transparent/TransparentPass.hpp"

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>

using namespace castor;

#define DISPLAY_DEBUG_DEFERRED_BUFFERS 1
#define DISPLAY_DEBUG_WEIGHTED_BLEND_BUFFERS 1
#define DISPLAY_DEBUG_IBL_BUFFERS 0

#define USE_WEIGHTED_BLEND 1
#define DEBUG_FORWARD_RENDERING 0

namespace castor3d
{
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
			result = m_frameBuffer->create();
		}

		if ( result )
		{
			result = m_frameBuffer->initialise( size );

			if ( result )
			{
				m_frameBuffer->bind();
				m_frameBuffer->attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->getType() );
				m_frameBuffer->attach( AttachmentPoint::eDepthStencil, m_depthAttach, m_depthBuffer->getType() );
				m_frameBuffer->setDrawBuffer( m_colourAttach );
				result = m_frameBuffer->isComplete();
				m_frameBuffer->unbind();
			}
			else
			{
				m_frameBuffer->destroy();
			}
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

			m_frameBuffer->destroy();

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
				m_initialised = m_opaquePass->initialiseShadowMaps();
			}

			if ( m_initialised )
			{
				m_initialised = m_transparentPass->initialiseShadowMaps();
			}

			if ( m_initialised )
			{
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

			m_particleTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Particles" ) );
			m_postFxTimer = std::make_shared< RenderPassTimer >( *getEngine(), cuT( "Post effects" ) );
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
		m_transparentPass->cleanupShadowMaps();
		m_opaquePass->cleanupShadowMaps();
		m_transparentPass->cleanup();
		m_opaquePass->cleanup();
		m_initialised = false;
		m_frameBuffer.cleanup();
	}

	void RenderTechnique::update( RenderQueueArray & queues )
	{
		m_opaquePass->update( queues );
		m_transparentPass->update( queues );
		m_opaquePass->updateShadowMaps( queues );
		m_transparentPass->updateShadowMaps( queues );
		auto & maps = m_renderTarget.getScene()->getEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().update( queues );
		}
	}

	void RenderTechnique::render( RenderInfo & info )
	{
		auto & scene = *m_renderTarget.getScene();
		scene.getLightCache().updateLights();
		m_renderSystem.pushScene( &scene );
		auto & maps = scene.getEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().render();
		}

		auto & camera = *m_renderTarget.getCamera();
		camera.resize( m_size );
		camera.update();

#if DEBUG_FORWARD_RENDERING

		getEngine()->setPerObjectLighting( true );
		m_opaquePass->renderShadowMaps();
		m_renderTarget.getCamera()->apply();
		m_frameBuffer.m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->setDrawBuffers();
		m_frameBuffer.m_frameBuffer->clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->render( info, m_renderTarget.getScene()->hasShadows() );

#else

		m_deferredRendering->render( info
			, scene
			, camera );

#endif

		scene.renderBackground( getSize(), camera );

		m_particleTimer->start();
		scene.getParticleSystemCache().forEach( [this, &info]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.update();
			info.m_particlesCount += p_particleSystem.getParticlesCount();
		} );

		m_frameBuffer.m_frameBuffer->unbind();
		m_particleTimer->stop();

#if USE_WEIGHTED_BLEND
#	if !DEBUG_FORWARD_RENDERING

		m_deferredRendering->blitDepthInto( m_weightedBlendRendering->getFbo() );

#	endif

		m_weightedBlendRendering->render( info
			, scene
			, camera );

#else

		getEngine()->setPerObjectLighting( true );
		m_transparentPass->renderShadowMaps();
		m_renderTarget.getCamera()->apply();
		m_frameBuffer.m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->setDrawBuffers();
		m_transparentPass->render( info, m_renderTarget.getScene()->hasShadows() );
		m_frameBuffer.m_frameBuffer->unbind();

#endif

		m_postFxTimer->start();

		for ( auto effect : m_renderTarget.getPostEffects() )
		{
			effect->apply( *m_frameBuffer.m_frameBuffer );
		}

		m_postFxTimer->stop();
		m_renderSystem.popScene();
	}

	bool RenderTechnique::writeInto( castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::addShadowProducer( Light & light )
	{
		m_transparentPass->addShadowProducer( light );
		m_opaquePass->addShadowProducer( light );
	}


	void RenderTechnique::debugDisplay( Size const & size )const
	{
#if DISPLAY_DEBUG_DEFERRED_BUFFERS && !DEBUG_FORWARD_RENDERING && !defined( NDEBUG )

		m_deferredRendering->debugDisplay();

#endif
#if USE_WEIGHTED_BLEND && DISPLAY_DEBUG_WEIGHTED_BLEND_BUFFERS && !defined( NDEBUG )

		m_weightedBlendRendering->debugDisplay();

#endif
#if DISPLAY_DEBUG_IBL_BUFFERS && !defined( NDEBUG )

		m_frameBuffer.m_frameBuffer->bind();
		scene.getSkybox().getIbl().debugDisplay( size );
		m_frameBuffer.m_frameBuffer->unbind();

#endif
	}
}
