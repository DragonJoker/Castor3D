#include "Dx11DepthStencilState.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxDepthStencilState::DxDepthStencilState( DxRenderSystem * p_pRenderSystem )
		: DepthStencilState( *p_pRenderSystem->GetOwner() )
		, m_renderSystem( p_pRenderSystem )
		, m_pDepthStencilState( NULL )
	{
		CreateCurrent();
	}

	DxDepthStencilState::~DxDepthStencilState()
	{
		DestroyCurrent();
	}

	bool DxDepthStencilState::Initialise()
	{
		bool l_return = !m_pDepthStencilState;

		if ( l_return )
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
			HRESULT l_hr = m_renderSystem->GetDevice()->CreateDepthStencilState( &l_depthStencilDesc, &m_pDepthStencilState );
			dxTrack( m_renderSystem, m_pDepthStencilState, DepthStencilState );
			l_return = dxCheckError( l_hr, "CreateDepthStencilState" );
			m_bChanged = false;
		}

		return l_return;
	}

	void DxDepthStencilState::Cleanup()
	{
		ReleaseTracked( m_renderSystem, m_pDepthStencilState );
	}

	bool DxDepthStencilState::Apply()
	{
		bool l_return = true;

		if ( m_bChanged )
		{
			Cleanup();
			l_return = Initialise();
		}

		if ( l_return && m_pDepthStencilState )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

			if ( l_pDeviceContext )
			{
				l_pDeviceContext->OMSetDepthStencilState( m_pDepthStencilState, m_stStencilFront.m_ref );
			}
		}

		return l_return;
	}

	DepthStencilStateSPtr DxDepthStencilState::DoCreateCurrent()
	{
		return std::make_unique< DxDepthStencilState >( m_renderSystem );
	}
}
