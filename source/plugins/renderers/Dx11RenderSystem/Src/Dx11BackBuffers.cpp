#include "Dx11BackBuffers.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11Context.hpp"
#include "Dx11DynamicTexture.hpp"
#include "Dx11FrameBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxBackBuffers::DxBackBuffers( DxRenderSystem * p_renderSystem )
		: BackBuffers( *p_renderSystem->GetOwner() )
		, m_renderSystem( p_renderSystem )
		, m_renderTargetView( NULL )
		, m_depthStencilView( NULL )
	{
	}

	DxBackBuffers::~DxBackBuffers()
	{
	}

	bool DxBackBuffers::Create( int CU_PARAM_UNUSED( p_iSamplesCount ) )
	{
		return true;
	}

	void DxBackBuffers::Destroy()
	{
	}

	void DxBackBuffers::DoUpdateClearColour()
	{
		m_fClearColour[0] = GetClearColour().red().value();
		m_fClearColour[1] = GetClearColour().green().value();
		m_fClearColour[2] = GetClearColour().blue().value();
		m_fClearColour[3] = GetClearColour().alpha().value();
	}

	void DxBackBuffers::DoClear( uint32_t p_targets )
	{
		REQUIRE( m_depthStencilView && m_renderTargetView );
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_deviceContext->ClearRenderTargetView( m_renderTargetView, m_fClearColour );
		l_deviceContext->ClearDepthStencilView( m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0 );
	}

	bool DxBackBuffers::Bind( eBUFFER p_buffer, eFRAMEBUFFER_TARGET p_target )
	{
		REQUIRE( m_depthStencilView && m_renderTargetView );
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

		if ( p_target == eFRAMEBUFFER_TARGET_DRAW )
		{
			if ( p_buffer == eBUFFER_BACK || p_buffer == eBUFFER_FRONT_AND_BACK || p_buffer == eBUFFER_BACK_LEFT || p_buffer == eBUFFER_BACK_RIGHT )
			{
				l_deviceContext->OMSetRenderTargets( 1, &m_renderTargetView, m_depthStencilView );
			}
		}
		else if ( p_target == eFRAMEBUFFER_TARGET_READ )
		{
		}

		return true;
	}

	void DxBackBuffers::SetDrawBuffers( FrameBuffer::AttachArray const & p_attaches )
	{
	}

	bool DxBackBuffers::DownloadBuffer( eATTACHMENT_POINT p_point, uint8_t p_index, PxBufferBaseSPtr p_buffer )
	{
		DxDynamicTextureSPtr l_texture;

		if ( p_point == eATTACHMENT_POINT_COLOUR )
		{
			l_texture = m_colorBuffer;
		}
		else
		{
			l_texture = m_depthBuffer;
		}

		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		uint8_t * l_buffer = NULL;

		if ( p_point == eATTACHMENT_POINT_COLOUR )
		{
			REQUIRE( m_renderTargetView );
			ID3D11Texture2D * l_tex;
			m_renderTargetView->GetResource( reinterpret_cast< ID3D11Resource ** >( &l_tex ) );
			l_deviceContext->CopyResource( l_texture->GetTexture2D(), l_tex );
			l_buffer = l_texture->Lock( eACCESS_TYPE_READ );
		}

		if ( l_buffer )
		{
			p_buffer->assign( std::vector< uint8_t >( l_buffer, l_buffer + l_texture->GetBuffer()->size() ), l_texture->GetPixelFormat() );
			l_texture->Unlock( false );
		}

		return l_buffer != nullptr;
	}

	bool DxBackBuffers::DoInitialise( Castor::Size const & p_size )
	{
		m_size = p_size;
		IDXGISwapChain * l_swapChain = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetSwapChain();
		REQUIRE( l_swapChain );

		ID3D11Texture2D * l_pRTTex = NULL;
		HRESULT l_hr = S_OK;
		bool l_continue = true;

		if ( l_continue )
		{
			l_hr = l_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void ** >( &l_pRTTex ) );
			l_continue = dxCheckError( l_hr, "IDXGISwapChain::GetBuffer" );
		}

		if ( l_continue )
		{
			l_hr = m_renderSystem->GetDevice()->CreateRenderTargetView( l_pRTTex, nullptr, &m_renderTargetView );
			dxTrack( m_renderSystem, m_renderTargetView, ContextRTView );
			l_continue = dxCheckError( l_hr, "ID3D11Device::CreateRenderTargetView" );
			l_pRTTex->Release();
		}

		ID3D11Texture2D * l_pDSTex = NULL;

		if ( l_continue )
		{
			// Create depth stencil texture
			D3D11_TEXTURE2D_DESC l_descDepth = { 0 };
			l_descDepth.Width = p_size.width();
			l_descDepth.Height = p_size.height();
			l_descDepth.MipLevels = 1;
			l_descDepth.ArraySize = 1;
			l_descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			l_descDepth.SampleDesc.Count = 1;
			l_descDepth.SampleDesc.Quality = 0;
			l_descDepth.Usage = D3D11_USAGE_DEFAULT;
			l_descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			l_descDepth.CPUAccessFlags = 0;
			l_descDepth.MiscFlags = 0;
			l_hr = m_renderSystem->GetDevice()->CreateTexture2D( &l_descDepth, nullptr, &l_pDSTex );
			l_continue = dxCheckError( l_hr, "ID3D11Device::CreateTexture2D" );
		}

		if ( l_continue )
		{
			// Create the depth stencil view
			D3D11_DEPTH_STENCIL_VIEW_DESC l_descDSV = D3D11_DEPTH_STENCIL_VIEW_DESC();
			l_descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			l_descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			l_descDSV.Texture2D.MipSlice = 0;
			l_hr = m_renderSystem->GetDevice()->CreateDepthStencilView( l_pDSTex, &l_descDSV, &m_depthStencilView );
			dxTrack( m_renderSystem, m_depthStencilView, ContextDSView );
			l_continue = dxCheckError( l_hr, "ID3D11Device::CreateDepthStencilView" );
			l_pDSTex->Release();
		}

		ENSURE( m_depthStencilView && m_renderTargetView );

		if ( l_continue )
		{
			m_colorBuffer = std::static_pointer_cast< DxDynamicTexture >( GetOwner()->GetRenderSystem()->CreateDynamicTexture() );
			l_continue = m_colorBuffer != nullptr;
		}

		if ( l_continue )
		{
			m_colorBuffer->SetType( eTEXTURE_TYPE_2D );
			m_colorBuffer->SetImage( p_size, ePIXEL_FORMAT_A8B8G8R8 );
			l_continue = m_colorBuffer->Create();
		}

		if ( l_continue )
		{
			m_colorBuffer->Initialise( 0, eACCESS_TYPE_READ, eACCESS_TYPE_WRITE );
		}

		if ( l_continue )
		{
			m_depthBuffer = std::static_pointer_cast< DxDynamicTexture >( GetOwner()->GetRenderSystem()->CreateDynamicTexture() );
			l_continue = m_depthBuffer != nullptr;
		}

		if ( l_continue )
		{
			m_depthBuffer->SetType( eTEXTURE_TYPE_2D );
			m_depthBuffer->SetImage( p_size, ePIXEL_FORMAT_DEPTH24S8 );
			l_continue = m_depthBuffer->Create();
		}

		if ( l_continue )
		{
			l_continue = m_depthBuffer->Initialise( 0, eACCESS_TYPE_READ, eACCESS_TYPE_WRITE );
		}

		return l_continue;
	}

	void DxBackBuffers::DoCleanup()
	{
		ReleaseTracked( m_renderSystem, m_depthStencilView );
		ReleaseTracked( m_renderSystem, m_renderTargetView );
		m_depthBuffer->Cleanup();
		m_depthBuffer->Destroy();
		m_depthBuffer.reset();

		m_colorBuffer->Cleanup();
		m_colorBuffer->Destroy();
		m_colorBuffer.reset();
	}

	void DxBackBuffers::DoResize( Castor::Size const & p_size )
	{
	}

	bool DxBackBuffers::DoBlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectDst, uint32_t p_uiComponents, eINTERPOLATION_MODE CU_PARAM_UNUSED( p_interpolation ) )
	{
		typedef std::pair< ID3D11View *, ID3D11View * > SrcDstPair;
		DECLARE_VECTOR( SrcDstPair, SrcDstPair );
		SrcDstPairArray l_arrayPairs;
		DxFrameBufferSPtr l_pBuffer = std::static_pointer_cast< DxFrameBuffer >( p_buffer );
		bool l_bDepth = ( p_uiComponents & eBUFFER_COMPONENT_DEPTH ) == eBUFFER_COMPONENT_DEPTH;
		bool l_bStencil = ( p_uiComponents & eBUFFER_COMPONENT_STENCIL ) == eBUFFER_COMPONENT_STENCIL;
		bool l_bColour = ( p_uiComponents & eBUFFER_COMPONENT_COLOUR ) == eBUFFER_COMPONENT_COLOUR;

		if ( l_bColour )
		{
			ID3D11View * l_pDst = l_pBuffer->GetSurface( eATTACHMENT_POINT_COLOUR );

			if ( l_pDst )
			{
				l_arrayPairs.push_back( std::make_pair( m_renderTargetView, l_pDst ) );
			}
		}

		if ( m_renderSystem->GetFeatureLevel() > D3D_FEATURE_LEVEL_10_1 )
		{
			if ( l_bDepth )
			{
				ID3D11View * l_pDst = l_pBuffer->GetSurface( eATTACHMENT_POINT_DEPTH );

				if ( l_pDst )
				{
					l_arrayPairs.push_back( std::make_pair( m_depthStencilView, l_pDst ) );
				}
			}

			if ( l_bStencil )
			{
				ID3D11View * l_pDst = l_pBuffer->GetSurface( eATTACHMENT_POINT_STENCIL );

				if ( l_pDst )
				{
					l_arrayPairs.push_back( std::make_pair( m_depthStencilView, l_pDst ) );
				}
			}
		}

		HRESULT l_hr = S_OK;
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();

		for ( auto l_itArray : l_arrayPairs )
		{
			try
			{
				ID3D11Resource * l_pDstSurface;
				ID3D11Resource * l_pSrcSurface;
				l_itArray.first->GetResource( &l_pSrcSurface );
				l_itArray.second->GetResource( &l_pDstSurface );

				if ( l_pDstSurface && l_pSrcSurface )
				{
					D3D11_BOX l_box = { 0 };
					l_box.front = 0;
					l_box.back = 1;
					l_box.left = p_rectDst.left();
					l_box.right = p_rectDst.right();
					l_box.top = p_rectDst.top();
					l_box.bottom = p_rectDst.bottom();
					l_deviceContext->CopySubresourceRegion( l_pDstSurface, 0, p_rectDst.left(), p_rectDst.top(), 0, l_pSrcSurface, 0, &l_box );
				}

				SafeRelease( l_pSrcSurface );
				SafeRelease( l_pDstSurface );
			}
			catch ( ... )
			{
				Logger::LogError( cuT( "Error while blitting frame buffer" ) );
				l_hr = E_FAIL;
			}
		}

		return l_hr == S_OK;
	}
}
