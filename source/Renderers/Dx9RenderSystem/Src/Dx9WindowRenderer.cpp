#include "Dx9WindowRenderer.hpp"
#include "Dx9Context.hpp"
#include "Dx9RenderSystem.hpp"

#include <Logger.hpp>
#include <Engine.hpp>
#include <MaterialManager.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx9Render
{
#if DX_DEBUG_2
	static DWORD const CUSTOMFVF = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

	struct CUSTOMVERTEX
	{
		FLOAT x, y, z, rhw;	// from the D3DFVF_XYZRHW flag
		DWORD color;		// from the D3DFVF_DIFFUSE flag
	};

	LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;
#endif

	DxWindowRenderer::DxWindowRenderer( DxRenderSystem * p_pRenderSystem )
		:	WindowRenderer( p_pRenderSystem	)
	{
	}

	DxWindowRenderer::~DxWindowRenderer()
	{
		m_context.reset();
	}

	bool DxWindowRenderer::SetCurrent()
	{
		bool l_bReturn = m_context.lock()->IsInitialised();

		if ( l_bReturn )
		{
			m_context.lock()->SetCurrent();
		}

		return l_bReturn;
	}

	void DxWindowRenderer::EndCurrent()
	{
		m_context.lock()->EndCurrent();
	}

	bool DxWindowRenderer::BeginScene()
	{
		m_context.lock()->SetCurrent();
#if DX_DEBUG
		IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();
		HRESULT l_hr = l_pDevice->BeginScene();
#endif
		return true;
	}

	bool DxWindowRenderer::EndScene()
	{
		/**/
#if DX_DEBUG
		HRESULT l_hr = S_OK;
#else
		IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();
		HRESULT l_hr = l_pDevice->BeginScene();
#endif
		/**/
		return l_hr == S_OK;
	}

	void DxWindowRenderer::EndRender()
	{
		IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();
		/**/
#if DX_DEBUG_2
		l_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0 );

		if ( v_buffer )
		{
			// select which vertex format we are using
			l_pDevice->SetFVF( CUSTOMFVF );

			// select the vertex buffer to display
			l_pDevice->SetStreamSource( 0, v_buffer, 0, sizeof( CUSTOMVERTEX ) );

			// copy the vertex buffer to the back buffer
			l_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
		}

#endif
		l_pDevice->EndScene();
		/**/
		m_context.lock()->EndCurrent();
		m_context.lock()->SwapBuffers();
	}

	bool DxWindowRenderer::DoInitialise()
	{
		m_context = std::static_pointer_cast< DxContext >( m_target->GetContext() );

		if ( !m_target->IsInitialised() )
		{
			Logger::LogInfo( cuT( "DxWindowRenderer::StartRender - Initialisation" ) );

			if ( !m_pRenderSystem->IsInitialised() )
			{
				m_pRenderSystem->Initialise();
				m_pRenderSystem->GetEngine()->GetMaterialManager().Initialise();
			}
		}

#if DX_DEBUG_2
		CUSTOMVERTEX OurVertices[] =
		{
			{ 320.0f, 50.0f, 1.0f, 1.0f, D3DCOLOR_XRGB( 0, 0, 255 ), },
			{ 520.0f, 400.0f, 1.0f, 1.0f, D3DCOLOR_XRGB( 0, 255, 0 ), },
			{ 120.0f, 400.0f, 1.0f, 1.0f, D3DCOLOR_XRGB( 255, 0, 0 ), },
		};

		IDirect3DDevice9 * l_pDevice = static_cast< DxRenderSystem * >( m_pRenderSystem )->GetDevice();
		l_pDevice->CreateVertexBuffer(	sizeof( OurVertices ),
										0,
										CUSTOMFVF,
										D3DPOOL_MANAGED,
										&v_buffer,
										NULL );

		VOID * pVoid;    // the void pointer
		v_buffer->Lock( 0, 0, reinterpret_cast< void ** >( &pVoid ), 0 );    // locks v_buffer, the buffer we made earlier
		memcpy( pVoid, OurVertices, sizeof( OurVertices ) );    // copy vertices to the vertex buffer
		v_buffer->Unlock();    // unlock v_buffer
#endif

		return m_target->IsInitialised();
	}

	void DxWindowRenderer::DoCleanup()
	{
#if DX_DEBUG_2
		v_buffer->Release();    // close and release the vertex buffer
		v_buffer = NULL;
#endif
	}
}
