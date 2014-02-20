#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxTextureRenderer.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"
#include <CastorUtils/Pixel.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace Dx11Render;

#pragma warning( push)
#pragma warning( disable:4996)

//*************************************************************************************************

DxSamplerRenderer :: DxSamplerRenderer( DxRenderSystem * p_pRenderSystem )
	:	SamplerRenderer		( p_pRenderSystem	)
	,	m_pDxRenderSystem	( p_pRenderSystem	)
	,	m_pSamplerState		( NULL				)
{
}

DxSamplerRenderer :: ~DxSamplerRenderer()
{
}

bool DxSamplerRenderer :: Initialise()
{
	HRESULT l_hr = S_OK;

	if( !m_pSamplerState )
	{
		ID3D11Device *			l_pDevice		= m_pDxRenderSystem->GetDevice();
 		m_tex2dSampler.AddressU					= DirectX11::Get( m_target->GetWrappingMode( eTEXTURE_UVW_U ) );
 		m_tex2dSampler.AddressV					= DirectX11::Get( m_target->GetWrappingMode( eTEXTURE_UVW_V ) );
 		m_tex2dSampler.AddressW					= DirectX11::Get( m_target->GetWrappingMode( eTEXTURE_UVW_W ) );
 		m_tex2dSampler.BorderColor[0]			= 0;
 		m_tex2dSampler.BorderColor[1]			= 0;
 		m_tex2dSampler.BorderColor[2]			= 0;
 		m_tex2dSampler.BorderColor[3]			= 1;
 		m_tex2dSampler.ComparisonFunc			= D3D11_COMPARISON_ALWAYS;
 		m_tex2dSampler.MaxAnisotropy			= 0;
 		m_tex2dSampler.MaxLOD					= 6;
 		m_tex2dSampler.MinLOD					= 0;
 		m_tex2dSampler.MipLODBias				= 2;
		eINTERPOLATION_MODE l_eMinMode			= m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIN );
		eINTERPOLATION_MODE l_eMagMode			= m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MAG );
		eINTERPOLATION_MODE l_eMipMode			= m_target->GetInterpolationMode( eINTERPOLATION_FILTER_MIP );
 
		if( l_eMinMode == l_eMipMode && l_eMipMode == l_eMagMode )
		{
			if( l_eMinMode == eINTERPOLATION_MODE_NEAREST )
			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
			}
			else if( l_eMinMode == eINTERPOLATION_MODE_LINEAR )
			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
			else
			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_ANISOTROPIC;
				m_tex2dSampler.MaxAnisotropy	= UINT( m_target->GetMaxAnisotropy() );
			}
		}
 		else if( l_eMinMode == eINTERPOLATION_MODE_NEAREST )
 		{
 			if( l_eMagMode == eINTERPOLATION_MODE_NEAREST )
 			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
 			}
 			else if( l_eMagMode == eINTERPOLATION_MODE_LINEAR )
 			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
 			}
 			else
 			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_ANISOTROPIC;
				m_tex2dSampler.MaxAnisotropy	= UINT( m_target->GetMaxAnisotropy() );
 			}
 		}
 		else if( l_eMinMode == eINTERPOLATION_MODE_LINEAR )
 		{
 			if( l_eMagMode == eINTERPOLATION_MODE_NEAREST )
 			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
 			}
 			else if( l_eMagMode == eINTERPOLATION_MODE_LINEAR )
 			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
 			}
 			else
 			{
 				m_tex2dSampler.Filter			= D3D11_FILTER_ANISOTROPIC;
 				m_tex2dSampler.MaxAnisotropy	= UINT( m_target->GetMaxAnisotropy() );
 			}
 		}
 		else
 		{
 			m_tex2dSampler.Filter				= D3D11_FILTER_ANISOTROPIC;
 			m_tex2dSampler.MaxAnisotropy		= UINT( m_target->GetMaxAnisotropy() );
		}

		l_hr = l_pDevice->CreateSamplerState( &m_tex2dSampler, &m_pSamplerState );
		dxDebugName( m_pSamplerState, SamplerState );
	}

	return l_hr == S_OK;
}

