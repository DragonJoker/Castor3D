#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/DxTextureRenderer.hpp"
#include "Dx9RenderSystem/DxRenderSystem.hpp"
#include <CastorUtils/Pixel.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace Dx9Render;

#pragma warning( push)
#pragma warning( disable:4996)


Castor3D::SamplerSPtr DxSamplerRenderer :: sm_pCurrent = nullptr;

//*************************************************************************************************

DxSamplerRenderer :: DxSamplerRenderer( DxRenderSystem * p_pRenderSystem )
	:	SamplerRenderer		( p_pRenderSystem	)
	,	m_pDxRenderSystem	( p_pRenderSystem	)
{
}

DxSamplerRenderer :: ~DxSamplerRenderer()
{
}

bool DxSamplerRenderer :: Initialise()
{
	if( !sm_pCurrent )
	{
		sm_pCurrent = std::make_shared< Sampler >( m_pRenderSystem->GetEngine() );
	}
	return true;
}

void DxSamplerRenderer :: Cleanup()
{
}

bool DxSamplerRenderer :: Bind( eTEXTURE_DIMENSION CU_PARAM_UNUSED( p_eDimension ), uint32_t p_uiIndex )
{
	IDirect3DDevice9 *	l_pDevice		= m_pDxRenderSystem->GetDevice();
	D3DCOLOR l_d3dColour = m_target->GetBorderColour().to_argb();

	if( sm_pCurrent->GetWrappingMode( eTEXTURE_UVW_U ) != m_target->GetWrappingMode( eTEXTURE_UVW_U ) )
	{
		sm_pCurrent->SetWrappingMode( eTEXTURE_UVW_U, m_target->GetWrappingMode( eTEXTURE_UVW_U ) );
		l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_ADDRESSU, DirectX9::Get( m_target->GetWrappingMode( eTEXTURE_UVW_U ) ) );
	}
	
	if( sm_pCurrent->GetWrappingMode( eTEXTURE_UVW_V ) != m_target->GetWrappingMode( eTEXTURE_UVW_V ) )
	{
		sm_pCurrent->SetWrappingMode( eTEXTURE_UVW_U, m_target->GetWrappingMode( eTEXTURE_UVW_V ) );
		l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_ADDRESSV, DirectX9::Get( m_target->GetWrappingMode( eTEXTURE_UVW_V ) ) );
	}
	
	if( sm_pCurrent->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) != m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) )
	{
		sm_pCurrent->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) );
		l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_MINFILTER, DirectX9::Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN ) ) );
	}
	
	if( sm_pCurrent->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) != m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) )
	{
		sm_pCurrent->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) );
		l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_MAGFILTER, DirectX9::Get( m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG ) ) );
	}
	
	if( sm_pCurrent->GetBorderColour() != m_target->GetBorderColour() )
	{
		sm_pCurrent->SetBorderColour( m_target->GetBorderColour() );
		l_pDevice->SetSamplerState( p_uiIndex,	D3DSAMP_BORDERCOLOR, l_d3dColour );
	}

	return true;
}

void DxSamplerRenderer :: Unbind()
{
}

//*************************************************************************************************

DxStaticTexture :: DxStaticTexture( DxRenderSystem * p_pRenderSystem )
	:	StaticTexture		( p_pRenderSystem	)
	,	m_pTextureObject	( NULL				)
	,	m_pRenderSystem		( p_pRenderSystem	)
{
}

DxStaticTexture :: ~DxStaticTexture()
{
}

bool DxStaticTexture :: Create()
{
	return true;
}

void DxStaticTexture :: Destroy()
{
}

void DxStaticTexture :: Cleanup()
{
	StaticTexture::Cleanup();
	SafeRelease( m_pTextureObject );
}

uint8_t * DxStaticTexture :: Lock( uint32_t CU_PARAM_UNUSED( p_uiMode ) )
{
	uint8_t * l_pReturn = NULL;
	
	D3DLOCKED_RECT l_lockedRect;
	RECT l_rcLock = { 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() };

	if( m_pTextureObject->LockRect( 0, &l_lockedRect, &l_rcLock, 0 ) == S_OK )
	{
		l_pReturn = static_cast< uint8_t* >( l_lockedRect.pBits );
	}

	return l_pReturn;
}

