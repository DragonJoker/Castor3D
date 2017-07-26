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

using namespace Castor;

#define DEBUG_DEFERRED_BUFFERS 1
#define DEBUG_WEIGHTED_BLEND_BUFFERS 1
#define DEBUG_IBL_BUFFERS 0
#define USE_WEIGHTED_BLEND 1

#define DEBUG_FORWARD_RENDERING 0

namespace Castor3D
{
	//*************************************************************************************************

	RenderTechnique::TechniqueFbo::TechniqueFbo( RenderTechnique & technique )
		: m_technique{ technique }
	{
	}

	bool RenderTechnique::TechniqueFbo::Initialise( Size size )
	{
		m_colourTexture = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGBA16F32F
			, size );
		m_colourTexture->GetImage().InitialiseSource();
		size = m_colourTexture->GetDimensions();

		bool result = m_colourTexture->Initialise();

		if ( result )
		{
			m_frameBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_depthBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eD24S8
				, size );
			m_depthBuffer->GetImage().InitialiseSource();
			result = m_depthBuffer->Initialise();
		}

		if ( result )
		{
			m_colourAttach = m_frameBuffer->CreateAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
			result = m_frameBuffer->Create();
		}

		if ( result )
		{
			result = m_frameBuffer->Initialise( size );

			if ( result )
			{
				m_frameBuffer->Bind();
				m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::eDepthStencil, m_depthAttach, m_depthBuffer->GetType() );
				m_frameBuffer->SetDrawBuffer( m_colourAttach );
				result = m_frameBuffer->IsComplete();
				m_frameBuffer->Unbind();
			}
			else
			{
				m_frameBuffer->Destroy();
			}
		}

		return result;
	}

	void RenderTechnique::TechniqueFbo::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind();
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_colourTexture->Cleanup();
			m_depthBuffer->Cleanup();

			m_frameBuffer->Destroy();

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

		: OwnedBy< Engine >{ *renderSystem.GetEngine() }
		, Named{ name }
		, m_renderTarget{ renderTarget }
		, m_renderSystem{ renderSystem }
#if DEBUG_FORWARD_RENDERING
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "opaque_pass" )
			, *renderTarget.GetScene()
			, renderTarget.GetCamera().get()
			, false
			, nullptr
			, config ) }
#else
		, m_opaquePass{ std::make_unique< OpaquePass >( *renderTarget.GetScene()
			, renderTarget.GetCamera().get()
			, config ) }
#endif
#if USE_WEIGHTED_BLEND
		, m_transparentPass{ std::make_unique< TransparentPass >( *renderTarget.GetScene()
			, renderTarget.GetCamera().get()
			, config ) }
