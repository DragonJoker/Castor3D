#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( disable:4503 )
#endif

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
#include "Shader/PassBuffer.hpp"
#include "ShadowMap/ShadowMapPass.hpp"
#include "Technique/RenderTechniquePass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Technique/ForwardRenderTechniquePass.hpp"
#include "Technique/Deferred/OpaquePass.hpp"

#include <GlslSource.hpp>
#include <GlslMaterial.hpp>

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	namespace
	{
		uint8_t & GetSamplesCountParam( Parameters const & p_params, uint8_t & p_result )
		{
			String l_count;
			uint8_t l_result = 0;

			if ( p_params.Get( cuT( "samples_count" ), l_result ) )
			{
				p_result = uint8_t( l_result );
			}

			if ( p_result <= 1 )
			{
				p_result = 0;
			}

			return p_result;
		}
	}

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
		, m_opaquePass{ std::make_unique< OpaquePass >( *p_renderTarget.GetScene()
			, p_renderTarget.GetCamera().get()
			, p_config ) }
		, m_transparentPass{ std::make_unique< ForwardRenderTechniquePass >( cuT( "technique_transparent_pass" )
			, *p_renderTarget.GetScene()
			, p_renderTarget.GetCamera().get()
			, false
			, GetSamplesCountParam( p_params, m_msaa.m_samplesCount ) > 1
			, false
			, nullptr
			, p_config ) }
		, m_initialised{ false }
		, m_frameBuffer{ *this }
		, m_ssaoConfig{ p_config }
		, m_sceneUbo{ *p_renderSystem.GetEngine() }
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
				m_initialised = DoInitialiseMsaa();
			}

			if ( m_initialised )
			{
				m_initialised = DoInitialiseDeferred( p_index );
			}

			if ( m_msaa.m_samplesCount > 1 )
			{
				m_currentFrameBuffer = m_msaa.m_frameBuffer.get();
				m_currentDepthAttach = m_msaa.m_depthAttach.get();
			}
			else
			{
				m_currentFrameBuffer = m_frameBuffer.m_frameBuffer.get();
				m_currentDepthAttach = m_frameBuffer.m_depthAttach.get();
			}

			ENSURE( m_initialised );
		}

		return m_initialised;
	}

	void RenderTechnique::Cleanup()
	{
		DoCleanupDeferred();
		DoCleanupMsaa();
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

		DoRenderOpaque( p_info );
		l_scene.RenderBackground( GetSize(), l_camera );

		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
		l_scene.GetParticleSystemCache().ForEach( [this, &p_info]( ParticleSystem & p_particleSystem )
		{
			p_particleSystem.Update();
			p_info.m_particlesCount += p_particleSystem.GetParticlesCount();
		} );

		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
		DoRenderTransparent( p_info );

		GetEngine()->GetMaterialCache().GetPassBuffer().Bind();
		for ( auto l_effect : m_renderTarget.GetPostEffects() )
		{
			l_effect->Apply( *m_frameBuffer.m_frameBuffer );
		}

		m_renderSystem.PopScene();
	}

	bool RenderTechnique::WriteInto( Castor::TextFile & p_file )
	{
		return p_file.WriteText( cuT( "samples_count " ) + string::to_string( int( m_msaa.m_samplesCount ) ) ) > 0;
	}

	void RenderTechnique::DoRenderOpaque( RenderInfo & p_info )
	{
		GetEngine()->SetPerObjectLighting( false );
		auto & l_scene = *m_renderTarget.GetScene();
		auto & l_camera = *m_renderTarget.GetCamera();
		auto l_invView = l_camera.GetView().get_inverse().get_transposed();
		auto l_invProj = l_camera.GetViewport().GetProjection().get_inverse();
		auto l_invViewProj = ( l_camera.GetViewport().GetProjection() * l_camera.GetView() ).get_inverse();
		l_camera.Apply();
		m_geometryPassFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_geometryPassTexAttachs[size_t( DsTexture::eDepth )]->Attach( AttachmentPoint::eDepth );
		m_geometryPassFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_geometryPassFrameBuffer->Unbind();

		m_currentFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_currentDepthAttach->Attach( AttachmentPoint::eDepthStencil );
		m_currentFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eStencil );
		m_currentFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->BlitInto( *m_currentFrameBuffer
			, Rectangle{ Position{}, GetSize() }
			, BufferComponent::eDepth | BufferComponent::eStencil );

		m_sceneUbo.Update( *m_renderTarget.GetScene(), *m_renderTarget.GetCamera() );
		m_lightingPass->Render( l_scene
			, l_camera
			, m_geometryPassResult
			, l_invViewProj
			, l_invView
			, l_invProj );
		TextureUnit const * l_result = &m_lightingPass->GetResult();

		if ( !m_renderTarget.GetScene()->GetEnvironmentMaps().empty() )
		{
			m_reflection->Render( m_geometryPassResult
				, *m_lightingPass->GetResult().GetTexture()
				, l_scene
				, l_camera
				, l_invViewProj
				, l_invView
				, l_invProj );
			l_result = &m_reflection->GetResult();
		}

		m_combinePass->Render( m_geometryPassResult
			, *l_result
			, l_camera
			, l_invViewProj
			, l_invView
			, l_invProj
			, l_scene.GetFog()
			, *m_currentFrameBuffer );
	}

	void RenderTechnique::DoRenderTransparent( RenderInfo & p_info )
	{
		m_currentFrameBuffer->Unbind();
		GetEngine()->SetPerObjectLighting( true );
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_currentFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_currentFrameBuffer->Unbind();

		if ( IsMultisampling() )
		{
			m_msaa.m_frameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer, m_msaa.m_rect, BufferComponent::eColour | BufferComponent::eDepth );
		}

