#include "Dx10DepthStencilState.hpp"
#include "Dx10RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
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
		D3D10_DEPTH_STENCIL_DESC l_depthStencilDesc = { 0 };
		// Set up the description of the stencil state.
		l_depthStencilDesc.DepthEnable = m_bDepthTest;
		l_depthStencilDesc.DepthWriteMask = DirectX10::Get( m_eDepthMask );
		l_depthStencilDesc.DepthFunc = DirectX10::Get( m_eDepthFunc );
		l_depthStencilDesc.StencilEnable = m_bStencilTest;
		l_depthStencilDesc.StencilReadMask = UINT8( m_uiStencilReadMask );
		l_depthStencilDesc.StencilWriteMask = UINT8( m_uiStencilWriteMask );
		// Stencil operations if pixel is front-facing.
		l_depthStencilDesc.FrontFace.StencilFailOp = DirectX10::Get( m_stStencilFront.m_eFailOp );
		l_depthStencilDesc.FrontFace.StencilDepthFailOp = DirectX10::Get( m_stStencilFront.m_eDepthFailOp );
		l_depthStencilDesc.FrontFace.StencilPassOp = DirectX10::Get( m_stStencilFront.m_ePassOp );
		l_depthStencilDesc.FrontFace.StencilFunc = DirectX10::Get( m_stStencilFront.m_eFunc );
		// Stencil operations if pixel is back-facing.
		l_depthStencilDesc.BackFace.StencilFailOp = DirectX10::Get( m_stStencilBack.m_eFailOp );
		l_depthStencilDesc.BackFace.StencilDepthFailOp = DirectX10::Get( m_stStencilBack.m_eDepthFailOp );
		l_depthStencilDesc.BackFace.StencilPassOp = DirectX10::Get( m_stStencilBack.m_ePassOp );
		l_depthStencilDesc.BackFace.StencilFunc = DirectX10::Get( m_stStencilBack.m_eFunc );
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
			m_pRenderSystem->GetDevice()->OMSetDepthStencilState( m_pDepthStencilState, 1 );
		}

		return l_bReturn;
	}
}
