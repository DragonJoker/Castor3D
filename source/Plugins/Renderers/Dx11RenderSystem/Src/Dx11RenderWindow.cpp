#include "Dx11RenderWindow.hpp"

#include "Dx11BackBuffers.hpp"
#include "Dx11Context.hpp"
#include "Dx11RenderSystem.hpp"

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxRenderWindow::DxRenderWindow( DxRenderSystem * p_renderSystem )
		: RenderWindow( *p_renderSystem->GetOwner() )
	{
		DoSetBackBuffers( std::make_shared< DxBackBuffers >( p_renderSystem ) );
	}

	DxRenderWindow::~DxRenderWindow()
	{
	}

	bool DxRenderWindow::DoInitialise()
	{
		return true;
	}

	void DxRenderWindow::DoCleanup()
	{
	}

	void DxRenderWindow::DoBeginScene()
	{
	}

	void DxRenderWindow::DoEndScene()
	{
	}
}