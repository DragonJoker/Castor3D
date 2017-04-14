#include "DeferredRenderTechnique.hpp"

#include <DirectionalLightPass.hpp>
#include <LightPassShadow.hpp>
#include <OpaquePass.hpp>

#include <FrameBuffer/ColourRenderBuffer.hpp>
#include <FrameBuffer/DepthStencilRenderBuffer.hpp>
#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/RenderBufferAttachment.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Mesh/Buffer/BufferElementGroup.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/RenderTarget.hpp>
#include <Scene/Light/DirectionalLight.hpp>
#include <Scene/Light/PointLight.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Technique/ForwardRenderTechniquePass.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#define DEBUG_DEFERRED_BUFFERS 1

using namespace Castor;
using namespace Castor3D;

namespace deferred
{
	namespace
	{
		bool DoUsesSsao( Parameters const & p_params )
		{
			bool l_ssao{ false };
			p_params.Get( cuT( "ssao" ), l_ssao );
			return l_ssao;
		}
	}

	String const RenderTechnique::Type = cuT( "deferred" );
	String const RenderTechnique::Name = cuT( "Deferred Lighting Render Technique" );

	RenderTechnique::RenderTechnique( RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, Parameters const & p_params )
		: Castor3D::RenderTechnique( RenderTechnique::Type
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< deferred_common::OpaquePass >( *p_renderTarget.GetScene()
				, p_renderTarget.GetCamera().get() )
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "deferred_transparent" )
				, *p_renderTarget.GetScene()
				, p_renderTarget.GetCamera().get()
				, false
				, false
				, false
				, nullptr )
			, p_params )
		, m_sceneUbo{ *p_renderSystem.GetEngine() }
		, m_ssaoEnabled{ DoUsesSsao( p_params ) }
		, m_lightPassResult{ *p_renderSystem.GetEngine() }
	{
		Logger::LogInfo( cuT( "Using deferred rendering" ) );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params )
	{
		// No make_shared because ctor is protected;
		return RenderTechniqueSPtr( new RenderTechnique( p_renderTarget, p_renderSystem, p_params ) );
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
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
			m_fogPass = std::make_unique< deferred_common::FogPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize() );
		}

		if ( l_return && m_ssaoEnabled )
		{
			m_ssao = std::make_unique< deferred_common::SsaoPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize() );
		}

		m_frameBuffer.m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		m_ssao.reset();
		m_fogPass.reset();
		m_reflection.reset();
		DoCleanupGeometryPass();
		DoCleanupLightPass();
	}

	void RenderTechnique::DoUpdateSceneUbo()
	{
		m_sceneUbo.Update( *m_renderTarget.GetScene(), *m_renderTarget.GetCamera(), false );
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

		if ( m_ssaoEnabled )
		{
			m_ssao->Render( m_geometryPassResult, *m_renderTarget.GetCamera(), l_invViewProj, l_invView, l_invProj );
		}

		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_depthAttach->Attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eStencil );
		m_geometryPassFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer
			, Rectangle{ Position{}, m_size }
			, BufferComponent::eDepth );
		m_frameBuffer.m_frameBuffer->Unbind();

		DoUpdateSceneUbo();

		auto & l_cache = m_renderTarget.GetScene()->GetLightCache();

		m_lightPassFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_lightPassFrameBuffer->Clear( BufferComponent::eColour );

		bool l_first{ true };

		if ( !l_cache.IsEmpty() )
		{
#if !defined( NDEBUG )

			deferred_common::g_index = 0;

#endif

			auto l_lock = make_unique_lock( l_cache );
			DoRenderLights( LightType::eDirectional, l_invViewProj, l_invView, l_invProj, l_first );
			DoRenderLights( LightType::ePoint, l_invViewProj, l_invView, l_invProj, l_first );
			DoRenderLights( LightType::eSpot, l_invViewProj, l_invView, l_invProj, l_first );
			l_first = false;
		}

		if ( !m_renderTarget.GetScene()->GetEnvironmentMaps().empty() )
		{
			m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
			m_frameBuffer.m_frameBuffer->SetDrawBuffers();
			m_reflection->Render( m_geometryPassResult
				, *m_frameBuffer.m_colourTexture
				, l_scene
				, l_camera
				, l_invViewProj
				, l_invView
				, l_invProj );
			m_frameBuffer.m_frameBuffer->Unbind();
			l_first = false;
		}

		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_frameBuffer->SetDrawBuffers();

		m_fogPass->Render( m_geometryPassResult
			, m_lightPassResult
			, l_camera
			, l_invViewProj
			, l_invView
			, l_invProj
			, l_scene.GetFog() );
	}

	void RenderTechnique::DoRenderTransparent( RenderInfo & p_info )
	{
		m_frameBuffer.m_frameBuffer->Unbind();
		GetEngine()->SetPerObjectLighting( true );
		m_transparentPass->RenderShadowMaps();
		m_renderTarget.GetCamera()->Apply();
		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_transparentPass->Render( p_info, m_renderTarget.GetScene()->HasShadows() );
		m_frameBuffer.m_frameBuffer->Unbind();

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
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_lightPassResult.GetTexture() );

		if ( m_ssaoEnabled )
		{
			l_context.RenderTexture( Position{ l_width * ( l_index++ ), 0 }, l_size, m_ssao->GetRaw() );
		}

		m_frameBuffer.m_frameBuffer->Unbind();

