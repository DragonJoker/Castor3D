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
#ifndef ___GL_MSW_CONTEXT_H___
#define ___GL_MSW_CONTEXT_H___

#ifdef CASTOR_PLATFORM_WINDOWS
#include "Common/GlHolder.hpp"
#include "Common/OpenGl.hpp"

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
		void DoInitialiseOpenGL( Castor::PixelFormat p_colour, bool p_stereo );
		HGLRC DoCreateDummyContext( Castor::PixelFormat p_colour, bool p_stereo );
		bool DoSelectPixelFormat( Castor::PixelFormat p_colour, bool p_stereo );
		bool DoSelectStereoPixelFormat( Castor::PixelFormat p_colour );
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
