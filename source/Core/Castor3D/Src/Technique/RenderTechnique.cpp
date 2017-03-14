#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( disable:4503 )
#endif

#include "RenderTechnique.hpp"

#include "Engine.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Scene/Camera.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Scene/Scene.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPass.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>
#include <GlslShadow.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	RenderTechnique::stFRAME_BUFFER::stFRAME_BUFFER( RenderTechnique & p_technique )
		: m_technique{ p_technique }
	{
	}

	bool RenderTechnique::stFRAME_BUFFER::Initialise( Size p_size )
	{
		m_colourTexture = m_technique.GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead | AccessType::eWrite, PixelFormat::eRGBA16F32F, p_size );
		m_colourTexture->GetImage().InitialiseSource();
		p_size = m_colourTexture->GetDimensions();

		bool l_return = m_colourTexture->Initialise();

		if ( l_return )
		{
			m_frameBuffer = m_technique.GetEngine()->GetRenderSystem()->CreateFrameBuffer();
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32FS8 );
			l_return = m_depthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_depthBuffer->Initialise( p_size );

			if ( !l_return )
			{
				m_depthBuffer->Destroy();
			}
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
				m_frameBuffer->Attach( AttachmentPoint::eDepthStencil, m_depthAttach );
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

	void RenderTechnique::stFRAME_BUFFER::Cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->Bind();
			m_frameBuffer->DetachAll();
			m_frameBuffer->Unbind();
			m_frameBuffer->Cleanup();
			m_colourTexture->Cleanup();
			m_depthBuffer->Cleanup();

			m_depthBuffer->Destroy();
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
		, std::unique_ptr< RenderTechniquePass > && p_opaquePass
		, std::unique_ptr< RenderTechniquePass > && p_transparentPass
		, Parameters const & CU_PARAM_UNUSED( p_params )
		, bool p_multisampling )

		: OwnedBy< Engine >{ *p_renderSystem.GetEngine() }
		, Named{ p_name }
		, m_renderTarget{ p_renderTarget }
		, m_renderSystem{ p_renderSystem }
		, m_opaquePass{ std::move( p_opaquePass ) }
		, m_transparentPass{ std::move( p_transparentPass ) }
		, m_initialised{ false }
		, m_frameBuffer{ *this }
	{
	}

	RenderTechnique::~RenderTechnique()
	{
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
				m_initialised = DoInitialise( p_index );
			}

			ENSURE( m_initialised );
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		DoCleanup();
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
	}

	void RenderTechnique::Render( uint32_t & p_visible, uint32_t & p_particles )
	{
		auto & l_scene = *m_renderTarget.GetScene();
		l_scene.GetLightCache().UpdateLights();
		m_renderSystem.PushScene( &l_scene );
		bool l_shadows = l_scene.HasShadows();
		
		auto & l_camera = *m_renderTarget.GetCamera();
		l_camera.Resize( m_size );
		l_camera.Update();

		DoRenderOpaque( p_visible );

		if ( l_scene.GetFog().GetType() == GLSL::FogType::eDisabled )
		{
			l_scene.RenderBackground( GetSize(), l_camera );
		}

		l_scene.GetParticleSystemCache().ForEach( [this, &p_particles]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.Update();
			p_particles += p_particleSystem.GetParticlesCount();
		} );

		DoRenderTransparent( p_visible );

		for ( auto l_effect : m_renderTarget.GetPostEffects() )
		{
			l_effect->Apply( *m_frameBuffer.m_frameBuffer );
		}

#if !defined( NDEBUG )

		//m_frameBuffer.m_frameBuffer->Bind();

		//auto & l_depthMap = m_opaquePass->GetDirectionalShadowMap().GetTexture();
		//Size l_size{ 256u, 256u };
		//m_renderSystem.GetCurrentContext()->RenderDepth( l_size
		//	, *l_depthMap.GetTexture(), 0u );

		//auto & l_depthMap = m_opaquePass->GetSpotShadowMap().GetTexture();
		//Size l_size{ 256u, 256u };
		//m_renderSystem.GetCurrentContext()->RenderDepth( Position{}
		//	, l_size
		//	, *l_depthMap.GetTexture(), 0u );

		//Size l_size{ 512u, 512u };
		//auto & l_depthMap1 = m_opaquePass->GetPointShadowMaps()[0u];
		//m_renderSystem.GetCurrentContext()->RenderDepthCube( Position{ 0, 0 }
		//	, Size{ l_size.width() / 4, l_size.height() / 4 }
		//	, *l_depthMap1.GetTexture() );
		//auto & l_depthMap2 = m_opaquePass->GetPointShadowMaps()[1u];
		//m_renderSystem.GetCurrentContext()->RenderDepthCube( Position{ 0, 512 }
		//	, Size{ l_size.width() / 4, l_size.height() / 4 }
		//	, *l_depthMap2.GetTexture() );

		//m_frameBuffer.m_frameBuffer->Unbind();

#endif

		m_renderSystem.PopScene();
	}

	bool RenderTechnique::WriteInto( Castor::TextFile & p_file )
	{
		return DoWriteInto( p_file );
	}
}
