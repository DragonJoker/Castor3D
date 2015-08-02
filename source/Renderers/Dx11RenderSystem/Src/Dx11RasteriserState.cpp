#include "Dx11RasteriserState.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxRasteriserState::DxRasteriserState( DxRenderSystem * p_pRenderSystem )
		: RasteriserState()
		, m_pRenderSystem( p_pRenderSystem )
		, m_pRasteriserState( NULL )
	{
	}

	DxRasteriserState::~DxRasteriserState()
	{
	}

	bool DxRasteriserState::Initialise()
	{
		D3D11_RASTERIZER_DESC l_rasterDesc = D3D11_RASTERIZER_DESC();
		// Setup the raster description which will determine how and what polygons will be drawn.
		l_rasterDesc.AntialiasedLineEnable = m_bAntialiasedLines;
		l_rasterDesc.CullMode = DirectX11::Get( m_eCulledFaces );
		l_rasterDesc.DepthBias = 0;
		l_rasterDesc.DepthBiasClamp = 0.0f;
		l_rasterDesc.SlopeScaledDepthBias = m_fDepthBias / 65535.0f;
		l_rasterDesc.DepthClipEnable = m_bDepthClipping;
		l_rasterDesc.FillMode = DirectX11::Get( m_eFillMode );
		l_rasterDesc.FrontCounterClockwise = m_bFrontCCW;
		l_rasterDesc.MultisampleEnable = m_bMultisampled;
		l_rasterDesc.ScissorEnable = m_bScissor;
		HRESULT l_hr = m_pRenderSystem->GetDevice()->CreateRasterizerState( &l_rasterDesc, &m_pRasteriserState );
		dxDebugName( m_pRasteriserState, RasteriserState );
		m_bChanged = false;
		return dxCheckError( l_hr, "CreateRasterizerState" );
	}

	void DxRasteriserState::Cleanup()
	{
		SafeRelease( m_pRasteriserState );
	}

	bool DxRasteriserState::Apply()
	{
		bool l_bReturn = true;

		if ( m_bChanged )
		{
			Cleanup();
			l_bReturn = Initialise();
		}

		if ( l_bReturn && m_pRasteriserState )
		{
			ID3D11DeviceContext * l_pDC;
			m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDC );

			if ( l_pDC )
			{
				l_pDC->RSSetState( m_pRasteriserState );
				SafeRelease( l_pDC );
			}
		}

		return l_bReturn;
	}
}
