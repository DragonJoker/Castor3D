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
#ifndef ___C3DGLES3_EGL_CONTEXT_H___
#define ___C3DGLES3_EGL_CONTEXT_H___

#include <Config/PlatformConfig.hpp>

#if defined( CASTOR_PLATFORM_ANDROID )
#include "Common/GlES3Holder.hpp"
#include "Common/OpenGlES3.hpp"

#include <Miscellaneous/GpuInformations.hpp>

namespace GlES3Render
{
	class GlES3Context;

	class GlES3ContextImpl
		: public Holder
	{
	public:
		GlES3ContextImpl( OpenGlES3 & p_gl, GlES3Context * p_context );
		virtual ~GlES3ContextImpl();

		bool Initialise( Castor3D::RenderWindow * p_window );
		void Cleanup();
		void SetCurrent();
		void EndCurrent();
		void SwapBuffers();
		void UpdateVSync( bool p_enable );

		inline EGLContext GetContext()
		{
			return m_eglContext;
		}

		inline Castor3D::GpuInformations && GetGpuInformations()
		{
			return std::move( m_gpuInformations );
		}

	protected:
		GlES3Context * m_context{ nullptr };
		int m_eglVersion{ 10 };
		EGLDisplay m_display{ 0 };
		EGLNativeWindowType m_window{ 0 };
		EGLContext m_eglContext{ 0 };
		EGLSurface m_eglSurface{ 0 };
		EGLConfig m_eglConfig{ 0 };
		bool m_initialised{ false };
		Castor3D::GpuInformations m_gpuInformations;
	};
}
#endif

#endif