void DxSamplerRenderer :: Cleanup()
{
	SafeRelease( m_pSamplerState );
}

bool DxSamplerRenderer :: Bind( eTEXTURE_DIMENSION CU_PARAM_UNUSED( p_eDimension ), uint32_t p_uiIndex )
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pDxRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->PSSetSamplers( p_uiIndex, 1, &m_pSamplerState );
	l_pDeviceContext->Release();
	return true;
}

void DxSamplerRenderer :: Unbind()
{
}

//*************************************************************************************************

DxStaticTexture :: DxStaticTexture( DxRenderSystem * p_pRenderSystem )
	:	StaticTexture			( p_pRenderSystem	)
	,	m_pShaderResourceView	( NULL				)
	,	m_pRenderSystem			( p_pRenderSystem	)
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
	SafeRelease( m_pShaderResourceView );
}

uint8_t * DxStaticTexture :: Lock( uint32_t p_uiMode )
{
	uint8_t * l_pReturn = NULL;
	D3D11_MAPPED_SUBRESOURCE l_mappedResource;
	ID3D11Resource * l_pResource;
	m_pShaderResourceView->GetResource( &l_pResource );
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	HRESULT l_hr = l_pDeviceContext->Map( l_pResource, 0, D3D11_MAP( DirectX11::GetLockFlags( p_uiMode ) ), 0, &l_mappedResource );
	l_pDeviceContext->Release();
	l_pResource->Release();

	if( l_hr == S_OK )
	{
		l_pReturn = static_cast< uint8_t* >( l_mappedResource.pData );
	}

	return l_pReturn;
}

void DxStaticTexture :: Unlock( bool CU_PARAM_UNUSED( p_bModified ) )
{
	ID3D11Resource * l_pResource;
	m_pShaderResourceView->GetResource( &l_pResource );
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->Unmap( l_pResource, 0 );
	l_pDeviceContext->Release();
	l_pResource->Release();
}

bool DxStaticTexture :: DoInitialise()
{
	ID3D11Device *			l_pDevice		= m_pRenderSystem->GetDevice();
	D3D11_TEXTURE2D_DESC	l_tex2dDesc		= { 0 };
	D3D11_SUBRESOURCE_DATA	l_tex2dData		= { 0 };
	HRESULT					l_hr			= S_OK;
	ID3D11Texture2D *		l_pTexture		= NULL;

	DoInitTex2DDesc( l_tex2dDesc );
	DoInitTex2DData( l_tex2dData );

	if( l_hr == S_OK )
	{
		l_hr = l_pDevice->CreateTexture2D( &l_tex2dDesc, &l_tex2dData, &l_pTexture );
	}

	if( l_hr == S_OK )
	{
		D3D10_SHADER_RESOURCE_VIEW_DESC l_desc;
		l_desc.Format						= DirectX11::Get( m_pPixelBuffer->format() );
		l_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		l_desc.Texture2D.MipLevels			= 2;
		l_desc.Texture2D.MostDetailedMip	= 0;
		l_hr = l_pDevice->CreateShaderResourceView( l_pTexture, NULL, &m_pShaderResourceView );
		dxDebugName( m_pShaderResourceView, StaticSRView );
	}

	SafeRelease( l_pTexture );
	CASTOR_ASSERT( l_hr == S_OK || m_pPixelBuffer->width() == 0 );
	return l_hr == S_OK;
}

bool DxStaticTexture :: DoBind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->PSSetShaderResources( m_uiIndex, 1, &m_pShaderResourceView );
	l_pDeviceContext->Release();
	return true;
}

void DxStaticTexture :: DoUnbind()
{
}

