#include "WindowRenderer.hpp"
#include "RenderWindow.hpp"
#include "Context.hpp"
#include "RenderTarget.hpp"
#include "Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	WindowRenderer::WindowRenderer( RenderSystem * p_pRenderSystem )
		:	Renderer< RenderWindow, WindowRenderer >( p_pRenderSystem )
	{
	}

	WindowRenderer::~WindowRenderer()
	{
	}

	bool WindowRenderer::Initialise()
	{
		ContextRPtr l_pContext = GetRenderSystem()->GetCurrentContext();
		SceneSPtr l_pScene = m_target->GetScene();
		RenderTargetSPtr l_pTarget = m_target->GetRenderTarget();

		if ( m_target->IsUsingDeferredRendering() )
		{
			l_pContext->SetDeferredShading( true );
		}

		if ( l_pScene )
		{
			l_pContext->SetClearColour( l_pScene->GetBackgroundColour() );
		}
		else
		{
			l_pContext->SetClearColour( Colour::from_components( 0.5, 0.5, 0.5, 1.0 ) );
		}

		bool l_bReturn = DoInitialise();

		if ( l_pTarget )
		{
			l_pTarget->Initialise( 0 );
		}

		m_target->SetInitialised();
		return l_bReturn;
	}

	void WindowRenderer::Cleanup()
	{
		RenderTargetSPtr l_pTarget = m_target->GetRenderTarget();

		if ( l_pTarget )
		{
			l_pTarget->Cleanup();
		}

		DoCleanup();
	}
}