void DxStaticTexture :: Unlock( bool CU_PARAM_UNUSED( p_bModified ) )
{
	m_pTextureObject->UnlockRect( 0 );
}

bool DxStaticTexture :: DoInitialise()
{
	bool				l_bReturn		= false;
	IDirect3DDevice9 *	l_pDevice		= m_pRenderSystem->GetDevice();
	ePIXEL_FORMAT		l_ePixelFormat	= m_pPixelBuffer->format();

	if( l_ePixelFormat == ePIXEL_FORMAT_R8G8B8 )
	{
		// f****ing Direct3D that doesn't support RGB24...
		m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
		l_ePixelFormat = ePIXEL_FORMAT_A8R8G8B8;
	}
	else if( l_ePixelFormat == ePIXEL_FORMAT_A4R4G4B4 )
	{
		m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
		l_ePixelFormat = ePIXEL_FORMAT_A8R8G8B8;
	}
	else if( l_ePixelFormat == ePIXEL_FORMAT_STENCIL1 )
	{
		m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
		l_ePixelFormat = ePIXEL_FORMAT_STENCIL8;
	}

	D3DFORMAT	l_d3dFormat		= DirectX9::Get( l_ePixelFormat );
	HRESULT		l_hr			= m_pRenderSystem->GetD3dObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, l_d3dFormat, D3DUSAGE_AUTOGENMIPMAP, D3DRTYPE_TEXTURE, l_d3dFormat );
	bool		l_bAutoMipmaps	= l_hr == S_OK;
	DWORD		l_dwUsage		= l_bAutoMipmaps ? D3DUSAGE_AUTOGENMIPMAP : 0;

	if( l_pDevice->CreateTexture( m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_dwUsage, l_d3dFormat, D3DPOOL_MANAGED, &m_pTextureObject, NULL ) == S_OK )
	{
		D3DLOCKED_RECT l_lockedRect;
		RECT l_rcLock = { 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() };

		if( m_pTextureObject->LockRect( 0, &l_lockedRect, &l_rcLock, 0 ) == S_OK )
		{
			uint8_t * l_pDestPix = reinterpret_cast<uint8_t *>( l_lockedRect.pBits);
			uint8_t const * l_pSrcPix = m_pPixelBuffer->const_ptr();
			uint32_t l_uiBpp = Castor::PF::GetBytesPerPixel( m_pPixelBuffer->format() );
			std::copy( l_pSrcPix, l_pSrcPix + m_pPixelBuffer->width() * l_uiBpp * m_pPixelBuffer->height(), l_pDestPix);
			m_pTextureObject->UnlockRect( 0 );
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

bool DxStaticTexture :: DoBind()
{
	HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, m_pTextureObject );
	dxCheckError( l_hr, "DxStaticTexture::Bind" );
	return l_hr == S_OK;
}

void DxStaticTexture :: DoUnbind()
{
	HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, NULL );
	dxCheckError( l_hr, "DxStaticTexture::Unbind" );
}

//*************************************************************************************************

DxDynamicTexture :: DxDynamicTexture( DxRenderSystem * p_pRenderSystem )
	:	DynamicTexture		( p_pRenderSystem	)
	,	m_pTextureObject	( NULL				)
	,	m_pRenderSystem		( p_pRenderSystem	)
	,	m_pSurface			( NULL				)
	,	m_pLockSurface		( NULL				)
{
}

DxDynamicTexture :: ~DxDynamicTexture()
{
}

bool DxDynamicTexture :: Create()
{
	return true;
}

void DxDynamicTexture :: Destroy()
{
}

void DxDynamicTexture :: Cleanup()
{
	DynamicTexture::Cleanup();
	SafeRelease( m_pLockSurface );
	SafeRelease( m_pSurface );
	SafeRelease( m_pTextureObject );
}

