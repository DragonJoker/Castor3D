#include "DeferredRendering.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Scene/Skybox.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Texture/Sampler.hpp"

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr DoCreateSampler( Engine & p_engine
			, String const & p_name )
		{
			SamplerSPtr l_result;
			auto & l_cache = p_engine.GetSamplerCache();

			if ( l_cache.Has( p_name ) )
			{
				l_result = l_cache.Find( p_name );
			}
			else
			{
				l_result = p_engine.GetRenderSystem()->CreateSampler( p_name );
				l_result->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				l_result->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				l_result->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				l_result->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				l_result->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				l_result->Initialise();
				l_cache.Add( p_name, l_result );
			}

			return l_result;
		}
	}

	//*********************************************************************************************
	DeferredRendering::DeferredRendering( Engine & p_engine
		, OpaquePass & p_opaquePass
		, FrameBuffer & p_frameBuffer
		, FrameBufferAttachment & p_depthAttach
		, Size const & p_size
		, Scene const & p_scene
		, SsaoConfig const & p_config )
		: m_engine{ p_engine }
		, m_ssaoConfig{ p_config }
		, m_opaquePass{ p_opaquePass }
		, m_frameBuffer{ p_frameBuffer }
		, m_depthAttach{ p_depthAttach }
		, m_size{ p_size }
		, m_sceneUbo{ p_engine }
	{
		auto & l_renderSystem = *p_engine.GetRenderSystem();
		m_geometryPassFrameBuffer = l_renderSystem.CreateFrameBuffer();
		bool l_result = m_geometryPassFrameBuffer->Create();

		if ( l_result )
		{
			l_result = m_geometryPassFrameBuffer->Initialise( m_size );
		}

		if ( l_result )
		{
			for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
			{
				auto l_texture = l_renderSystem.CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, GetTextureFormat( DsTexture( i ) )
					, m_size );
				l_texture->GetImage().InitialiseSource();

				m_geometryPassResult[i] = std::make_unique< TextureUnit >( p_engine );
				m_geometryPassResult[i]->SetIndex( i );
				m_geometryPassResult[i]->SetTexture( l_texture );
				m_geometryPassResult[i]->SetSampler( DoCreateSampler( p_engine, GetTextureName( DsTexture( i ) ) ) );
				m_geometryPassResult[i]->Initialise();

				m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( l_texture );
			}

			m_geometryPassFrameBuffer->Bind();

			for ( int i = 0; i < size_t( DsTexture::eCount ) && l_result; i++ )
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

		if ( l_result )
		{
			m_lightingPass = std::make_unique< LightingPass >( p_engine
				, m_size
				, p_scene
				, m_opaquePass
				, m_depthAttach
				, m_sceneUbo );
			m_reflection = std::make_unique< ReflectionPass >( p_engine
				, m_size
				, m_sceneUbo );
			m_combinePass = std::make_unique< CombinePass >( p_engine
				, m_size
				, m_sceneUbo
				, m_ssaoConfig );
		}

		ENSURE( l_result );
	}

	DeferredRendering::~DeferredRendering()
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

		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
		m_combinePass.reset();
		m_reflection.reset();
		m_lightingPass.reset();
		m_sceneUbo.GetUbo().Cleanup();
	}

	void DeferredRendering::BlitDepthInto( FrameBuffer & p_fbo )
	{
		m_geometryPassFrameBuffer->BlitInto( p_fbo
			, Rectangle{ Position{}, m_size }
		, BufferComponent::eDepth | BufferComponent::eStencil );
	}

	void DeferredRendering::Render( RenderInfo & p_info
		, Scene const & p_scene
		, Camera const & p_camera )
	{
		m_engine.SetPerObjectLighting( false );
		auto l_invView = p_camera.GetView().get_inverse().get_transposed();
		auto l_invProj = p_camera.GetViewport().GetProjection().get_inverse();
		auto l_invViewProj = ( p_camera.GetViewport().GetProjection() * p_camera.GetView() ).get_inverse();
		p_camera.Apply();
		m_geometryPassFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_geometryPassTexAttachs[size_t( DsTexture::eDepth )]->Attach( AttachmentPoint::eDepth );
		m_geometryPassFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass.Render( p_info, p_scene.HasShadows() );
		m_geometryPassFrameBuffer->Unbind();

		BlitDepthInto( m_frameBuffer );

		m_sceneUbo.Update( p_scene, p_camera );
		m_lightingPass->Render( p_scene
			, p_camera
			, m_geometryPassResult
			, l_invViewProj
			, l_invView
			, l_invProj );

		m_reflection->Render( m_geometryPassResult
			, m_lightingPass->GetResult()
			, p_scene
			, p_camera
			, l_invViewProj
			, l_invView
			, l_invProj );

		if ( p_scene.GetMaterialsType() == MaterialType::ePbrMetallicRoughness
			|| p_scene.GetMaterialsType() == MaterialType::ePbrSpecularGlossiness )
		{
			m_combinePass->Render( m_geometryPassResult
				, m_lightingPass->GetResult()
				, m_reflection->GetReflection()
				, m_reflection->GetRefraction()
				, p_scene.GetSkybox().GetIbl()
				, p_camera
				, l_invViewProj
				, l_invView
				, l_invProj
				, p_scene.GetFog()
				, m_frameBuffer );
		}
		else
		{
			m_combinePass->Render( m_geometryPassResult
				, m_lightingPass->GetResult()
				, m_reflection->GetReflection()
				, m_reflection->GetRefraction()
				, p_camera
				, l_invViewProj
				, l_invView
				, l_invProj
				, p_scene.GetFog()
				, m_frameBuffer );
		}
	}

	void DeferredRendering::Debug( Camera const & p_camera )
	{
		auto l_count = 6 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		int l_width = int( m_size.width() ) / l_count;
		int l_height = int( m_size.height() ) / l_count;
		int l_left = int( m_size.width() ) - l_width;
		auto l_size = Size( l_width, l_height );
		auto & l_context = *m_engine.GetRenderSystem()->GetCurrentContext();
		p_camera.Apply();
		m_frameBuffer.Bind();
		auto l_index = 0;
		l_context.RenderDepth( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eData1 )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eData2 )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eData3 )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_geometryPassResult[size_t( DsTexture::eData4 )]->GetTexture() );
		l_context.RenderTexture( Position{ l_width * l_index++, 0 }, l_size, *m_lightingPass->GetResult().GetTexture() );

		if ( m_ssaoConfig.m_enabled )
		{
			l_context.RenderTexture( Position{ l_width * ( l_index++ ), 0 }, l_size, m_combinePass->GetSsao() );
		}

		m_frameBuffer.Unbind();
	}
}