void DxStaticTexture :: DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_tex2dDesc )
{
	p_tex2dDesc.Width				= m_pPixelBuffer->width();
	p_tex2dDesc.Height				= m_pPixelBuffer->height();
	p_tex2dDesc.Format				= DirectX11::Get( m_pPixelBuffer->format() );
	p_tex2dDesc.ArraySize			= 1;
	p_tex2dDesc.Usage				= D3D11_USAGE_IMMUTABLE;
	p_tex2dDesc.CPUAccessFlags		= 0;
	p_tex2dDesc.MipLevels			= 1;
	p_tex2dDesc.SampleDesc.Count	= 1;
	p_tex2dDesc.SampleDesc.Quality	= 0;
	p_tex2dDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;

	if( p_tex2dDesc.Format == DXGI_FORMAT_UNKNOWN )
	{
		if( m_pPixelBuffer->format() == ePIXEL_FORMAT_R8G8B8 )
		{
			// f****ing Direct3D that doesn't support RGB24...
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
			p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
		}
		else if( m_pPixelBuffer->format() == ePIXEL_FORMAT_A4R4G4B4 )
		{
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
			p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
		}
		else if( m_pPixelBuffer->format() == ePIXEL_FORMAT_STENCIL1 )
		{
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
			p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
		}
	}
}

void DxStaticTexture :: DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_tex2dData )
{
	p_tex2dData.pSysMem			= m_pPixelBuffer->const_ptr();
	p_tex2dData.SysMemPitch		= UINT( m_pPixelBuffer->size() / m_pPixelBuffer->height() );
}

//*************************************************************************************************

DxDynamicTexture :: DxDynamicTexture( DxRenderSystem * p_pRenderSystem )
	:	DynamicTexture			( p_pRenderSystem	)
	,	m_pRenderTargetView		( NULL				)
	,	m_pShaderResourceView	( NULL				)
	,	m_pRenderSystem			( p_pRenderSystem	)
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
	SafeRelease( m_pRenderTargetView );
	SafeRelease( m_pShaderResourceView );
}

uint8_t * DxDynamicTexture :: Lock( uint32_t p_uiMode )
{
	uint8_t * l_pReturn = NULL;
	D3D11_MAPPED_SUBRESOURCE l_mappedResource;
	ID3D11Resource * l_pResource;
	m_pShaderResourceView->GetResource( &l_pResource );
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	HRESULT l_hr = l_pDeviceContext->Map( l_pResource, 0, D3D11_MAP( DirectX11::GetLockFlags( p_uiMode ) ), 0, &l_mappedResource );
	l_pDeviceContext->Release();
	l_pResource->Release();

	if( l_hr == S_OK )
	{
		l_pReturn = static_cast< uint8_t* >( l_mappedResource.pData );
	}

	return l_pReturn;
}

void DxDynamicTexture :: Unlock( bool CU_PARAM_UNUSED( p_bModified ) )
{
	ID3D11Resource * l_pResource;
	m_pShaderResourceView->GetResource( &l_pResource );
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->Unmap( l_pResource, 0 );
	l_pDeviceContext->Release();
	l_pResource->Release();
}

bool DxDynamicTexture :: DoInitialise()
{
	ID3D11Device *			l_pDevice		= m_pRenderSystem->GetDevice();
	D3D11_TEXTURE2D_DESC	l_tex2dDesc		= { 0 };
	D3D11_SUBRESOURCE_DATA	l_tex2dData		= { 0 };
	ID3D11Texture2D *		l_pTexture		= NULL;

	DoInitTex2DDesc( l_tex2dDesc );
	DoInitTex2DData( l_tex2dData );

	HRESULT l_hr = l_pDevice->CreateTexture2D( &l_tex2dDesc, &l_tex2dData, &l_pTexture );

	if( l_hr == S_OK )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC l_desc;
		l_desc.Format						= DirectX11::Get( m_pPixelBuffer->format() );
		l_desc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		l_desc.Texture2D.MipLevels			= 2;
		l_desc.Texture2D.MostDetailedMip	= 0;

		if( IsRenderTarget() )
		{
			l_hr = l_pDevice->CreateRenderTargetView( l_pTexture, NULL, &m_pRenderTargetView );
			dxDebugName( m_pRenderTargetView, DynamicRTView );
		}
		
		l_hr = l_pDevice->CreateShaderResourceView( l_pTexture, NULL, &m_pShaderResourceView );
		dxDebugName( m_pShaderResourceView, DynamicSRView );
	}

	SafeRelease( l_pTexture );
	CASTOR_ASSERT( l_hr == S_OK || m_pPixelBuffer->width() == 0 );
	return l_hr == S_OK;
}

