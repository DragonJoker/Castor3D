#include "Dx11DepthStencilState.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxDepthStencilState::DxDepthStencilState( DxRenderSystem * p_pRenderSystem )
		:	DepthStencilState()
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pDepthStencilState( NULL )
	{
	}

	DxDepthStencilState::~DxDepthStencilState()
	{
	}

	bool DxDepthStencilState::Initialise()
	{
		D3D11_DEPTH_STENCIL_DESC l_depthStencilDesc = { 0 };
		// Set up the description of the stencil state.
		l_depthStencilDesc.DepthEnable = m_bDepthTest;
		l_depthStencilDesc.DepthWriteMask = DirectX11::Get( m_eDepthMask );
		l_depthStencilDesc.DepthFunc = DirectX11::Get( m_eDepthFunc );
		l_depthStencilDesc.StencilEnable = m_bStencilTest;
		l_depthStencilDesc.StencilReadMask = UINT8( m_uiStencilReadMask );
		l_depthStencilDesc.StencilWriteMask = UINT8( m_uiStencilWriteMask );
		// Stencil operations if pixel is front-facing.
		l_depthStencilDesc.FrontFace.StencilFailOp = DirectX11::Get( m_stStencilFront.m_eFailOp );
		l_depthStencilDesc.FrontFace.StencilDepthFailOp = DirectX11::Get( m_stStencilFront.m_eDepthFailOp );
		l_depthStencilDesc.FrontFace.StencilPassOp = DirectX11::Get( m_stStencilFront.m_ePassOp );
		l_depthStencilDesc.FrontFace.StencilFunc = DirectX11::Get( m_stStencilFront.m_eFunc );
		// Stencil operations if pixel is back-facing.
		l_depthStencilDesc.BackFace.StencilFailOp = DirectX11::Get( m_stStencilBack.m_eFailOp );
		l_depthStencilDesc.BackFace.StencilDepthFailOp = DirectX11::Get( m_stStencilBack.m_eDepthFailOp );
		l_depthStencilDesc.BackFace.StencilPassOp = DirectX11::Get( m_stStencilBack.m_ePassOp );
		l_depthStencilDesc.BackFace.StencilFunc = DirectX11::Get( m_stStencilBack.m_eFunc );
		HRESULT l_hr = m_pRenderSystem->GetDevice()->CreateDepthStencilState( &l_depthStencilDesc, &m_pDepthStencilState );
		dxDebugName( m_pDepthStencilState, DepthStencilState );
		m_bChanged = false;
		return dxCheckError( l_hr, "CreateDepthStencilState" );
	}

	void DxDepthStencilState::Cleanup()
	{
		SafeRelease( m_pDepthStencilState );
	}

	bool DxDepthStencilState::Apply()
	{
		bool l_bReturn = true;

		if ( m_bChanged )
		{
			Cleanup();
			l_bReturn = Initialise();
		}

		if ( l_bReturn && m_pDepthStencilState )
		{
			ID3D11DeviceContext * l_pDC;
			m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDC );

			if ( l_pDC )
			{
				l_pDC->OMSetDepthStencilState( m_pDepthStencilState, 1 );
				SafeRelease( l_pDC );
			}
		}

		return l_bReturn;
	}
}
