#include "DeferredRendering.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Scene/Skybox.hpp"
#include "Technique/Opaque/OpaquePass.hpp"
#include "Texture/Sampler.hpp"

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	namespace
	{
		SamplerSPtr DoCreateSampler( Engine & engine
			, String const & name )
		{
			SamplerSPtr result;
			auto & cache = engine.GetSamplerCache();

			if ( cache.Has( name ) )
			{
				result = cache.Find( name );
			}
			else
			{
				result = engine.GetRenderSystem()->CreateSampler( name );
				result->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eNearest );
				result->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eNearest );
				result->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
				result->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
				result->SetWrappingMode( TextureUVW::eW, WrapMode::eClampToEdge );
				result->Initialise();
				cache.Add( name, result );
			}

			return result;
		}
	}

	//*********************************************************************************************
	DeferredRendering::DeferredRendering( Engine & engine
		, OpaquePass & opaquePass
		, FrameBuffer & frameBuffer
		, FrameBufferAttachment & depthAttach
		, Size const & size
		, Scene const & scene
		, SsaoConfig const & config )
		: m_engine{ engine }
		, m_ssaoConfig{ config }
		, m_opaquePass{ opaquePass }
		, m_frameBuffer{ frameBuffer }
		, m_depthAttach{ depthAttach }
		, m_size{ size }
		, m_sceneUbo{ engine }
		, m_gpInfoUbo{ engine }
	{
		auto & renderSystem = *engine.GetRenderSystem();
		m_geometryPassFrameBuffer = renderSystem.CreateFrameBuffer();
		bool result = m_geometryPassFrameBuffer->Create();

		if ( result )
		{
			result = m_geometryPassFrameBuffer->Initialise( m_size );
		}

		if ( result )
		{
			for ( uint32_t i = 0; i < uint32_t( DsTexture::eCount ); i++ )
			{
				auto texture = renderSystem.CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, GetTextureFormat( DsTexture( i ) )
					, m_size );
				texture->GetImage().InitialiseSource();

				m_geometryPassResult[i] = std::make_unique< TextureUnit >( engine );
				m_geometryPassResult[i]->SetIndex( i );
				m_geometryPassResult[i]->SetTexture( texture );
				m_geometryPassResult[i]->SetSampler( DoCreateSampler( engine, GetTextureName( DsTexture( i ) ) ) );
				m_geometryPassResult[i]->Initialise();

				m_geometryPassTexAttachs[i] = m_geometryPassFrameBuffer->CreateAttachment( texture );
			}

			m_geometryPassFrameBuffer->Bind();

			for ( int i = 0; i < size_t( DsTexture::eCount ) && result; i++ )
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

		if ( result )
		{
			m_lightingPass = std::make_unique< LightingPass >( engine
				, m_size
				, scene
				, m_opaquePass
				, m_depthAttach
				, m_sceneUbo
				, m_gpInfoUbo );
			m_reflection = std::make_unique< ReflectionPass >( engine
				, m_size
				, m_sceneUbo
				, m_gpInfoUbo );
			m_combinePass = std::make_unique< CombinePass >( engine
				, m_size
				, m_sceneUbo
				, m_gpInfoUbo
				, m_ssaoConfig );
		}

		ENSURE( result );
	}

	DeferredRendering::~DeferredRendering()
	{
		m_geometryPassFrameBuffer->Bind();
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();

		for ( auto & attach : m_geometryPassTexAttachs )
		{
			attach.reset();
		}

		for ( auto & texture : m_geometryPassResult )
		{
			texture->Cleanup();
			texture.reset();
		}

		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();
		m_combinePass.reset();
		m_reflection.reset();
		m_lightingPass.reset();
		m_sceneUbo.GetUbo().Cleanup();
	}

	void DeferredRendering::BlitDepthInto( FrameBuffer & fbo )
	{
		m_geometryPassFrameBuffer->BlitInto( fbo
			, Rectangle{ Position{}, m_size }
		, BufferComponent::eDepth | BufferComponent::eStencil );
	}

	void DeferredRendering::Render( RenderInfo & info
		, Scene const & scene
		, Camera const & camera )
	{
		m_engine.SetPerObjectLighting( false );
		auto invView = camera.GetView().get_inverse().get_transposed();
		auto invProj = camera.GetViewport().GetProjection().get_inverse();
		auto invViewProj = ( camera.GetViewport().GetProjection() * camera.GetView() ).get_inverse();
		camera.Apply();
		m_geometryPassFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_geometryPassTexAttachs[size_t( DsTexture::eDepth )]->Attach( AttachmentPoint::eDepth );
		m_geometryPassFrameBuffer->Clear( BufferComponent::eColour | BufferComponent::eDepth | BufferComponent::eStencil );
		m_opaquePass.Render( info, scene.HasShadows() );
		m_geometryPassFrameBuffer->Unbind();

		BlitDepthInto( m_frameBuffer );

		m_sceneUbo.Update( scene, camera );
		m_gpInfoUbo.Update( m_size
			, camera
			, invViewProj
			, invView
			, invProj );
		m_lightingPass->Render( scene
			, camera
			, m_geometryPassResult
			, info );

		if ( scene.HasSkybox() )
		{
			m_reflection->Render( m_geometryPassResult
				, m_lightingPass->GetResult()
				, scene
				, info );

			if ( scene.GetMaterialsType() == MaterialType::ePbrMetallicRoughness
				|| scene.GetMaterialsType() == MaterialType::ePbrSpecularGlossiness )
			{
				m_combinePass->Render( m_geometryPassResult
					, m_lightingPass->GetResult()
					, m_reflection->GetReflection()
					, m_reflection->GetRefraction()
					, scene.GetSkybox().GetIbl()
					, scene.GetFog()
					, m_frameBuffer
					, info );
			}
			else
			{
				m_combinePass->Render( m_geometryPassResult
					, m_lightingPass->GetResult()
					, m_reflection->GetReflection()
					, m_reflection->GetRefraction()
					, scene.GetFog()
					, m_frameBuffer
					, info );
			}
		}
		else
		{
			m_combinePass->Render( m_geometryPassResult
				, m_lightingPass->GetResult()
				, m_reflection->GetReflection()
				, m_reflection->GetRefraction()
				, scene.GetFog()
				, m_frameBuffer
				, info );
		}
	}

	void DeferredRendering::Debug( Camera const & camera )
	{
		auto count = 8 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
		int width = int( m_size.width() ) / count;
		int height = int( m_size.height() ) / count;
		int left = int( m_size.width() ) - width;
		auto size = Size( width, height );
		auto & context = *m_engine.GetRenderSystem()->GetCurrentContext();
		camera.Apply();
		m_frameBuffer.Bind();
		auto index = 0;
		context.RenderDepth( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eDepth )]->GetTexture() );
		context.RenderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData1 )]->GetTexture() );
		context.RenderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData2 )]->GetTexture() );
		context.RenderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData3 )]->GetTexture() );
		context.RenderTexture( Position{ width * index++, 0 }, size, *m_geometryPassResult[size_t( DsTexture::eData4 )]->GetTexture() );
		context.RenderTexture( Position{ width * index++, 0 }, size, *m_lightingPass->GetResult().GetTexture() );
		context.RenderTexture( Position{ width * index++, 0 }, size, *m_reflection->GetReflection().GetTexture() );
		context.RenderTexture( Position{ width * index++, 0 }, size, *m_reflection->GetRefraction().GetTexture() );

		if ( m_ssaoConfig.m_enabled )
		{
			context.RenderTexture( Position{ width * ( index++ ), 0 }, size, m_combinePass->GetSsao() );
		}

		m_frameBuffer.Unbind();
	}
}
