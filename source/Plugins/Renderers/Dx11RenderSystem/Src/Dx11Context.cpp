#include "Dx11Context.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11BackBuffers.hpp"

#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <VertexBuffer.hpp>
#include <RenderWindow.hpp>
#include <PlatformWindowHandle.hpp>
#include <OneFrameVariable.hpp>
#include <Texture.hpp>
#include <Logger.hpp>

#include <d3dx10math.h>

using namespace Castor3D;
using namespace Castor;

namespace ShaderModel1_2_3_4
{
	static const String VtxShader =
		cuT( "struct VtxInput\n" )
		cuT( "{\n" )
		cuT( "	float2 Position: POSITION;\n" )
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
		cuT( "	l_output.Position = mul( float4( p_input.Position, 0.0, 1.0 ), c3d_mtxProjection );\n" )
		cuT( "	l_output.TextureUV = p_input.TextureUV;\n" )
		cuT( "	return l_output;\n" )
		cuT( "}\n" );

	static const String PxlShader =
		cuT( "struct PxlInput\n" )
		cuT( "{\n" )
		cuT( "	float4 Position: SV_POSITION;\n" )
		cuT( "	float2 TextureUV: TEXCOORD0;\n" )
		cuT( "};\n" )
		cuT( "Texture2D diffuseTexture: register( t0 );\n" )
		cuT( "SamplerState diffuseSampler: register( s0 );\n" )
		cuT( "float4 mainPx( in PxlInput p_input ): SV_TARGET\n" )
		cuT( "{\n" )
		cuT( "	return diffuseTexture.Sample( diffuseSampler, p_input.TextureUV );\n" )
		cuT( "}\n" );
}

namespace Dx11Render
{
	DxContext::DxContext( DxRenderSystem & p_renderSystem )
		: Context( p_renderSystem )
		, m_swapChain( NULL )
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
			m_swapChain->SetFullscreenState( TRUE, NULL );
		}
		else
		{
			m_swapChain->SetFullscreenState( FALSE, NULL );
		}
	}

	void DxContext::BToBRender( Castor::Size const & p_size, TextureBaseSPtr p_pTexture )
	{
		ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
		m_viewport.SetSize( p_size );
		m_viewport.Render( GetOwner()->GetPipeline() );
		uint32_t l_id = p_pTexture->GetIndex();
		p_pTexture->SetIndex( 0 );

		if ( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			m_mapDiffuse->SetValue( p_pTexture.get() );
			FrameVariableBufferSPtr l_matrices = l_pProgram->FindFrameVariableBuffer( ShaderProgramBase::BufferMatrix );

			if ( l_matrices )
			{
				GetOwner()->GetPipeline().ApplyProjection( *l_matrices );
			}

			l_pProgram->Bind( 0, 1 );
		}

		if ( p_pTexture->BindAt( 0 ) )
		{
			m_pGeometryBuffers->Draw( eTOPOLOGY_TRIANGLES, l_pProgram, m_arrayVertex.size(), 0 );
			p_pTexture->UnbindFrom( 0 );
		}

		if ( l_pProgram && l_pProgram->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			l_pProgram->Unbind();
		}

		p_pTexture->SetIndex( l_id );
	}

	bool DxContext::DoInitialise()
	{
		if ( !m_bInitialised )
		{
			DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( GetOwner() );
			m_hWnd = m_pWindow->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
			m_size = m_pWindow->GetSize();

			if ( DoInitPresentParameters() == S_OK &&  l_renderSystem->InitialiseDevice( m_hWnd, m_deviceParams ) )
			{
				DoInitVolatileResources();
				Logger::LogInfo( StringStream() << cuT( "Dx11Context::DoInitialise - Context for window 0x" ) << std::hex << m_hWnd << cuT( " initialised" ) );
				m_bInitialised = true;
			}

			if ( m_bInitialised )
			{
				auto l_uniforms = UniformsBase::Get( *l_renderSystem );
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
		DoFreeVolatileResources();
	}

	void DxContext::DoSetCurrent()
	{
		static_cast< DxRenderSystem * >( GetOwner() )->GetDevice()->GetImmediateContext( &m_pDeviceContext );
		dxTrack( static_cast< DxRenderSystem * >( GetOwner() ), m_pDeviceContext, D3D11DeviceContext );
	}

	void DxContext::DoEndCurrent()
	{
		ReleaseTracked( static_cast< DxRenderSystem * >( GetOwner() ), m_pDeviceContext );
	}

	void DxContext::DoSwapBuffers()
	{
		if ( m_swapChain )
		{
			if ( m_pWindow->GetVSync() )
			{
				m_swapChain->Present( 1, 0 );
			}
			else
			{
				m_swapChain->Present( 0, 0 );
			}
		}
	}

	void DxContext::DoSetAlphaFunc( eALPHA_FUNC CU_PARAM_UNUSED( p_eFunc ), uint8_t CU_PARAM_UNUSED( p_byValue ) )
	{
		// Dropped out.
	}

	void DxContext::DoInitVolatileResources()
	{
		DoSetCurrent();
		DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( GetOwner() );
		IDXGIFactory * l_factory = NULL;
		HRESULT l_hr = CreateDXGIFactory( __uuidof( IDXGIFactory ), reinterpret_cast< void ** >( &l_factory ) );

		if ( dxCheckError( l_hr, "CreateDXGIFactory" ) && l_factory )
		{
			HRESULT l_hr = l_factory->CreateSwapChain( l_renderSystem->GetDevice(), &m_deviceParams, &m_swapChain );
			l_factory->Release();

			if ( dxCheckError( l_hr, "IDXGIFactory::CreateSwapChain" ) && m_swapChain )
			{
				dxTrack( l_renderSystem, m_swapChain, SwapChain );
			}
		}

		DoEndCurrent();
	}

	void DxContext::DoFreeVolatileResources()
	{
		if ( m_swapChain )
		{
			m_swapChain->SetFullscreenState( false, NULL );
		}

		ReleaseTracked( GetOwner(), m_swapChain );
	}

	HRESULT DxContext::DoInitPresentParameters()
	{
		std::memset( &m_deviceParams, 0, sizeof( m_deviceParams ) );
		m_deviceParams.Windowed = TRUE;
		m_deviceParams.BufferCount = 1;
		m_deviceParams.BufferDesc.Width = ::GetSystemMetrics( SM_CXFULLSCREEN );
		m_deviceParams.BufferDesc.Height = ::GetSystemMetrics( SM_CYFULLSCREEN );
		m_deviceParams.BufferDesc.Format = DirectX11::Get( m_pWindow->GetPixelFormat() );// DXGI_FORMAT_R8G8B8A8_UNORM;
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

		return S_OK;
	}

	void DxContext::DoCullFace( eFACE CU_PARAM_UNUSED( p_eCullFace ) )
	{
	}
}
