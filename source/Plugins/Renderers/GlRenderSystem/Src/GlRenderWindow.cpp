#include "GlRenderWindow.hpp"

#include "GlRenderSystem.hpp"

using namespace Castor3D;

namespace GlRender
{
	GlRenderWindow::GlRenderWindow( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem )
		: RenderWindow( p_pRenderSystem->GetEngine() )
		, m_gl( p_gl )
		, m_pRenderSystem( p_pRenderSystem )
	{
	}

	GlRenderWindow::~GlRenderWindow()
	{
	}

	void GlRenderWindow::DoBeginScene()
	{
	}

	void GlRenderWindow::DoEndScene()
	{
	}
}
