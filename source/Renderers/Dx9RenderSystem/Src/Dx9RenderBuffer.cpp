#include "Dx9RenderBuffer.hpp"
#include "Dx9RenderSystem.hpp"

#include <RenderBuffer.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
	DxRenderBuffer::DxRenderBuffer( DxRenderSystem * p_pRenderSystem, D3DFORMAT p_eFormat, eBUFFER_COMPONENT p_eComponent, RenderBuffer & p_renderBuffer )
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

#if DX_DEBUG
		l_bReturn = true;
#else
		if ( !l_bReturn )
		{
			m_size = p_size;
			Logger::LogDebug( L"Initialising RenderBuffer [0x%08X] with size %dx%d", this, p_size.width(), p_size.height() );

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
				D3DMULTISAMPLE_TYPE l_eMsType		= D3DMULTISAMPLE_NONE;
				DWORD				l_dwMsQuality	= 0;
				BOOL				l_bLockable		= TRUE;

				if ( m_renderBuffer.GetSamplesCount() > 1 )
				{
					l_eMsType = D3DMULTISAMPLE_TYPE( m_renderBuffer.GetSamplesCount() );

					if ( m_pRenderSystem->GetD3dObject()->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_eFormat, TRUE, l_eMsType, &l_dwMsQuality ) == S_OK )
					{
						if ( l_dwMsQuality > 0 )
						{
							l_dwMsQuality--;
						}

						l_bLockable = FALSE;
					}
					else
					{
						l_eMsType		= D3DMULTISAMPLE_NONE;
						l_dwMsQuality	= 0;
					}
				}

				UINT uiWidth = p_size.width();
				UINT uiHeight = p_size.height();

				if ( m_eComponent == eBUFFER_COMPONENT_COLOUR )
				{
					l_bReturn = m_pDevice->CreateRenderTarget( uiWidth, uiHeight, m_eFormat, l_eMsType, l_dwMsQuality, l_bLockable, &m_pSurface, NULL ) == S_OK;
				}
				else
				{
					l_bReturn = m_pDevice->CreateDepthStencilSurface( uiWidth, uiHeight, m_eFormat, l_eMsType, l_dwMsQuality, FALSE, &m_pSurface, NULL ) == S_OK;
				}
			}
		}

#endif
		return l_bReturn;
	}

	void DxRenderBuffer::Cleanup()
	{
		SafeRelease( m_pSurface );
		SafeRelease( m_pOldSurface );
	}

	bool DxRenderBuffer::Bind( DWORD dwIndex )
	{
		HRESULT l_hr = S_FALSE;

#if DX_DEBUG
		l_hr = S_OK;
#else

		if ( m_pSurface )
		{
			if ( m_eComponent == eBUFFER_COMPONENT_COLOUR )
			{
				l_hr = m_pDevice->GetRenderTarget( dwIndex, &m_pOldSurface );
				l_hr = l_hr == S_OK ? m_pDevice->SetRenderTarget( dwIndex, m_pSurface ) : l_hr;
			}
			else
			{
				l_hr = m_pDevice->GetDepthStencilSurface( &m_pOldSurface );
				l_hr = l_hr == S_OK ? m_pDevice->SetDepthStencilSurface( m_pSurface ) : l_hr;
			}
		}

#endif

		return l_hr == S_OK;
	}

	void DxRenderBuffer::Unbind( DWORD dwIndex )
	{
#if DX_DEBUG
#else

		if ( m_pOldSurface )
		{
			if ( m_eComponent == eBUFFER_COMPONENT_COLOUR )
			{
				m_pDevice->SetRenderTarget( dwIndex, m_pOldSurface );
			}
			else
			{
				m_pDevice->SetDepthStencilSurface( m_pOldSurface );
			}

			SafeRelease( m_pOldSurface );
		}

#endif
	}

	bool DxRenderBuffer::Resize( Size const & p_size )
	{
		return Initialise( p_size );
	}
}
