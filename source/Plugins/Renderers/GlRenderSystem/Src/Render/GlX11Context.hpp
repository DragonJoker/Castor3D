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
#ifndef ___GL_X11_CONTEXT_H___
#define ___GL_X11_CONTEXT_H___

#include <Config/PlatformConfig.hpp>

#if defined( CASTOR_PLATFORM_LINUX )
#include "Common/GlHolder.hpp"
#include "Common/OpenGl.hpp"

#include <Miscellaneous/GpuInformations.hpp>

namespace GlRender
{
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

		inline GLXContext getContext()
		{
			return m_glxContext;
		}

		inline castor3d::GpuInformations && getGpuInformations()
		{
			return std::move( m_gpuInformations );
		}

	private:
		XVisualInfo * doCreateVisualInfoWithFBConfig( castor3d::RenderWindow * p_window, castor::IntArray p_arrayAttribs, int p_screen );
		XVisualInfo * doCreateVisualInfoWithoutFBConfig( castor::IntArray p_arrayAttribs, int p_screen );
		bool doCreateGl3Context( castor3d::RenderWindow * p_window );

	protected:
		GLXContext m_glxContext;
		int m_glxVersion;
		Display * m_display;
		GLXDrawable m_drawable;
		GLXFBConfig * m_fbConfig;
		GlContext * m_context;
		bool m_initialised;
		castor3d::GpuInformations m_gpuInformations;
	};
}
#endif

#endif
