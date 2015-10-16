#include "Dx11Texture.hpp"

#include "Dx11RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	DxTexture::DxTexture( DxRenderSystem & p_renderSystem, bool p_static )
		: m_shaderResourceView( NULL )
		, m_renderTargetView( NULL )
		, m_renderSystem( &p_renderSystem )
		, m_depth( 1 )
		, m_samples( 0 )
		, m_renderTarget( false )
		, m_static( p_static )
	{
	}

	DxTexture::~DxTexture()
	{
	}

	bool DxTexture::Initialise( eTEXTURE_TYPE p_dimension, PxBufferBaseSPtr p_pixelBuffer, uint32_t p_depth, bool p_renderTarget, uint32_t p_samples )
	{
		m_pixelBuffer = p_pixelBuffer;
		m_depth = p_depth;
		m_samples = p_samples;
		m_renderTarget = p_renderTarget;
		bool l_return = false;

		switch ( p_dimension )
		{
		case eTEXTURE_TYPE_BUFFER:
			l_return = DoInitialise1D();
			break;

		case eTEXTURE_TYPE_1D:
			l_return = DoInitialise1D();
			break;

		case eTEXTURE_TYPE_1DARRAY:
			l_return = DoInitialise1DArray();
			break;

		case eTEXTURE_TYPE_2D:
			l_return = DoInitialise2D();
			break;

		case eTEXTURE_TYPE_2DARRAY:
			l_return = DoInitialise2DArray();
			break;

		case eTEXTURE_TYPE_2DMS:
			l_return = DoInitialise2DMS();
			break;

		case eTEXTURE_TYPE_2DMSARRAY:
			l_return = DoInitialise2DMSArray();
			break;

		case eTEXTURE_TYPE_3D:
			l_return = DoInitialise3D();
			break;

		case eTEXTURE_TYPE_CUBE:
			l_return = DoInitialiseCube();
			break;

		case eTEXTURE_TYPE_CUBEARRAY:
			l_return = DoInitialiseCubeArray();
			break;
		}

		return l_return;
	}

	void DxTexture::Cleanup()
	{
		ReleaseTracked( m_renderSystem, m_renderTargetView );
		ReleaseTracked( m_renderSystem, m_shaderResourceView );
	}

	uint8_t * DxTexture::Lock( uint32_t p_mode )
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

	void DxTexture::Unlock( bool CU_PARAM_UNUSED( p_modified ) )
	{
		ID3D11Resource * l_pResource;
		m_shaderResourceView->GetResource( &l_pResource );
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->Unmap( l_pResource, 0 );
		l_pResource->Release();
	}

	bool DxTexture::Bind( uint32_t p_index )
	{
#if DX_DEBUG_RT

		if ( m_shaderResourceView )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

			ID3D11Resource * l_pResource;
			m_shaderResourceView->GetResource( &l_pResource );
			StringStream l_name;
			l_name << Engine::GetEngineDirectory() << cuT( "\\Texture" );

			if ( m_static )
			{
				l_name << cuT( "(Static)" );
			}
			else
			{
				l_name << cuT( "(Dynamic)" );
			}

			l_name << cuT( "_" ) << ( void * )this << cuT( "_SRV.png" );
			D3DX11SaveTextureToFile( l_pDeviceContext, l_pResource, D3DX11_IFF_PNG, l_name.str().c_str() );
			l_pResource->Release();
		}

