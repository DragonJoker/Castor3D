#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9Context.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"

using namespace Castor3D;

Dx9Context :: Dx9Context( RenderWindow * p_window)
    :	Context( p_window)
	,	m_pSwapChain( NULL)
{
	_createDxContext();
}

Dx9Context :: ~Dx9Context()
{
	if (m_pBackBuffer != NULL)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = NULL;
	}

	if (m_pSwapChain != NULL)
	{
		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	}
}

void Dx9Context :: SetCurrent()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderTarget( 0, m_pBackBuffer), CU_T( "Dx9Context :: SetCurrent"), false);
}

void Dx9Context :: EndCurrent()
{
	CheckDxError( m_pSwapChain->Present( NULL, NULL, m_hWnd, NULL, 0), CU_T( "Dx9Context :: SwapBuffers"), false);
}

void Dx9Context :: SwapBuffers()
{
}

void Dx9Context :: _createDxContext()
{
	if ( ! Dx9RenderSystem::IsInitialised())
	{
		Dx9RenderSystem::GetSingletonPtr()->Initialise();
		Dx9RenderSystem::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->Initialise();
	}

	m_hWnd = (HWND)m_pWindow->GetHandle();
	RECT l_rcRect;
	GetClientRect( m_hWnd, & l_rcRect);

	// Initialisation des paramètres du device
	D3DPRESENT_PARAMETERS l_presentParameters;
	memset( & l_presentParameters, 0, sizeof( D3DPRESENT_PARAMETERS));
	l_presentParameters.FullScreen_RefreshRateInHz = 0;
	l_presentParameters.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	l_presentParameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	l_presentParameters.BackBufferWidth            = l_rcRect.right - l_rcRect.left;
	l_presentParameters.BackBufferHeight           = l_rcRect.bottom - l_rcRect.top;
	l_presentParameters.BackBufferFormat           = D3DFMT_X8R8G8B8;
	l_presentParameters.AutoDepthStencilFormat     = D3DFMT_D24S8;
	l_presentParameters.EnableAutoDepthStencil     = true;
	l_presentParameters.Windowed                   = true;

	Dx9RenderSystem::GetSingletonPtr()->InitialiseDevice( m_hWnd, & l_presentParameters);

	Dx9Context * l_pMainContext = Dx9RenderSystem::GetSingletonPtr()->GetMainContext();

	if (l_pMainContext == NULL)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->GetSwapChain( 0, & m_pSwapChain), CU_T( "Dx9Context :: _createDxContext - GetSwapChain"), false);
		RenderSystem::GetSingletonPtr()->SetMainContext( this);
	}
	else
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->CreateAdditionalSwapChain( & l_presentParameters, & m_pSwapChain), CU_T( "Dx9Context :: _createDxContext - CreateAdditionalSwapChain"), false);
	}

	CheckDxError( m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, & m_pBackBuffer), CU_T( "Dx9Context :: _createDxContext - GetBackBuffer"), false);

	Logger::LogMessage( CU_T( "Dx9Context :: _createDxContext - Main Context set"));
	RenderSystem::GetSingletonPtr()->AddContext( this, m_pWindow);
	Logger::LogMessage( CU_T( "Dx9Context :: _createDxContext - Context added"));
}