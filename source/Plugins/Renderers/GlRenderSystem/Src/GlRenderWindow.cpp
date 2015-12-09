﻿#include "GlRenderWindow.hpp"

#include "GlRenderSystem.hpp"
#include "GlBackBuffers.hpp"

using namespace Castor3D;

namespace GlRender
{
	GlRenderWindow::GlRenderWindow( OpenGl & p_gl, GlRenderSystem * p_renderSystem )
		: RenderWindow( *p_renderSystem->GetOwner() )
		, Holder( p_gl )
		, m_renderSystem( p_renderSystem )
	{
		DoSetBackBuffers( std::make_shared< GlBackBuffers >( GetOpenGl(), *p_renderSystem->GetOwner() ) );
	}

	GlRenderWindow::~GlRenderWindow()
	{
	}

	bool GlRenderWindow::DoInitialise()
	{
		return true;
	}

	void GlRenderWindow::DoCleanup()
	{
	}

	void GlRenderWindow::DoBeginScene()
	{
	}

	void GlRenderWindow::DoEndScene()
	{
	}
}
