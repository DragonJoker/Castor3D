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
#ifndef ___GL_MSW_CONTEXT_H___
#define ___GL_MSW_CONTEXT_H___

#ifdef _WIN32

#include "GlHolder.hpp"

#include <Miscellaneous/GpuInformations.hpp>

#ifndef _WINDEF_

#define DECLARE_WIN_HANDLE( Name ) struct Name##__{int unused;}; typedef struct Name##__ *Name
DECLARE_WIN_HANDLE( HDC );
DECLARE_WIN_HANDLE( HGLRC );
DECLARE_WIN_HANDLE( HWND );

#endif

namespace GlRender
{
	class GlContext;
	class GlContext;

	class GlContextImpl
		: public Holder
	{
	public:
		GlContextImpl( OpenGl & p_gl, GlContext * p_context );
		virtual ~GlContextImpl();

		bool Initialise( Castor3D::RenderWindow * p_window );
		void Cleanup();
		void SetCurrent();
		void EndCurrent();
		void SwapBuffers();
		void UpdateVSync( bool p_enable );

		inline HDC GetHDC()const
		{
			return m_hDC;
		}

		inline HGLRC GetContext()const
		{
			return m_hContext;
		}

		inline Castor3D::GpuInformations && GetGpuInformations()
		{
			return std::move( m_gpuInformations );
		}

	protected:
		void DoInitialiseOpenGL( Castor::ePIXEL_FORMAT p_colour, bool p_stereo );
		HGLRC DoCreateDummyContext( Castor::ePIXEL_FORMAT p_colour, bool p_stereo );
		bool DoSelectPixelFormat( Castor::ePIXEL_FORMAT p_colour, bool p_stereo );
		bool DoSelectStereoPixelFormat( Castor::ePIXEL_FORMAT p_colour );
		bool DoCreateGl3Context();

	protected:
		HDC m_hDC;
		HGLRC m_hContext;
		HWND m_hWnd;
		GlContext * m_context;
		bool m_initialised;
		Castor3D::GpuInformations m_gpuInformations;
	};
}

#undef DECLARE_WIN_HANDLE
#endif

#endif
