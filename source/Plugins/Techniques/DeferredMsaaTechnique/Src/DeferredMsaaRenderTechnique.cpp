#include "DeferredMsaaRenderTechnique.hpp"

#include <OpaquePass.hpp>

#include <Engine.hpp>
#include <Cache/CameraCache.hpp>
#include <Cache/LightCache.hpp>
#include <Cache/SceneCache.hpp>
#include <Cache/SceneNodeCache.hpp>
#include <Cache/ShaderCache.hpp>
#include <Cache/TargetCache.hpp>

#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferDeclaration.hpp>
#include <Mesh/Buffer/BufferElementDeclaration.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/Context.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/Viewport.hpp>
#include <Scene/Camera.hpp>
#include <Scene/Scene.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/UniformBuffer.hpp>
#include <Shader/ShaderProgram.hpp>
#include <State/BlendState.hpp>
#include <State/DepthStencilState.hpp>
#include <State/MultisampleState.hpp>
#include <State/RasteriserState.hpp>
#include <Technique/ForwardRenderTechniquePass.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

#include <Log/Logger.hpp>

#include <GlslSource.hpp>
#include <GlslLight.hpp>
#include <GlslShadow.hpp>

#define DEBUG_DEFERRED_BUFFERS 1

using namespace Castor;
using namespace Castor3D;

namespace deferred_msaa
{
	namespace
	{
		int & GetSamplesCountParam( Parameters const & p_params, int & p_count )
		{
			String l_count;

			if ( p_params.Get( cuT( "samples_count" ), l_count ) )
			{
				p_count = string::to_int( l_count );
			}

			if ( p_count <= 1 )
			{
				p_count = 0;
			}

			return p_count;
		}

		bool DoUsesSsao( Parameters const & p_params )
		{
			bool l_ssao{ false };
			p_params.Get( cuT( "ssao" ), l_ssao );
			return l_ssao;
		}

		static constexpr uint32_t VertexCount = 6u;
	}

	String const RenderTechnique::Type = cuT( "deferred_msaa" );
	String const RenderTechnique::Name = cuT( "Deferred Lighting MSAA Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, Parameters const & p_params )
		: Castor3D::RenderTechnique( cuT( "deferred_msaa" )
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< deferred_common::OpaquePass >( *p_renderTarget.GetScene()
				, p_renderTarget.GetCamera().get() )
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "deferred_msaa_transparent" )
				, *p_renderTarget.GetScene()
				, p_renderTarget.GetCamera().get()
				, false
				, GetSamplesCountParam( p_params, m_samplesCount ) > 1
				, false
				, nullptr )
			, p_params
			, GetSamplesCountParam( p_params, m_samplesCount ) > 1 )
		, m_viewport{ *p_renderSystem.GetEngine() }
		, m_sceneUbo{ *p_renderSystem.GetEngine() }
		, m_ssaoEnabled{ DoUsesSsao( p_params ) }
	{
		Logger::LogInfo( StringStream() << cuT( "Using Deferred MSAA, " ) << m_samplesCount << cuT( " samples" ) );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return std::make_shared< RenderTechnique >( p_renderTarget, p_renderSystem, p_params );
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		return DoInitialiseMsaa()
			&& DoInitialiseDeferred( p_index );
	}

	void RenderTechnique::DoCleanup()
	{
		DoCleanupDeferred();
		DoCleanupMsaa();
	}

	void RenderTechnique::DoUpdateSceneUbo()
	{
		m_sceneUbo.Update( *m_renderTarget.GetScene(), *m_renderTarget.GetCamera() );
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
		m_geometryPassTexAttachs[size_t( deferred_common::DsTexture::eDepth )]->Attach( AttachmentPoint::eDepth );
		m_geometryPassFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_geometryPassFrameBuffer->Unbind();

		m_msaaFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_msaaDepthAttach->Attach( AttachmentPoint::eDepthStencil );
		m_msaaFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eStencil );
		m_msaaFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->BlitInto( *m_msaaFrameBuffer
			, Rectangle{ Position{}, GetSize() }
			, BufferComponent::eDepth | BufferComponent::eStencil );

		DoUpdateSceneUbo();
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

		m_msaaFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_msaaFrameBuffer->SetDrawBuffers();

		m_fogPass->Render( m_geometryPassResult
			, *l_result
			, l_camera
			, l_invViewProj
			, l_invView
			, l_invProj
			, l_scene.GetFog() );
	}

	void RenderTechnique::DoRenderTransparent( RenderInfo & p_info )
	{
		m_msaaFrameBuffer->Unbind();
		GetEngine()->SetPerObjectLighting( true );
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_msaaFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_msaaFrameBuffer->Unbind();
		m_msaaFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer, m_rect, BufferComponent::eColour | BufferComponent::eDepth );

#if DEBUG_DEFERRED_BUFFERS && !defined( NDEBUG )

		auto l_count = 6 + ( m_ssaoEnabled ? 1 : 0 );
		int l_width = int( m_size.width() ) / l_count;
		int l_height = int( m_size.height() ) / l_count;
		int l_left = int( m_size.width() ) - l_width;
		auto l_size = Size( l_width, l_height );
		auto & l_context = *m_renderSystem.GetCurrentContext();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind();
		auto l_index = 0;
		l_context.RenderDepth( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( deferred_common::DsTexture::eDepth )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( deferred_common::DsTexture::eNormal )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( deferred_common::DsTexture::eDiffuse )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( deferred_common::DsTexture::eSpecular )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( deferred_common::DsTexture::eEmissive )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_lightingPass->GetResult().GetTexture() );

		if ( m_ssaoEnabled )
		{
			l_context.RenderTexture( Position{ l_width * ( l_index++ ), 0 }, l_size, m_lightingPass->GetSsao() );
		}

		m_frameBuffer.m_frameBuffer->Unbind();

