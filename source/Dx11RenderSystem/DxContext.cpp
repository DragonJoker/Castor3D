#include "Dx11RenderSystem/PrecompiledHeader.hpp"

#include "Dx11RenderSystem/DxContext.hpp"
#include "Dx11RenderSystem/DxRenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;
using namespace Dx11Render;

namespace ShaderModel1_2_3_4
{
	static const String VtxShader =
		cuT( "cbuffer matrices : register( cb0 )\n"													)
		cuT( "{\n"																					)
		cuT( "	matrix	c3d_mtxProjection;\n"														)
		cuT( "	matrix	c3d_mtxModel;\n"															)
		cuT( "	matrix	c3d_mtxView;\n"																)
		cuT( "	matrix	c3d_mtxModelView;\n"														)
		cuT( "	matrix	c3d_mtxProjectionModelView;\n"												)
		cuT( "	matrix	c3d_mtxNormal;\n"															)
		cuT( "	matrix	c3d_mtxTexture0;\n"															)
		cuT( "	matrix	c3d_mtxTexture1;\n"															)
		cuT( "	matrix	c3d_mtxTexture2;\n"															)
		cuT( "	matrix	c3d_mtxTexture3;\n"															)
		cuT( "};\n"																					)
		cuT( "struct VtxInput\n"																	)
		cuT( "{\n"																					)
		cuT( "	float4	Position	:	POSITION;\n"												)
		cuT( "	float2	TextureUV	:	TEXCOORD0;\n"												)
		cuT( "};\n"																					)
		cuT( "struct VtxOutput\n"																	)
		cuT( "{\n"																					)
		cuT( "	float4	Position	:	SV_POSITION;\n"												)
		cuT( "	float2	TextureUV	:	TEXCOORD0;\n"												)
		cuT( "};\n"																					)
		cuT( "VtxOutput mainVx( in VtxInput p_input )\n"											)
		cuT( "{\n"																					)
		cuT( "	VtxOutput l_output;\n"																)
		cuT( "	p_input.Position.w = 1.0f;\n"														)
		cuT( "	l_output.Position		= mul( p_input.Position, c3d_mtxProjectionModelView );\n"	)
		cuT( "	l_output.TextureUV		= p_input.TextureUV;\n"										)
		cuT( "	return l_output;\n"																	)
		cuT( "}\n"																					);

	static const String PxlShader =
		cuT( "struct PxlInput\n"																	)
		cuT( "{\n"																					)
		cuT( "	float4	Position	:	SV_POSITION;\n"												)
		cuT( "	float2	TextureUV	:	TEXCOORD0;\n"												)
		cuT( "};\n"																					)
		cuT( "Texture2D diffuseTexture : register( t0 );\n"											)
		cuT( "SamplerState DiffuseSampler : register( s0 );\n"										)
		cuT( "float4 mainPx( in PxlInput p_input ) : SV_Target\n"									)
		cuT( "{\n"																					)
		cuT( "	return diffuseTexture.Sample( DiffuseSampler, p_input.TextureUV );\n"				)
		cuT( "}\n"																					);
}

//*************************************************************************************************

DxBlendState :: DxBlendState( DxRenderSystem * p_pRenderSystem )
	:	BlendState			(					)
	,	m_pRenderSystem		( p_pRenderSystem	)
	,	m_pBlendState		( NULL				)
{
}

DxBlendState :: ~DxBlendState()
{
}