uint8_t * DxDynamicTexture :: Lock( uint32_t CU_PARAM_UNUSED( p_uiMode ) )
{
	uint8_t * l_pReturn = NULL;
	D3DLOCKED_RECT l_lockedRect;
	RECT l_rcLock = { 0, 0, m_pPixelBuffer->width(), m_pPixelBuffer->height() };
	
	HRESULT l_hr = m_pSurface->LockRect( &l_lockedRect, &l_rcLock, 0 );

	if( l_hr == S_OK )
	{
		l_pReturn = static_cast< uint8_t* >( l_lockedRect.pBits );
	}

	return l_pReturn;
}

void DxDynamicTexture :: Unlock( bool CU_PARAM_UNUSED( p_bModified ) )
{
	m_pSurface->UnlockRect();
}

bool DxDynamicTexture :: DoInitialise()
{
	bool				l_bReturn		= false;
	IDirect3DDevice9 *	l_pDevice		= m_pRenderSystem->GetDevice();
	ePIXEL_FORMAT		l_ePixelFormat	= m_pPixelBuffer->format();

	if( l_ePixelFormat == ePIXEL_FORMAT_R8G8B8 )
	{
		// f****ing Direct3D that doesn't support RGB24...
		m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
		l_ePixelFormat = ePIXEL_FORMAT_A8R8G8B8;
	}
	else if( l_ePixelFormat == ePIXEL_FORMAT_A4R4G4B4 )
	{
		m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
		l_ePixelFormat = ePIXEL_FORMAT_A8R8G8B8;
	}
	else if( l_ePixelFormat == ePIXEL_FORMAT_STENCIL1 )
	{
		m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, m_pPixelBuffer->const_ptr(), l_ePixelFormat );
		l_ePixelFormat = ePIXEL_FORMAT_STENCIL8;
	}

	D3DFORMAT	l_d3dFormat		= DirectX9::Get( l_ePixelFormat );
	HRESULT		l_hr			= m_pRenderSystem->GetD3dObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, l_d3dFormat, D3DUSAGE_AUTOGENMIPMAP, D3DRTYPE_TEXTURE, l_d3dFormat );
	bool		l_bAutoMipmaps	= l_hr == S_OK;
	DWORD		l_dwUsage;
	D3DPOOL		l_pool;

	if( IsRenderTarget() )
	{
		l_dwUsage	= D3DUSAGE_RENDERTARGET;
		l_pool		= D3DPOOL_DEFAULT;

		if( GetSamplesCount() )
		{
			DWORD				l_dwQualityLevels		= 0;
			D3DMULTISAMPLE_TYPE l_d3dMultisampleType	= D3DMULTISAMPLE_TYPE( GetSamplesCount() );

			l_hr = m_pRenderSystem->GetD3dObject()->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, l_d3dFormat, TRUE, l_d3dMultisampleType, &l_dwQualityLevels );

			if( l_hr == S_OK && l_dwQualityLevels > 0 )
			{
				l_dwQualityLevels--;
			}
			else
			{
				l_d3dMultisampleType = D3DMULTISAMPLE_NONE;
				l_dwQualityLevels = 0;
			}

			if( l_ePixelFormat == ePIXEL_FORMAT_DEPTH16 || l_ePixelFormat == ePIXEL_FORMAT_DEPTH24 || l_ePixelFormat == ePIXEL_FORMAT_DEPTH32 || l_ePixelFormat == ePIXEL_FORMAT_STENCIL1 || l_ePixelFormat == ePIXEL_FORMAT_STENCIL8 )
			{
				l_hr = l_pDevice->CreateDepthStencilSurface( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, l_d3dMultisampleType, l_dwQualityLevels, FALSE, &m_pSurface, NULL );
				
				if( l_hr != S_OK )
				{
					l_hr = l_pDevice->CreateDepthStencilSurface( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pSurface, NULL );
				}
			}
			else
			{

				l_hr = l_pDevice->CreateRenderTarget( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, l_d3dMultisampleType, l_dwQualityLevels, FALSE, &m_pSurface, NULL );

				if( l_hr != S_OK )
				{
					l_hr = l_pDevice->CreateRenderTarget( m_pPixelBuffer->width(), m_pPixelBuffer->height(), l_d3dFormat, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pSurface, NULL );
				}
			}

			l_bReturn = l_hr == S_OK;
		}
		else
		{
			l_hr = S_OK;
			
			if( l_hr == S_OK )
			{
				l_hr = l_pDevice->CreateTexture( m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_dwUsage, l_d3dFormat, l_pool, &m_pTextureObject, NULL );
			}

			if( l_hr == S_OK )
			{
				l_hr = m_pTextureObject->GetSurfaceLevel( 0, &m_pSurface );
			}

			l_bReturn = l_hr == S_OK;
		}
	}
	else
	{
		if( l_bAutoMipmaps )
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
		
		if( l_hr == S_OK )
		{
			l_hr = l_pDevice->CreateTexture( m_pPixelBuffer->width(), m_pPixelBuffer->height(), 0, l_dwUsage, l_d3dFormat, l_pool, &m_pTextureObject, NULL );
		}

		if( l_hr == S_OK )
		{
			l_hr = m_pTextureObject->GetSurfaceLevel( 0, &m_pSurface );
		}

		if( l_hr == S_OK )
		{
			uint8_t * l_pDestPix = Lock( 0 );

			if( l_pDestPix )
			{
				uint8_t const * l_pSrcPix = m_pPixelBuffer->const_ptr();
				uint32_t l_uiBpp = Castor::PF::GetBytesPerPixel( m_pPixelBuffer->format() );
				std::copy( l_pSrcPix, l_pSrcPix + m_pPixelBuffer->width() * l_uiBpp * m_pPixelBuffer->height(), l_pDestPix);
				Unlock( true );
			}
		}

		l_bReturn = l_hr == S_OK;
	}

	CASTOR_ASSERT( l_bReturn );
	return l_bReturn;
}

