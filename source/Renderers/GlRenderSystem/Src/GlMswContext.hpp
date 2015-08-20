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
#ifndef ___GL_MSW_CONTEXT_H___
#define ___GL_MSW_CONTEXT_H___

#ifdef _WIN32
#include "GlRenderSystemPrerequisites.hpp"

#include <NonCopyable.hpp>
#include <Castor3DPrerequisites.hpp>
#include <Windows.h>

namespace GlRender
{
	class GlContext;

	class GlContextImpl
		:	public Castor::NonCopyable
	{
	public:
		GlContextImpl( OpenGl & p_gl, GlContext * p_pContext );
		virtual ~GlContextImpl();
		
		bool Initialise( Castor3D::RenderWindow * p_window );
		void Cleanup();
		void SetCurrent();
		void EndCurrent();
		void SwapBuffers();
		void UpdateVSync( bool p_bEnable );

		inline HDC GetHDC()const
		{
			return m_hDC;
		}
		inline HGLRC GetContext()const
		{
			return m_hContext;
		}

	protected:
		bool DoCreateGl3Context( Castor3D::RenderWindow * p_window );
		HGLRC DoCreateDummyContext( Castor3D::RenderWindow * p_window );
		bool DoSelectPixelFormat( Castor3D::RenderWindow * p_window );
		bool DoSelectStereoPixelFormat( Castor3D::RenderWindow * p_window );

	protected:
		HDC m_hDC;
		HGLRC m_hContext;
		HWND m_hWnd;
		GlContext * m_pContext;
		bool m_bInitialised;
		OpenGl & m_gl;
	};
}
#endif

#endif