bool DxBlendState :: Initialise()
{
	D3D11_BLEND_DESC l_blendDesc						= { 0 };
	l_blendDesc.AlphaToCoverageEnable					= FALSE;
	l_blendDesc.IndependentBlendEnable					= TRUE;

	for( uint8_t i = 0; i < 8; i++ )
	{
		l_blendDesc.RenderTarget[i].BlendEnable				= m_rtStates[i].m_bEnableBlend;
		l_blendDesc.RenderTarget[i].SrcBlend				= DirectX11::Get( m_rtStates[i].m_eRgbSrcBlend );
		l_blendDesc.RenderTarget[i].DestBlend				= DirectX11::Get( m_rtStates[i].m_eRgbDstBlend );
		l_blendDesc.RenderTarget[i].BlendOp					= DirectX11::Get( m_rtStates[i].m_eRgbBlendOp );
		l_blendDesc.RenderTarget[i].SrcBlendAlpha			= DirectX11::Get( m_rtStates[i].m_eAlphaSrcBlend );
		l_blendDesc.RenderTarget[i].DestBlendAlpha			= DirectX11::Get( m_rtStates[i].m_eAlphaDstBlend );
		l_blendDesc.RenderTarget[i].BlendOpAlpha			= DirectX11::Get( m_rtStates[i].m_eAlphaBlendOp );
		l_blendDesc.RenderTarget[i].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	HRESULT l_hr = m_pRenderSystem->GetDevice()->CreateBlendState( &l_blendDesc, &m_pBlendState );
	dxDebugName( m_pBlendState, BlendState );
	m_bChanged = false;
	return dxCheckError( l_hr, "CreateBlendState" );
}

void DxBlendState :: Cleanup()
{
	SafeRelease( m_pBlendState );
}

bool DxBlendState :: Apply()
{
	bool l_bReturn = true;
	
	if( m_bChanged )
	{
		Cleanup();
		l_bReturn = Initialise();
	}

	if( l_bReturn && m_pBlendState )
	{
		ID3D11DeviceContext * l_pDC;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDC );

		if( l_pDC )
		{
			l_pDC->OMSetBlendState( m_pBlendState, m_blendFactors.const_ptr(), m_uiSampleMask );
			SafeRelease( l_pDC );
		}
	}

	return l_bReturn;
}

//*************************************************************************************************

DxRasteriserState :: DxRasteriserState( DxRenderSystem * p_pRenderSystem )
	:	RasteriserState		(					)
	,	m_pRenderSystem		( p_pRenderSystem	)
	,	m_pRasteriserState	( NULL				)
{
}

DxRasteriserState :: ~DxRasteriserState()
{
}

bool DxRasteriserState :: Initialise()
{
	D3D11_RASTERIZER_DESC l_rasterDesc = D3D11_RASTERIZER_DESC();
	// Setup the raster description which will determine how and what polygons will be drawn.
	l_rasterDesc.AntialiasedLineEnable = m_bAntialiasedLines;
	l_rasterDesc.CullMode = DirectX11::Get( m_eCulledFaces );
	l_rasterDesc.DepthBias = 0;
	l_rasterDesc.DepthBiasClamp = 0.0f;
	l_rasterDesc.SlopeScaledDepthBias = m_fDepthBias / 65535.0f;
	l_rasterDesc.DepthClipEnable = m_bDepthClipping;
	l_rasterDesc.FillMode = DirectX11::Get( m_eFillMode );
	l_rasterDesc.FrontCounterClockwise = m_bFrontCCW;
	l_rasterDesc.MultisampleEnable = m_bMultisampled;
	l_rasterDesc.ScissorEnable = m_bScissor;

	HRESULT l_hr = m_pRenderSystem->GetDevice()->CreateRasterizerState( &l_rasterDesc, &m_pRasteriserState );
	dxDebugName( m_pRasteriserState, RasterState );
	m_bChanged = false;
	return dxCheckError( l_hr, "CreateRasterizerState" );
}

void DxRasteriserState :: Cleanup()
{
	SafeRelease( m_pRasteriserState );
}

bool DxRasteriserState :: Apply()
{
	bool l_bReturn = true;
	
	if( m_bChanged )
	{
		Cleanup();
		l_bReturn = Initialise();
	}

	if( l_bReturn && m_pRasteriserState )
	{
		ID3D11DeviceContext * l_pDC;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDC );

		if( l_pDC )
		{
			l_pDC->RSSetState( m_pRasteriserState );
			SafeRelease( l_pDC );
		}
	}

	return l_bReturn;
}

//*************************************************************************************************

DxDepthStencilState :: DxDepthStencilState( DxRenderSystem * p_pRenderSystem )
	:	DepthStencilState		(					)
	,	m_pRenderSystem			( p_pRenderSystem	)
	,	m_pDepthStencilState	( NULL				)
{
}

DxDepthStencilState :: ~DxDepthStencilState()
{
}

