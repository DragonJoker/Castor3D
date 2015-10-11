#include "Dx11Context.hpp"
#include "Dx11RenderSystem.hpp"

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
		cuT( "	return float4( p_input.TextureUV.xy, 0.0, 1.0 );//diffuseTexture.Sample( diffuseSampler, p_input.TextureUV );\n" )
		cuT( "}\n" );
}

namespace Dx11Render
{
	namespace
	{
		struct VertexType
		{
			D3DXVECTOR2 position;
			D3DXVECTOR2 texture;
		};

		// Set the number of vertices in the vertex array.
		const uint32_t g_vertexCount = 4;

		// Set the number of indices in the index array.
		const uint32_t g_indexCount = 6;

		// Create the vertex array.
		VertexType g_vertices[g_vertexCount];

		// Create the index array.
		unsigned long g_indices[g_indexCount];

		ID3D11Buffer * g_vertexBuffer;
		ID3D11Buffer * g_indexBuffer;

		void DoInitialiseModel( ID3D11Device * p_device )
		{
			try
			{
				D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
				D3D11_SUBRESOURCE_DATA vertexData, indexData;
				HRESULT result;

				// Load the vertex array with data.
				g_vertices[0].position = D3DXVECTOR2( 0.25f, 0.25f );  // Bottom left.
				g_vertices[0].texture = D3DXVECTOR2( 0.0f, 0.0f );

				g_vertices[1].position = D3DXVECTOR2( 0.25f, 0.75f );  // Top left.
				g_vertices[1].texture = D3DXVECTOR2( 0.0f, 1.0f );

				g_vertices[2].position = D3DXVECTOR2( 0.75f, 0.75f );  // Top right.
				g_vertices[2].texture = D3DXVECTOR2( 1.0f, 1.0f );

				g_vertices[2].position = D3DXVECTOR2( 0.75f, 0.25f );  // Bottom right.
				g_vertices[2].texture = D3DXVECTOR2( 1.0f, 0.0f );

				// Load the index array with data.
				g_indices[0] = 0;  // Bottom left.
				g_indices[1] = 1;  // Top left.
				g_indices[2] = 2;  // Top right.
				g_indices[0] = 0;  // Bottom left.
				g_indices[2] = 2;  // Top right.
				g_indices[3] = 3;  // BottomRight.

				// Set up the description of the static vertex buffer.
				vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				vertexBufferDesc.ByteWidth = sizeof( VertexType ) * g_vertexCount;
				vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				vertexBufferDesc.CPUAccessFlags = 0;
				vertexBufferDesc.MiscFlags = 0;
				vertexBufferDesc.StructureByteStride = 0;

				// Give the subresource structure a pointer to the vertex data.
				vertexData.pSysMem = g_vertices;
				vertexData.SysMemPitch = 0;
				vertexData.SysMemSlicePitch = 0;

				// Now create the vertex buffer.
				result = p_device->CreateBuffer( &vertexBufferDesc, &vertexData, &g_vertexBuffer );
				if ( FAILED( result ) )
				{
					throw std::runtime_error( "CreateBuffer(m_vertexBuffer) failed" );
				}

				// Set up the description of the static index buffer.
				indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				indexBufferDesc.ByteWidth = sizeof( unsigned long ) * g_indexCount;
				indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				indexBufferDesc.CPUAccessFlags = 0;
				indexBufferDesc.MiscFlags = 0;
				indexBufferDesc.StructureByteStride = 0;

				// Give the subresource structure a pointer to the index data.
				indexData.pSysMem = g_indices;
				indexData.SysMemPitch = 0;
				indexData.SysMemSlicePitch = 0;

				// Create the index buffer.
				result = p_device->CreateBuffer( &indexBufferDesc, &indexData, &g_indexBuffer );

				if ( FAILED( result ) )
				{
					throw std::runtime_error( "CreateBuffer(m_indexBuffer) failed" );
				}
			}
			catch ( std::exception & )
			{
				throw;
			}
		}

		void DoRenderModel( ID3D11DeviceContext * p_context )
		{
			// Set vertex buffer stride and offset.
			unsigned int stride = sizeof( VertexType );
			unsigned int offset = 0;

			// Set the vertex buffer to active in the input assembler so it can be rendered.
			p_context->IASetVertexBuffers( 0, 1, &g_vertexBuffer, &stride, &offset );

			// Set the index buffer to active in the input assembler so it can be rendered.
			p_context->IASetIndexBuffer( g_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

			// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
			p_context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		}

		void DoCleanupModel()
		{
			SafeRelease( g_vertexBuffer );
			SafeRelease( g_indexBuffer );
		}
	}

