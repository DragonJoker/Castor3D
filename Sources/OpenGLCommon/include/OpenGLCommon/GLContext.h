/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___GL_Context___
#define ___GL_Context___

#include "Module_GLRender.h"
#include <Castor3D/render_system/Context.h>

namespace Castor3D
{
	class GLContext : public Context, public MemoryTraced<GLContext>
    {
    protected:
#ifdef WIN32
        HDC m_hDC;
        HGLRC m_context;
		HWND m_hWnd;
#else
#	ifdef __GNUG__
		Display * m_display;
		GLXContext m_context;
		Window m_xWindow;
		GLXDrawable m_drawable;
		int m_iSelectedConfigIndex;
#	endif
#endif

	public:
		GLContext();
        GLContext( RenderWindow * p_window, void * p_win);
        virtual ~GLContext();
	
        virtual void SetCurrent();
        virtual void EndCurrent();

#ifdef WIN32
		inline HDC		GetHDC		()const { return m_hDC; }
		inline HGLRC	GetContext	()const { return m_context; }
#else
#	ifdef __GNUG__
		inline Display *	GetDisplay	()const { return m_display; }
		inline GLXContext	GetContext	()const { return m_context; }
		inline Window		GetWindow	()const { return m_xWindow; }

	protected:
		bool _checkConfig( const GLXFBConfig & p_config);
		bool _checkConfigAttrib( const GLXFBConfig & p_config, int p_attrib, int p_condition);
#	endif
#endif

	protected:
		void _createGLContext();
		void _setMainContext();
    };
}

#endif