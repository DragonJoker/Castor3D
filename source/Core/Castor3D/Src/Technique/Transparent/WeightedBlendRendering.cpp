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
		auto & renderSystem = *p_engine.GetRenderSystem();
		m_weightedBlendPassFrameBuffer = renderSystem.CreateFrameBuffer();
		m_weightedBlendPassFrameBuffer->SetClearColour( Colour::from_predef( PredefinedColour::eOpaqueBlack ) );
		bool result = m_weightedBlendPassFrameBuffer->Create();

		if ( result )
		{
			result = m_weightedBlendPassFrameBuffer->Initialise( m_size );
		}

		if ( result )
		{
			for ( uint32_t i = 0; i < uint32_t( WbTexture::eCount ); i++ )
			{
				auto texture = renderSystem.CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead | AccessType::eWrite
					, GetTextureFormat( WbTexture( i ) )
					, m_size );
				texture->GetImage().InitialiseSource();

				m_weightedBlendPassResult[i] = std::make_unique< TextureUnit >( m_engine );
				m_weightedBlendPassResult[i]->SetIndex( i );
				m_weightedBlendPassResult[i]->SetTexture( texture );
				m_weightedBlendPassResult[i]->SetSampler( m_engine.GetLightsSampler() );
				m_weightedBlendPassResult[i]->Initialise();

				m_weightedBlendPassTexAttachs[i] = m_weightedBlendPassFrameBuffer->CreateAttachment( texture );
			}

			m_weightedBlendPassFrameBuffer->Bind();

			for ( int i = 0; i < size_t( WbTexture::eCount ) && result; i++ )
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

		if ( result )
		{
			m_finalCombinePass = std::make_unique< FinalCombinePass >( *renderSystem.GetEngine()
				, m_size );
		}

		ENSURE( result );
	}

	WeightedBlendRendering::~WeightedBlendRendering()
	{
		m_weightedBlendPassFrameBuffer->Bind();
		m_weightedBlendPassFrameBuffer->DetachAll();
		m_weightedBlendPassFrameBuffer->Unbind();
		m_weightedBlendPassFrameBuffer->Cleanup();

		for ( auto & attach : m_weightedBlendPassTexAttachs )
		{
			attach.reset();
		}

		for ( auto & texture : m_weightedBlendPassResult )
		{
			texture->Cleanup();
			texture.reset();
		}

		m_weightedBlendPassFrameBuffer->Destroy();
		m_weightedBlendPassFrameBuffer.reset();
	}

	void WeightedBlendRendering::Render( RenderInfo & p_info
		, Scene const & p_scene
		, Camera const & p_camera )
	{
		static Colour accumClear = Colour::from_predef( PredefinedColour::eTransparentBlack );
		static Colour revealClear = Colour::from_predef( PredefinedColour::eOpaqueWhite );
		auto invView = p_camera.GetView().get_inverse().get_transposed();
		auto invProj = p_camera.GetViewport().GetProjection().get_inverse();
		auto invViewProj = ( p_camera.GetViewport().GetProjection() * p_camera.GetView() ).get_inverse();

		m_engine.SetPerObjectLighting( true );
		m_transparentPass.RenderShadowMaps();
		p_camera.Apply();

		// Accumulate blend.
		m_weightedBlendPassFrameBuffer->Bind( FrameBufferTarget::eDraw );
		m_weightedBlendPassTexAttachs[size_t( WbTexture::eAccumulation )]->Clear( accumClear );
		m_weightedBlendPassTexAttachs[size_t( WbTexture::eRevealage )]->Clear( revealClear );
		m_transparentPass.Render( p_info, p_scene.HasShadows() );
		m_weightedBlendPassFrameBuffer->Unbind();

		m_finalCombinePass->Render( m_weightedBlendPassResult
			, m_frameBuffer
			, p_camera
			, invViewProj
			, invView
			, invProj
			, p_scene.GetFog() );
	}

	void WeightedBlendRendering::Debug( Camera const & p_camera )
	{
		auto count = 2;
		int width = int( m_size.width() ) / 6;
		int height = int( m_size.height() ) / 6;
		int left = int( m_size.width() ) - width;
		int top = int( m_size.height() ) - height;
		auto size = Size( width, height );
		auto & context = *m_engine.GetRenderSystem()->GetCurrentContext();
		p_camera.Apply();

		m_frameBuffer.Bind();
		auto index = 0;
		context.RenderDepth( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eDepth )]->GetTexture() );
		context.RenderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eRevealage )]->GetTexture() );
		context.RenderTexture( Position{ left, top - height * index++ }, size, *m_weightedBlendPassResult[size_t( WbTexture::eAccumulation )]->GetTexture() );
		m_frameBuffer.Unbind();
	}
}
