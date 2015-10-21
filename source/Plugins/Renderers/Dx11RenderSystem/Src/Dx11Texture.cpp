#include "Dx11Texture.hpp"

#include "Dx11RenderSystem.hpp"

#include <Pixel.hpp>
#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	namespace
	{
		template< typename DescType >
		void InitDescAccessFlags( DescType & p_desc, uint8_t p_cpuAccess, uint8_t p_gpuAccess, bool p_static, bool p_rt )
		{
			if ( p_static )
			{
				p_desc.Usage = D3D11_USAGE_IMMUTABLE;
				p_desc.CPUAccessFlags = 0;
			}
			else if ( p_cpuAccess == eACCESS_TYPE_READ && p_gpuAccess == eACCESS_TYPE_WRITE )
			{
				p_desc.Usage = D3D11_USAGE_STAGING;
				p_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				p_desc.BindFlags = 0;
			}
			else
			{
				p_desc.Usage = D3D11_USAGE_DYNAMIC;
				p_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}

			if ( p_rt )
			{
				p_desc.CPUAccessFlags = 0;
				p_desc.Usage = D3D11_USAGE_DEFAULT;
				p_desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
				//l_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			}
		}
	}

	DxTexture::DxTexture( DxRenderSystem & p_renderSystem, bool p_static )
		: m_shaderResourceView( NULL )
		, m_renderTargetView( NULL )
		, m_renderSystem( &p_renderSystem )
		, m_depth( 1 )
		, m_samples( 0 )
		, m_renderTarget( false )
		, m_static( p_static )
	{
		m_texture.m_texture1D = NULL;
	}

	DxTexture::~DxTexture()
	{
	}

	bool DxTexture::Initialise( eTEXTURE_TYPE p_dimension, PxBufferBaseSPtr p_pixelBuffer, uint32_t p_depth, uint8_t p_cpuAccess, uint8_t p_gpuAccess, bool p_renderTarget, uint32_t p_samples )
	{
		m_type = p_dimension;
		m_pixelBuffer = p_pixelBuffer;
		m_depth = p_depth;
		m_samples = p_samples;
		m_renderTarget = p_renderTarget;
		bool l_return = false;

		switch ( m_type )
		{
		case eTEXTURE_TYPE_BUFFER:
			l_return = DoInitialiseBuffer( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_1D:
			l_return = DoInitialise1D( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_1DARRAY:
			l_return = DoInitialise1DArray( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_2D:
			l_return = DoInitialise2D( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_2DARRAY:
			l_return = DoInitialise2DArray( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_2DMS:
			l_return = DoInitialise2DMS( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_2DMSARRAY:
			l_return = DoInitialise2DMSArray( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_3D:
			l_return = DoInitialise3D( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_CUBE:
			l_return = DoInitialiseCube( p_cpuAccess, p_gpuAccess );
			break;

		case eTEXTURE_TYPE_CUBEARRAY:
			l_return = DoInitialiseCubeArray( p_cpuAccess, p_gpuAccess );
			break;
		}

		return l_return;
	}

	void DxTexture::Cleanup()
	{
		ReleaseTracked( m_renderSystem, m_texture.m_texture1D );
		ReleaseTracked( m_renderSystem, m_renderTargetView );
		ReleaseTracked( m_renderSystem, m_shaderResourceView );
	}

	uint8_t * DxTexture::Lock( uint32_t p_mode )
	{
		uint8_t * l_return = NULL;
		D3D11_MAPPED_SUBRESOURCE l_mappedResource;
		ID3D11Resource * l_pResource = m_texture.m_texture1D;
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		HRESULT l_hr = l_deviceContext->Map( l_pResource, 0, D3D11_MAP( DirectX11::GetLockFlags( p_mode ) ), 0, &l_mappedResource );

		if ( l_hr == S_OK )
		{
			l_return = static_cast< uint8_t * >( l_mappedResource.pData );
		}

		return l_return;
	}

	void DxTexture::Unlock( bool CU_PARAM_UNUSED( p_modified ) )
	{
		ID3D11Resource * l_pResource = m_texture.m_texture1D;
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_deviceContext->Unmap( l_pResource, 0 );
	}

	bool DxTexture::Bind( uint32_t p_index )
	{
		return true;
	}

	void DxTexture::Unbind( uint32_t p_index )
	{
	}

	bool DxTexture::DoInitialiseBuffer( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return DoInitialise1D( p_cpuAccess, p_gpuAccess );
	}

	bool DxTexture::DoInitialise1D( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE1D_DESC l_texDesc = DoInitTex1DDesc( p_cpuAccess, p_gpuAccess );
		D3D11_SUBRESOURCE_DATA l_texData = DoInitTex1DData();
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateTexture1D( &l_texDesc, &l_texData, &m_texture.m_texture1D );
		dxTrack( m_renderSystem, m_texture.m_texture1D, ID3D11Texture1D );

		if ( l_hr == S_OK )
		{
			DoCreateResources( m_texture.m_texture1D, l_texDesc.BindFlags != 0, D3D11_SRV_DIMENSION_TEXTURE1D, m_renderTarget, D3D11_RTV_DIMENSION_TEXTURE1D );
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise1DArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE1D_DESC l_texDesc = DoInitTex1DDesc( p_cpuAccess, p_gpuAccess );
		D3D11_SUBRESOURCE_DATA l_texData = DoInitTex1DData();
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateTexture1D( &l_texDesc, &l_texData, &m_texture.m_texture1D );
		dxTrack( m_renderSystem, m_texture.m_texture1D, ID3D11Texture1D );

		if ( l_hr == S_OK )
		{
			DoCreateResources( m_texture.m_texture1D, l_texDesc.BindFlags != 0, D3D11_SRV_DIMENSION_TEXTURE1DARRAY, m_renderTarget, D3D11_RTV_DIMENSION_TEXTURE1DARRAY );
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2D( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE2D_DESC l_texDesc = DoInitTex2DDesc( p_cpuAccess, p_gpuAccess );
		D3D11_SUBRESOURCE_DATA l_texData = DoInitTex2DData();
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateTexture2D( &l_texDesc, &l_texData, &m_texture.m_texture2D );
		dxTrack( m_renderSystem, m_texture.m_texture2D, ID3D11Texture2D );

		if ( l_hr == S_OK )
		{
			DoCreateResources( m_texture.m_texture2D, l_texDesc.BindFlags != 0, D3D11_SRV_DIMENSION_TEXTURE2D, m_renderTarget, D3D11_RTV_DIMENSION_TEXTURE2D );
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2DArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE2D_DESC l_texDesc = DoInitTex2DDesc( p_cpuAccess, p_gpuAccess );
		D3D11_SUBRESOURCE_DATA l_texData = DoInitTex2DData();
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateTexture2D( &l_texDesc, &l_texData, &m_texture.m_texture2D );
		dxTrack( m_renderSystem, m_texture.m_texture2D, ID3D11Texture2D );

		if ( l_hr == S_OK )
		{
			DoCreateResources( m_texture.m_texture2D, l_texDesc.BindFlags != 0, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, m_renderTarget, D3D11_RTV_DIMENSION_TEXTURE2DARRAY );
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2DMS( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE2D_DESC l_texDesc = DoInitTex2DDesc( p_cpuAccess, p_gpuAccess );
		D3D11_SUBRESOURCE_DATA l_texData = DoInitTex2DData();
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateTexture2D( &l_texDesc, &l_texData, &m_texture.m_texture2D );
		dxTrack( m_renderSystem, m_texture.m_texture2D, ID3D11Texture2D );

		if ( l_hr == S_OK )
		{
			DoCreateResources( m_texture.m_texture2D, l_texDesc.BindFlags != 0, D3D11_SRV_DIMENSION_TEXTURE2DMS, m_renderTarget, D3D11_RTV_DIMENSION_TEXTURE2DMS );
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise2DMSArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE2D_DESC l_texDesc = DoInitTex2DDesc( p_cpuAccess, p_gpuAccess );
		D3D11_SUBRESOURCE_DATA l_texData = DoInitTex2DData();
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateTexture2D( &l_texDesc, &l_texData, &m_texture.m_texture2D );
		dxTrack( m_renderSystem, m_texture.m_texture2D, ID3D11Texture2D );

		if ( l_hr == S_OK )
		{
			DoCreateResources( m_texture.m_texture2D, l_texDesc.BindFlags != 0, D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY, m_renderTarget, D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY );
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialise3D( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE3D_DESC l_texDesc = DoInitTex3DDesc( p_cpuAccess, p_gpuAccess );
		D3D11_SUBRESOURCE_DATA l_texData = DoInitTex3DData();
		HRESULT l_hr = m_renderSystem->GetDevice()->CreateTexture3D( &l_texDesc, &l_texData, &m_texture.m_texture3D );
		dxTrack( m_renderSystem, m_texture.m_texture3D, ID3D11Texture3D );

		if ( l_hr == S_OK )
		{
			DoCreateResources( m_texture.m_texture3D, l_texDesc.BindFlags != 0, D3D11_SRV_DIMENSION_TEXTURE3D, m_renderTarget, D3D11_RTV_DIMENSION_TEXTURE3D );
		}

		ENSURE( l_hr == S_OK );
		return l_hr == S_OK;
	}

	bool DxTexture::DoInitialiseCube( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return false;
	}

	bool DxTexture::DoInitialiseCubeArray( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		return false;
	}

	D3D11_TEXTURE1D_DESC DxTexture::DoInitTex1DDesc( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE1D_DESC l_desc = { 0 };
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		ePIXEL_FORMAT l_ePixelFormat = l_pxBuffer->format();
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		l_desc.Width = l_pxBuffer->width();
		l_desc.ArraySize = l_pxBuffer->height();
		l_desc.Format = DoCheckPixelFormat( DirectX11::Get( l_ePixelFormat ) );
		l_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		InitDescAccessFlags( l_desc, p_cpuAccess, p_gpuAccess, m_static, m_renderTarget );
		l_desc.MipLevels = 1;
		return l_desc;
	}

	D3D11_SUBRESOURCE_DATA DxTexture::DoInitTex1DData()
	{
		D3D11_SUBRESOURCE_DATA l_data = { 0 };
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		l_data.pSysMem = l_pxBuffer->const_ptr();
		l_data.SysMemPitch = UINT( l_pxBuffer->size() / l_pxBuffer->height() );
		return l_data;
	}

	D3D11_TEXTURE2D_DESC DxTexture::DoInitTex2DDesc( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE2D_DESC l_desc = { 0 };
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		ePIXEL_FORMAT l_ePixelFormat = l_pxBuffer->format();
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		l_desc.Width = l_pxBuffer->width();
		l_desc.Height = l_pxBuffer->height() / m_depth;
		l_desc.ArraySize = m_depth;
		l_desc.Format = DoCheckPixelFormat( DirectX11::GetBound( l_ePixelFormat ) );
		l_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		InitDescAccessFlags( l_desc, p_cpuAccess, p_gpuAccess, m_static, m_renderTarget );
		l_desc.MipLevels = 1;
		l_desc.SampleDesc.Count = 1;
		l_desc.SampleDesc.Quality = 0;

		if ( m_samples )
		{
			l_desc.SampleDesc.Count = m_samples;
			HRESULT l_hr = l_pDevice->CheckMultisampleQualityLevels( DirectX11::Get( l_ePixelFormat ), l_desc.SampleDesc.Count, &l_desc.SampleDesc.Quality );

			if ( l_hr == S_OK && l_desc.SampleDesc.Quality > 0 )
			{
				l_desc.SampleDesc.Quality--;
			}
		}

		return l_desc;
	}

	D3D11_SUBRESOURCE_DATA DxTexture::DoInitTex2DData()
	{
		D3D11_SUBRESOURCE_DATA l_data = { 0 };
		PxBufferBaseSPtr l_pxBuffer = m_pixelBuffer;
		l_data.pSysMem = l_pxBuffer->const_ptr();
		l_data.SysMemPitch = UINT( l_pxBuffer->size() / l_pxBuffer->height() );
		return l_data;
	}

	D3D11_TEXTURE3D_DESC DxTexture::DoInitTex3DDesc( uint8_t p_cpuAccess, uint8_t p_gpuAccess )
	{
		D3D11_TEXTURE3D_DESC l_desc;
		PxBufferBaseSPtr l_pxBuffer = GetPixelBuffer();
		ePIXEL_FORMAT l_ePixelFormat = l_pxBuffer->format();
		ID3D11Device * l_pDevice = m_renderSystem->GetDevice();
		l_desc.Width = l_pxBuffer->width();
		l_desc.Height = l_pxBuffer->height() / m_depth;
		l_desc.Depth = m_depth;
		l_desc.Format = DoCheckPixelFormat( DirectX11::Get( l_ePixelFormat ) );
		l_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		InitDescAccessFlags( l_desc, p_cpuAccess, p_gpuAccess, m_static, m_renderTarget );
		l_desc.MipLevels = 1;
		return l_desc;
	}

	D3D11_SUBRESOURCE_DATA DxTexture::DoInitTex3DData()
	{
		D3D11_SUBRESOURCE_DATA l_data = { 0 };
		PxBufferBaseSPtr l_pxBuffer = GetPixelBuffer();
		l_data.pSysMem = l_pxBuffer->const_ptr();
		l_data.SysMemPitch = UINT( l_pxBuffer->size() / l_pxBuffer->height() );
		return l_data;
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
