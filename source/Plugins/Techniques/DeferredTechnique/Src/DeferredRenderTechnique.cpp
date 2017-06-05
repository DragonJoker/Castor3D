#include "DeferredRenderTechnique.hpp"

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
		, Parameters const & p_params
		, SsaoConfig const & p_config )
		: Castor3D::RenderTechnique( RenderTechnique::Type
			, p_renderTarget
			, p_renderSystem
			, std::make_unique< deferred_common::OpaquePass >( *p_renderTarget.GetScene()
				, p_renderTarget.GetCamera().get()
				, p_config )
			, std::make_unique< ForwardRenderTechniquePass >( cuT( "deferred_transparent" )
				, *p_renderTarget.GetScene()
				, p_renderTarget.GetCamera().get()
				, false
				, false
				, false
				, nullptr
				, p_config )
			, p_params )
		, m_sceneUbo{ *p_renderSystem.GetEngine() }
		, m_ssaoConfig{ p_config }
	{
		Logger::LogInfo( cuT( "Using deferred rendering" ) );
	}

	RenderTechnique::~RenderTechnique()
	{
	}

	RenderTechniqueSPtr RenderTechnique::CreateInstance( RenderTarget & p_renderTarget
		, RenderSystem & p_renderSystem
		, Parameters const & p_params
		, SsaoConfig const & p_config )
	{
		return std::make_shared< RenderTechnique >( p_renderTarget
			, p_renderSystem
			, p_params
			, p_config );
	}

	bool RenderTechnique::DoInitialise( uint32_t & p_index )
	{
		bool l_return = DoInitialiseGeometryPass( p_index );

		if ( l_return )
		{
			m_lightingPass = std::make_unique< deferred_common::LightingPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize()
				, *m_renderTarget.GetScene()
				, static_cast< deferred_common::OpaquePass & >( *m_opaquePass )
				, *m_frameBuffer.m_depthAttach
				, m_sceneUbo );
			m_reflection = std::make_unique< deferred_common::ReflectionPass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize() );
			m_combinePass = std::make_unique< deferred_common::CombinePass >( *m_renderSystem.GetEngine()
				, m_renderTarget.GetSize()
				, m_ssaoConfig );
		}

		m_frameBuffer.m_frameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		return l_return;
	}

	void RenderTechnique::DoCleanup()
	{
		DoCleanupGeometryPass();
		m_combinePass.reset();
		m_reflection.reset();
		m_lightingPass.reset();
		m_sceneUbo.GetUbo().Cleanup();
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

		m_frameBuffer.m_frameBuffer->Bind( FrameBufferTarget::eDraw );
		m_frameBuffer.m_depthAttach->Attach( AttachmentPoint::eDepthStencil );
		m_frameBuffer.m_frameBuffer->Clear( BufferComponent::eColour | BufferComponent::eStencil );
		m_frameBuffer.m_frameBuffer->Unbind();
		m_geometryPassFrameBuffer->BlitInto( *m_frameBuffer.m_frameBuffer
			, Rectangle{ Position{}, m_size }
			, BufferComponent::eDepth );

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

		m_combinePass->Render( m_geometryPassResult
			, *l_result
			, l_camera
			, l_invViewProj
			, l_invView
			, l_invProj
			, l_scene.GetFog()
			, *m_frameBuffer.m_frameBuffer );
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

		auto l_count = 6 + ( m_ssaoConfig.m_enabled ? 1 : 0 );
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

		if ( m_ssaoConfig.m_enabled )
		{
			l_context.RenderTexture( Position{ l_width * ( l_index++ ), 0 }, l_size, m_combinePass->GetSsao() );
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

	void RenderTechnique::DoCleanupGeometryPass()
	{
		m_geometryPassFrameBuffer->Bind();
		m_geometryPassFrameBuffer->DetachAll();
		m_geometryPassFrameBuffer->Unbind();
		m_geometryPassFrameBuffer->Cleanup();
		m_geometryPassFrameBuffer->Destroy();
		m_geometryPassFrameBuffer.reset();

		for ( auto & l_unit : m_geometryPassResult )
		{
			l_unit->Cleanup();
			l_unit.reset();
		}
	}
}
