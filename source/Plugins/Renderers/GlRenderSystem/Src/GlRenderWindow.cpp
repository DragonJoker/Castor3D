#include "GlRenderWindow.hpp"

#include "GlRenderSystem.hpp"

using namespace Castor3D;

namespace GlRender
{
	GlRenderWindow::GlRenderWindow( OpenGl & p_gl, GlRenderSystem * p_renderSystem )
		: RenderWindow( *p_renderSystem->GetOwner() )
		, m_gl( p_gl )
		, m_renderSystem( p_renderSystem )
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