bool DxDepthStencilState :: Initialise()
{
	D3D11_DEPTH_STENCIL_DESC l_depthStencilDesc = { 0 };
	// Set up the description of the stencil state.
	l_depthStencilDesc.DepthEnable = m_bDepthTest;
	l_depthStencilDesc.DepthWriteMask = DirectX11::Get( m_eDepthMask );
	l_depthStencilDesc.DepthFunc = DirectX11::Get( m_eDepthFunc );

	l_depthStencilDesc.StencilEnable = m_bStencilTest;
	l_depthStencilDesc.StencilReadMask = UINT8( m_uiStencilReadMask );
	l_depthStencilDesc.StencilWriteMask = UINT8( m_uiStencilWriteMask );

	// Stencil operations if pixel is front-facing.
	l_depthStencilDesc.FrontFace.StencilFailOp = DirectX11::Get( m_stStencilFront.m_eFailOp );
	l_depthStencilDesc.FrontFace.StencilDepthFailOp = DirectX11::Get( m_stStencilFront.m_eDepthFailOp );
	l_depthStencilDesc.FrontFace.StencilPassOp = DirectX11::Get( m_stStencilFront.m_ePassOp );
	l_depthStencilDesc.FrontFace.StencilFunc = DirectX11::Get( m_stStencilFront.m_eFunc );

	// Stencil operations if pixel is back-facing.
	l_depthStencilDesc.BackFace.StencilFailOp = DirectX11::Get( m_stStencilBack.m_eFailOp );
	l_depthStencilDesc.BackFace.StencilDepthFailOp = DirectX11::Get( m_stStencilBack.m_eDepthFailOp );
	l_depthStencilDesc.BackFace.StencilPassOp = DirectX11::Get( m_stStencilBack.m_ePassOp );
	l_depthStencilDesc.BackFace.StencilFunc = DirectX11::Get( m_stStencilBack.m_eFunc );

	HRESULT l_hr = m_pRenderSystem->GetDevice()->CreateDepthStencilState( &l_depthStencilDesc, &m_pDepthStencilState );
	dxDebugName( m_pDepthStencilState, DepthStencilState );
	m_bChanged = false;
	return dxCheckError( l_hr, "CreateDepthStencilState" );
}

void DxDepthStencilState :: Cleanup()
{
	SafeRelease( m_pDepthStencilState );
}

bool DxDepthStencilState :: Apply()
{
	bool l_bReturn = true;
	
	if( m_bChanged )
	{
		Cleanup();
		l_bReturn = Initialise();
	}

	if( l_bReturn && m_pDepthStencilState )
	{
		ID3D11DeviceContext * l_pDC;
		m_pRenderSystem->GetDevice()->GetImmediateContext( &l_pDC );

		if( l_pDC )
		{
			l_pDC->OMSetDepthStencilState( m_pDepthStencilState, 1 );
			SafeRelease( l_pDC );
		}
	}

	return l_bReturn;
}

//*************************************************************************************************

DxContext :: DxContext()
    :	Context				(		)
	,	m_pSwapChain		( NULL	)
	,	m_pRenderTargetView	( NULL	)
	,	m_pDepthStencilView	( NULL	)
{
}

DxContext :: ~DxContext()
{
}

void DxContext :: UpdateFullScreen( bool p_bVal )
{
	if( p_bVal )
	{
		m_pSwapChain->SetFullscreenState( TRUE, NULL );
	}
	else
	{
		m_pSwapChain->SetFullscreenState( FALSE, NULL );
	}
}

bool DxContext :: DoInitialise()
{
	if( !m_bInitialised )
	{
		DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem* >( m_pRenderSystem );

		m_hWnd = m_pWindow->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
//		m_size.set( ::GetSystemMetrics( SM_CXFULLSCREEN ), ::GetSystemMetrics( SM_CYFULLSCREEN ) );
		m_size = m_pWindow->GetSize();
		
		if( DoInitPresentParameters() == S_OK &&  l_pRenderSystem->InitialiseDevice( m_hWnd, m_deviceParams ) )
		{
			DoInitVolatileResources();
			Logger::LogMessage( cuT( "Dx11Context :: DoInitialise - Context for window 0x%X initialised" ), m_hWnd );
			m_bInitialised = true;
		}

		if( m_bInitialised )
		{
			ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
			OneTextureFrameVariableSPtr l_pVariable = l_pProgram->CreateFrameVariable( cuT( "c3d_mapDiffuse" ), eSHADER_TYPE_PIXEL );
			l_pProgram->SetEntryPoint( eSHADER_TYPE_VERTEX,	cuT( "mainVx" ) );
			l_pProgram->SetEntryPoint( eSHADER_TYPE_PIXEL,	cuT( "mainPx" ) );
			l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL_4, ShaderModel1_2_3_4::VtxShader 	);
			l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL_4, ShaderModel1_2_3_4::PxlShader	);
			l_pProgram->Initialise();

			m_pGeometryBuffers->GetIndexBuffer()->Create();
			m_pGeometryBuffers->GetVertexBuffer()->Create();
			m_pGeometryBuffers->GetIndexBuffer()->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
			m_pGeometryBuffers->GetVertexBuffer()->Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
			m_pGeometryBuffers->Initialise();
		}
	}

	return m_bInitialised;
}

