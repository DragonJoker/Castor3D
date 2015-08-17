#include "Dx11RenderBuffer.hpp"
#include "Dx11RenderSystem.hpp"

#include <RenderBuffer.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxRenderBuffer::DxRenderBuffer( DxRenderSystem * p_pRenderSystem, DXGI_FORMAT p_eFormat, eBUFFER_COMPONENT p_eComponent, RenderBuffer & p_renderBuffer )
		: m_size( 0, 0 )
		, m_pSurface( NULL )
		, m_pOldSurface( NULL )
		, m_pRenderSystem( p_pRenderSystem )
		, m_eFormat( p_eFormat )
		, m_eComponent( p_eComponent )
		, m_renderBuffer( p_renderBuffer )
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
				HRESULT l_hr;

				if ( m_eComponent == eBUFFER_COMPONENT_COLOUR )
				{
					D3D11_TEXTURE2D_DESC l_descTex;
					l_descTex.Width = uiWidth;
					l_descTex.Height = uiHeight;
					l_descTex.MipLevels = 1;
					l_descTex.ArraySize = 1;
					l_descTex.Format = m_eFormat;
					l_descTex.SampleDesc.Count = l_dwMsType;
					l_descTex.SampleDesc.Quality = l_dwMsQuality;
					l_descTex.Usage = D3D11_USAGE_DEFAULT;
					l_descTex.BindFlags = D3D11_BIND_RENDER_TARGET;
					l_descTex.CPUAccessFlags = 0;
					l_descTex.MiscFlags = 0;
					l_hr = m_pRenderSystem->GetDevice()->CreateTexture2D( &l_descTex, NULL, &m_pTexture );
					dxDebugName( m_pRenderSystem, m_pTexture, RTTexture );

					if ( l_hr == S_OK )
					{
						l_hr = m_pRenderSystem->GetDevice()->CreateRenderTargetView( m_pTexture, NULL, reinterpret_cast< ID3D11RenderTargetView ** >( &m_pSurface ) );
						dxDebugName( m_pRenderSystem, m_pSurface, RTView );
					}
				}
				else
				{
					D3D11_TEXTURE2D_DESC l_descDepth;
					l_descDepth.Width = uiWidth;
					l_descDepth.Height = uiHeight;
					l_descDepth.MipLevels = 1;
					l_descDepth.ArraySize = 1;
					l_descDepth.Format = m_eFormat;
					l_descDepth.SampleDesc.Count = 1;
					l_descDepth.SampleDesc.Quality = 0;
					l_descDepth.Usage = D3D11_USAGE_DEFAULT;
					l_descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
					l_descDepth.CPUAccessFlags = 0;
					l_descDepth.MiscFlags = 0;
					l_hr = m_pRenderSystem->GetDevice()->CreateTexture2D( &l_descDepth, NULL, &m_pTexture );
					dxDebugName( m_pRenderSystem, m_pTexture, DSTexture );

					if ( l_hr == S_OK )
					{
						l_hr = m_pRenderSystem->GetDevice()->CreateDepthStencilView( m_pTexture, NULL, reinterpret_cast< ID3D11DepthStencilView ** >( &m_pSurface ) );
						dxDebugName( m_pRenderSystem, m_pSurface, DSView );
					}
				}

				l_bReturn = l_hr == S_OK;
			}
		}

		return l_bReturn;
	}

	void DxRenderBuffer::Cleanup()
	{
		ReleaseTracked( m_pRenderSystem, m_pSurface );
		ReleaseTracked( m_pRenderSystem, m_pTexture );
		SafeRelease( m_pOldSurface );
	}

	bool DxRenderBuffer::Resize( Size const & p_size )
	{
		return Initialise( p_size );
	}
}
