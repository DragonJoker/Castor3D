#include "RenderTechnique.hpp"
#include "FrameBuffer.hpp"
#include "ColourRenderBuffer.hpp"
#include "DepthStencilRenderBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "RenderTarget.hpp"
#include "BlendState.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "Engine.hpp"
#include "RenderSystem.hpp"
#include "Camera.hpp"
#include "DynamicTexture.hpp"
#include "Scene.hpp"
#include "Sampler.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderTechniqueBase::RenderTechniqueBase( String const & p_name )
		:	m_pRenderTarget( NULL )
		,	m_pRenderSystem( NULL )
		,	m_name( p_name )
	{
	}

	RenderTechniqueBase::RenderTechniqueBase( String const & p_name, RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & CU_PARAM_UNUSED( p_params ) )
		:	m_pRenderTarget( &p_renderTarget )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pEngine( p_pRenderSystem->GetEngine() )
		,	m_name( p_name )
	{
		m_sampler = m_pEngine->CreateSampler( cuT( "RENDER_TECHNIQUE_SAMPLER" ) );
		m_pFrameBuffer = m_pRenderTarget->CreateFrameBuffer();
		m_pColorBuffer = m_pRenderSystem->CreateDynamicTexture();
		m_pDepthBuffer = m_pFrameBuffer->CreateDepthStencilRenderBuffer(	ePIXEL_FORMAT_DEPTH24S8 );
		m_pColorBuffer->SetRenderTarget( true );
		m_pColorAttach = m_pRenderTarget->CreateAttachment( m_pColorBuffer );
		m_pDepthAttach = m_pRenderTarget->CreateAttachment( m_pDepthBuffer );
		m_wp2DBlendState = m_pEngine->CreateBlendState( cuT( "RT_OVERLAY_BLEND" ) );
		m_wp2DDepthStencilState = m_pEngine->CreateDepthStencilState( cuT( "RT_OVERLAY_DS" ) );

		m_sampler->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_EDGE );
		m_sampler->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_EDGE );
		m_sampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_NEAREST );
		m_sampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_NEAREST );
	}

	RenderTechniqueBase::~RenderTechniqueBase()
	{
	}

	bool RenderTechniqueBase::Create()
	{
		bool l_bReturn = true;
		l_bReturn &= m_pFrameBuffer->Create( 0 );
		l_bReturn &= m_pColorBuffer->Create();
		l_bReturn &= m_pDepthBuffer->Create();

		if ( l_bReturn )
		{
			l_bReturn = DoCreate();
		}

		return l_bReturn;
	}

	void RenderTechniqueBase::Destroy()
	{
		DoDestroy();
		m_pColorBuffer->Destroy();
		m_pDepthBuffer->Destroy();
		m_pFrameBuffer->Destroy();
	}

	bool RenderTechniqueBase::Initialise( uint32_t & p_index )
	{
		bool l_bReturn = m_sampler->Initialise();

		if ( l_bReturn )
		{
#if DX_DEBUG
			m_size.set( m_pRenderTarget->GetSize().width(), m_pRenderTarget->GetSize().height() );
#else
			m_size.set( m_pRenderTarget->GetCamera()->GetWidth(), m_pRenderTarget->GetCamera()->GetHeight() );
#endif
			m_pColorBuffer->SetDimension( eTEXTURE_DIMENSION_2D );
			m_pColorBuffer->SetImage( m_size, ePIXEL_FORMAT_A8R8G8B8 );
			m_pColorBuffer->SetSampler( m_sampler );
			m_size = m_pColorBuffer->GetDimensions();
			m_rect.set( 0, 0, m_size.width(), m_size.height() );
			l_bReturn = m_pColorBuffer->Initialise( p_index++ );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_pDepthBuffer->Initialise( m_size );
		}

		if ( l_bReturn )
		{
			l_bReturn = m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );

			if ( l_bReturn )
			{
				l_bReturn = m_pColorAttach->Attach( eATTACHMENT_POINT_COLOUR0, m_pFrameBuffer, eTEXTURE_TARGET_2D );

				if ( l_bReturn && m_pDepthAttach->GetRenderBuffer() == m_pDepthBuffer )
				{
					l_bReturn = m_pDepthAttach->Attach( eATTACHMENT_POINT_DEPTH, m_pFrameBuffer );
				}

				m_pFrameBuffer->Unbind();
			}
		}

		if ( l_bReturn )
		{
			BlendStateSPtr l_pState = m_wp2DBlendState.lock();
			l_pState->EnableAlphaToCoverage( false );
			l_pState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
			l_pState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
			l_pState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
			l_pState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
			l_pState->EnableBlend( true );
			l_bReturn = l_pState->Initialise();
		}

		if ( l_bReturn )
		{
			DepthStencilStateSPtr l_pState = m_wp2DDepthStencilState.lock();
			l_pState->SetDepthTest( false );
			l_bReturn = l_pState->Initialise();
		}

		if ( l_bReturn )
		{
			l_bReturn = DoInitialise( p_index );
		}

		return l_bReturn;
	}

	void RenderTechniqueBase::Cleanup()
	{
		BlendStateSPtr l_pBlendState = m_wp2DBlendState.lock();
		l_pBlendState->Cleanup();
		m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_CONFIG );
		m_pColorAttach->Detach();
		m_pDepthAttach->Detach();
		m_pFrameBuffer->Unbind();
		//m_pFrameBuffer->DetachAll();
		m_pColorBuffer->Cleanup();
		m_pDepthBuffer->Cleanup();
		m_sampler->Cleanup();
	}

	bool RenderTechniqueBase::BeginRender()
	{
		return DoBeginRender();
	}

	bool RenderTechniqueBase::Render( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_pRenderSystem->PushScene( &p_scene );
		return DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechniqueBase::EndRender()
	{
		DoEndRender();
#if !DX_DEBUG
		BlendStateSPtr l_pBlendState = m_wp2DBlendState.lock();
		DepthStencilStateSPtr l_pDepthStencilState = m_wp2DDepthStencilState.lock();
		m_pFrameBuffer->Bind( eFRAMEBUFFER_MODE_AUTOMATIC, eFRAMEBUFFER_TARGET_DRAW );
		l_pBlendState->Apply();
		l_pDepthStencilState->Apply();
		m_pEngine->RenderOverlays( *m_pRenderSystem->GetTopScene(), m_size );
		m_pFrameBuffer->Unbind();
		m_pRenderSystem->PopScene();
		m_pFrameBuffer->RenderToBuffer( m_pRenderTarget->GetFrameBuffer(), m_pRenderTarget->GetSize(), eBUFFER_COMPONENT_COLOUR | eBUFFER_COMPONENT_DEPTH, m_pRenderTarget->GetDepthStencilState(), m_pRenderTarget->GetRasteriserState() );
#endif
	}

	bool RenderTechniqueBase::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		p_camera.Render();
		p_scene.Render( p_ePrimitives, p_dFrameTime, p_camera );
		p_camera.EndRender();
		return true;
	}
}
