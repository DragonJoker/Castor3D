#include "DeferredMsaaRenderTechnique.hpp"

#include <DirectionalLightPass.hpp>
#include <LightPassShadow.hpp>
#include <PointLightPass.hpp>
#include <SpotLightPass.hpp>

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

#define DEBUG_DEFERRED_BUFFERS 0

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
		m_geometryPassFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_geometryPassFrameBuffer->Unbind();

		if ( m_ssaoEnabled )
		{
			m_ssao->Render( m_lightPassTextures, *m_renderTarget.GetCamera(), l_invViewProj, l_invView, l_invProj );
		}

		m_geometryPassFrameBuffer->BlitInto( *m_msaaFrameBuffer
			, Rectangle{ Position{}, GetSize() }
			, BufferComponent::eDepth | BufferComponent::eStencil );

		m_msaaFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_msaaDepthAttach->Attach( AttachmentPoint::eDepthStencil );
		m_msaaFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eStencil );
		m_msaaFrameBuffer->Unbind();

		DoUpdateSceneUbo();

		auto & l_cache = m_renderTarget.GetScene()->GetLightCache();

		if ( !l_cache.IsEmpty() )
		{
#if !defined( NDEBUG )

			deferred_common::g_index = 0;

#endif

			auto l_lock = make_unique_lock( l_cache );
			bool l_first{ true };
			auto l_invViewProj = ( l_camera.GetViewport().GetProjection() * l_camera.GetView() ).get_inverse();
			auto l_invProj = l_camera.GetViewport().GetProjection().get_inverse();
			DoRenderLights( LightType::eDirectional, l_invViewProj, l_invView, l_invProj, l_first );
			DoRenderLights( LightType::ePoint, l_invViewProj, l_invView, l_invProj, l_first );
			DoRenderLights( LightType::eSpot, l_invViewProj, l_invView, l_invProj, l_first );
		}

		m_msaaFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_msaaFrameBuffer->SetDrawBuffers();
		m_reflection->Render( m_lightPassTextures
			, *m_frameBuffer.m_colourTexture
			, l_scene
			, l_camera
			, l_invViewProj
			, l_invView
			, l_invProj );
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

		auto l_count = 5 + ( m_ssaoEnabled ? 1 : 0 );
		int l_width = int( m_size.width() ) / l_count;
		int l_height = int( m_size.height() ) / l_count;
		int l_left = int( m_size.width() ) - l_width;
		auto l_size = Size( l_width, l_height );
		auto & l_context = *m_renderSystem.GetCurrentContext();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind();
		l_context.RenderDepth( Position{ l_width * 0, 0 }, l_size, *m_lightPassTextures[size_t( deferred_common::DsTexture::eDepth )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * 1, 0 }, l_size, *m_lightPassTextures[size_t( deferred_common::DsTexture::eNormal )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * 2, 0 }, l_size, *m_lightPassTextures[size_t( deferred_common::DsTexture::eDiffuse )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * 3, 0 }, l_size, *m_lightPassTextures[size_t( deferred_common::DsTexture::eSpecular )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * 4, 0 }, l_size, *m_lightPassTextures[size_t( deferred_common::DsTexture::eEmissive )]->GetTexture() );

		if ( m_ssaoEnabled )
		{
			l_context.RenderTexture( Position{ l_width * 5, 0 }, l_size, m_ssao->GetRaw() );
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
			l_return = DoInitialiseLightPass();
		}

		if ( l_return )
		{
			m_reflection = std::make_unique< deferred_common::ReflectionPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize() );
		}

		if ( l_return && m_ssaoEnabled )
		{
			m_ssao = std::make_unique< deferred_common::SsaoPass >( *m_renderSystem.GetEngine()
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
		m_ssao.reset();
		m_reflection.reset();
		DoCleanupGeometryPass();
		DoCleanupLightPass();
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

				m_lightPassTextures[i] = std::make_unique< TextureUnit >( *GetEngine() );
				m_lightPassTextures[i]->SetIndex( i );
				m_lightPassTextures[i]->SetTexture( l_texture );
				m_lightPassTextures[i]->SetSampler( GetEngine()->GetLightsSampler() );
				m_lightPassTextures[i]->Initialise();

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
					, m_lightPassTextures[i]->GetType() );
			}

			ENSURE( m_geometryPassFrameBuffer->IsComplete() );
			m_geometryPassFrameBuffer->SetDrawBuffers();
			m_geometryPassFrameBuffer->Unbind();
		}

		return l_return;
	}

	bool RenderTechnique::DoInitialiseLightPass()
	{
		auto & l_opaquePass = *reinterpret_cast< deferred_common::OpaquePass * >( m_opaquePass.get() );
		auto & l_scene = *m_renderTarget.GetScene();
		m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< deferred_common::DirectionalLightPass >( *m_renderTarget.GetEngine()
			, *m_msaaFrameBuffer
			, *m_msaaDepthAttach
			, m_ssaoEnabled
			, false );
		m_lightPass[size_t( LightType::ePoint )] = std::make_unique< deferred_common::PointLightPass >( *m_renderTarget.GetEngine()
			, *m_msaaFrameBuffer
			, *m_msaaDepthAttach
			, m_ssaoEnabled
			, false );
		m_lightPass[size_t( LightType::eSpot )] = std::make_unique< deferred_common::SpotLightPass >( *m_renderTarget.GetEngine()
			, *m_msaaFrameBuffer
			, *m_msaaDepthAttach
			, m_ssaoEnabled
			, false );
		m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< deferred_common::DirectionalLightPassShadow >( *m_renderTarget.GetEngine()
			, *m_msaaFrameBuffer
			, *m_msaaDepthAttach
			, m_ssaoEnabled
			, l_opaquePass.GetDirectionalShadowMap() );
		m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< deferred_common::PointLightPassShadow >( *m_renderTarget.GetEngine()
			, *m_msaaFrameBuffer
			, *m_msaaDepthAttach
			, m_ssaoEnabled
			, l_opaquePass.GetPointShadowMaps() );
		m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< deferred_common::SpotLightPassShadow >( *m_renderTarget.GetEngine()
			, *m_msaaFrameBuffer
			, *m_msaaDepthAttach
			, m_ssaoEnabled
			, l_opaquePass.GetSpotShadowMap() );

		for ( auto & l_lightPass : m_lightPass )
		{
			l_lightPass->Initialise( l_scene, m_sceneUbo );
		}

		for ( auto & l_lightPass : m_lightPassShadow )
		{
			l_lightPass->Initialise( l_scene, m_sceneUbo );
		}

		return true;
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

	void RenderTechnique::DoCleanupLightPass()
	{
		for ( auto & l_unit : m_lightPassTextures )
		{
			l_unit->Cleanup();
			l_unit.reset();
		}

		for ( auto & l_lightPass : m_lightPass )
		{
			l_lightPass->Cleanup();
			l_lightPass.reset();
		}

		for ( auto & l_lightPass : m_lightPassShadow )
		{
			l_lightPass->Cleanup();
			l_lightPass.reset();
		}

		m_sceneUbo.GetUbo().Cleanup();
	}

	void RenderTechnique::DoRenderLights( LightType p_type
		, Matrix4x4r const & p_invViewProj
		, Matrix4x4r const & p_invView
		, Matrix4x4r const & p_invProj
		, bool & p_first )
	{
		auto & l_scene = *m_renderTarget.GetScene();
		auto & l_cache = l_scene.GetLightCache();

		if ( l_cache.GetLightsCount( p_type ) )
		{
			auto l_fogType = l_scene.GetFog().GetType();
			auto & l_camera = *m_renderTarget.GetCamera();
			auto & l_lightPass = *m_lightPass[size_t( p_type )];
			auto & l_lightPassShadow = *m_lightPassShadow[size_t( p_type )];

			for ( auto & l_light : l_cache.GetLights( p_type ) )
			{
				if ( l_light->IsShadowProducer() )
				{
					l_lightPassShadow.Render( m_size
						, m_lightPassTextures
						, *l_light
						, l_camera
						, p_invViewProj
						, p_invView
						, p_invProj
						, l_fogType
						, m_ssaoEnabled ? &m_ssao->GetResult() : nullptr
						, p_first );
				}
				else
				{
					l_lightPass.Render( m_size
						, m_lightPassTextures
						, *l_light
						, l_camera
						, p_invViewProj
						, p_invView
						, p_invProj
						, l_fogType
						, m_ssaoEnabled ? &m_ssao->GetResult() : nullptr
						, p_first );
				}

				p_first = false;
			}
		}
	}
}
