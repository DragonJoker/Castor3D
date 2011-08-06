#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9Context.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"

using namespace Castor3D;

Dx9Context :: Dx9Context( RenderWindow * p_window)
    :	Context( p_window)
	,	m_pSwapChain( nullptr)
{
	_createDxContext();
}

Dx9Context :: ~Dx9Context()
{
	if (m_pBackBuffer)
	{
		m_pBackBuffer->Release();
		m_pBackBuffer = nullptr;
	}

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = nullptr;
	}
}

void Dx9Context :: SetCurrent()
{
	CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderTarget( 0, m_pBackBuffer), cuT( "Dx9Context :: SetCurrent"), false);
}

void Dx9Context :: EndCurrent()
{
	CheckDxError( m_pSwapChain->Present( nullptr, nullptr, m_hWnd, nullptr, 0), cuT( "Dx9Context :: SwapBuffers"), false);
}

void Dx9Context :: SwapBuffers()
{
}

void Dx9Context :: _createDxContext()
{
	if ( ! Dx9RenderSystem::IsInitialised())
	{
		Dx9RenderSystem::GetSingletonPtr()->Initialise();
		Root::GetSingleton()->GetMaterialManager()->Initialise();
	}

	m_hWnd = (HWND)m_pWindow->GetHandle();
	DWORD dwWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	DWORD dwHeight = GetSystemMetrics(SM_CYFULLSCREEN);

	// Initialisation des paramètres du device
	D3DPRESENT_PARAMETERS l_presentParameters;
	memset( & l_presentParameters, 0, sizeof( D3DPRESENT_PARAMETERS));
	l_presentParameters.FullScreen_RefreshRateInHz = 0;
	l_presentParameters.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	l_presentParameters.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	l_presentParameters.BackBufferWidth            = dwWidth;
	l_presentParameters.BackBufferHeight           = dwHeight;
	l_presentParameters.BackBufferFormat           = D3DFMT_X8R8G8B8;
	l_presentParameters.AutoDepthStencilFormat     = D3DFMT_D24S8;
	l_presentParameters.EnableAutoDepthStencil     = true;
	l_presentParameters.Windowed                   = true;

	Dx9RenderSystem::GetSingletonPtr()->InitialiseDevice( m_hWnd, & l_presentParameters);

	Dx9Context * l_pMainContext = Dx9RenderSystem::GetSingletonPtr()->GetMainContext();

	if (l_pMainContext == NULL)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->GetSwapChain( 0, & m_pSwapChain), cuT( "Dx9Context :: _createDxContext - GetSwapChain"), false);
		RenderSystem::GetSingletonPtr()->SetMainContext( this);
	}
	else
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->CreateAdditionalSwapChain( & l_presentParameters, & m_pSwapChain), cuT( "Dx9Context :: _createDxContext - CreateAdditionalSwapChain"), false);
	}

	CheckDxError( m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, & m_pBackBuffer), cuT( "Dx9Context :: _createDxContext - GetBackBuffer"), false);

	Logger::LogMessage( cuT( "Dx9Context :: _createDxContext - Main Context set"));
	RenderSystem::GetSingletonPtr()->AddContext( this, m_pWindow);
	Logger::LogMessage( cuT( "Dx9Context :: _createDxContext - Context added"));
	m_bInitialised = true;
}
