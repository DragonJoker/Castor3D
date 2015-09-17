#include "Dx11Context.hpp"
#include "Dx11RenderSystem.hpp"

#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <RenderWindow.hpp>
#include <PlatformWindowHandle.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace ShaderModel1_2_3_4
{
	static const String VtxShader =
		cuT( "struct VtxInput\n" )
		cuT( "{\n" )
		cuT( "	float4 Position: POSITION;\n" )
		cuT( "	float2 TextureUV: TEXCOORD0;\n" )
		cuT( "};\n" )
		cuT( "struct VtxOutput\n" )
		cuT( "{\n" )
		cuT( "	float4 Position: SV_POSITION;\n" )
		cuT( "	float2 TextureUV: TEXCOORD0;\n" )
		cuT( "};\n" )
		cuT( "VtxOutput mainVx( VtxInput p_input )\n" )
		cuT( "{\n" )
		cuT( "	VtxOutput l_output;\n" )
		cuT( "	p_input.Position.w = 1.0f;\n" )
		cuT( "	l_output.Position = mul( p_input.Position, c3d_mtxProjection );\n" )
		cuT( "	l_output.TextureUV = p_input.TextureUV;\n" )
		cuT( "	return l_output;\n" )
		cuT( "}\n" );

	static const String PxlShader =
		cuT( "Texture2D diffuseTexture: register( t0 );\n" )
		cuT( "SamplerState DiffuseSampler: register( s0 );\n" )
		cuT( "struct PxlInput\n" )
		cuT( "{\n" )
		cuT( "	float4 Position: SV_POSITION;\n" )
		cuT( "	float2 TextureUV: TEXCOORD0;\n" )
		cuT( "};\n" )
		cuT( "float4 mainPx( PxlInput p_input ): SV_TARGET\n" )
		cuT( "{\n" )
		cuT( "	return diffuseTexture.Sample( DiffuseSampler, p_input.TextureUV );\n" )
		cuT( "}\n" );
}

namespace Dx11Render
{
	DxContext::DxContext()
		: Context()
		, m_pSwapChain( NULL )
		, m_pRenderTargetView( NULL )
		, m_pDepthStencilView( NULL )
		, m_pDeviceContext( NULL )
	{
	}

	DxContext::~DxContext()
	{
	}

	void DxContext::UpdateFullScreen( bool p_bVal )
	{
		if ( p_bVal )
		{
			m_pSwapChain->SetFullscreenState( TRUE, NULL );
		}
		else
		{
			m_pSwapChain->SetFullscreenState( FALSE, NULL );
		}
	}

