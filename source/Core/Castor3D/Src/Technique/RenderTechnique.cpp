#include "RenderTechnique.hpp"

#include "Engine.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/ColourRenderBuffer.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "FrameBuffer/RenderBufferAttachment.hpp"
#include "EnvironmentMap/EnvironmentMap.hpp"
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

	RenderTechnique::TechniqueFbo::TechniqueFbo( RenderTechnique & p_technique )
		: m_technique{ p_technique }
	{
	}

	bool RenderTechnique::TechniqueFbo::Initialise( Size p_size )
	{
		m_colourTexture = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGBA16F32F
			, p_size );
		m_colourTexture->GetImage().InitialiseSource();
		p_size = m_colourTexture->GetDimensions();

		bool l_return = m_colourTexture->Initialise();

		if ( l_return )
		{
			m_frameBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_depthBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eD24S8
				, p_size );
			m_depthBuffer->GetImage().InitialiseSource();
			l_return = m_depthBuffer->Initialise();
		}

		if ( l_return )
		{
			m_colourAttach = m_frameBuffer->CreateAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->CreateAttachment( m_depthBuffer );
			l_return = m_frameBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_frameBuffer->Initialise( p_size );

			if ( l_return )
			{
				m_frameBuffer->Bind();
				m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::eDepthStencil, m_depthAttach, m_depthBuffer->GetType() );
				m_frameBuffer->SetDrawBuffer( m_colourAttach );
				l_return = m_frameBuffer->IsComplete();
				m_frameBuffer->Unbind();
			}
			else
			{
				m_frameBuffer->Destroy();
			}
		}

		return l_return;
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

	RenderTechnique::RenderTechnique( String const & p_name
		, RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, Parameters const & p_params
		, SsaoConfig const & p_config )

		: OwnedBy< Engine >{ *p_renderSystem.GetEngine() }
		, Named{ p_name }
		, m_renderTarget{ p_renderTarget }
		, m_renderSystem{ p_renderSystem }
#if DEBUG_FORWARD_RENDERING
		, m_opaquePass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "opaque_pass" )
			, *p_renderTarget.GetScene()
			, p_renderTarget.GetCamera().get()
			, false
			, nullptr
			, p_config ) }
#else
		, m_opaquePass{ std::make_unique< OpaquePass >( *p_renderTarget.GetScene()
			, p_renderTarget.GetCamera().get()
			, p_config ) }
#endif
#if USE_WEIGHTED_BLEND
		, m_transparentPass{ std::make_unique< TransparentPass >( *p_renderTarget.GetScene()
			, p_renderTarget.GetCamera().get()
			, p_config ) }
#else
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "forward_transparent" )
			, *p_renderTarget.GetScene()
			, p_renderTarget.GetCamera().get()
			, false
			, false
			, false
			, nullptr
			, p_config ) }
#endif
		, m_initialised{ false }
		, m_frameBuffer{ *this }
		, m_ssaoConfig{ p_config }
	{
	}

	RenderTechnique::~RenderTechnique()
	{
		m_transparentPass.reset();
		m_opaquePass.reset();
	}

	bool RenderTechnique::Initialise( uint32_t & p_index )
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

			ENSURE( m_initialised );
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		m_weightedBlendRendering.reset();
		m_deferredRendering.reset();
		m_transparentPass->CleanupShadowMaps();
		m_opaquePass->CleanupShadowMaps();
		m_transparentPass->Cleanup();
		m_opaquePass->Cleanup();
		m_initialised = false;
		m_frameBuffer.Cleanup();
	}

	void RenderTechnique::Update( RenderQueueArray & p_queues )
	{
		m_opaquePass->Update( p_queues );
		m_transparentPass->Update( p_queues );
		m_opaquePass->UpdateShadowMaps( p_queues );
		m_transparentPass->UpdateShadowMaps( p_queues );
		auto & l_maps = m_renderTarget.GetScene()->GetEnvironmentMaps();

		for ( auto & l_map : l_maps )
		{
			l_map.get().Update( p_queues );
		}
	}

	void RenderTechnique::Render( RenderInfo & p_info )
	{
		auto & l_scene = *m_renderTarget.GetScene();
		l_scene.GetLightCache().UpdateLights();
		m_renderSystem.PushScene( &l_scene );
		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
		auto & l_maps = l_scene.GetEnvironmentMaps();

		for ( auto & l_map : l_maps )
		{
			l_map.get().Render();
		}

		auto & l_camera = *m_renderTarget.GetCamera();
		l_camera.Resize( m_size );
		l_camera.Update();

#if DEBUG_FORWARD_RENDERING

		GetEngine()->SetPerObjectLighting( true );
		m_opaquePass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetDrawBuffers();
		m_frameBuffer.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );

#else

		m_deferredRendering->Render( p_info
			, l_scene
			, l_camera );

#endif

		l_scene.RenderBackground( GetSize(), l_camera );

		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
		l_scene.GetParticleSystemCache().ForEach( [this, &p_info]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.Update();
			p_info.m_particlesCount += p_particleSystem.GetParticlesCount();
		} );

		m_frameBuffer.m_frameBuffer->Unbind();
		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();

#if USE_WEIGHTED_BLEND
#	if !DEBUG_FORWARD_RENDERING

		m_deferredRendering->BlitDepthInto( m_weightedBlendRendering->GetFbo() );

#	endif

		m_weightedBlendRendering->Render( p_info
			, l_scene
			, l_camera );

#else

		GetEngine()->SetPerObjectLighting( true );
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetDrawBuffers();
		m_transparentPass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_frameBuffer.m_frameBuffer->Unbind();

#endif

#if DEBUG_DEFERRED_BUFFERS && !DEBUG_FORWARD_RENDERING && !defined( NDEBUG )
		m_deferredRendering->Debug( l_camera );
#endif

#if USE_WEIGHTED_BLEND && DEBUG_WEIGHTED_BLEND_BUFFERS && !defined( NDEBUG )
		m_weightedBlendRendering->Debug( l_camera );
#endif

#if DEBUG_IBL_BUFFERS && !defined( NDEBUG )
		m_frameBuffer.m_frameBuffer->Bind();
		l_scene.GetSkybox().GetIbl().Debug( l_camera );
		m_frameBuffer.m_frameBuffer->Unbind();
#endif

		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
		for ( auto l_effect : m_renderTarget.GetPostEffects() )
		{
			l_effect->Apply( *m_frameBuffer.m_frameBuffer );
		}

		m_renderSystem.PopScene();
	}

	bool RenderTechnique::WriteInto( Castor::TextFile & p_file )
	{
		return true;
	}

	void RenderTechnique::AddShadowProducer( Light & p_light )
	{
		m_transparentPass->AddShadowProducer( p_light );
		m_opaquePass->AddShadowProducer( p_light );
	}
}
