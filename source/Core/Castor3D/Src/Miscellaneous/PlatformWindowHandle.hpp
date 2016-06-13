/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_PLATFORM_WINDOW_HANDLE_H___
#define ___C3D_PLATFORM_WINDOW_HANDLE_H___

#include "WindowHandle.hpp"

//*************************************************************************************************
//
// INCLUDE THIS FILE ONLY WHERE YOU NEED IT (for instance not in precompiled header file)
//
//*************************************************************************************************

#if defined( _WIN32 )
#	include <windows.h>
#elif defined( __linux__ )
#	include <GL/glx.h>
#else
#	error "Yet unsupported OS"
#endif

namespace Castor3D
{

#if defined( _WIN32 )

	class IMswWindowHandle
		: public IWindowHandle
	{
	private:
		HWND m_hWnd;

	public:
		IMswWindowHandle( HWND p_hWnd )
			: m_hWnd( p_hWnd )
		{
		}

		virtual ~IMswWindowHandle()
		{
		}

		virtual operator bool()
		{
			return m_hWnd != nullptr;
		}

		inline HWND GetHwnd()const
		{
			return m_hWnd;
		}
	};

#elif defined( __linux__ )

	class IXWindowHandle
		: public IWindowHandle
	{
	private:
		GLXDrawable m_drawable;
		Display * m_display;

	public:
		IXWindowHandle( GLXDrawable p_drawable, Display * p_pDisplay )
			: m_drawable( p_drawable )
			, m_display( p_pDisplay )
		{
		}

		virtual ~IXWindowHandle()
		{
		}

		virtual operator bool()
		{
			return m_drawable != 0 && m_display != nullptr;
		}

		inline GLXDrawable GetDrawable()const
		{
			return m_drawable;
		}
		inline Display * GetDisplay()const
		{
			return m_display;
		}
	};

#endif

}

#endif
