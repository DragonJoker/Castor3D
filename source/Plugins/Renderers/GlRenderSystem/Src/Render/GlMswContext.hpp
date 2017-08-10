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

	class GlContextImpl
		: public Holder
	{
	public:
		GlContextImpl( OpenGl & p_gl, GlContext * p_context );
		~GlContextImpl();

		bool initialise( castor3d::RenderWindow * p_window );
		void cleanup();
		void setCurrent();
		void endCurrent();
		void swapBuffers();
		void updateVSync( bool p_enable );

		inline HDC getHDC()const
		{
			return m_hDC;
		}

		inline HGLRC getContext()const
		{
			return m_hContext;
		}

		inline castor3d::GpuInformations && getGpuInformations()
		{
			return std::move( m_gpuInformations );
		}

	protected:
		void doInitialiseOpenGL( castor::PixelFormat p_colour, bool p_stereo );
		HGLRC doCreateDummyContext( castor::PixelFormat p_colour, bool p_stereo );
		bool doSelectPixelFormat( castor::PixelFormat p_colour, bool p_stereo );
		bool doSelectStereoPixelFormat( castor::PixelFormat p_colour );
		bool doCreateGl3Context();

	protected:
		HDC m_hDC;
		HGLRC m_hContext;
		HWND m_hWnd;
		GlContext * m_context;
		bool m_initialised;
		castor3d::GpuInformations m_gpuInformations;
	};
}

#undef DECLARE_WIN_HANDLE
#endif

#endif