#else
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "forward_transparent" )
			, *renderTarget.GetScene()
			, renderTarget.GetCamera().get()
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

	bool RenderTechnique::Initialise( uint32_t & index )
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget.GetSize();
			m_initialised = m_frameBuffer.Initialise( m_size );

			if ( m_initialised )
			{
				m_initialised = m_opaquePass->InitialiseShadowMaps();
			}

			if ( m_initialised )
			{
				m_initialised = m_transparentPass->InitialiseShadowMaps();
			}

			if ( m_initialised )
			{
				m_opaquePass->Initialise( m_size );
				m_transparentPass->Initialise( m_size );
			}

			if ( m_initialised )
			{
#if !DEBUG_FORWARD_RENDERING
				m_deferredRendering = std::make_unique< DeferredRendering >( *GetEngine()
					, static_cast< OpaquePass & >( *m_opaquePass )
					, *m_frameBuffer.m_frameBuffer
					, *m_frameBuffer.m_depthAttach
					, m_renderTarget.GetSize()
					, *m_renderTarget.GetScene()
					, m_ssaoConfig );
#endif
#if USE_WEIGHTED_BLEND
				m_weightedBlendRendering = std::make_unique< WeightedBlendRendering >( *GetEngine()
					, static_cast< TransparentPass & >( *m_transparentPass )
					, *m_frameBuffer.m_frameBuffer
					, m_renderTarget.GetSize()
					, *m_renderTarget.GetScene() );
#endif
			}

			m_particleTimer = std::make_shared< RenderPassTimer >( *GetEngine(), cuT( "Particles" ) );
			m_postFxTimer = std::make_shared< RenderPassTimer >( *GetEngine(), cuT( "Post effects" ) );
			ENSURE( m_initialised );
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		m_particleTimer.reset();
		m_postFxTimer.reset();
		m_weightedBlendRendering.reset();
		m_deferredRendering.reset();
		m_transparentPass->CleanupShadowMaps();
		m_opaquePass->CleanupShadowMaps();
		m_transparentPass->Cleanup();
		m_opaquePass->Cleanup();
		m_initialised = false;
		m_frameBuffer.Cleanup();
	}

	void RenderTechnique::Update( RenderQueueArray & queues )
	{
		m_opaquePass->Update( queues );
		m_transparentPass->Update( queues );
		m_opaquePass->UpdateShadowMaps( queues );
		m_transparentPass->UpdateShadowMaps( queues );
		auto & maps = m_renderTarget.GetScene()->GetEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().Update( queues );
		}
	}

	void RenderTechnique::Render( RenderInfo & info )
	{
		auto & scene = *m_renderTarget.GetScene();
		scene.GetLightCache().UpdateLights();
		m_renderSystem.PushScene( &scene );
		auto & maps = scene.GetEnvironmentMaps();

		for ( auto & map : maps )
		{
			map.get().Render();
		}

		auto & camera = *m_renderTarget.GetCamera();
		camera.Resize( m_size );
		camera.Update();

#if DEBUG_FORWARD_RENDERING

		GetEngine()->SetPerObjectLighting( true );
		m_opaquePass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetDrawBuffers();
		m_frameBuffer.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->Render( info, m_renderTarget.GetScene()->HasShadows() );

#else

		m_deferredRendering->Render( info
			, scene
			, camera );

#endif

		scene.RenderBackground( GetSize(), camera );

		m_particleTimer->Start();
		scene.GetParticleSystemCache().ForEach( [this, &info]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.Update();
			info.m_particlesCount += p_particleSystem.GetParticlesCount();
		} );

		m_frameBuffer.m_frameBuffer->Unbind();
		m_particleTimer->Stop();

#if USE_WEIGHTED_BLEND
#	if !DEBUG_FORWARD_RENDERING

		m_deferredRendering->BlitDepthInto( m_weightedBlendRendering->GetFbo() );

#	endif

		m_weightedBlendRendering->Render( info
			, scene
			, camera );

#else

		GetEngine()->SetPerObjectLighting( true );
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetDrawBuffers();
		m_transparentPass->Render( info, m_renderTarget.GetScene()->HasShadows() );
		m_frameBuffer.m_frameBuffer->Unbind();

#endif

#if DEBUG_DEFERRED_BUFFERS && !DEBUG_FORWARD_RENDERING && !defined( NDEBUG )
		m_deferredRendering->Debug( camera );
#endif

#if USE_WEIGHTED_BLEND && DEBUG_WEIGHTED_BLEND_BUFFERS && !defined( NDEBUG )
		m_weightedBlendRendering->Debug( camera );
#endif

#if DEBUG_IBL_BUFFERS && !defined( NDEBUG )
		m_frameBuffer.m_frameBuffer->Bind();
		scene.GetSkybox().GetIbl().Debug( camera );
		m_frameBuffer.m_frameBuffer->Unbind();
#endif

		m_postFxTimer->Start();

		for ( auto effect : m_renderTarget.GetPostEffects() )
		{
			effect->Apply( *m_frameBuffer.m_frameBuffer );
		}

		m_postFxTimer->Stop();
		m_renderSystem.PopScene();
	}

	bool RenderTechnique::WriteInto( Castor::TextFile & file )
	{
		return true;
	}

	void RenderTechnique::AddShadowProducer( Light & light )
	{
		m_transparentPass->AddShadowProducer( light );
		m_opaquePass->AddShadowProducer( light );
	}
}
