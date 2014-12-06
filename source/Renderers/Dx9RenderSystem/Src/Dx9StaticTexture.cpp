#include "Dx9StaticTexture.hpp"
#include "Dx9RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

#pragma warning( push )
#pragma warning( disable:4996 )

namespace Dx9Render
{
	DxStaticTexture::DxStaticTexture( DxRenderSystem * p_pRenderSystem )
		:	StaticTexture( p_pRenderSystem )
		,	m_pTextureObject( NULL )
		,	m_pRenderSystem( p_pRenderSystem )
	{
	}

	DxStaticTexture::~DxStaticTexture()
	{
	}

	bool DxStaticTexture::Create()
	{
		return true;
	}

	void DxStaticTexture::Destroy()
	{
	}

	void DxStaticTexture::Cleanup()
	{
		StaticTexture::Cleanup();
		SafeRelease( m_pTextureObject );
	}

	uint8_t * DxStaticTexture::Lock( uint32_t CU_PARAM_UNUSED( p_uiMode ) )
	{
		uint8_t * l_pReturn = NULL;
		D3DLOCKED_RECT l_lockedRect;
		RECT l_rcLock = { 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() };

		if ( m_pTextureObject->LockRect( 0, &l_lockedRect, &l_rcLock, 0 ) == S_OK )
		{
			l_pReturn = static_cast< uint8_t * >( l_lockedRect.pBits );
		}

		return l_pReturn;
	}

	void DxStaticTexture::Unlock( bool CU_PARAM_UNUSED( p_bModified ) )
	{
		m_pTextureObject->UnlockRect( 0 );
	}

	bool DxStaticTexture::DoInitialise()
	{
		Logger::LogDebug( L"Initialising StaticTexture [0x%08X] with size %dx%d", this, m_pPixelBuffer->dimensions().width(), m_pPixelBuffer->dimensions().height() );
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
		DWORD		l_dwUsage		= l_bAutoMipmaps ? D3DUSAGE_AUTOGENMIPMAP : 0;

		if ( l_pDevice->CreateTexture( m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_dwUsage, l_d3dFormat, D3DPOOL_MANAGED, &m_pTextureObject, NULL ) == S_OK )
		{
			D3DLOCKED_RECT l_lockedRect;
			RECT l_rcLock = { 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() };

			if ( m_pTextureObject->LockRect( 0, &l_lockedRect, &l_rcLock, 0 ) == S_OK )
			{
				uint8_t * l_pDestPix = reinterpret_cast<uint8_t *>( l_lockedRect.pBits );
				uint8_t const * l_pSrcPix = m_pPixelBuffer->const_ptr();
				uint32_t l_uiBpp = Castor::PF::GetBytesPerPixel( m_pPixelBuffer->format() );
				std::copy( l_pSrcPix, l_pSrcPix + m_pPixelBuffer->width() * l_uiBpp * m_pPixelBuffer->height(), l_pDestPix );
				m_pTextureObject->UnlockRect( 0 );
				l_bReturn = true;
			}
		}

		return l_bReturn;
	}

	bool DxStaticTexture::DoBind()
	{
		HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, m_pTextureObject );
		dxCheckError( l_hr, "DxStaticTexture::Bind" );
		return l_hr == S_OK;
	}

	void DxStaticTexture::DoUnbind()
	{
		HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, NULL );
		dxCheckError( l_hr, "DxStaticTexture::Unbind" );
	}
}

#pragma warning( pop )