	bool DxContext::DoInitialise()
	{
		if ( !m_bInitialised )
		{
			DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_renderSystem );
			m_hWnd = m_pWindow->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
			m_size = m_pWindow->GetSize();

			if ( DoInitPresentParameters() == S_OK &&  l_pRenderSystem->InitialiseDevice( m_hWnd, m_deviceParams ) )
			{
				DoInitVolatileResources();
				Logger::LogInfo( StringStream() << cuT( "Dx11Context::DoInitialise - Context for window 0x" ) << std::hex << m_hWnd << cuT( " initialised" ) );
				m_bInitialised = true;
			}

			if ( m_bInitialised )
			{
				auto l_uniforms = UniformsBase::Get( *l_pRenderSystem );
				StringStream l_vtxShader;
				l_vtxShader << l_uniforms->GetVertexInMatrices( 0 ) << std::endl;
				l_vtxShader << ShaderModel1_2_3_4::VtxShader;
				ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
				l_pProgram->SetEntryPoint( eSHADER_TYPE_VERTEX, cuT( "mainVx" ) );
				l_pProgram->SetEntryPoint( eSHADER_TYPE_PIXEL, cuT( "mainPx" ) );
				l_pProgram->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_5, l_vtxShader.str() );
				l_pProgram->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_5, ShaderModel1_2_3_4::PxlShader );
			}
		}

		return m_bInitialised;
	}

	void DxContext::DoCleanup()
	{
		SafeRelease( m_pDeviceContext );
		DoFreeVolatileResources();
	}

	void DxContext::DoSetCurrent()
	{
		static_cast< DxRenderSystem * >( m_renderSystem )->GetDevice()->GetImmediateContext( &m_pDeviceContext );
	}

	void DxContext::DoEndCurrent()
	{
		SafeRelease( m_pDeviceContext );
	}

	void DxContext::DoSwapBuffers()
	{
		if ( m_pSwapChain )
		{
			if ( m_pWindow->GetVSync() )
			{
				m_pSwapChain->Present( 1, 0 );
			}
			else
			{
				m_pSwapChain->Present( 0, 0 );
			}
		}
	}

	void DxContext::DoSetClearColour( Colour const & p_clrClear )
	{
		m_fClearColour[0] = p_clrClear.red().value();
		m_fClearColour[1] = p_clrClear.green().value();
		m_fClearColour[2] = p_clrClear.blue().value();
		m_fClearColour[3] = p_clrClear.alpha().value();
	}

	void DxContext::DoClear( uint32_t p_uiTargets )
	{
		ID3D11DeviceContext * l_pDeviceContext = GetDeviceContext();

		if ( p_uiTargets & eBUFFER_COMPONENT_COLOUR )
		{
			l_pDeviceContext->ClearRenderTargetView( m_pRenderTargetView, m_fClearColour );
		}

		UINT l_uiFlags = 0;

		if ( p_uiTargets & eBUFFER_COMPONENT_DEPTH )
		{
			l_uiFlags |= D3D11_CLEAR_DEPTH;
		}

		if ( p_uiTargets & eBUFFER_COMPONENT_STENCIL )
		{
			l_uiFlags |= D3D11_CLEAR_STENCIL;
		}

		if ( l_uiFlags )
		{
			l_pDeviceContext->ClearDepthStencilView( m_pDepthStencilView, l_uiFlags, 0, 0 );
		}
	}

	void DxContext::DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget )
	{
		if ( p_eTarget == eFRAMEBUFFER_TARGET_DRAW )
		{
			if ( p_eBuffer == eBUFFER_BACK || p_eBuffer == eBUFFER_BACK_LEFT || p_eBuffer == eBUFFER_BACK_RIGHT )
			{
				ID3D11DeviceContext * l_pDeviceContext = GetDeviceContext();
				l_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
			}
		}
		else if ( p_eTarget == eFRAMEBUFFER_TARGET_READ )
		{
		}
	}

	void DxContext::DoSetAlphaFunc( eALPHA_FUNC CU_PARAM_UNUSED( p_eFunc ), uint8_t CU_PARAM_UNUSED( p_byValue ) )
	{
		// Dropped out.
	}

	void DxContext::DoInitVolatileResources()
	{
		DoSetCurrent();
		DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem * >( m_renderSystem );
		DxContextSPtr l_pMainContext = std::static_pointer_cast< DxContext >( l_pRenderSystem->GetMainContext() );
		IDXGIFactory * l_factory = NULL;
		HRESULT l_hr = CreateDXGIFactory( __uuidof( IDXGIFactory ) , reinterpret_cast< void ** >( &l_factory ) );
		ID3D11Texture2D * l_pDSTex = NULL;
		ID3D11Texture2D * l_pRTTex = NULL;

		if ( l_factory )
		{
			HRESULT l_hr = l_factory->CreateSwapChain( l_pRenderSystem->GetDevice(), &m_deviceParams, &m_pSwapChain );
			dxTrack( l_pRenderSystem, m_pSwapChain, SwapChain );
			l_factory->Release();
			bool l_bContinue = dxCheckError( l_hr, "CreateSwapChain" );

			if ( l_bContinue )
			{
				l_hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void ** >( &l_pRTTex ) );
				l_bContinue = dxCheckError( l_hr, "SwapChain::GetBuffer" );
			}

			if ( l_bContinue )
			{
				l_hr = l_pRenderSystem->GetDevice()->CreateRenderTargetView( l_pRTTex, NULL, &m_pRenderTargetView );
				dxTrack( l_pRenderSystem, m_pRenderTargetView, ContextRTView );
				l_bContinue = dxCheckError( l_hr, "CreateRenderTargetView" );
				l_pRTTex->Release();
			}

			if ( l_bContinue )
			{
				// Create depth stencil texture
				D3D11_TEXTURE2D_DESC l_descDepth = { 0 };
				l_descDepth.Width = m_deviceParams.BufferDesc.Width;
				l_descDepth.Height = m_deviceParams.BufferDesc.Height;
				l_descDepth.MipLevels = 1;
				l_descDepth.ArraySize = 1;
				l_descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				l_descDepth.SampleDesc.Count = 1;
				l_descDepth.SampleDesc.Quality = 0;
				l_descDepth.Usage = D3D11_USAGE_DEFAULT;
				l_descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				l_descDepth.CPUAccessFlags = 0;
				l_descDepth.MiscFlags = 0;
				l_hr = l_pRenderSystem->GetDevice()->CreateTexture2D( &l_descDepth, NULL, &l_pDSTex );
				l_bContinue = dxCheckError( l_hr, "CreateTexture2D" );
			}

			if ( l_bContinue )
			{
				// Create the depth stencil view
				D3D11_DEPTH_STENCIL_VIEW_DESC l_descDSV = D3D11_DEPTH_STENCIL_VIEW_DESC();
				l_descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				l_descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				l_descDSV.Texture2D.MipSlice = 0;
				l_hr = l_pRenderSystem->GetDevice()->CreateDepthStencilView( l_pDSTex, &l_descDSV, &m_pDepthStencilView );
				dxTrack( l_pRenderSystem, m_pDepthStencilView, ContextDSView );
				l_bContinue = dxCheckError( l_hr, "CreateDepthStencilView" );
				l_pDSTex->Release();
			}

			if ( l_bContinue )
			{
				ID3D11DeviceContext * l_pDeviceContext = GetDeviceContext();
				l_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
				l_pDeviceContext->Release();
				dxCheckError( l_hr, "OMSetRenderTargets" );
			}
		}

		DoEndCurrent();
	}

	void DxContext::DoFreeVolatileResources()
	{
		if ( m_pSwapChain )
		{
			m_pSwapChain->SetFullscreenState( false, NULL );
		}

		ReleaseTracked( m_renderSystem, m_pDepthStencilView );
		ReleaseTracked( m_renderSystem, m_pRenderTargetView );
		ReleaseTracked( m_renderSystem, m_pSwapChain );
	}

	HRESULT DxContext::DoInitPresentParameters()
	{
		HRESULT l_hr = E_FAIL;
		std::memset( &m_deviceParams, 0, sizeof( m_deviceParams ) );
		m_deviceParams.Windowed = TRUE;
		m_deviceParams.BufferCount = 1;
		m_deviceParams.BufferDesc.Width = ::GetSystemMetrics( SM_CXFULLSCREEN );
		m_deviceParams.BufferDesc.Height = ::GetSystemMetrics( SM_CYFULLSCREEN );
		m_deviceParams.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_deviceParams.BufferDesc.RefreshRate.Numerator = 60;
		m_deviceParams.BufferDesc.RefreshRate.Denominator = 1;
		m_deviceParams.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_deviceParams.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		m_deviceParams.SampleDesc.Count = 1;
		m_deviceParams.SampleDesc.Quality = 0;
		m_deviceParams.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_deviceParams.OutputWindow = m_hWnd;
		m_deviceParams.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		if ( m_pWindow->IsFullscreen() )
		{
			m_deviceParams.Windowed = FALSE;
		}
		else
		{
			m_deviceParams.Windowed = TRUE;
		}

		l_hr = S_OK;
		return l_hr;
	}

	void DxContext::DoCullFace( eFACE CU_PARAM_UNUSED( p_eCullFace ) )
	{
	}
}
