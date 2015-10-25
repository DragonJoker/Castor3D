#include "RenderTechnique.hpp"

#include "BlendStateManager.hpp"
#include "Camera.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "ColourRenderBuffer.hpp"
#include "DepthStencilRenderBuffer.hpp"
#include "DepthStencilStateManager.hpp"
#include "FrameBuffer.hpp"
#include "OverlayManager.hpp"
#include "RasteriserState.hpp"
#include "RenderBufferAttachment.hpp"
#include "RenderSystem.hpp"
#include "RenderTarget.hpp"
#include "SamplerManager.hpp"
#include "Scene.hpp"
#include "TextureAttachment.hpp"

using namespace Castor;

namespace Castor3D
{
	RenderTechniqueBase::RenderTechniqueBase( String const & p_name, RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & CU_PARAM_UNUSED( p_params ) )
		: OwnedBy< Engine >( *p_renderSystem->GetOwner() )
		, m_pRenderTarget( &p_renderTarget )
		, m_renderSystem( p_renderSystem )
		, m_name( p_name )
	{
		m_wp2DBlendState = GetOwner()->GetBlendStateManager().Create( cuT( "RT_OVERLAY_BLEND" ) );
		m_wp2DDepthStencilState = GetOwner()->GetDepthStencilStateManager().Create( cuT( "RT_OVERLAY_DS" ) );
	}

	RenderTechniqueBase::~RenderTechniqueBase()
	{
	}

	bool RenderTechniqueBase::Create()
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoCreate();
		}

		return l_return;
	}

	void RenderTechniqueBase::Destroy()
	{
		DoDestroy();
	}

	bool RenderTechniqueBase::Initialise( uint32_t & p_index )
	{
		BlendStateSPtr l_pState = m_wp2DBlendState.lock();
		l_pState->EnableAlphaToCoverage( false );
		l_pState->SetAlphaSrcBlend( eBLEND_SRC_ALPHA );
		l_pState->SetAlphaDstBlend( eBLEND_INV_SRC_ALPHA );
		l_pState->SetRgbSrcBlend( eBLEND_SRC_ALPHA );
		l_pState->SetRgbDstBlend( eBLEND_INV_SRC_ALPHA );
		l_pState->EnableBlend( true );
		bool l_return = l_pState->Initialise();

		if ( l_return )
		{
			DepthStencilStateSPtr l_pState = m_wp2DDepthStencilState.lock();
			l_pState->SetDepthTest( false );
			l_return = l_pState->Initialise();
		}

		if ( l_return )
		{
			l_return = DoInitialise( p_index );
		}

		return l_return;
	}

	void RenderTechniqueBase::Cleanup()
	{
		DoCleanup();
		BlendStateSPtr l_pBlendState = m_wp2DBlendState.lock();
		l_pBlendState->Cleanup();
	}

	bool RenderTechniqueBase::BeginRender()
	{
		return DoBeginRender();
	}

	bool RenderTechniqueBase::Render( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		m_renderSystem->PushScene( &p_scene );
		return DoRender( p_scene, p_camera, p_ePrimitives, p_dFrameTime );
	}

	void RenderTechniqueBase::EndRender()
	{
		DoEndRender();
		m_wp2DBlendState.lock()->Apply();
		m_wp2DDepthStencilState.lock()->Apply();
		GetOwner()->GetOverlayManager().RenderOverlays( *m_renderSystem->GetTopScene(), m_pRenderTarget->GetSize() );
		m_pRenderTarget->GetFrameBuffer()->Unbind();
		m_renderSystem->PopScene();
	}

	String RenderTechniqueBase::GetPixelShaderSource( uint32_t p_uiFlags )const
	{
		return DoGetPixelShaderSource( p_uiFlags );
	}

	bool RenderTechniqueBase::DoRender( Scene & p_scene, Camera & p_camera, eTOPOLOGY p_ePrimitives, double p_dFrameTime )
	{
		p_camera.Render();
		p_scene.Render( *this, p_ePrimitives, p_dFrameTime, p_camera );
		p_camera.EndRender();
		return true;
	}
}