#if DEBUG_DEFERRED_BUFFERS && !defined( NDEBUG )

		auto l_count = 6 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		int l_width = int( m_size.width() ) / l_count;
		int l_height = int( m_size.height() ) / l_count;
		int l_left = int( m_size.width() ) - l_width;
		auto l_size = Size( l_width, l_height );
		auto & l_context = *m_renderSystem.GetCurrentContext();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind();
		auto l_index = 0;
		l_context.RenderDepth( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eNormal )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eDiffuse )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eSpecular )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eEmissive )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_lightingPass->GetResult().GetTexture() );

		if ( m_ssaoConfig.m_enabled )
		{
			l_context.RenderTexture( Position{ l_width * ( l_index++ ), 0 }, l_size, m_combinePass->GetSsao() );
		}

		m_frameBuffer.m_frameBuffer->Unbind();

#endif
	}

	bool RenderTechnique::DoInitialiseDeferred( uint32_t & p_index )
	{
		bool l_return = DoInitialiseGeometryPass( p_index );

		if ( l_return )
		{
			m_lightingPass = std::make_unique< LightingPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize()
				, *m_renderTarget.GetScene()
				, static_cast< OpaquePass & >( *m_opaquePass )
				, *m_frameBuffer.m_depthAttach
				, m_sceneUbo );
			m_reflection = std::make_unique< ReflectionPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize() );
			m_combinePass = std::make_unique< CombinePass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize()
				, m_ssaoConfig );
		}

		return l_return;
	}

	bool RenderTechnique::DoInitialiseMsaa()
	{
		auto l_result = true;

		if ( IsMultisampling() )
		{
			m_msaa.m_rect = Castor::Rectangle( Position(), m_size );
			m_msaa.m_frameBuffer = m_renderSystem.CreateFrameBuffer();
			m_msaa.m_colorBuffer = m_msaa.m_frameBuffer->CreateColourRenderBuffer( PixelFormat::eRGBA16F32F );
			m_msaa.m_depthBuffer = m_msaa.m_frameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD24S8 );
			m_msaa.m_colorAttach = m_msaa.m_frameBuffer->CreateAttachment( m_msaa.m_colorBuffer );
			m_msaa.m_depthAttach = m_msaa.m_frameBuffer->CreateAttachment( m_msaa.m_depthBuffer );

			l_result = m_msaa.m_frameBuffer->Create();
			m_msaa.m_colorBuffer->SetSamplesCount( m_msaa.m_samplesCount );
			m_msaa.m_depthBuffer->SetSamplesCount( m_msaa.m_samplesCount );

			if ( l_result )
			{
				l_result = m_msaa.m_colorBuffer->Create();
			}

			if ( l_result )
			{
				l_result = m_msaa.m_depthBuffer->Create();
			}

			if ( l_result )
			{
				l_result = m_msaa.m_colorBuffer->Initialise( m_size );
			}

			if ( l_result )
			{
				l_result = m_msaa.m_depthBuffer->Initialise( m_size );
			}

			if ( l_result )
			{
				l_result = m_msaa.m_frameBuffer->Initialise( m_size );
			}

			if ( l_result )
			{
				m_msaa.m_frameBuffer->Bind();
				m_msaa.m_frameBuffer->Attach( AttachmentPoint::eColour, m_msaa.m_colorAttach );
				m_msaa.m_frameBuffer->Attach( AttachmentPoint::eDepth, m_msaa.m_depthAttach );
				m_msaa.m_frameBuffer->SetDrawBuffer( m_msaa.m_colorAttach );
				REQUIRE( m_msaa.m_frameBuffer->IsComplete() );
				m_msaa.m_frameBuffer->Unbind();
			}
		}

		return l_result;
	}

	void RenderTechnique::DoCleanupDeferred()
	{
		DoCleanupGeometryPass();
		m_combinePass.reset();
		m_reflection.reset();
		m_lightingPass.reset();
		m_sceneUbo.GetUbo().Cleanup();
	}

	void RenderTechnique::DoCleanupMsaa()
	{
		if ( IsMultisampling() )
		{
			m_msaa.m_frameBuffer->Bind();
			m_msaa.m_frameBuffer->DetachAll();
			m_msaa.m_frameBuffer->Unbind();
			m_msaa.m_frameBuffer->Cleanup();
			m_msaa.m_colorBuffer->Cleanup();
			m_msaa.m_depthBuffer->Cleanup();
			m_msaa.m_colorBuffer->Destroy();
			m_msaa.m_depthBuffer->Destroy();
			m_msaa.m_frameBuffer->Destroy();
			m_msaa.m_depthAttach.reset();
			m_msaa.m_colorAttach.reset();
			m_msaa.m_colorBuffer.reset();
			m_msaa.m_depthBuffer.reset();
			m_msaa.m_frameBuffer.reset();
		}
	}

	bool RenderTechnique::DoInitialiseGeometryPass( uint32_t & p_index )
	{
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_geometryPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		bool l_return = m_geometryPassFrameBuffer->Create();

		if ( IsMultisampling() )
		{
			m_msaa.m_lightPassDepthBuffer = m_geometryPassFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD24S8 );
			m_msaa.m_geometryPassDepthAttach = m_geometryPassFrameBuffer->CreateAttachment( m_msaa.m_lightPassDepthBuffer );

			if ( l_return )
			{
				l_return = m_msaa.m_lightPassDepthBuffer->Create();
			}

			if ( l_return )
			{
				m_msaa.m_lightPassDepthBuffer->Initialise( m_size );
				m_geometryPassFrameBuffer->Attach( AttachmentPoint::eDepthStencil, m_msaa.m_geometryPassDepthAttach );
			}
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
			{
				auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, GetTextureFormat( DsTexture( i ) )
					, m_size );
				l_texture->GetImage().InitialiseSource();

				m_geometryPassResult[i] = std::make_unique< TextureUnit >( *GetEngine() );
				m_geometryPassResult[i]->SetIndex( i );
				m_geometryPassResult[i]->SetTexture( l_texture );
				m_geometryPassResult[i]->SetSampler( GetEngine()->GetLightsSampler() );
				m_geometryPassResult[i]->Initialise();

				m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( l_texture );
				p_index++;
			}

			m_geometryPassFrameBuffer->Bind();

			for ( int i = 0; i < size_t( DsTexture::eCount ) && l_return; i++ )
			{
				m_geometryPassFrameBuffer->Attach( GetTextureAttachmentPoint( DsTexture( i ) )
					, GetTextureAttachmentIndex( DsTexture( i ) )
					, m_geometryPassTexAttachs[i]
					, m_geometryPassResult[i]->GetType() );
			}

			ENSURE( m_geometryPassFrameBuffer->IsComplete() );
			m_geometryPassFrameBuffer->SetDrawBuffers();
			m_geometryPassFrameBuffer->Unbind();
		}

		return l_return;
	}

	void RenderTechnique::DoCleanupGeometryPass()
	{
		m_geometryPassFrameBuffer->Bind();
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();

		for ( auto & l_attach : m_geometryPassTexAttachs )
		{
			l_attach.reset();
		}

		for ( auto & l_texture : m_geometryPassResult )
		{
			l_texture->Cleanup();
			l_texture.reset();
		}

		if ( IsMultisampling() )
		{
			m_msaa.m_lightPassDepthBuffer->Destroy();
			m_msaa.m_lightPassDepthBuffer.reset();
		}

		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
	}
}
