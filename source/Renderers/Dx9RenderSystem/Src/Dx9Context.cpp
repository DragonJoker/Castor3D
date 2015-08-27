#include "Dx9Context.hpp"
#include "Dx9RenderSystem.hpp"

#include <Material.hpp>
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
		cuT( "uniform	float4x4	c3d_mtxProjection;\n"	)
		cuT( "uniform	float4x4	c3d_mtxModel;\n"	)
		cuT( "uniform	float4x4	c3d_mtxView;\n"	)
		cuT( "uniform	float4x4	c3d_mtxModelView;\n"	)
		cuT( "uniform	float4x4	c3d_mtxProjectionModelView;\n"	)
		cuT( "uniform	float4x4	c3d_mtxNormal;\n"	)
		cuT( "uniform	float4x4	c3d_mtxTexture0;\n"	)
		cuT( "uniform	float4x4	c3d_mtxTexture1;\n"	)
		cuT( "uniform	float4x4	c3d_mtxTexture2;\n"	)
		cuT( "uniform	float4x4	c3d_mtxTexture3;\n"	)
		cuT( "struct VtxInput\n"	)
		cuT( "{\n"	)
		cuT( "	float4	Position	:	POSITION;\n"	)
		cuT( "	float2	TextureUV	:	TEXCOORD0;\n"	)
		cuT( "};\n"	)
		cuT( "struct VtxOutput\n"	)
		cuT( "{\n"	)
		cuT( "	float4	Position	:	POSITION0;\n"	)
		cuT( "	float2	TextureUV	:	TEXCOORD0;\n"	)
		cuT( "};\n"	)
		cuT( "VtxOutput mainVx( in VtxInput p_input )\n"	)
		cuT( "{\n"	)
		cuT( "	VtxOutput l_output;\n"	)
		cuT( "	l_output.Position		= mul( p_input.Position, c3d_mtxProjectionModelView );\n"	)
		cuT( "	l_output.TextureUV		= p_input.TextureUV;\n"	)
		cuT( "	return l_output;\n"	)
		cuT( "}\n"	)
		cuT( "struct PxlInput\n"	)
		cuT( "{\n"	)
		cuT( "	float4	Position	:	POSITION0;\n"	)
		cuT( "	float2	TextureUV	:	TEXCOORD0;\n"	)
		cuT( "};\n"	)
		cuT( "texture diffuseTexture : register( t0 );\n"	)
		cuT( "sampler DiffuseSampler\n"	)
		cuT( "{\n"	)
		cuT( "	Texture = < diffuseTexture >;\n"	)
		cuT( "};\n"	)
		cuT( "float4 mainPx( in PxlInput p_input ) : COLOR0\n"	)
		cuT( "{\n"	)
		cuT( "	return tex2D( DiffuseSampler, p_input.TextureUV );\n"	)
		cuT( "}\n"	)
		cuT( "technique RenderPass\n"	)
		cuT( "{\n"	)
		cuT( "	pass p0\n"	)
		cuT( "	{\n"	)
		cuT( "		CullMode=none;\n"	)
		cuT( "		PixelShader = compile ps_3_0 mainPx();\n"	)
		cuT( "		VertexShader = compile vs_3_0 mainVx();\n"	)
		cuT( "	}\n"	)
		cuT( "};\n"	);
}

namespace
{
	HRESULT LookUpDisplayMode( IDirect3D9 * p_pD3D9, D3DFORMAT p_eFormat, UINT p_uiCount, Size const & p_sizeWanted, D3DDISPLAYMODE & p_mode )
	{
		HRESULT l_hr = S_FALSE;
		D3DDISPLAYMODE l_selDisplayMode = { 0 };
		UINT l_uiMode = 0;

		while ( l_hr != S_OK && l_uiMode < p_uiCount )
		{
			l_hr = p_pD3D9->EnumAdapterModes( D3DADAPTER_DEFAULT, p_eFormat, l_uiMode++, &l_selDisplayMode );

			if ( l_selDisplayMode.Width < p_sizeWanted.width() || l_selDisplayMode.Height < p_sizeWanted.height() )
			{
				// The display format isn't large enough for the window
				l_hr = S_FALSE;

				if ( p_mode.Width < l_selDisplayMode.Width && p_mode.Height < l_selDisplayMode.Height )
				{
					// we store it if the dimensions are greater to the ones already stored
					p_mode = l_selDisplayMode;
				}
			}
		}

		return l_hr;
	}

