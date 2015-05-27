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
			Logger::LogMessage( cuT( "DxWindowRenderer::StartRender - Initialisation" ) );

			if ( !m_pRenderSystem->IsInitialised() )
			{
				m_pRenderSystem->Initialise();
				m_pRenderSystem->GetEngine()->GetMaterialManager().Initialise();
			}
		}

		return m_target->IsInitialised();
	}

	void DxWindowRenderer::DoCleanup()
	{
	}
}