#endif
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return p_file.WriteText( cuT( " -samples_count=" ) + string::to_string( m_samplesCount ) ) > 0;
	}

	bool RenderTechnique::DoInitialiseDeferred( uint32_t & p_index )
	{
		bool l_return = DoInitialiseGeometryPass( p_index );

		if ( l_return )
		{
			m_lightingPass = std::make_unique< deferred_common::LightingPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize()
				, *m_renderTarget.GetScene()
				, static_cast< deferred_common::OpaquePass & >( *m_opaquePass )
				, m_ssaoEnabled
				, *m_frameBuffer.m_depthAttach
				, m_sceneUbo );
			m_reflection = std::make_unique< deferred_common::ReflectionPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize() );
			m_fogPass = std::make_unique< deferred_common::FogPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize() );
		}

		return l_return;
	}

	bool RenderTechnique::DoInitialiseMsaa()
	{
		m_rect = Castor::Rectangle( Position(), m_size );
		m_msaaFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_msaaColorBuffer = m_msaaFrameBuffer->CreateColourRenderBuffer( PixelFormat::eRGBA16F32F );
		m_msaaDepthBuffer = m_msaaFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD24S8 );
		m_msaaColorAttach = m_msaaFrameBuffer->CreateAttachment( m_msaaColorBuffer );
		m_msaaDepthAttach = m_msaaFrameBuffer->CreateAttachment( m_msaaDepthBuffer );

		bool l_result = m_msaaFrameBuffer->Create();
		m_msaaColorBuffer->SetSamplesCount( m_samplesCount );
		m_msaaDepthBuffer->SetSamplesCount( m_samplesCount );

		if ( l_result )
		{
			l_result = m_msaaColorBuffer->Create();
		}

		if ( l_result )
		{
			l_result = m_msaaDepthBuffer->Create();
		}

		if ( l_result )
		{
			l_result = m_msaaColorBuffer->Initialise( m_size );
		}

		if ( l_result )
		{
			l_result = m_msaaDepthBuffer->Initialise( m_size );
		}

		if ( l_result )
		{
			l_result = m_msaaFrameBuffer->Initialise( m_size );
		}

		if ( l_result )
		{
			m_msaaFrameBuffer->Bind();
			m_msaaFrameBuffer->Attach( AttachmentPoint::eColour, m_msaaColorAttach );
			m_msaaFrameBuffer->Attach( AttachmentPoint::eDepth, m_msaaDepthAttach );
			m_msaaFrameBuffer->SetDrawBuffer( m_msaaColorAttach );
			REQUIRE( m_msaaFrameBuffer->IsComplete() );
			m_msaaFrameBuffer->Unbind();
		}

		return l_result;
	}

	void RenderTechnique::DoCleanupDeferred()
	{
		DoCleanupGeometryPass();
		m_fogPass.reset();
		m_reflection.reset();
		m_lightingPass.reset();
		m_sceneUbo.GetUbo().Cleanup();
	}

	void RenderTechnique::DoCleanupMsaa()
	{
		m_msaaFrameBuffer->Bind();
		m_msaaFrameBuffer->DetachAll();
		m_msaaFrameBuffer->Unbind();
		m_msaaFrameBuffer->Cleanup();
		m_msaaColorBuffer->Cleanup();
		m_msaaDepthBuffer->Cleanup();
		m_msaaColorBuffer->Destroy();
		m_msaaDepthBuffer->Destroy();
		m_msaaFrameBuffer->Destroy();
		m_msaaDepthAttach.reset();
		m_msaaColorAttach.reset();
		m_msaaColorBuffer.reset();
		m_msaaDepthBuffer.reset();
		m_msaaFrameBuffer.reset();
	}

	bool RenderTechnique::DoInitialiseGeometryPass( uint32_t & p_index )
	{
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_geometryPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		m_lightPassDepthBuffer = m_geometryPassFrameBuffer->CreateDepthStencilRenderBuffer( PixelFormat::eD24S8 );
		m_geometryPassDepthAttach = m_geometryPassFrameBuffer->CreateAttachment( m_lightPassDepthBuffer );
		bool l_return = m_geometryPassFrameBuffer->Create();

		if ( l_return )
		{
			l_return = m_lightPassDepthBuffer->Create();
		}

		if ( l_return )
		{
			l_return = m_geometryPassFrameBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			for ( uint32_t i = 0; i < uint32_t( deferred_common::DsTexture::eCount ); i++ )
			{
				auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, GetTextureFormat( deferred_common::DsTexture( i ) )
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

			m_lightPassDepthBuffer->Initialise( m_size );
			m_geometryPassFrameBuffer->Bind();
			m_geometryPassFrameBuffer->Attach( AttachmentPoint::eDepthStencil, m_geometryPassDepthAttach );

			for ( int i = 0; i < size_t( deferred_common::DsTexture::eCount ) && l_return; i++ )
			{
				m_geometryPassFrameBuffer->Attach( GetTextureAttachmentPoint( deferred_common::DsTexture( i ) )
					, GetTextureAttachmentIndex( deferred_common::DsTexture( i ) )
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
		m_lightPassDepthBuffer->Destroy();
		m_lightPassDepthBuffer.reset();
		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
	}
}