	bool InitialisePresentParameters( D3DDISPLAYMODE const & p_selDisplayMode, HWND p_hWnd, RenderWindowRPtr p_pWindow, Dx9Render::DxRenderSystem * p_pRenderSystem, D3DPRESENT_PARAMETERS & p_presentParameters )
	{
		bool l_bReturn = false;
		bool l_bFullscreen = false;
		// Initialising the device present parameters
		p_presentParameters.BackBufferCount					= 1;
		p_presentParameters.BackBufferWidth					= p_selDisplayMode.Width;
		p_presentParameters.BackBufferHeight				= p_selDisplayMode.Height;
		p_presentParameters.BackBufferFormat				= p_selDisplayMode.Format;
		p_presentParameters.AutoDepthStencilFormat			= D3DFMT_D24S8;
		p_presentParameters.EnableAutoDepthStencil			= TRUE;
		p_presentParameters.hDeviceWindow					= p_hWnd;

		if ( p_pWindow->IsFullscreen() )
		{
			p_presentParameters.Windowed					= FALSE;
			p_presentParameters.SwapEffect					= D3DSWAPEFFECT_FLIP;
			p_presentParameters.FullScreen_RefreshRateInHz	= p_selDisplayMode.RefreshRate;

			if ( p_pWindow->GetVSync() )
			{
				p_presentParameters.PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT;
			}
			else
			{
				p_presentParameters.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
			}

			l_bFullscreen = p_pRenderSystem->InitialiseDevice( p_hWnd, &p_presentParameters );
			l_bReturn = l_bFullscreen;
		}
		else
		{
			l_bFullscreen = false;
		}

		if ( !l_bFullscreen )
		{
			p_pWindow->SetFullscreen( false );
			p_presentParameters.SwapEffect					= D3DSWAPEFFECT_DISCARD;
			p_presentParameters.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
			p_presentParameters.Windowed					= TRUE;
			p_presentParameters.FullScreen_RefreshRateInHz	= 0;
			l_bReturn = p_pRenderSystem->InitialiseDevice( p_hWnd, &p_presentParameters );
		}

		return l_bReturn;
	}
}

namespace Dx9Render
{
	eFACE DxContext::sm_eCullFace = eFACE_FRONT;

	DxContext::DxContext()
		: Context()
		, m_pSwapChain( NULL )
		, m_pBackBuffer( NULL )
		, m_dwClearColour( 0 )
		, m_pDevice( NULL )
	{
	}

	DxContext::~DxContext()
	{
		SafeRelease( m_pBackBuffer );
		SafeRelease( m_pSwapChain );
	}

	void DxContext::UpdateFullScreen( bool CU_PARAM_UNUSED( p_bVal ) )
	{
	}

