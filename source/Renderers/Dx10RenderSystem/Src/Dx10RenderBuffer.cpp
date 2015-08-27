#include "Dx10RenderBuffer.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10TextureRenderer.hpp"

#include <RenderBuffer.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx10Render
{
	DxRenderBuffer::DxRenderBuffer( DxRenderSystem * p_pRenderSystem, DXGI_FORMAT p_eFormat, eBUFFER_COMPONENT p_eComponent, RenderBuffer & p_renderBuffer )
		:	m_size( 0, 0	)
		,	m_pSurface( NULL	)
		,	m_pOldSurface( NULL	)
		,	m_pRenderSystem( p_pRenderSystem	)
		,	m_pDevice( NULL	)
		,	m_eFormat( p_eFormat	)
		,	m_eComponent( p_eComponent	)
		,	m_renderBuffer( p_renderBuffer	)
	{
	}

	DxRenderBuffer::~DxRenderBuffer()
	{
	}

	bool DxRenderBuffer::Create()
	{
		return true;
	}

	void DxRenderBuffer::Destroy()
	{
	}

	bool DxRenderBuffer::Initialise( Size const & p_size )
	{
		bool l_bReturn = m_size == p_size && m_pSurface != NULL;

		if ( !l_bReturn )
		{
			m_size = p_size;

			if ( !m_pDevice )
			{
				m_pDevice = m_pRenderSystem->GetDevice();
			}

			if ( m_pSurface )
			{
				Cleanup();
			}

			if ( !m_pSurface )
			{
				DWORD l_dwMsType	= 1;
				DWORD l_dwMsQuality	= 0;

				if ( m_renderBuffer.GetSamplesCount() > 1 )
				{
					l_dwMsType = m_renderBuffer.GetSamplesCount();

					if ( m_pRenderSystem->GetDevice()->CheckMultisampleQualityLevels( m_eFormat, l_dwMsType, ( UINT * )&l_dwMsQuality ) == S_OK )
					{
						if ( l_dwMsQuality > 0 )
						{
							l_dwMsQuality--;
						}
					}
					else
					{
						l_dwMsType		= 1;
						l_dwMsQuality	= 0;
					}
				}

				UINT uiWidth = p_size.width();
				UINT uiHeight = p_size.height();
				//			uiWidth = ::GetSystemMetrics( SM_CXFULLSCREEN );
				//			uiHeight = ::GetSystemMetrics( SM_CYFULLSCREEN );
				HRESULT l_hr;

				if ( m_eComponent == eBUFFER_COMPONENT_COLOUR )
				{
					D3D10_TEXTURE2D_DESC l_descTex;
					l_descTex.Width					= uiWidth;
					l_descTex.Height				= uiHeight;
					l_descTex.MipLevels				= 1;
					l_descTex.ArraySize				= 1;
					l_descTex.Format				= m_eFormat;
					l_descTex.SampleDesc.Count		= l_dwMsType;
					l_descTex.SampleDesc.Quality	= l_dwMsQuality;
					l_descTex.Usage					= D3D10_USAGE_DEFAULT;
					l_descTex.BindFlags				= D3D10_BIND_RENDER_TARGET;
					l_descTex.CPUAccessFlags		= 0;
					l_descTex.MiscFlags				= 0;
					l_hr = m_pDevice->CreateTexture2D( &l_descTex, NULL, &m_pTexture );
					dxDebugName( m_pTexture, RTTexture );

					if ( l_hr == S_OK )
					{
						l_hr = m_pDevice->CreateRenderTargetView( m_pTexture, NULL, reinterpret_cast< ID3D10RenderTargetView ** >( &m_pSurface ) );
						dxDebugName( m_pSurface, RTView );
					}
				}
				else
				{
					D3D10_TEXTURE2D_DESC l_descDepth;
					l_descDepth.Width				= uiWidth;
					l_descDepth.Height				= uiHeight;
					l_descDepth.MipLevels			= 1;
					l_descDepth.ArraySize			= 1;
					l_descDepth.Format				= m_eFormat;
					l_descDepth.SampleDesc.Count	= 1;
					l_descDepth.SampleDesc.Quality	= 0;
					l_descDepth.Usage				= D3D10_USAGE_DEFAULT;
					l_descDepth.BindFlags			= D3D10_BIND_DEPTH_STENCIL;
					l_descDepth.CPUAccessFlags		= 0;
					l_descDepth.MiscFlags			= 0;
					l_hr = m_pDevice->CreateTexture2D( &l_descDepth, NULL, &m_pTexture );
					dxDebugName( m_pTexture, DSTexture );

					if ( l_hr == S_OK )
					{
						l_hr = m_pDevice->CreateDepthStencilView( m_pTexture, NULL, reinterpret_cast< ID3D10DepthStencilView ** >( &m_pSurface ) );
						dxDebugName( m_pSurface, DSView );
					}
				}

				l_bReturn = l_hr == S_OK;
			}
		}

		return l_bReturn;
	}

	void DxRenderBuffer::Cleanup()
	{
		SafeRelease( m_pSurface );
		SafeRelease( m_pTexture );
		SafeRelease( m_pOldSurface );
	}

	bool DxRenderBuffer::Bind()
	{
#if 1
		return true;
#else
		HRESULT l_hr = S_FALSE;

		if ( m_pSurface )
		{
			ID3D10Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( m_eComponent == eBUFFER_COMPONENT_COLOUR )
			{
				ID3D10DepthStencilView * l_pView;
				l_pDevice->OMGetRenderTargets( 1, reinterpret_cast< ID3D10RenderTargetView ** >( &m_pOldSurface ), &l_pView );
				l_pDevice->OMSetRenderTargets( 1, reinterpret_cast< ID3D10RenderTargetView ** >( &m_pSurface ), l_pView );
				SafeRelease( l_pView );
			}
			else
			{
				ID3D10RenderTargetView * l_pView;
				l_pDevice->OMGetRenderTargets( 1, &l_pView, reinterpret_cast< ID3D10DepthStencilView ** >( &m_pSurface ) );
				l_pDevice->OMSetRenderTargets( 1, &l_pView, reinterpret_cast< ID3D10DepthStencilView * >( m_pOldSurface ) );
				SafeRelease( l_pView );
			}

			return true;
		}

		return l_hr == S_OK;
#endif
	}

	void DxRenderBuffer::Unbind()
	{
		return;

		if ( m_pOldSurface )
		{
			ID3D10Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( m_eComponent == eBUFFER_COMPONENT_COLOUR )
			{
				ID3D10DepthStencilView * l_pView;
				l_pDevice->OMGetRenderTargets( 1, NULL, &l_pView );
				l_pDevice->OMSetRenderTargets( 1, reinterpret_cast< ID3D10RenderTargetView ** >( &m_pOldSurface ), l_pView );
				SafeRelease( l_pView );
			}
			else
			{
				ID3D10RenderTargetView * l_pView;
				l_pDevice->OMGetRenderTargets( 1, &l_pView, NULL );
				l_pDevice->OMSetRenderTargets( 1, &l_pView, reinterpret_cast< ID3D10DepthStencilView * >( m_pOldSurface ) );
				SafeRelease( l_pView );
			}

			SafeRelease( m_pOldSurface );
		}
	}

	bool DxRenderBuffer::Resize( Size const & p_size )
	{
		return Initialise( p_size );
	}
}
