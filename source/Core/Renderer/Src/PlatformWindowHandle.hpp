/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_PlatformWindowHandle_HPP___
#define ___Renderer_PlatformWindowHandle_HPP___

#include "WindowHandle.hpp"

//*************************************************************************************************
//
// INCLUDE THIS FILE ONLY WHERE YOU NEED IT (for instance not in precompiled header file)
//
//*************************************************************************************************

#if RENDERLIB_WIN32
#	include <windows.h>
#elif RENDERLIB_XLIB
#	include <GL/glx.h>
#elif RENDERLIB_ANDROID
#	include <EGL/egl.h>
#else
#	error "Yet unsupported OS"
#endif

namespace renderer
{

#if RENDERLIB_WIN32

	class IMswWindowHandle
		: public IWindowHandle
	{
	public:
		explicit IMswWindowHandle( HINSTANCE hInstance
			, HWND hWnd )
			: m_hInstance{ hInstance }
			, m_hWnd{ hWnd }
		{
		}

		virtual ~IMswWindowHandle()
		{
		}

		virtual operator bool()
		{
			return m_hWnd != nullptr
				&& m_hInstance != nullptr;
		}

		inline HINSTANCE getHinstance()const
		{
			return m_hInstance;
		}

		inline HWND getHwnd()const
		{
			return m_hWnd;
		}

	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
	};

#elif RENDERLIB_XLIB

	class IXWindowHandle
		: public IWindowHandle
	{
	public:
		IXWindowHandle( GLXDrawable drawable
			, Display * display )
			: m_drawable{ drawable }
			, m_display{ display }
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

#elif RENDERLIB_ANDROID

	class IEglWindowHandle
		: public IWindowHandle
	{
	public:
		IEglWindowHandle( EGLNativeWindowType window
			, EGLDisplay display )
			: m_window{ window }
			, m_display{ display }
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
