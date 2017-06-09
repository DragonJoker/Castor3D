#include "WeightedBlendRendering.hpp"

#include "FrameBuffer/DepthStencilRenderBuffer.hpp"
#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Render/RenderInfo.hpp"
#include "Texture/TextureLayout.hpp"
#include "Shader/PassBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	WeightedBlendRendering::WeightedBlendRendering( Engine & p_engine
		, TransparentPass & p_transparentPass
		, FrameBuffer & p_frameBuffer
		, Castor::Size const & p_size
		, Scene const & p_scene )
		: m_engine{ p_engine }
		, m_transparentPass{ p_transparentPass }
		, m_frameBuffer{ p_frameBuffer }
		, m_size{ p_size }
	{
		auto & l_renderSystem = *p_engine.GetRenderSystem();
		m_weightedBlendPassFrameBuffer = l_renderSystem.CreateFrameBuffer();
		m_weightedBlendPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		bool l_result = m_weightedBlendPassFrameBuffer->Create();

		if ( l_result )
		{
			l_result = m_weightedBlendPassFrameBuffer->Initialise( m_size );
		}

		if ( l_result )
		{
			for ( uint32_t i = 0; i < uint32_t( WbTexture::eCount ); i++ )
			{
				auto l_texture = l_renderSystem.CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, GetTextureFormat( WbTexture( i ) )
					, m_size );
				l_texture->GetImage().InitialiseSource();

				m_weightedBlendPassResult[i] = std::make_unique< TextureUnit >( m_engine );
				m_weightedBlendPassResult[i]->SetIndex( i );
				m_weightedBlendPassResult[i]->SetTexture( l_texture );
				m_weightedBlendPassResult[i]->SetSampler( m_engine.GetLightsSampler() );
				m_weightedBlendPassResult[i]->Initialise();

				m_weightedBlendPassTexAttachs[i] = m_weightedBlendPassFrameBuffer->CreateAttachment( l_texture );
			}

			m_weightedBlendPassFrameBuffer->Bind();

			for ( int i = 0; i < size_t( WbTexture::eCount ) && l_result; i++ )
			{
				m_weightedBlendPassFrameBuffer->Attach( GetTextureAttachmentPoint( WbTexture( i ) )
					, GetTextureAttachmentIndex( WbTexture( i ) )
					, m_weightedBlendPassTexAttachs[i]
					, m_weightedBlendPassResult[i]->GetType() );
			}

			ENSURE( m_weightedBlendPassFrameBuffer->IsComplete() );
			m_weightedBlendPassFrameBuffer->SetDrawBuffers();
			m_weightedBlendPassFrameBuffer->Unbind();
		}

		if ( l_result )
		{
			m_finalCombinePass = std::make_unique< FinalCombinePass >( *l_renderSystem.GetEngine()
				, m_size );
		}

		ENSURE( l_result );
	}

	WeightedBlendRendering::~WeightedBlendRendering()
	{
		m_weightedBlendPassFrameBuffer->Bind();
		m_weightedBlendPassFrameBuffer->DetachAll();
		m_weightedBlendPassFrameBuffer->Unbind();
		m_weightedBlendPassFrameBuffer->Cleanup();

		for ( auto & l_attach : m_weightedBlendPassTexAttachs )
		{
			l_attach.reset();
		}

		for ( auto & l_texture : m_weightedBlendPassResult )
		{
			l_texture->Cleanup();
			l_texture.reset();
		}

		m_weightedBlendPassFrameBuffer->Destroy();
		m_weightedBlendPassFrameBuffer.reset();
	}

	void WeightedBlendRendering::Render( RenderInfo & p_info
		, Scene const & p_scene
		, Camera const & p_camera )
	{
		static Colour l_accumClear = Colour::from_predef( PredefinedColour::eTransparentBlack );
		static Colour l_revealClear = Colour::from_predef( PredefinedColour::eOpaqueWhite );
		auto l_invView = p_camera.GetView().get_inverse().get_transposed();
		auto l_invProj = p_camera.GetViewport().GetProjection().get_inverse();
		auto l_invViewProj = ( p_camera.GetViewport().GetProjection() * p_camera.GetView() ).get_inverse();

		m_engine.SetPerObjectLighting( true );
		m_transparentPass.RenderShadowMaps();
		p_camera.Apply();

		// Accumulate blend.
		m_weightedBlendPassFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_weightedBlendPassTexAttachs[size_t( WbTexture::eAccumulation )]->Clear( l_accumClear );
		m_weightedBlendPassTexAttachs[size_t( WbTexture::eRevealage )]->Clear( l_revealClear );
		m_transparentPass.Render( p_info, p_scene.HasShadows() );
		m_weightedBlendPassFrameBuffer->Unbind();

		m_finalCombinePass->Render( m_weightedBlendPassResult
			, m_frameBuffer
			, p_camera
			, l_invViewProj
			, l_invView
			, l_invProj
			, p_scene.GetFog() );
	}

	void WeightedBlendRendering::Debug( Camera const & p_camera )
	{
		auto l_count = 2;
		int l_width = int( m_size.width() ) / 6;
		int l_height = int( m_size.height() ) / 6;
		int l_left = int( m_size.width() ) - l_width;
		int l_top = int( m_size.height() ) - l_height;
		auto l_size = Size( l_width, l_height );
		auto & l_context = *m_engine.GetRenderSystem()->GetCurrentContext();
		p_camera.Apply();

		m_frameBuffer.Bind();
		auto l_index = 0;
		l_context.RenderDepth( Position{ l_left, l_top - l_height * l_index++ }, l_size, *m_weightedBlendPassResult[size_t( WbTexture::eDepth )]->GetTexture() );
		l_context.RenderTexture( Position{ l_left, l_top - l_height * l_index++ }, l_size, *m_weightedBlendPassResult[size_t( WbTexture::eRevealage )]->GetTexture() );
		l_context.RenderTexture( Position{ l_left, l_top - l_height * l_index++ }, l_size, *m_weightedBlendPassResult[size_t( WbTexture::eAccumulation )]->GetTexture() );
		m_frameBuffer.Unbind();
	}
}