bool DxDynamicTexture :: DoBind()
{
	HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, m_pTextureObject );
	dxCheckError( l_hr, "DxDynamicTexture::Bind" );
	return l_hr == S_OK;
}

void DxDynamicTexture :: DoUnbind()
{
	HRESULT l_hr = m_pRenderSystem->GetDevice()->SetTexture( m_uiIndex, NULL );
	dxCheckError( l_hr, "DxDynamicTexture::Unbind" );
}

//*************************************************************************************************

DxTextureRenderer :: DxTextureRenderer( DxRenderSystem * p_pRenderSystem )
	:	TextureRenderer		( p_pRenderSystem	)
{
}

DxTextureRenderer :: ~DxTextureRenderer()
{
}

bool DxTextureRenderer :: Render()
{
	DWORD l_dwSampler = m_target->GetIndex();
	IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();

	if( m_target->GetRgbFunction() != eRGB_BLEND_FUNC_NONE )
	{
		l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_COLOROP,   DirectX9::Get( m_target->GetRgbFunction() ) );
		l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_COLORARG1, DirectX9::Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_0 ) ) );
		l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_COLORARG2, DirectX9::Get( m_target->GetRgbArgument( eBLEND_SRC_INDEX_1 ) ) );
	}

	if( m_target->GetAlpFunction() != eALPHA_BLEND_FUNC_NONE )
	{
		l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_ALPHAOP,   DirectX9::Get( m_target->GetAlpFunction() ) );
		l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_ALPHAARG1, DirectX9::Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_0 ) ) );
		l_pDevice->SetTextureStageState( l_dwSampler, D3DTSS_ALPHAARG2, DirectX9::Get( m_target->GetAlpArgument( eBLEND_SRC_INDEX_1 ) ) );
	}

	if( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
	{
		l_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, true );
		l_pDevice->SetRenderState( D3DRS_ALPHAFUNC, DirectX9::Get( m_target->GetAlphaFunc() ) );
		l_pDevice->SetRenderState( D3DRS_ALPHAREF, DWORD( m_target->GetAlphaValue() * 255) );
	}

	return true;
}

void DxTextureRenderer :: EndRender()
{
	IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();

	if( m_target->GetAlphaFunc() != eALPHA_FUNC_ALWAYS )
	{
		l_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, false );
	}
}

//*************************************************************************************************
#pragma warning( pop)
