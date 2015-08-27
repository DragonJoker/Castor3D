#include "Dx9DepthStencilState.hpp"
#include "Dx9RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	Castor3D::DepthStencilStateSPtr DxDepthStencilState::sm_pCurrent;

	DxDepthStencilState::DxDepthStencilState( DxRenderSystem * p_pRenderSystem )
		:	DepthStencilState()
		,	m_pRenderSystem( p_pRenderSystem )
	{
	}

	DxDepthStencilState::~DxDepthStencilState()
	{
	}

	bool DxDepthStencilState::Initialise()
	{
		if ( !sm_pCurrent )
		{
			sm_pCurrent = std::make_shared< DxDepthStencilState >( m_pRenderSystem );
		}

		m_bChanged = false;
		return true;
	}

	void DxDepthStencilState::Cleanup()
	{
	}

	bool DxDepthStencilState::Apply()
	{
		bool l_bReturn = true;

		if ( sm_pCurrent->GetDepthTest() != GetDepthTest() )
		{
			sm_pCurrent->SetDepthTest( GetDepthTest() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ZENABLE, GetDepthTest() ) == S_OK;
		}

		if ( sm_pCurrent->GetDepthFunc() != GetDepthFunc() )
		{
			sm_pCurrent->SetDepthFunc( GetDepthFunc() );

			if ( GetDepthTest() )
			{
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ZFUNC, DirectX9::Get( GetDepthFunc() ) ) == S_OK;
			}
		}

		if ( sm_pCurrent->GetDepthMask() != GetDepthMask() )
		{
			sm_pCurrent->SetDepthMask( GetDepthMask() );

			if ( GetDepthMask() != eWRITING_MASK_ZERO )
			{
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE ) == S_OK;
			}
			else
			{
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE ) == S_OK;
			}
		}

		if ( sm_pCurrent->GetStencilTest() != GetStencilTest() )
		{
			sm_pCurrent->SetStencilTest( GetStencilTest() );
			l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILENABLE, GetStencilTest() ) == S_OK;
		}

		if ( GetStencilTest() )
		{
			if ( sm_pCurrent->GetStencilReadMask() != GetStencilReadMask() )
			{
				sm_pCurrent->SetStencilReadMask( GetStencilReadMask() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILMASK, GetStencilReadMask() ) == S_OK;
			}

			if ( sm_pCurrent->GetStencilWriteMask() != GetStencilWriteMask() )
			{
				sm_pCurrent->SetStencilWriteMask( GetStencilWriteMask() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILWRITEMASK, GetStencilWriteMask() ) == S_OK;
			}

			if ( sm_pCurrent->GetStencilFrontPassOp() != GetStencilFrontPassOp() )
			{
				sm_pCurrent->SetStencilFrontPassOp( GetStencilFrontPassOp() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILPASS, DirectX9::Get( GetStencilFrontPassOp() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetStencilFrontFailOp() != GetStencilFrontFailOp() )
			{
				sm_pCurrent->SetStencilFrontFailOp( GetStencilFrontFailOp() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILFAIL, DirectX9::Get( GetStencilFrontFailOp() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetStencilFrontDepthFailOp() != GetStencilFrontDepthFailOp() )
			{
				sm_pCurrent->SetStencilFrontDepthFailOp( GetStencilFrontDepthFailOp() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILZFAIL, DirectX9::Get( GetStencilFrontDepthFailOp() ) ) == S_OK;
			}

			if ( sm_pCurrent->GetStencilFrontRef() != GetStencilFrontRef() )
			{
				sm_pCurrent->SetStencilFrontRef( GetStencilFrontRef() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILREF, GetStencilFrontRef() ) == S_OK;
			}

			if ( sm_pCurrent->GetStencilFrontFunc() != GetStencilFrontFunc() )
			{
				sm_pCurrent->SetStencilFrontFunc( GetStencilFrontFunc() );
				l_bReturn &= m_pRenderSystem->GetDevice()->SetRenderState( D3DRS_STENCILFUNC, DirectX9::Get( GetStencilFrontFunc() ) ) == S_OK;
			}
		}

		return l_bReturn;
	}
}