	bool DxContext::DoInitialise()
	{
		if ( !m_bInitialised )
		{
			DxRenderSystem *	l_pRenderSystem		= static_cast< DxRenderSystem * >( m_pRenderSystem );
			HRESULT				l_hr				= E_FAIL;

			if ( !l_pRenderSystem->IsInitialised() )
			{
				l_pRenderSystem->Initialise();
				l_pRenderSystem->GetEngine()->GetMaterialManager().Initialise();
			}

			m_hWnd = m_pWindow->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
			m_size = Size( GetSystemMetrics( SM_CXFULLSCREEN ), GetSystemMetrics( SM_CYFULLSCREEN ) );
			m_size = m_pWindow->GetSize();
			UINT l_uiCount = l_pRenderSystem->GetD3dObject()->GetAdapterModeCount( D3DADAPTER_DEFAULT, DirectX9::Get( m_pWindow->GetPixelFormat() ) );
			D3DDISPLAYMODE l_defDisplayMode = { 0 };

			if ( l_uiCount > 0 )
			{
				// We first try to find a device format (size and pixel format) corresponding to the one asked
				l_hr = LookUpDisplayMode( l_pRenderSystem->GetD3dObject(), DirectX9::Get( m_pWindow->GetPixelFormat() ), l_uiCount, m_size, l_defDisplayMode );
			}

			if ( l_hr != S_OK && l_defDisplayMode.Width == 0 && l_defDisplayMode.Height == 0 )
			{
				D3DDISPLAYMODE l_d3dDisplayMode = { 0 };
				// Failed to find the device format corresponding to the asked pixel format, we try with current screen pixel format
				l_hr = l_pRenderSystem->GetD3dObject()->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &l_d3dDisplayMode );
				l_uiCount = l_pRenderSystem->GetD3dObject()->GetAdapterModeCount( D3DADAPTER_DEFAULT, l_d3dDisplayMode.Format );

				if ( l_uiCount > 0 )
				{
					l_hr = LookUpDisplayMode( l_pRenderSystem->GetD3dObject(), l_d3dDisplayMode.Format, l_uiCount, m_size, l_defDisplayMode );
				}
			}

			D3DDISPLAYMODE l_selDisplayMode = { 0 };

			if ( l_defDisplayMode.Width != 0 && l_defDisplayMode.Height != 0 )
			{
				// No exact display mode but an approximate one, we then select it
				l_hr = S_OK;
				l_selDisplayMode = l_defDisplayMode;
			}

			if ( l_hr == S_OK )
			{
				D3DPRESENT_PARAMETERS l_presentParameters = { 0 };
				bool l_bInitialised = InitialisePresentParameters( l_selDisplayMode, m_hWnd, m_pWindow, l_pRenderSystem, l_presentParameters );

				if ( l_bInitialised )
				{
					DxContextSPtr l_pMainContext = std::static_pointer_cast< DxContext >( l_pRenderSystem->GetMainContext() );
					m_pDevice = l_pRenderSystem->GetDevice();

					if ( !l_pMainContext )
					{
						m_pDevice->GetSwapChain( 0, &m_pSwapChain );
					}
					else
					{
						m_pDevice->CreateAdditionalSwapChain( &l_presentParameters, &m_pSwapChain );
					}

					m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer );
					D3DSURFACE_DESC desc;

					if ( SUCCEEDED( m_pBackBuffer->GetDesc( &desc ) ) )
					{
						Logger::LogMessage( cuT( "DxContext::DoInitialise - Back buffer dimensions : %dx%d" ), desc.Width, desc.Height );
						m_size.set( desc.Width, desc.Height );
					}

					Logger::LogMessage( cuT( "DxContext::DoInitialise - Context for window 0x%X initialised" ), m_hWnd );
					m_bInitialised = true;
				}

				if ( m_bInitialised )
				{
					ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
					OneTextureFrameVariableSPtr l_pVariable = l_pProgram->CreateFrameVariable( cuT( "c3d_mapDiffuse" ), eSHADER_TYPE_PIXEL );
					l_pProgram->SetEntryPoint( eSHADER_TYPE_VERTEX,	cuT( "mainVx" ) );
					l_pProgram->SetEntryPoint( eSHADER_TYPE_PIXEL,	cuT( "mainPx" ) );
					l_pProgram->SetSource( eSHADER_TYPE_VERTEX,	eSHADER_MODEL_2, ShaderModel1_2_3_4::VtxShader	);
					l_pProgram->SetSource( eSHADER_TYPE_PIXEL,	eSHADER_MODEL_2, ShaderModel1_2_3_4::VtxShader	);
					l_pProgram->Initialise();
					m_pGeometryBuffers->GetIndexBuffer().Create();
					m_pGeometryBuffers->GetVertexBuffer().Create();
					m_pGeometryBuffers->GetIndexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
					m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
					m_pGeometryBuffers->Initialise();
				}
			}
		}

		return m_bInitialised;
	}

	void DxContext::DoCleanup()
	{
		SafeRelease( m_pBackBuffer );
		SafeRelease( m_pSwapChain );
	}

	void DxContext::DoSetCurrent()
	{
		if ( m_pBackBuffer )
		{
			m_pDevice->SetRenderTarget( 0, m_pBackBuffer );
		}
	}

	void DxContext::DoEndCurrent()
	{
	}

	void DxContext::DoSwapBuffers()
	{
		if ( m_pSwapChain )
		{
			//		m_pDevice->SetRenderTarget( 0, m_pBackBuffer );
			m_pSwapChain->Present( NULL, NULL, m_hWnd, NULL, 0 );
		}
	}

	void DxContext::DoSetClearColour( Colour const & p_clrClear )
	{
		m_dwClearColour = p_clrClear.to_argb();
	}

	void DxContext::DoClear( uint32_t p_uiTargets )
	{
		if ( m_pWindow->GetPixelFormat() == ePIXEL_FORMAT_STENCIL1 || m_pWindow->GetPixelFormat() == ePIXEL_FORMAT_STENCIL8 )
		{
			m_pDevice->Clear( 0, NULL, DirectX9::GetComponents( p_uiTargets ), m_dwClearColour, 1.0f, 0x00 );
		}
		else
		{
			m_pDevice->Clear( 0, NULL, DirectX9::GetComponents( ( p_uiTargets & eBUFFER_COMPONENT_COLOUR ) | ( p_uiTargets & eBUFFER_COMPONENT_DEPTH ) ), m_dwClearColour, 1.0f, 0x00 );
		}
	}

	void DxContext::DoBind( Castor3D::eBUFFER p_eBuffer, Castor3D::eFRAMEBUFFER_TARGET p_eTarget )
	{
		if ( p_eTarget == eFRAMEBUFFER_TARGET_DRAW )
		{
			if ( m_pBackBuffer && ( p_eBuffer == eBUFFER_BACK || p_eBuffer == eBUFFER_BACK_LEFT || p_eBuffer == eBUFFER_BACK_RIGHT ) )
			{
				m_pDevice->SetRenderTarget( 0, m_pBackBuffer );
			}
		}
		else if ( p_eTarget == eFRAMEBUFFER_TARGET_READ )
		{
		}
	}

	void DxContext::DoSetAlphaFunc( eALPHA_FUNC p_eFunc, uint8_t p_byValue )
	{
		m_pDevice->SetRenderState( D3DRS_ALPHAFUNC, DirectX9::Get( p_eFunc )	);
		m_pDevice->SetRenderState( D3DRS_ALPHAREF,	DWORD( p_byValue )	);
	}

	void DxContext::DoCullFace( eFACE p_eCullFace )
	{
		if ( sm_eCullFace != p_eCullFace )
		{
			sm_eCullFace = p_eCullFace;
			m_pDevice->SetRenderState( D3DRS_CULLMODE, DWORD( DirectX9::Get( p_eCullFace ) ) );
		}
	}
}
