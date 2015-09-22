#include "Dx11DynamicTexture.hpp"

#include "Dx11RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

namespace Dx11Render
{
	DxDynamicTexture::DxDynamicTexture( DxRenderSystem * p_renderSystem )
		: DynamicTexture( p_renderSystem )
		, m_renderTargetView( NULL )
		, m_shaderResourceView( NULL )
		, m_renderSystem( p_renderSystem )
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
		ReleaseTracked( m_renderSystem, m_renderTargetView );
		ReleaseTracked( m_renderSystem, m_shaderResourceView );
	}

	uint8_t * DxDynamicTexture::Lock( uint32_t p_mode )
	{
		uint8_t * l_return = NULL;
		D3D11_MAPPED_SUBRESOURCE l_mappedResource;
		ID3D11Resource * l_pResource;
		m_shaderResourceView->GetResource( &l_pResource );
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		HRESULT l_hr = l_pDeviceContext->Map( l_pResource, 0, D3D11_MAP( DirectX11::GetLockFlags( p_mode ) ), 0, &l_mappedResource );
		l_pResource->Release();

		if ( l_hr == S_OK )
		{
			l_return = static_cast< uint8_t * >( l_mappedResource.pData );
		}

		return l_return;
	}

	void DxDynamicTexture::Unlock( bool CU_PARAM_UNUSED( p_modified ) )
	{
		ID3D11Resource * l_pResource;
		m_shaderResourceView->GetResource( &l_pResource );
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->Unmap( l_pResource, 0 );
		l_pResource->Release();
	}

	bool DxDynamicTexture::DoInitialise()
	{
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		D3D11_TEXTURE2D_DESC l_tex2dDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_tex2dData = { 0 };
		ID3D11Texture2D * l_pTexture = NULL;
		DoInitTex2DDesc( l_tex2dDesc );
		DoInitTex2DData( l_tex2dData );
		HRESULT l_hr = l_pDevice->CreateTexture2D( &l_tex2dDesc, &l_tex2dData, &l_pTexture );

		if ( l_hr == S_OK )
		{
			if ( GetRenderTarget() )
			{
				D3D11_RENDER_TARGET_VIEW_DESC l_desc = {};
				l_desc.Format = DirectX11::Get( m_pPixelBuffer->format() );
				l_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				l_desc.Texture2D.MipSlice = 0;
				l_hr = l_pDevice->CreateRenderTargetView( l_pTexture, NULL, &m_renderTargetView );
				dxTrack( m_renderSystem, m_renderTargetView, DynamicRTView );
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC l_desc = {};
			l_desc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			l_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			l_desc.Texture2D.MipLevels = 1;
			l_desc.Texture2D.MostDetailedMip = 0;
			l_hr = l_pDevice->CreateShaderResourceView( l_pTexture, &l_desc, &m_shaderResourceView );
			dxTrack( m_renderSystem, m_shaderResourceView, DynamicSRView );
			l_pTexture->Release();
		}

		CASTOR_ASSERT( l_hr == S_OK || m_pPixelBuffer->width() == 0 );
		return l_hr == S_OK;
	}

	bool DxDynamicTexture::DoBind( uint32_t p_index )
	{
#if DX_DEBUG_RT

		if ( m_shaderResourceView )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

			ID3D11Resource * l_pResource;
			m_shaderResourceView->GetResource( &l_pResource );
			StringStream l_name;
			l_name << Engine::GetEngineDirectory() << cuT( "\\DynamicTexture_" ) << ( void * )this << cuT( "_SRV.png" );
			D3DX11SaveTextureToFile( l_pDeviceContext, l_pResource, D3DX11_IFF_PNG, l_name.str().c_str() );
			l_pResource->Release();
		}

#endif

		return true;
	}

	void DxDynamicTexture::DoUnbind( uint32_t p_index )
	{
	}

	void DxDynamicTexture::DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_tex2dDesc )
	{
		ePIXEL_FORMAT l_ePixelFormat = m_pPixelBuffer->format();
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		p_tex2dDesc.Width = m_pPixelBuffer->width();
		p_tex2dDesc.Height = m_pPixelBuffer->height();
		p_tex2dDesc.Format = DirectX11::Get( l_ePixelFormat );
		p_tex2dDesc.ArraySize = 1;
		p_tex2dDesc.Usage = D3D11_USAGE_DYNAMIC;
		p_tex2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		p_tex2dDesc.MipLevels = 1;
		p_tex2dDesc.SampleDesc.Count = 1;
		p_tex2dDesc.SampleDesc.Quality = 0;
		p_tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		if ( p_tex2dDesc.Format == DXGI_FORMAT_UNKNOWN )
		{
			if ( m_pPixelBuffer->format() == ePIXEL_FORMAT_R8G8B8 )
			{
				// f****ing Direct3D that doesn't support RGB24...
				m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8B8G8R8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
				p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			}
			else if ( m_pPixelBuffer->format() == ePIXEL_FORMAT_A4R4G4B4 )
			{
				m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_A8B8G8R8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
				p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			}
			else if ( m_pPixelBuffer->format() == ePIXEL_FORMAT_STENCIL1 )
			{
				m_pPixelBuffer = PxBufferBase::create( m_pPixelBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, m_pPixelBuffer->const_ptr(), m_pPixelBuffer->format() );
				p_tex2dDesc.Format = DirectX11::Get( m_pPixelBuffer->format() );
			}
		}

		if ( GetSamplesCount() )
		{
			p_tex2dDesc.SampleDesc.Count = GetSamplesCount();
			HRESULT l_hr = l_pDevice->CheckMultisampleQualityLevels( DirectX11::Get( l_ePixelFormat ), p_tex2dDesc.SampleDesc.Count, &p_tex2dDesc.SampleDesc.Quality );

			if ( l_hr == S_OK && p_tex2dDesc.SampleDesc.Quality > 0 )
			{
				p_tex2dDesc.SampleDesc.Quality--;
			}
		}

		if ( GetRenderTarget() )
		{
			p_tex2dDesc.CPUAccessFlags = 0;
			p_tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
			p_tex2dDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			//p_tex2dDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
	}

	void DxDynamicTexture::DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_tex2dData )
	{
		p_tex2dData.pSysMem = m_pPixelBuffer->const_ptr();
		p_tex2dData.SysMemPitch = UINT( m_pPixelBuffer->size() / m_pPixelBuffer->height() );
	}
}

#pragma warning( pop)
