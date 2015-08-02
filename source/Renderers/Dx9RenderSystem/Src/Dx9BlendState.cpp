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
			std::shared_ptr< DxBlendState > l_current = std::make_shared< DxBlendState >( m_pRenderSystem );
			l_current->SetColourMask( eWRITING_MASK_ALL, eWRITING_MASK_ALL, eWRITING_MASK_ALL, eWRITING_MASK_ALL );
			l_current->EnableBlend( false );
			l_current->SetAlphaSrcBlend( eBLEND_ONE );
			l_current->SetAlphaDstBlend( eBLEND_ZERO );
			l_current->SetAlphaBlendOp( eBLEND_OP_ADD );
			l_current->SetRgbSrcBlend( eBLEND_ONE );
			l_current->SetRgbDstBlend( eBLEND_ZERO );
			l_current->SetRgbBlendOp( eBLEND_OP_ADD );
			l_current->m_coloursMask = D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA;
			sm_pCurrent = l_current;
		}

		m_coloursMask = ( GetColourMaskR() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_RED : 0 )
		| ( GetColourMaskG() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_GREEN : 0 )
		| ( GetColourMaskB() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_BLUE : 0 )
		| ( GetColourMaskA() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_ALPHA : 0 );

		m_bChanged = false;
		return true;
	}

	void DxBlendState::Cleanup()
	{
	}

	bool DxBlendState::Apply()
	{
		bool l_bReturn = true;
		DxBlendState & l_current = *std::static_pointer_cast< DxBlendState >( sm_pCurrent );

		if ( m_bChanged )
		{
			m_coloursMask = ( GetColourMaskR() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_RED : 0 )
			| ( GetColourMaskG() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_GREEN : 0 )
			| ( GetColourMaskB() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_BLUE : 0 )
			| ( GetColourMaskA() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_ALPHA : 0 );
		}

		if ( m_coloursMask != l_current.m_coloursMask )
		{
			l_current.SetColourMask( GetColourMaskR(), GetColourMaskG(), GetColourMaskB(), GetColourMaskA() );
			l_current.m_coloursMask = m_coloursMask;
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 
				( GetColourMaskR() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_RED : 0 )
				| ( GetColourMaskG() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_GREEN : 0 )
				| ( GetColourMaskB() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_BLUE : 0 )
				| ( GetColourMaskA() == eWRITING_MASK_ALL ? D3DCOLORWRITEENABLE_ALPHA : 0 ) ) == S_OK;
		}

		if ( m_rtStates[0].m_bEnableBlend )
		{
			if ( !l_current.IsBlendEnabled() )
			{
				l_current.EnableBlend( true );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE ) == S_OK;
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) == S_OK;
			}

			if ( l_current.GetAlphaSrcBlend() != m_rtStates[0].m_eAlphaSrcBlend )
			{
				l_current.SetAlphaSrcBlend( GetAlphaSrcBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLENDALPHA, DirectX9::Get( GetAlphaSrcBlend() ) ) == S_OK;
			}

			if ( l_current.GetAlphaDstBlend() != GetAlphaDstBlend() )
			{
				l_current.SetAlphaDstBlend( GetAlphaDstBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLENDALPHA, DirectX9::Get( GetAlphaDstBlend() ) ) == S_OK;
			}

			if ( l_current.GetAlphaBlendOp() != GetAlphaBlendOp() )
			{
				l_current.SetAlphaBlendOp( GetAlphaBlendOp() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOPALPHA, DirectX9::Get( GetAlphaBlendOp() ) ) == S_OK;
			}

			if ( l_current.GetRgbSrcBlend() != m_rtStates[0].m_eRgbSrcBlend )
			{
				l_current.SetRgbSrcBlend( GetRgbSrcBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLEND, DirectX9::Get( GetRgbSrcBlend() ) ) == S_OK;
			}

			if ( l_current.GetRgbDstBlend() != GetRgbDstBlend() )
			{
				l_current.SetRgbDstBlend( GetRgbDstBlend() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLEND, DirectX9::Get( GetRgbDstBlend() ) ) == S_OK;
			}

			if ( l_current.GetRgbBlendOp() != GetRgbBlendOp() )
			{
				l_current.SetRgbBlendOp( GetRgbBlendOp() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOP, DirectX9::Get( GetRgbBlendOp() ) ) == S_OK;
			}
		}
		else
		{
			if ( l_current.IsBlendEnabled() )
			{
				l_current.EnableBlend( false );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, FALSE ) == S_OK;
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) == S_OK;
			}

			if ( l_current.GetAlphaSrcBlend() != eBLEND_ONE )
			{
				l_current.SetAlphaSrcBlend( eBLEND_ONE );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE ) == S_OK;
			}

			if ( l_current.GetAlphaDstBlend() != eBLEND_ZERO )
			{
				l_current.SetAlphaDstBlend( eBLEND_ZERO );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO ) == S_OK;
			}

			if ( l_current.GetAlphaBlendOp() != eBLEND_OP_ADD )
			{
				l_current.SetAlphaBlendOp( eBLEND_OP_ADD );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD ) == S_OK;
			}

			if ( l_current.GetRgbSrcBlend() != eBLEND_ONE )
			{
				l_current.SetRgbSrcBlend( eBLEND_ONE );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ) == S_OK;
			}

			if ( l_current.GetRgbDstBlend() != eBLEND_ZERO )
			{
				l_current.SetRgbDstBlend( eBLEND_ZERO );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO ) == S_OK;
			}

			if ( l_current.GetRgbBlendOp() != eBLEND_OP_ADD )
			{
				l_current.SetRgbBlendOp( eBLEND_OP_ADD );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD ) == S_OK;
			}
		}

		return l_bReturn;
	}
}
