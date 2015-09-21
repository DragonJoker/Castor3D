#include "Dx11RasteriserState.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxRasteriserState::DxRasteriserState( DxRenderSystem * p_pRenderSystem )
		: RasteriserState()
		, m_renderSystem( p_pRenderSystem )
		, m_pRasteriserState( NULL )
	{
		CreateCurrent();
	}

	DxRasteriserState::~DxRasteriserState()
	{
		DestroyCurrent();
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
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateRasterizerState( &l_rasterDesc, &m_pRasteriserState );
		dxTrack( m_renderSystem, m_pRasteriserState, RasteriserState );
		m_bChanged = false;
		return dxCheckError( l_hr, "CreateRasterizerState" );
	}

	void DxRasteriserState::Cleanup()
	{
		ReleaseTracked( m_renderSystem, m_pRasteriserState );
	}

	bool DxRasteriserState::Apply()
	{
		bool l_return = true;

		if ( m_bChanged )
		{
			Cleanup();
			l_return = Initialise();
		}

		if ( l_return && m_pRasteriserState )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

			if ( l_pDeviceContext )
			{
				l_pDeviceContext->RSSetState( m_pRasteriserState );
			}
		}

		return l_return;
	}

	RasteriserStateSPtr DxRasteriserState::DoCreateCurrent()
	{
		return std::make_unique< DxRasteriserState >( m_renderSystem );
	}
}