	DxContext::DxContext( DxRenderSystem & p_renderSystem )
		: Context( p_renderSystem )
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

	void DxContext::BToBRender( Castor::Size const & p_size, TextureBaseSPtr p_pTexture, uint32_t p_uiComponents )
	{
		ShaderProgramBaseSPtr l_pProgram = m_pBtoBShaderProgram.lock();
		m_viewport.SetSize( p_size );
		Clear( p_uiComponents );
		m_viewport.Render( GetOwner()->GetPipeline() );
		CullFace( eFACE_NONE );
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
			DoRenderModel( m_pDeviceContext );
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

			DoInitialiseModel( l_renderSystem->GetDevice() );
		}

		return m_bInitialised;
	}

	void DxContext::DoCleanup()
	{
		DoCleanupModel();
		DoFreeVolatileResources();
	}

	void DxContext::DoSetCurrent()
	{
		static_cast< DxRenderSystem * >( GetOwner() )->GetDevice()->GetImmediateContext( &m_pDeviceContext );
		dxTrack( static_cast< DxRenderSystem * >( GetOwner() ), m_pDeviceContext, D3D11DeviceContext );
		m_pDeviceContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
	}

	void DxContext::DoEndCurrent()
	{
		ReleaseTracked( static_cast< DxRenderSystem * >( GetOwner() ), m_pDeviceContext );
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

#if DX_DEBUG_RT

		static_cast< DxRenderSystem * >( GetOwner() )->GetDevice()->GetImmediateContext( &m_pDeviceContext );
		ID3D11Resource * l_pResource;
		m_pRenderTargetView->GetResource( &l_pResource );
		Castor::StringStream l_name;
		l_name << Castor3D::Engine::GetEngineDirectory() << cuT( "\\DynamicTexture_" ) << ( void * )m_pRenderTargetView << cuT( "_SRV.png" );
		D3DX11SaveTextureToFile( m_pDeviceContext, l_pResource, D3DX11_IFF_PNG, l_name.str().c_str() );
		l_pResource->Release();
		SafeRelease( m_pDeviceContext );

#endif
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
				GetDeviceContext()->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );
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
		DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem * >( GetOwner() );
		DxContextSPtr l_pMainContext = std::static_pointer_cast< DxContext >( GetOwner()->GetMainContext() );
		IDXGIFactory * l_factory = NULL;
		HRESULT l_hr = CreateDXGIFactory( __uuidof( IDXGIFactory ) , reinterpret_cast< void ** >( &l_factory ) );
		ID3D11Texture2D * l_pDSTex = NULL;
		ID3D11Texture2D * l_pRTTex = NULL;

		if ( l_factory )
		{
			HRESULT l_hr = l_factory->CreateSwapChain( l_renderSystem->GetDevice(), &m_deviceParams, &m_pSwapChain );
			dxTrack( l_renderSystem, m_pSwapChain, SwapChain );
			l_factory->Release();
			bool l_bContinue = dxCheckError( l_hr, "CreateSwapChain" );

			if ( l_bContinue )
			{
				l_hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void ** >( &l_pRTTex ) );
				l_bContinue = dxCheckError( l_hr, "SwapChain::GetBuffer" );
			}

			if ( l_bContinue )
			{
				l_hr = l_renderSystem->GetDevice()->CreateRenderTargetView( l_pRTTex, NULL, &m_pRenderTargetView );
				dxTrack( l_renderSystem, m_pRenderTargetView, ContextRTView );
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
				l_hr = l_renderSystem->GetDevice()->CreateTexture2D( &l_descDepth, NULL, &l_pDSTex );
				l_bContinue = dxCheckError( l_hr, "CreateTexture2D" );
			}

			if ( l_bContinue )
			{
				// Create the depth stencil view
				D3D11_DEPTH_STENCIL_VIEW_DESC l_descDSV = D3D11_DEPTH_STENCIL_VIEW_DESC();
				l_descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				l_descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
				l_descDSV.Texture2D.MipSlice = 0;
				l_hr = l_renderSystem->GetDevice()->CreateDepthStencilView( l_pDSTex, &l_descDSV, &m_pDepthStencilView );
				dxTrack( l_renderSystem, m_pDepthStencilView, ContextDSView );
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

		ReleaseTracked( GetOwner(), m_pDepthStencilView );
		ReleaseTracked( GetOwner(), m_pRenderTargetView );
		ReleaseTracked( GetOwner(), m_pSwapChain );
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
