#include "Dx11BlendState.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxBlendState::DxBlendState( DxRenderSystem * p_renderSystem )
		: BlendState( *p_renderSystem->GetOwner() )
		, m_renderSystem( p_renderSystem )
		, m_pBlendState( NULL )
	{
		CreateCurrent();
	}

	DxBlendState::~DxBlendState()
	{
		DestroyCurrent();
	}

	bool DxBlendState::Initialise()
	{
		Cleanup();
		D3D11_BLEND_DESC l_blendDesc = { 0 };
		l_blendDesc.AlphaToCoverageEnable = FALSE;
		l_blendDesc.IndependentBlendEnable = TRUE;

		UINT8 l_writeMask = ( GetColourMaskR() == eWRITING_MASK_ALL ? D3D11_COLOR_WRITE_ENABLE_RED : 0 )
							| ( GetColourMaskG() == eWRITING_MASK_ALL ? D3D11_COLOR_WRITE_ENABLE_GREEN : 0 )
							| ( GetColourMaskB() == eWRITING_MASK_ALL ? D3D11_COLOR_WRITE_ENABLE_BLUE : 0 )
							| ( GetColourMaskA() == eWRITING_MASK_ALL ? D3D11_COLOR_WRITE_ENABLE_ALPHA : 0 );

		for ( uint8_t i = 0; i < 8; i++ )
		{
			l_blendDesc.RenderTarget[i].BlendEnable = m_rtStates[i].m_bEnableBlend;
			l_blendDesc.RenderTarget[i].SrcBlend = DirectX11::Get( m_rtStates[i].m_eRgbSrcBlend );
			l_blendDesc.RenderTarget[i].DestBlend = DirectX11::Get( m_rtStates[i].m_eRgbDstBlend );
			l_blendDesc.RenderTarget[i].BlendOp = DirectX11::Get( m_rtStates[i].m_eRgbBlendOp );
			l_blendDesc.RenderTarget[i].SrcBlendAlpha = DirectX11::Get( m_rtStates[i].m_eAlphaSrcBlend );
			l_blendDesc.RenderTarget[i].DestBlendAlpha = DirectX11::Get( m_rtStates[i].m_eAlphaDstBlend );
			l_blendDesc.RenderTarget[i].BlendOpAlpha = DirectX11::Get( m_rtStates[i].m_eAlphaBlendOp );
			l_blendDesc.RenderTarget[i].RenderTargetWriteMask = l_writeMask;
		}

		HRESULT l_hr = m_renderSystem->GetDevice()->CreateBlendState( &l_blendDesc, &m_pBlendState );
		dxTrack( m_renderSystem, m_pBlendState, BlendState );
		m_changed = false;
		return dxCheckError( l_hr, "CreateBlendState" );
	}

	void DxBlendState::Cleanup()
	{
		ReleaseTracked( m_renderSystem, m_pBlendState );
	}

	bool DxBlendState::Apply()
	{
		bool l_return = true;

		if ( m_changed )
		{
			Cleanup();
			l_return = Initialise();
		}

		if ( l_return && m_pBlendState )
		{
			ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

			if ( l_deviceContext )
			{
				l_deviceContext->OMSetBlendState( m_pBlendState, m_blendFactors.const_ptr(), m_uiSampleMask );
			}
		}

		return l_return;
	}

	BlendStateSPtr DxBlendState::DoCreateCurrent()
	{
		return std::make_unique< DxBlendState >( m_renderSystem );
	}
}