bool DxDynamicTexture :: DoBind()
{
	ID3D11DeviceContext * l_pDeviceContext;
	m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDeviceContext );
	l_pDeviceContext->PSSetShaderResources( m_uiIndex, 1, &m_pShaderResourceView );
	l_pDeviceContext->Release();
	return true;
}

void DxDynamicTexture :: DoUnbind()
{
}

void DxDynamicTexture :: DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_tex2dDesc )
{
	ePIXEL_FORMAT	l_ePixelFormat	= m_pPixelBuffer->format();
	ID3D11Device *	l_pDevice		= m_pRenderSystem->GetDevice();

	p_tex2dDesc.Width				= m_pPixelBuffer->width();
	p_tex2dDesc.Height				= m_pPixelBuffer->height();
	p_tex2dDesc.Format				= DirectX11::Get( l_ePixelFormat );
	p_tex2dDesc.ArraySize			= 1;
	p_tex2dDesc.Usage				= D3D11_USAGE_DYNAMIC;
	p_tex2dDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	p_tex2dDesc.MipLevels			= 1;
	p_tex2dDesc.SampleDesc.Count	= 1;
	p_tex2dDesc.SampleDesc.Quality	= 0;
	p_tex2dDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;

	if( p_tex2dDesc.Format == DXGI_FORMAT_UNKNOWN )
	{
		if( m_pPixelBuffer->format() == ePIXEL_FORMAT_R8G8B8 )
		{
			// f****ing Direct3D that doesn't support RGB24...
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
			p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
		}
		else if( m_pPixelBuffer->format() == ePIXEL_FORMAT_A4R4G4B4 )
		{
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
			p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
		}
		else if( m_pPixelBuffer->format() == ePIXEL_FORMAT_STENCIL1 )
		{
			m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
			p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
		}
	}

	if( GetSamplesCount() )
	{
		p_tex2dDesc.SampleDesc.Count = GetSamplesCount();
		HRESULT l_hr = l_pDevice->CheckMultisampleQualityLevels( DirectX11::Get( l_ePixelFormat ), p_tex2dDesc.SampleDesc.Count, &p_tex2dDesc.SampleDesc.Quality );

		if( l_hr == S_OK && p_tex2dDesc.SampleDesc.Quality > 0 )
		{
			p_tex2dDesc.SampleDesc.Quality--;
		}
	}

	if( IsRenderTarget() )
	{
		p_tex2dDesc.CPUAccessFlags	= 0;
		p_tex2dDesc.Usage			= D3D11_USAGE_DEFAULT;
		p_tex2dDesc.BindFlags		|= D3D11_BIND_RENDER_TARGET;
		p_tex2dDesc.MiscFlags		= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
}

void DxDynamicTexture :: DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_tex2dData )
{
	p_tex2dData.pSysMem			= m_pPixelBuffer->const_ptr();
	p_tex2dData.SysMemPitch		= UINT( m_pPixelBuffer->size() / m_pPixelBuffer->height() );
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
	return true;
}

void DxTextureRenderer :: EndRender()
{
}

//*************************************************************************************************
#pragma warning( pop)
