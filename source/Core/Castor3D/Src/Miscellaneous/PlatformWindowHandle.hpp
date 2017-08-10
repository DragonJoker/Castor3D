/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PLATFORM_WINDOW_HANDLE_H___
#define ___C3D_PLATFORM_WINDOW_HANDLE_H___

#include "WindowHandle.hpp"

//*************************************************************************************************
//
// INCLUDE THIS FILE ONLY WHERE YOU NEED IT (for instance not in precompiled header file)
//
//*************************************************************************************************

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include <windows.h>
#elif defined( CASTOR_PLATFORM_LINUX )
#	include <GL/glx.h>
#elif defined( CASTOR_PLATFORM_ANDROID )
#	include <EGL/egl.h>
#else
#	error "Yet unsupported OS"
#endif

namespace castor3d
{

#if defined( CASTOR_PLATFORM_WINDOWS )

	class IMswWindowHandle
		: public IWindowHandle
	{
	public:
		IMswWindowHandle( HWND p_hWnd )
			: m_hWnd{ p_hWnd }
		{
		}

		virtual ~IMswWindowHandle()
		{
		}

		virtual operator bool()
		{
			return m_hWnd != nullptr;
		}

		inline HWND getHwnd()const
		{
			return m_hWnd;
		}

	private:
		HWND m_hWnd;
	};

#elif defined( CASTOR_PLATFORM_LINUX )

	class IXWindowHandle
		: public IWindowHandle
	{
	public:
		IXWindowHandle( GLXDrawable p_drawable, Display * p_pDisplay )
			: m_drawable{ p_drawable }
			, m_display{ p_pDisplay }
		{
		}

		virtual ~IXWindowHandle()
		{
		}

		virtual operator bool()
		{
			return m_drawable != 0 && m_display != nullptr;
		}

		inline GLXDrawable getDrawable()const
		{
			return m_drawable;
		}
		inline Display * getDisplay()const
		{
			return m_display;
		}

	private:
		GLXDrawable m_drawable;
		Display * m_display;
	};

#elif defined( CASTOR_PLATFORM_ANDROID )

	class IEglWindowHandle
		: public IWindowHandle
	{
	public:
		IEglWindowHandle( EGLNativeWindowType p_window, EGLDisplay p_display )
			: m_window{ p_window }
			, m_display{ p_display }
		{
		}

		virtual ~IEglWindowHandle()
		{
		}

		virtual operator bool()
		{
			return m_window != 0 && m_display != 0;
		}

		inline EGLNativeWindowType getWindow()const
		{
			return m_window;
		}

		inline EGLDisplay getDisplay()const
		{
			return m_display;
		}

	private:
		EGLNativeWindowType m_window;
		EGLDisplay m_display;
	};

#endif
}

#endif
