#include "Dx9BlendState.hpp"
#include "Dx9RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
namespace Dx9Render
{
	Castor3D::BlendStateSPtr DxBlendState::sm_pCurrent;

	DxBlendState::DxBlendState( DxRenderSystem * p_pRenderSystem )
		:	BlendState(	)
		,	m_pRenderSystem( p_pRenderSystem	)
	{
	}

	DxBlendState::~DxBlendState()
	{
	}

	bool DxBlendState::Initialise()
	{
		if ( !sm_pCurrent )
		{
			sm_pCurrent = std::make_shared< DxBlendState >( m_pRenderSystem );
		}

		m_bChanged = false;
		return true;
	}

	void DxBlendState::Cleanup()
	{
	}

	bool DxBlendState::Apply()
	{
		bool l_bReturn = true;

		sm_pCurrent->SetColourMask( GetColourMaskR(), GetColourMaskG(), GetColourMaskB(), GetColourMaskA() );
		l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 
			( GetColourMaskR() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_RED : 0 )
			| ( GetColourMaskG() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_GREEN : 0 )
			| ( GetColourMaskB() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_BLUE : 0 )
			| ( GetColourMaskA() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_ALPHA : 0 ) ) == S_OK;

		if ( m_rtStates[0].m_bEnableBlend )
		{
			if ( !sm_pCurrent->IsBlendEnabled() )
			{
				sm_pCurrent->EnableBlend( true );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE ) == S_OK;
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) == S_OK;
			}

			if ( sm_pCurrent->GetAlphaSrcBlend() != m_rtStates[0].m_eAlphaSrcBlend )
			{
				sm_pCurrent->SetAlphaSrcBlend( GetAlphaSrcBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLENDALPHA, DirectX9::Get( GetAlphaSrcBlend() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetAlphaDstBlend() != GetAlphaDstBlend() )
			{
				sm_pCurrent->SetAlphaDstBlend( GetAlphaDstBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLENDALPHA, DirectX9::Get( GetAlphaDstBlend() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetAlphaBlendOp() != GetAlphaBlendOp() )
			{
				sm_pCurrent->SetAlphaBlendOp( GetAlphaBlendOp() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOPALPHA, DirectX9::Get( GetAlphaBlendOp() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetRgbSrcBlend() != m_rtStates[0].m_eRgbSrcBlend )
			{
				sm_pCurrent->SetRgbSrcBlend( GetRgbSrcBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLEND, DirectX9::Get( GetRgbSrcBlend() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetRgbDstBlend() != GetRgbDstBlend() )
			{
				sm_pCurrent->SetRgbDstBlend( GetRgbDstBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLEND, DirectX9::Get( GetRgbDstBlend() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetRgbBlendOp() != GetRgbBlendOp() )
			{
				sm_pCurrent->SetRgbBlendOp( GetRgbBlendOp() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOP, DirectX9::Get( GetRgbBlendOp() ) ) == S_OK;
			}
		}
		else
		{
			if ( sm_pCurrent->IsBlendEnabled() )
			{
				sm_pCurrent->EnableBlend( false );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE ) == S_OK;
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) == S_OK;
			}

			if ( sm_pCurrent->GetAlphaSrcBlend() != eBLEND_ONE )
			{
				sm_pCurrent->SetAlphaSrcBlend( eBLEND_ONE );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE ) == S_OK;
			}

			if ( sm_pCurrent->GetAlphaDstBlend() != eBLEND_ZERO )
			{
				sm_pCurrent->SetAlphaDstBlend( eBLEND_ZERO );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO ) == S_OK;
			}

			if ( sm_pCurrent->GetAlphaBlendOp() != eBLEND_OP_ADD )
			{
				sm_pCurrent->SetAlphaBlendOp( eBLEND_OP_ADD );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD ) == S_OK;
			}

			if ( sm_pCurrent->GetRgbSrcBlend() != eBLEND_ONE )
			{
				sm_pCurrent->SetRgbSrcBlend( eBLEND_ONE );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ) == S_OK;
			}

			if ( sm_pCurrent->GetRgbDstBlend() != eBLEND_ZERO )
			{
				sm_pCurrent->SetRgbDstBlend( eBLEND_ZERO );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO ) == S_OK;
			}

			if ( sm_pCurrent->GetRgbBlendOp() != eBLEND_OP_ADD )
			{
				sm_pCurrent->SetRgbBlendOp( eBLEND_OP_ADD );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ) == S_OK;
			}
		}

		return l_bReturn;
	}
}