#endif
	}

	bool RenderTechnique::DoWriteInto( TextFile & p_file )
	{
		return true;
	}

	bool RenderTechnique::DoInitialiseGeometryPass( uint32_t & p_index )
	{
		m_geometryPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_geometryPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eTransparentBlack ) );
		bool l_return = m_geometryPassFrameBuffer->Create();

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

			m_geometryPassFrameBuffer->Bind();

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

	bool RenderTechnique::DoInitialiseLightPass()
	{
		m_lightPassFrameBuffer = m_renderSystem.CreateFrameBuffer();
		m_lightPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eTransparentBlack ) );
		bool l_return = m_lightPassFrameBuffer->Create();

		if ( l_return )
		{
			l_return = m_lightPassFrameBuffer->Initialise( m_size );
		}

		if ( l_return )
		{
			auto l_texture = m_renderSystem.CreateTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eRGBA16F32F
				, m_size );
			l_texture->GetImage().InitialiseSource();

			m_lightPassResult.SetIndex( 0u );
			m_lightPassResult.SetTexture( l_texture );
			m_lightPassResult.SetSampler( GetEngine()->GetLightsSampler() );
			m_lightPassResult.Initialise();

			m_lightPassTexAttach = m_lightPassFrameBuffer->CreateAttachment( l_texture );

			m_lightPassFrameBuffer->Bind();
			m_lightPassFrameBuffer->Attach( AttachmentPoint::eColour
				, m_lightPassTexAttach
				, m_lightPassResult.GetTexture()->GetType() );
			ENSURE( m_lightPassFrameBuffer->IsComplete() );
			m_lightPassFrameBuffer->SetDrawBuffers();
			m_lightPassFrameBuffer->Unbind();

			auto & l_opaquePass = *reinterpret_cast< deferred_common::OpaquePass * >( m_opaquePass.get() );
			auto & l_scene = *m_renderTarget.GetScene();
			m_lightPass[size_t( LightType::eDirectional )] = std::make_unique< deferred_common::DirectionalLightPass >( *m_renderTarget.GetEngine()
				, *m_lightPassFrameBuffer
				, *m_frameBuffer.m_depthAttach
				, m_ssaoEnabled
				, false );
			m_lightPass[size_t( LightType::ePoint )] = std::make_unique< deferred_common::PointLightPass >( *m_renderTarget.GetEngine()
				, *m_lightPassFrameBuffer
				, *m_frameBuffer.m_depthAttach
				, m_ssaoEnabled
				, false );
			m_lightPass[size_t( LightType::eSpot )] = std::make_unique< deferred_common::SpotLightPass >( *m_renderTarget.GetEngine()
				, *m_lightPassFrameBuffer
				, *m_frameBuffer.m_depthAttach
				, m_ssaoEnabled
				, false );
			m_lightPassShadow[size_t( LightType::eDirectional )] = std::make_unique< deferred_common::DirectionalLightPassShadow >( *m_renderTarget.GetEngine()
				, *m_lightPassFrameBuffer
				, *m_frameBuffer.m_depthAttach
				, m_ssaoEnabled
				, l_opaquePass.GetDirectionalShadowMap() );
			m_lightPassShadow[size_t( LightType::ePoint )] = std::make_unique< deferred_common::PointLightPassShadow >( *m_renderTarget.GetEngine()
				, *m_lightPassFrameBuffer
				, *m_frameBuffer.m_depthAttach
				, m_ssaoEnabled
				, l_opaquePass.GetPointShadowMaps() );
			m_lightPassShadow[size_t( LightType::eSpot )] = std::make_unique< deferred_common::SpotLightPassShadow >( *m_renderTarget.GetEngine()
				, *m_lightPassFrameBuffer
				, *m_frameBuffer.m_depthAttach
				, m_ssaoEnabled
				, l_opaquePass.GetSpotShadowMap() );
			m_ambientPass = std::make_unique< deferred_common::AmbientLightPass >( *m_renderTarget.GetEngine()
				, *m_lightPassFrameBuffer
				, *m_frameBuffer.m_depthAttach
				, m_ssaoEnabled );

			for ( auto & l_lightPass : m_lightPass )
			{
				l_lightPass->Initialise( l_scene, m_sceneUbo );
			}

			for ( auto & l_lightPass : m_lightPassShadow )
			{
				l_lightPass->Initialise( l_scene, m_sceneUbo );
			}

			m_ambientPass->Initialise( l_scene, m_sceneUbo );
		}

		return true;
	}

	void RenderTechnique::DoCleanupGeometryPass()
	{
		m_geometryPassFrameBuffer->Bind();
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();
		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
	}

	void RenderTechnique::DoCleanupLightPass()
	{
		m_ambientPass->Cleanup();
		m_ambientPass.reset();

		for ( auto & l_unit : m_geometryPassResult )
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
			auto & l_camera = *m_renderTarget.GetCamera();
			auto & l_lightPass = *m_lightPass[size_t( p_type )];
			auto & l_lightPassShadow = *m_lightPassShadow[size_t( p_type )];

			for ( auto & l_light : l_cache.GetLights( p_type ) )
			{
				if ( l_light->IsShadowProducer() )
				{
					l_lightPassShadow.Render( m_size
						, m_geometryPassResult
						, *l_light
						, l_camera
						, p_invViewProj
						, p_invView
						, p_invProj
						, m_ssaoEnabled ? &m_ssao->GetResult() : nullptr
						, p_first );
				}
				else
				{
					l_lightPass.Render( m_size
						, m_geometryPassResult
						, *l_light
						, l_camera
						, p_invViewProj
						, p_invView
						, p_invProj
						, m_ssaoEnabled ? &m_ssao->GetResult() : nullptr
						, p_first );
				}

				p_first = false;
			}
		}
	}
}