#endif

		return true;
	}

	void DxTexture::Unbind( uint32_t p_index )
	{
	}

	bool DxTexture::DoInitialiseBuffer()
	{
		return false;
	}

	bool DxTexture::DoInitialise1D()
	{
		ID3D11Device * l_device = m_renderSystem->GetDevice();
		D3D11_TEXTURE1D_DESC l_texDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_texData = { 0 };
		DoInitTex1DDesc( l_texDesc );
		DoInitTex1DData( l_texData );
		ID3D11Texture1D * l_texture = NULL;
		HRESULT l_hr = l_device->CreateTexture1D( &l_texDesc, &l_texData, &l_texture );

		if ( l_hr == S_OK )
		{
			if ( m_renderTarget )
			{
				DoCreateRenderTargetView( D3D11_RTV_DIMENSION_TEXTURE1D, l_texture );
			}

			DoCreateShaderResourceView( D3D11_SRV_DIMENSION_TEXTURE1D, l_texture );
			l_texture->Release();
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise1DArray()
	{
		ID3D11Device * l_device = m_renderSystem->GetDevice();
		D3D11_TEXTURE1D_DESC l_texDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_texData = { 0 };
		DoInitTex1DDesc( l_texDesc );
		DoInitTex1DData( l_texData );
		ID3D11Texture1D * l_texture = NULL;
		HRESULT l_hr = l_device->CreateTexture1D( &l_texDesc, &l_texData, &l_texture );

		if ( l_hr == S_OK )
		{
			if ( m_renderTarget )
			{
				DoCreateRenderTargetView( D3D11_RTV_DIMENSION_TEXTURE1DARRAY, l_texture );
			}

			DoCreateShaderResourceView( D3D11_SRV_DIMENSION_TEXTURE1DARRAY, l_texture );
			l_texture->Release();
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2D()
	{
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		D3D11_TEXTURE2D_DESC l_texDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_texData = { 0 };
		DoInitTex2DDesc( l_texDesc );
		DoInitTex2DData( l_texData );
		ID3D11Texture2D * l_texture = NULL;
		HRESULT l_hr = l_pDevice->CreateTexture2D( &l_texDesc, &l_texData, &l_texture );

		if ( l_hr == S_OK )
		{
			if ( m_renderTarget )
			{
				DoCreateRenderTargetView( D3D11_RTV_DIMENSION_TEXTURE2D, l_texture );
			}

			DoCreateShaderResourceView( D3D11_SRV_DIMENSION_TEXTURE2D, l_texture );
			l_texture->Release();
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2DArray()
	{
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		D3D11_TEXTURE2D_DESC l_texDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_texData = { 0 };
		DoInitTex2DDesc( l_texDesc );
		DoInitTex2DData( l_texData );
		ID3D11Texture2D * l_texture = NULL;
		HRESULT l_hr = l_pDevice->CreateTexture2D( &l_texDesc, &l_texData, &l_texture );

		if ( l_hr == S_OK )
		{
			if ( m_renderTarget )
			{
				DoCreateRenderTargetView( D3D11_RTV_DIMENSION_TEXTURE2DARRAY, l_texture );
			}

			DoCreateShaderResourceView( D3D11_SRV_DIMENSION_TEXTURE2DARRAY, l_texture );
			l_texture->Release();
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2DMS()
	{
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		D3D11_TEXTURE2D_DESC l_texDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_texData = { 0 };
		DoInitTex2DDesc( l_texDesc );
		DoInitTex2DData( l_texData );
		ID3D11Texture2D * l_texture = NULL;
		HRESULT l_hr = l_pDevice->CreateTexture2D( &l_texDesc, &l_texData, &l_texture );

		if ( l_hr == S_OK )
		{
			if ( m_renderTarget )
			{
				DoCreateRenderTargetView( D3D11_RTV_DIMENSION_TEXTURE2DMS, l_texture );
			}

			DoCreateShaderResourceView( D3D11_SRV_DIMENSION_TEXTURE2DMS, l_texture );
			l_texture->Release();
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2DMSArray()
	{
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		D3D11_TEXTURE2D_DESC l_texDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_texData = { 0 };
		DoInitTex2DDesc( l_texDesc );
		DoInitTex2DData( l_texData );
		ID3D11Texture2D * l_texture = NULL;
		HRESULT l_hr = l_pDevice->CreateTexture2D( &l_texDesc, &l_texData, &l_texture );

		if ( l_hr == S_OK )
		{
			if ( m_renderTarget )
			{
				DoCreateRenderTargetView( D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY, l_texture );
			}

			DoCreateShaderResourceView( D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY, l_texture );
			l_texture->Release();
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise3D()
	{
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		D3D11_TEXTURE3D_DESC l_texDesc = { 0 };
		D3D11_SUBRESOURCE_DATA l_texData = { 0 };
		DoInitTex3DDesc( l_texDesc );
		DoInitTex3DData( l_texData );
		ID3D11Texture3D * l_texture = NULL;
		HRESULT l_hr = l_pDevice->CreateTexture3D( &l_texDesc, &l_texData, &l_texture );

		if ( l_hr == S_OK )
		{
			if ( m_renderTarget )
			{
				DoCreateRenderTargetView( D3D11_RTV_DIMENSION_TEXTURE3D, l_texture );
			}

			DoCreateShaderResourceView( D3D11_SRV_DIMENSION_TEXTURE3D, l_texture );
			l_texture->Release();
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialiseCube()
	{
		return false;
	}

	bool DxTexture::DoInitialiseCubeArray()
	{
		return false;
	}

	void DxTexture::DoInitTex1DDesc( D3D11_TEXTURE1D_DESC & p_desc )
	{
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		ePIXEL_FORMAT l_ePixelFormat = l_pxBuffer->format();
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		p_desc.Width = l_pxBuffer->width();
		p_desc.ArraySize = l_pxBuffer->height();
		p_desc.Format = DoCheckPixelFormat( DirectX11::Get( l_ePixelFormat ) );

		if ( m_static )
		{
			p_desc.Usage = D3D11_USAGE_IMMUTABLE;
			p_desc.CPUAccessFlags = 0;
		}
		else
		{
			p_desc.Usage = D3D11_USAGE_DYNAMIC;
			p_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		p_desc.MipLevels = 1;
		p_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		if ( !m_static && m_renderTarget )
		{
			p_desc.CPUAccessFlags = 0;
			p_desc.Usage = D3D11_USAGE_DEFAULT;
			p_desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			//p_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
	}

	void DxTexture::DoInitTex1DData( D3D11_SUBRESOURCE_DATA & p_data )
	{
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		p_data.pSysMem = l_pxBuffer->const_ptr();
		p_data.SysMemPitch = UINT( l_pxBuffer->size() / l_pxBuffer->height() );
	}

	void DxTexture::DoInitTex2DDesc( D3D11_TEXTURE2D_DESC & p_desc )
	{
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		ePIXEL_FORMAT l_ePixelFormat = l_pxBuffer->format();
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		p_desc.Width = l_pxBuffer->width();
		p_desc.Height = l_pxBuffer->height() / m_depth;
		p_desc.ArraySize = m_depth;
		p_desc.Format = DoCheckPixelFormat( DirectX11::Get( l_ePixelFormat ) );

		if ( m_static )
		{
			p_desc.Usage = D3D11_USAGE_IMMUTABLE;
			p_desc.CPUAccessFlags = 0;
		}
		else
		{
			p_desc.Usage = D3D11_USAGE_DYNAMIC;
			p_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		p_desc.MipLevels = 1;
		p_desc.SampleDesc.Count = 1;
		p_desc.SampleDesc.Quality = 0;
		p_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		if ( m_samples )
		{
			p_desc.SampleDesc.Count = m_samples;
			HRESULT l_hr = l_pDevice->CheckMultisampleQualityLevels( DirectX11::Get( l_ePixelFormat ), p_desc.SampleDesc.Count, &p_desc.SampleDesc.Quality );

			if ( l_hr == S_OK && p_desc.SampleDesc.Quality > 0 )
			{
				p_desc.SampleDesc.Quality--;
			}
		}

		if ( !m_static && m_renderTarget )
		{
			p_desc.CPUAccessFlags = 0;
			p_desc.Usage = D3D11_USAGE_DEFAULT;
			p_desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			//p_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
	}

	void DxTexture::DoInitTex2DData( D3D11_SUBRESOURCE_DATA & p_data )
	{
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		p_data.pSysMem = l_pxBuffer->const_ptr();
		p_data.SysMemPitch = UINT( l_pxBuffer->size() / l_pxBuffer->height() );
	}

	void DxTexture::DoInitTex3DDesc( D3D11_TEXTURE3D_DESC & p_desc )
	{
		PxBufferBaseSPtr l_pxBuffer = GetPixelBuffer();
		ePIXEL_FORMAT l_ePixelFormat = l_pxBuffer->format();
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		p_desc.Width = l_pxBuffer->width();
		p_desc.Height = l_pxBuffer->height() / m_depth;
		p_desc.Depth = m_depth;
		p_desc.Format = DoCheckPixelFormat( DirectX11::Get( l_ePixelFormat ) );

		if ( m_static )
		{
			p_desc.Usage = D3D11_USAGE_IMMUTABLE;
			p_desc.CPUAccessFlags = 0;
		}
		else
		{
			p_desc.Usage = D3D11_USAGE_DYNAMIC;
			p_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		p_desc.MipLevels = 1;
		p_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		if ( !m_static && m_renderTarget )
		{
			p_desc.CPUAccessFlags = 0;
			p_desc.Usage = D3D11_USAGE_DEFAULT;
			p_desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			//p_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
	}

	void DxTexture::DoInitTex3DData( D3D11_SUBRESOURCE_DATA & p_data )
	{
		PxBufferBaseSPtr l_pxBuffer = GetPixelBuffer();
		p_data.pSysMem = l_pxBuffer->const_ptr();
		p_data.SysMemPitch = UINT( l_pxBuffer->size() / l_pxBuffer->height() );
	}

	bool DxTexture::DoCreateRenderTargetView( D3D11_RTV_DIMENSION p_dimension, ID3D11Resource * p_texture )
	{
		PxBufferBaseSPtr l_pxBuffer = GetPixelBuffer();
		D3D11_RENDER_TARGET_VIEW_DESC l_desc = {};
		l_desc.Format = DirectX11::Get( l_pxBuffer->format() );
		l_desc.ViewDimension = p_dimension;

		switch ( p_dimension )
		{
		case D3D11_RTV_DIMENSION_BUFFER:
			l_desc.Buffer.ElementOffset = 0;
			l_desc.Buffer.ElementWidth = PF::GetBytesPerPixel( l_pxBuffer->format() );
			l_desc.Buffer.FirstElement = 0;
			l_desc.Buffer.NumElements = l_pxBuffer->count();
			break;

		case D3D11_RTV_DIMENSION_TEXTURE1D:
			l_desc.Texture1D.MipSlice = 0;
			break;

		case D3D11_RTV_DIMENSION_TEXTURE1DARRAY:
			l_desc.Texture1DArray.MipSlice = 0;
			l_desc.Texture1DArray.FirstArraySlice = 0;
			l_desc.Texture1DArray.ArraySize = l_pxBuffer->height();
			break;

		case D3D11_RTV_DIMENSION_TEXTURE2D:
			l_desc.Texture2D.MipSlice = 0;
			break;

		case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
			l_desc.Texture2DArray.MipSlice = 0;
			l_desc.Texture2DArray.FirstArraySlice = 0;
			l_desc.Texture2DArray.ArraySize = m_depth;
			break;

		case D3D11_RTV_DIMENSION_TEXTURE2DMS:
			break;

		case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
			l_desc.Texture2DMSArray.ArraySize = m_depth;
			l_desc.Texture2DMSArray.FirstArraySlice = 0;
			break;

		case D3D11_RTV_DIMENSION_TEXTURE3D:
			l_desc.Texture3D.WSize = m_depth;
			l_desc.Texture3D.FirstWSlice = 0;
			l_desc.Texture3D.MipSlice = 0;
			break;
		}

		HRESULT l_hr = m_renderSystem->GetDevice()->CreateRenderTargetView( p_texture, &l_desc, &m_renderTargetView );
		dxTrack( m_renderSystem, m_renderTargetView, DynamicRTView );
		return l_hr == S_OK;
	}

	bool DxTexture::DoCreateShaderResourceView( D3D11_SRV_DIMENSION p_dimension, ID3D11Resource * p_texture )
	{
		PxBufferBaseSPtr l_pxBuffer = GetPixelBuffer();
		D3D11_SHADER_RESOURCE_VIEW_DESC l_desc = {};
		l_desc.Format = DirectX11::Get( l_pxBuffer->format() );
		l_desc.ViewDimension = p_dimension;

		switch ( p_dimension )
		{
		case D3D11_SRV_DIMENSION_BUFFER:
			l_desc.Buffer.ElementOffset = 0;
			l_desc.Buffer.ElementWidth = PF::GetBytesPerPixel( l_pxBuffer->format() );
			l_desc.Buffer.FirstElement = 0;
			l_desc.Buffer.NumElements = l_pxBuffer->count();
			break;

		case D3D11_SRV_DIMENSION_TEXTURE1D:
			l_desc.Texture1D.MipLevels = 1;
			l_desc.Texture1D.MostDetailedMip = 0;
			break;

		case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
			l_desc.Texture1DArray.MipLevels = 1;
			l_desc.Texture1DArray.MostDetailedMip = 0;
			l_desc.Texture1DArray.FirstArraySlice = 0;
			l_desc.Texture1DArray.ArraySize = l_pxBuffer->height();
			break;

		case D3D11_SRV_DIMENSION_TEXTURE2D:
			l_desc.Texture2D.MipLevels = 1;
			l_desc.Texture2D.MostDetailedMip = 0;
			break;

		case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
			l_desc.Texture2DArray.MipLevels = 1;
			l_desc.Texture2DArray.MostDetailedMip = 0;
			l_desc.Texture2DArray.FirstArraySlice = 0;
			l_desc.Texture2DArray.ArraySize = m_depth;
			break;

		case D3D11_SRV_DIMENSION_TEXTURE2DMS:
			break;

		case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
			l_desc.Texture2DMSArray.ArraySize = m_depth;
			l_desc.Texture2DMSArray.FirstArraySlice = 0;
			break;

		case D3D11_SRV_DIMENSION_TEXTURE3D:
			l_desc.Texture3D.MipLevels = 1;
			l_desc.Texture3D.MostDetailedMip = 0;
			break;

		case D3D11_SRV_DIMENSION_TEXTURECUBE:
			l_desc.TextureCube.MipLevels = 1;
			l_desc.TextureCube.MostDetailedMip = 0;
			break;

		case D3D11_SRV_DIMENSION_TEXTURECUBEARRAY:
			l_desc.TextureCubeArray.MipLevels = 1;
			l_desc.TextureCubeArray.MostDetailedMip = 0;
			break;
		}

		HRESULT l_hr = m_renderSystem->GetDevice()->CreateShaderResourceView( p_texture, &l_desc, &m_shaderResourceView );
		dxTrack( m_renderSystem, m_shaderResourceView, DynamicSRView );
		return l_hr == S_OK;
	}

	DXGI_FORMAT DxTexture::DoCheckPixelFormat( DXGI_FORMAT p_format )
	{
		if ( p_format == DXGI_FORMAT_UNKNOWN )
		{
			PxBufferBaseSPtr l_pxBuffer = GetPixelBuffer();

			if ( l_pxBuffer->format() == ePIXEL_FORMAT_R8G8B8 )
			{
				// f****ing Direct3D that doesn't support RGB24...
				l_pxBuffer = PxBufferBase::create( l_pxBuffer->dimensions(), ePIXEL_FORMAT_A8B8G8R8, l_pxBuffer->const_ptr(), l_pxBuffer->format() );
				p_format = DirectX11::Get( l_pxBuffer->format() );
				m_pixelBuffer = l_pxBuffer;
			}
			else if ( l_pxBuffer->format() == ePIXEL_FORMAT_A4R4G4B4 )
			{
				l_pxBuffer = PxBufferBase::create( l_pxBuffer->dimensions(), ePIXEL_FORMAT_A8B8G8R8, l_pxBuffer->const_ptr(), l_pxBuffer->format() );
				p_format = DirectX11::Get( l_pxBuffer->format() );
				m_pixelBuffer = l_pxBuffer;
			}
			else if ( l_pxBuffer->format() == ePIXEL_FORMAT_STENCIL1 )
			{
				l_pxBuffer = PxBufferBase::create( l_pxBuffer->dimensions(), ePIXEL_FORMAT_STENCIL8, l_pxBuffer->const_ptr(), l_pxBuffer->format() );
				p_format = DirectX11::Get( l_pxBuffer->format() );
				m_pixelBuffer = l_pxBuffer;
			}
		}

		return p_format;
	}
}
