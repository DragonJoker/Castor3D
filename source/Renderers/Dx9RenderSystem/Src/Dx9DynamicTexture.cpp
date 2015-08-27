#include "Dx9DynamicTexture.hpp"
#include "Dx9RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx9Render
{
	DxDynamicTexture::DxDynamicTexture( DxRenderSystem * p_pRenderSystem )
		:	DynamicTexture( p_pRenderSystem )
		,	m_pTextureObject( NULL )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_pSurface( NULL )
		,	m_pLockSurface( NULL )
	{
	}

	DxDynamicTexture::~DxDynamicTexture()
	{
	}

	bool DxDynamicTexture::Create()
	{
		return true;
	}

	void DxDynamicTexture::Destroy()
	{
	}

	void DxDynamicTexture::Cleanup()
	{
		DynamicTexture::Cleanup();
		SafeRelease( m_pLockSurface );
		SafeRelease( m_pSurface );
		SafeRelease( m_pTextureObject );
	}

	uint8_t * DxDynamicTexture::Lock( uint32_t CU_PARAM_UNUSED( p_uiMode ) )
	{
		uint8_t * l_pReturn = NULL;
		D3DLOCKED_RECT l_lockedRect;
		RECT l_rcLock = { 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() };
		HRESULT l_hr = m_pSurface->LockRect( &l_lockedRect, &l_rcLock, 0 );

		if ( l_hr == S_OK )
		{
			l_pReturn = static_cast< uint8_t * >( l_lockedRect.pBits );
		}

		return l_pReturn;
	}

	void DxDynamicTexture::Unlock( bool CU_PARAM_UNUSED( p_bModified ) )
	{
		m_pSurface->UnlockRect();
	}

	bool DxDynamicTexture::DoInitialise()
	{
		Logger::LogDebug( L"Initialising DynamicTexture [0x%08X] with size %dx%d", this, m_pPixelBuffer->dimensions().width(), m_pPixelBuffer->dimensions().height() );
		bool				l_bReturn		= false;
		IDirect3DDevice9 *	l_pDevice		= m_pRenderSystem->GetDevice();
		ePIXEL_FORMAT		l_ePixelFormat	= m_pPixelBuffer->format();

		if ( l_ePixelFormat == ePIXEL_FORMAT_R8G8B8 )
		{
			// f****ing Direct3D that doesn't support RGB24...
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
			l_ePixelFormat = ePIXEL_FORMAT_A8R8G8B8;
		}
		else if ( l_ePixelFormat == ePIXEL_FORMAT_A4R4G4B4 )
		{
			// f****ing Direct3D that doesn't support ARGB16...
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
			l_ePixelFormat = ePIXEL_FORMAT_A8R8G8B8;
		}
		else if ( l_ePixelFormat == ePIXEL_FORMAT_STENCIL1 )
		{
			// f****ing Direct3D that doesn't support STENCIL1...
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
			l_ePixelFormat = ePIXEL_FORMAT_STENCIL8;
		}

		D3DFORMAT	l_d3dFormat		= DirectX9::Get( l_ePixelFormat );
		HRESULT		l_hr			= m_pRenderSystem->GetD3dObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, l_d3dFormat, D3DUSAGE_AUTOGENMIPMAP, D3DRTYPE_TEXTURE, l_d3dFormat );
		bool		l_bAutoMipmaps	= l_hr == S_OK;
		DWORD		l_dwUsage;
		D3DPOOL		l_pool;

		if ( IsRenderTarget() )
		{
			l_dwUsage	= D3DUSAGE_RENDERTARGET;
			l_pool		= D3DPOOL_DEFAULT;

			if ( GetSamplesCount() )
			{
				DWORD				l_dwQualityLevels		= 0;
				D3DMULTISAMPLE_TYPE l_d3dMultisampleType	= D3DMULTISAMPLE_TYPE( GetSamplesCount() );
				l_hr = m_pRenderSystem->GetD3dObject()->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, l_d3dFormat, TRUE, l_d3dMultisampleType, &l_dwQualityLevels );

				if ( l_hr == S_OK && l_dwQualityLevels > 0 )
				{
					l_dwQualityLevels--;
				}
				else
				{
					l_d3dMultisampleType = D3DMULTISAMPLE_NONE;
					l_dwQualityLevels = 0;
				}

				if ( l_ePixelFormat == ePIXEL_FORMAT_DEPTH16 || l_ePixelFormat == ePIXEL_FORMAT_DEPTH24 || l_ePixelFormat == ePIXEL_FORMAT_DEPTH32 || l_ePixelFormat == ePIXEL_FORMAT_STENCIL1 || l_ePixelFormat == ePIXEL_FORMAT_STENCIL8 )
				{
					l_hr = l_pDevice->CreateDepthStencilSurface( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, l_d3dMultisampleType, l_dwQualityLevels, FALSE, &m_pSurface, NULL );

					if ( l_hr != S_OK )
					{
						l_hr = l_pDevice->CreateDepthStencilSurface( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pSurface, NULL );
					}
				}
				else
				{
					l_hr = l_pDevice->CreateRenderTarget( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, l_d3dMultisampleType, l_dwQualityLevels, FALSE, &m_pSurface, NULL );

					if ( l_hr != S_OK )
					{
						l_hr = l_pDevice->CreateRenderTarget( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pSurface, NULL );
					}
				}

				l_bReturn = l_hr == S_OK;
			}
			else
			{
				l_hr = S_OK;

				if ( l_hr == S_OK )
				{
					l_hr = l_pDevice->CreateTexture( m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_dwUsage, l_d3dFormat, l_pool, &m_pTextureObject, NULL );
				}

				if ( l_hr == S_OK )
				{
					l_hr = m_pTextureObject->GetSurfaceLevel( 0, &m_pSurface );
				}

				l_bReturn = l_hr == S_OK;
			}
		}
		else
		{
			if ( l_bAutoMipmaps )
			{
				l_dwUsage	= D3DUSAGE_AUTOGENMIPMAP;
				l_pool		= D3DPOOL_MANAGED;
			}
			else
			{
				l_dwUsage	= D3DUSAGE_DYNAMIC;
				l_pool		= D3DPOOL_DEFAULT;
			}

			l_hr = S_OK;

			if ( l_hr == S_OK )
			{
				l_hr = l_pDevice->CreateTexture( m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_dwUsage, l_d3dFormat, l_pool, &m_pTextureObject, NULL );
			}

			if ( l_hr == S_OK )
			{
				l_hr = m_pTextureObject->GetSurfaceLevel( 0, &m_pSurface );
			}

			if ( l_hr == S_OK )
			{
				uint8_t * l_pDestPix = Lock( 0 );

				if ( l_pDestPix )
				{
					uint8_t const * l_pSrcPix = m_pPixelBuffer->const_ptr();
					uint32_t l_uiBpp = Castor::PF::GetBytesPerPixel( m_pPixelBuffer->format() );
					std::copy( l_pSrcPix, l_pSrcPix + m_pPixelBuffer->width() * l_uiBpp * m_pPixelBuffer->height(), l_pDestPix );
					Unlock( true );
				}
			}

			l_bReturn = l_hr == S_OK;
		}

		CASTOR_ASSERT( l_bReturn );
		return l_bReturn;
	}

	bool DxDynamicTexture::DoBind()
	{
		HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, m_pTextureObject );
		dxCheckError( l_hr, "DxDynamicTexture::Bind" );
		return l_hr == S_OK;
	}

	void DxDynamicTexture::DoUnbind()
	{
		HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, NULL );
		dxCheckError( l_hr, "DxDynamicTexture::Unbind" );
	}
}

#pragma warning( pop)