void DxContext :: DoCleanup()
{
	DoFreeVolatileResources();
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem* >( m_pRenderSystem );
}

void DxContext :: DoSetCurrent()
{
}

void DxContext :: DoEndCurrent()
{
}

void DxContext :: DoSwapBuffers()
{
	if( m_pSwapChain )
	{
		if( m_pWindow->GetVSync() )
		{
			m_pSwapChain->Present( 1, 0 );
		}
		else
		{
			m_pSwapChain->Present( 0, 0 );
		}
	}
}

void DxContext :: DoSetClearColour( Colour const & p_clrClear )
{
	m_fClearColour[0] = p_clrClear.red().value();
	m_fClearColour[1] = p_clrClear.green().value();
	m_fClearColour[2] = p_clrClear.blue().value();
	m_fClearColour[3] = p_clrClear.alpha().value();
}

void DxContext :: DoClear( uint32_t p_uiTargets )
{
	ID3D11DeviceContext * l_pDeviceContext;
	reinterpret_cast< DxRenderSystem* >( m_pRenderSystem )->GetDevice()->GetImmediateContext( &l_pDeviceContext );

	if( p_uiTargets & eBUFFER_COMPONENT_COLOUR )
	{
		l_pDeviceContext->ClearRenderTargetView( m_pRenderTargetView, m_fClearColour );
	}

	if( (p_uiTargets & eBUFFER_COMPONENT_DEPTH) || (p_uiTargets & eBUFFER_COMPONENT_STENCIL) )
	{
		UINT l_uiFlags = 0;

		if( p_uiTargets & eBUFFER_COMPONENT_DEPTH )
		{
			l_uiFlags |= D3D11_CLEAR_DEPTH;
		}

		if( p_uiTargets & eBUFFER_COMPONENT_STENCIL )
		{
			l_uiFlags |= D3D11_CLEAR_STENCIL;
		}

		l_pDeviceContext->ClearDepthStencilView( m_pDepthStencilView, l_uiFlags, 0, 0 );
	}

	l_pDeviceContext->Release();
}

void DxContext :: DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget )
{
	if( p_eTarget == eFRAMEBUFFER_TARGET_DRAW )
	{
		if( p_eBuffer == eBUFFER_BACK || p_eBuffer == eBUFFER_BACK_LEFT || p_eBuffer == eBUFFER_BACK_RIGHT )
		{
			ID3D11DeviceContext * l_pDeviceContext;
			reinterpret_cast< DxRenderSystem* >( m_pRenderSystem )->GetDevice()->GetImmediateContext( &l_pDeviceContext );
			l_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
			l_pDeviceContext->Release();
		}
	}
	else if( p_eTarget == eFRAMEBUFFER_TARGET_READ )
	{
	}
}

void DxContext :: DoSetAlphaFunc( eALPHA_FUNC CU_PARAM_UNUSED( p_eFunc ), uint8_t CU_PARAM_UNUSED( p_byValue ) )
{
	// Dropped out.
}

