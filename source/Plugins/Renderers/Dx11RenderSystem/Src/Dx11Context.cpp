#include "Dx11Context.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11BackBuffers.hpp"

#include <Engine.hpp>
#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <OneFrameVariable.hpp>
#include <PlatformWindowHandle.hpp>
#include <RenderLoop.hpp>
#include <RenderTarget.hpp>
#include <RenderWindow.hpp>
#include <Texture.hpp>
#include <VertexBuffer.hpp>

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
		cuT( "	return float4( diffuseTexture.Sample( diffuseSampler, p_input.TextureUV ).xyz, 1.0 );\n" )
		cuT( "}\n" );
}

namespace Dx11Render
{
	DxContext::DxContext( DxRenderSystem & p_renderSystem )
		: Context( p_renderSystem, true )
		, m_swapChain( NULL )
		, m_pDeviceContext( NULL )
	{
	}

	DxContext::~DxContext()
	{
	}

	void DxContext::UpdateFullScreen( bool p_value )
	{
		if ( p_value )
		{
			m_swapChain->SetFullscreenState( TRUE, NULL );
		}
		else
		{
			m_swapChain->SetFullscreenState( FALSE, NULL );
		}
	}

	bool DxContext::DoInitialise()
	{
		if ( !m_initialised )
		{
			DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( GetOwner() );
			m_hWnd = m_window->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
			m_size = m_window->GetSize();

			if ( DoInitPresentParameters() == S_OK &&  l_renderSystem->InitialiseDevice( m_hWnd, m_deviceParams ) )
			{
				m_window->Resize( m_deviceParams.BufferDesc.Width, m_deviceParams.BufferDesc.Height );
				m_size = m_window->GetSize();
				DoInitVolatileResources();
				Logger::LogInfo( StringStream() << cuT( "Dx11Context::DoInitialise - Context for window 0x" ) << std::hex << m_hWnd << cuT( " initialised" ) );
				m_initialised = true;
			}

			if ( m_initialised )
			{
				auto l_uniforms = UniformsBase::Get( *l_renderSystem );
				StringStream l_vtxShader;
				l_vtxShader << l_uniforms->GetVertexInMatrices( 0 ) << std::endl;
				l_vtxShader << ShaderModel1_2_3_4::VtxShader;
				ShaderProgramBaseSPtr l_program = m_renderTextureProgram.lock();
				l_program->SetEntryPoint( eSHADER_TYPE_VERTEX, cuT( "mainVx" ) );
				l_program->SetEntryPoint( eSHADER_TYPE_PIXEL, cuT( "mainPx" ) );
				l_program->SetSource( eSHADER_TYPE_VERTEX, eSHADER_MODEL_5, l_vtxShader.str() );
				l_program->SetSource( eSHADER_TYPE_PIXEL, eSHADER_MODEL_5, ShaderModel1_2_3_4::PxlShader );
			}
		}

		return m_initialised;
	}

	void DxContext::DoCleanup()
	{
		DoFreeVolatileResources();
	}

	void DxContext::DoSetCurrent()
	{
		static_cast< DxRenderSystem * >( GetOwner() )->GetDevice()->GetImmediateContext( &m_pDeviceContext );
	}

	void DxContext::DoEndCurrent()
	{
	}

	void DxContext::DoSwapBuffers()
	{
		if ( m_swapChain )
		{
			if ( m_window->GetVSync() )
			{
				m_swapChain->Present( 1, 0 );
			}
			else
			{
				m_swapChain->Present( 0, 0 );
			}
		}
	}

	void DxContext::DoSetAlphaFunc( eALPHA_FUNC CU_PARAM_UNUSED( p_func ), uint8_t CU_PARAM_UNUSED( p_value ) )
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
		m_deviceParams.BufferDesc.Width = m_window->GetSize().width();
		m_deviceParams.BufferDesc.Height = m_window->GetSize().height();
		m_deviceParams.BufferDesc.Format = DirectX11::Get( m_window->GetRenderTarget()->GetPixelFormat() );
		m_deviceParams.BufferDesc.RefreshRate.Numerator = GetOwner()->GetOwner()->GetRenderLoop().GetWantedFps();
		m_deviceParams.BufferDesc.RefreshRate.Denominator = 1;
		m_deviceParams.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_deviceParams.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		m_deviceParams.SampleDesc.Count = 1;
		m_deviceParams.SampleDesc.Quality = 0;
		m_deviceParams.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		m_deviceParams.OutputWindow = m_hWnd;
		m_deviceParams.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		if ( m_window->IsFullscreen() )
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
