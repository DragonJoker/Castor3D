/*
See LICENSE file in root folder
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
		explicit IMswWindowHandle( HWND p_hWnd )
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
