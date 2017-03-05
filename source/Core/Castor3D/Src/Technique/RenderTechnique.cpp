#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( disable:4503 )
#endif

#include "RenderTechnique.hpp"

#include "Engine.hpp"
#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
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
			m_depthBuffer = m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD32F );
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
				m_frameBuffer->Bind( FrameBufferMode::eConfig );
				m_frameBuffer->Attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->GetType() );
				m_frameBuffer->Attach( AttachmentPoint::eDepth, m_depthAttach );
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
			m_frameBuffer->Bind( FrameBufferMode::eConfig );
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
		, m_directionalShadowMap{ *p_renderTarget.GetEngine() }
		, m_spotShadowMap{ *p_renderTarget.GetEngine() }
		, m_pointShadowMap{ *p_renderTarget.GetEngine() }
	{
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	bool RenderTechnique::Create()
	{
		return DoCreate();
	}

	void RenderTechnique::Destroy()
	{
		DoDestroy();
	}

	bool RenderTechnique::Initialise( uint32_t & p_index )
	{
		if ( !m_initialised )
		{
			m_size = m_renderTarget.GetSize();
			m_initialised = DoInitialise( p_index );

			if ( m_initialised )
			{
				m_initialised = m_frameBuffer.Initialise( m_size );
			}

			auto & l_scene = *m_renderTarget.GetScene();

			if ( m_initialised )
			{
				l_scene.GetLightCache().ForEach( [&l_scene, this]( Light & p_light )
				{
					if ( p_light.IsShadowProducer() )
					{
						switch ( p_light.GetLightType() )
						{
						case LightType::eDirectional:
							m_directionalShadowMap.AddLight( p_light );
							break;

						case LightType::ePoint:
							m_pointShadowMap.AddLight( p_light );
							break;

						case LightType::eSpot:
							m_spotShadowMap.AddLight( p_light );
							break;
						}
					}
				} );

				m_initialised = m_directionalShadowMap.Initialise( Size{ 4096, 4096 } );

				if ( m_initialised )
				{
					m_initialised = m_spotShadowMap.Initialise( Size{ 1024, 1024 } );
				}

				if ( m_initialised )
				{
					m_initialised = m_pointShadowMap.Initialise( Size{ 1024, 1024 } );
				}
			}

			if ( m_initialised )
			{
				m_opaquePass->Initialise( m_size );
				m_transparentPass->Initialise( m_size );
			}
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		m_transparentPass->Cleanup();
		m_opaquePass->Cleanup();
		m_pointShadowMap.Cleanup();
		m_spotShadowMap.Cleanup();
		m_directionalShadowMap.Cleanup();
		m_initialised = false;
		m_frameBuffer.Cleanup();
		DoCleanup();
	}

	void RenderTechnique::Update( RenderQueueArray & p_queues )
	{
		m_opaquePass->Update( p_queues );
		m_transparentPass->Update( p_queues );

		m_pointShadowMap.Update( *m_renderTarget.GetCamera(), p_queues );
		m_spotShadowMap.Update( *m_renderTarget.GetCamera(), p_queues );
		m_directionalShadowMap.Update( *m_renderTarget.GetCamera(), p_queues );
	}

	void RenderTechnique::Render( uint32_t & p_visible, uint32_t & p_particles )
	{
		auto & l_scene = *m_renderTarget.GetScene();
		l_scene.GetLightCache().UpdateLights();
		m_renderSystem.PushScene( &l_scene );
		bool l_shadows = l_scene.HasShadows();
		m_directionalShadowMap.Render();
		m_pointShadowMap.Render();
		m_spotShadowMap.Render();

		DoBeginRender();
		l_scene.RenderBackground( GetSize() );
		auto & l_camera = *m_renderTarget.GetCamera();
		l_camera.Resize( m_size );
		l_camera.Update();
		l_camera.Apply();

		DoBeginOpaqueRendering();
		m_opaquePass->Render( p_visible, l_shadows );
		DoEndOpaqueRendering();

		if ( l_scene.GetFog().GetType() == FogType::eDisabled )
		{
			l_scene.RenderForeground( GetSize(), l_camera );
		}

		l_scene.GetParticleSystemCache().ForEach( [this, &p_particles]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.Update();
			p_particles += p_particleSystem.GetParticlesCount();
		} );

		DoBeginTransparentRendering();
		m_transparentPass->Render( p_visible, l_shadows );
		DoEndTransparentRendering();

#if 0//!defined( NDEBUG )

		if ( !m_shadowMaps.empty() )
		{
			auto l_it = m_shadowMaps.begin();
			auto & l_depthMap = l_it->second->GetShadowMap();
			auto l_lightNode = l_it->first->GetParent();
			Size l_size{ 512u, 512u };

			switch ( l_depthMap.GetType() )
			{
			case TextureType::eTwoDimensions:
				m_renderSystem.GetCurrentContext()->RenderDepth( l_size
					, *l_depthMap.GetTexture() );
				break;

			case TextureType::eTwoDimensionsArray:
				m_renderSystem.GetCurrentContext()->RenderDepth( l_size
					, *l_depthMap.GetTexture(), 0u );
				break;

			case TextureType::eCube:
				m_renderSystem.GetCurrentContext()->RenderDepthCube( Size{ l_size.width() / 4, l_size.height() / 4 }
					, *l_depthMap.GetTexture() );
				break;

			case TextureType::eCubeArray:
				m_renderSystem.GetCurrentContext()->RenderDepthCube( Size{ l_size.width() / 4, l_size.height() / 4 }
					, *l_depthMap.GetTexture()
					, 0u );
				break;
			}
		}

#endif

		DoEndRender();

		for ( auto l_effect : m_renderTarget.GetPostEffects() )
		{
			l_effect->Apply( *m_frameBuffer.m_frameBuffer );
		}

		m_renderSystem.PopScene();
	}

	bool RenderTechnique::WriteInto( Castor::TextFile & p_file )
	{
		return DoWriteInto( p_file );
	}
}