void DxContext :: DoInitVolatileResources()
{
	DxRenderSystem * l_pRenderSystem = static_cast< DxRenderSystem* >( m_pRenderSystem );
	DxContextSPtr l_pMainContext = std::static_pointer_cast< DxContext >( l_pRenderSystem->GetMainContext() );
	IDXGIFactory * l_pFactory = l_pRenderSystem->GetDXGIFactory();
	ID3D11Texture2D * l_pDSTex = NULL;
	ID3D11Texture2D * l_pRTTex = NULL;
	bool l_bContinue = false;

	if( l_pFactory )
	{
		HRESULT l_hr = l_pFactory->CreateSwapChain( l_pRenderSystem->GetDevice(), &m_deviceParams, &m_pSwapChain );
		dxDebugName( m_pSwapChain, SwapChain );
		l_bContinue = dxCheckError( l_hr, "CreateSwapChain" );

		if( l_bContinue )
		{
			l_hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void** >( &l_pRTTex ) );
			l_bContinue = dxCheckError( l_hr, "SwapChain::GetBuffer" );
		}
		
		if( l_bContinue )
		{
			l_hr = l_pRenderSystem->GetDevice()->CreateRenderTargetView( l_pRTTex, NULL, &m_pRenderTargetView );
			dxDebugName( m_pRenderTargetView, ContextRTView );
			l_bContinue = dxCheckError( l_hr, "CreateRenderTargetView" );
		}

		SafeRelease( l_pRTTex );
		
		if( l_bContinue )
		{
			// Create depth stencil texture
			D3D11_TEXTURE2D_DESC l_descDepth	= { 0 };
			l_descDepth.Width					= m_deviceParams.BufferDesc.Width;
			l_descDepth.Height					= m_deviceParams.BufferDesc.Height;
			l_descDepth.MipLevels				= 1;
			l_descDepth.ArraySize				= 1;
			l_descDepth.Format					= DXGI_FORMAT_D24_UNORM_S8_UINT;
			l_descDepth.SampleDesc.Count		= 1;
			l_descDepth.SampleDesc.Quality		= 0;
			l_descDepth.Usage					= D3D11_USAGE_DEFAULT;
			l_descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL;
			l_descDepth.CPUAccessFlags			= 0;
			l_descDepth.MiscFlags				= 0;

			l_hr = l_pRenderSystem->GetDevice()->CreateTexture2D( &l_descDepth, NULL, &l_pDSTex );
			l_bContinue = dxCheckError( l_hr, "CreateTexture2D" );
		}

		if( l_bContinue )
		{
			// Create the depth stencil view
			D3D11_DEPTH_STENCIL_VIEW_DESC l_descDSV = D3D11_DEPTH_STENCIL_VIEW_DESC();
			l_descDSV.Format              = DXGI_FORMAT_D24_UNORM_S8_UINT;
			l_descDSV.ViewDimension       = D3D11_DSV_DIMENSION_TEXTURE2D;
			l_descDSV.Texture2D.MipSlice  = 0;

			l_hr = l_pRenderSystem->GetDevice()->CreateDepthStencilView( l_pDSTex, &l_descDSV, &m_pDepthStencilView );
			dxDebugName( m_pDepthStencilView, ContextDSView );
			l_bContinue = dxCheckError( l_hr, "CreateDepthStencilView" );
		}

		SafeRelease( l_pDSTex );
		
		if( l_bContinue )
		{
			ID3D11DeviceContext * l_pDeviceContext;
			reinterpret_cast< DxRenderSystem* >( m_pRenderSystem )->GetDevice()->GetImmediateContext( &l_pDeviceContext );
			l_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
			l_pDeviceContext->Release();
			l_bContinue = dxCheckError( l_hr, "OMSetRenderTargets" );
		}
	}
}

void DxContext :: DoFreeVolatileResources()
{
	if( m_pSwapChain )
	{
		m_pSwapChain->SetFullscreenState( false, NULL );
	}

	SafeRelease( m_pDepthStencilView );
	SafeRelease( m_pRenderTargetView );
	SafeRelease( m_pSwapChain );
}

HRESULT DxContext :: DoInitPresentParameters()
{
	HRESULT l_hr = E_FAIL;
	std::memset( &m_deviceParams, 0, sizeof( m_deviceParams ) );

	m_deviceParams.Windowed								= TRUE;
	m_deviceParams.BufferCount							= 1;
	m_deviceParams.BufferDesc.Width						= ::GetSystemMetrics( SM_CXFULLSCREEN );
	m_deviceParams.BufferDesc.Height					= ::GetSystemMetrics( SM_CYFULLSCREEN );
	m_deviceParams.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	m_deviceParams.BufferDesc.RefreshRate.Numerator		= 60;
	m_deviceParams.BufferDesc.RefreshRate.Denominator	= 1;
    m_deviceParams.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    m_deviceParams.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	m_deviceParams.SampleDesc.Count						= 1;
	m_deviceParams.SampleDesc.Quality					= 0;
	m_deviceParams.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_deviceParams.OutputWindow							= m_hWnd;
	m_deviceParams.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	
	if( m_pWindow->IsFullscreen() )
	{
		m_deviceParams.Windowed							= FALSE;
	}
	else
	{
		m_deviceParams.Windowed							= TRUE;
	}

	l_hr = S_OK;
	return l_hr;
}

void DxContext :: DoCullFace( eFACE CU_PARAM_UNUSED( p_eCullFace ) )
{
}

//*************************************************************************************************
