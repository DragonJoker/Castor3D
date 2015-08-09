#include "Dx11StaticTexture.hpp"
#include "Dx11RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx11Render
{
	DxStaticTexture::DxStaticTexture( DxRenderSystem * p_pRenderSystem )
		: StaticTexture( p_pRenderSystem )
		, m_pShaderResourceView( NULL )
		, m_pRenderSystem( p_pRenderSystem )
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
		SafeRelease( m_pShaderResourceView );
	}

	uint8_t * DxStaticTexture::Lock( uint32_t p_uiMode )
	{
		uint8_t * l_pReturn = NULL;
		D3D11_MAPPED_SUBRESOURCE l_mappedResource;
		ID3D11Resource * l_pResource;
		m_pShaderResourceView->GetResource( &l_pResource );
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		HRESULT l_hr = l_pDeviceContext->Map( l_pResource, 0, D3D11_MAP( DirectX11::GetLockFlags( p_uiMode ) ), 0, &l_mappedResource );
		l_pResource->Release();

		if ( l_hr == S_OK )
		{
			l_pReturn = static_cast< uint8_t * >( l_mappedResource.pData );
		}

		return l_pReturn;
	}

	void DxStaticTexture::Unlock( bool CU_PARAM_UNUSED( p_bModified ) )
	{
		ID3D11Resource * l_pResource;
		m_pShaderResourceView->GetResource( &l_pResource );
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->Unmap( l_pResource, 0 );
		l_pResource->Release();
	}

	bool DxStaticTexture::DoInitialise()
	{
		ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();
		D3D11_TEXTURE2D_DESC l_tex2dDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_tex2dData = { 0 };
		HRESULT l_hr = S_OK;
		ID3D11Texture2D * l_pTexture = NULL;
		DoInitTex2DDesc( l_tex2dDesc );
		DoInitTex2DData( l_tex2dData );

		if ( l_hr == S_OK )
		{
			l_hr = l_pDevice->CreateTexture2D( &l_tex2dDesc, &l_tex2dData, &l_pTexture );
		}

		if ( l_hr == S_OK )
		{
			D3D10_SHADER_RESOURCE_VIEW_DESC l_desc;
			l_desc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			l_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			l_desc.Texture2D.MipLevels = 2;
			l_desc.Texture2D.MostDetailedMip = 0;
			l_hr = l_pDevice->CreateShaderResourceView( l_pTexture, NULL, &m_pShaderResourceView );
			dxDebugName( m_pShaderResourceView, StaticSRView );
		}

		SafeRelease( l_pTexture );
		CASTOR_ASSERT( l_hr == S_OK || m_pPixelBuffer->width() == 0 );
		return l_hr == S_OK;
	}

	bool DxStaticTexture::DoBind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();

		if ( m_pShaderResourceView )
		{
			ID3D11Resource * l_pResource;
			m_pShaderResourceView->GetResource( &l_pResource );
			StringStream l_name;
			l_name << cuT( "StaticTexture_" ) << ( void * )this << cuT( "_SRV.png" );
			D3DX11SaveTextureToFile( l_pDeviceContext, l_pResource, D3DX11_IFF_PNG, l_name.str().c_str() );
		}

		return true;
	}

	void DxStaticTexture::DoUnbind()
	{
	}

	void DxStaticTexture::DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_tex2dDesc )
	{
		p_tex2dDesc.Width = m_pPixelBuffer->width();
		p_tex2dDesc.Height = m_pPixelBuffer->height();
		p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
		p_tex2dDesc.ArraySize = 1;
		p_tex2dDesc.Usage = D3D11_USAGE_IMMUTABLE;
		p_tex2dDesc.CPUAccessFlags = 0;
		p_tex2dDesc.MipLevels = 1;
		p_tex2dDesc.SampleDesc.Count = 1;
		p_tex2dDesc.SampleDesc.Quality = 0;
		p_tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		if ( p_tex2dDesc.Format == DXGI_FORMAT_UNKNOWN )
		{
			if ( m_pPixelBuffer->format() == ePIXEL_FORMAT_R8G8B8 )
			{
				// f****ing Direct3D that doesn't support RGB24...
				m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
				p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			}
			else if ( m_pPixelBuffer->format() == ePIXEL_FORMAT_A4R4G4B4 )
			{
				m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8R8G8B8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
				p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			}
			else if ( m_pPixelBuffer->format() == ePIXEL_FORMAT_STENCIL1 )
			{
				m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
				p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			}
		}
	}

	void DxStaticTexture::DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_tex2dData )
	{
		p_tex2dData.pSysMem = m_pPixelBuffer->const_ptr();
		p_tex2dData.SysMemPitch = UINT( m_pPixelBuffer->size() / m_pPixelBuffer->height() );
	}
}

#pragma warning( pop)
