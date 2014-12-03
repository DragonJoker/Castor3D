/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_WINDOW_RENDERER_H___
#define ___GL_WINDOW_RENDERER_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <RenderWindow.hpp>

namespace GlRender
{
	class GlWindowRenderer
		:	public Castor3D::WindowRenderer
		,	public Castor::NonCopyable
	{
	private:
		typedef std::function< bool() > PBeginSceneFunction;
		typedef std::function< bool() > PEndSceneFunction;

	public:
		GlWindowRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		virtual ~GlWindowRenderer();

		virtual bool BeginScene();
		virtual bool EndScene();
		virtual void EndRender();
		virtual bool SetCurrent();
		virtual void EndCurrent();

	private:
		virtual bool DoInitialise();
		virtual void DoCleanup();

	protected:
		PBeginSceneFunction m_pfnBeginScene;
		PEndSceneFunction m_pfnEndScene;
		OpenGl & m_gl;
	};
}

#endif
