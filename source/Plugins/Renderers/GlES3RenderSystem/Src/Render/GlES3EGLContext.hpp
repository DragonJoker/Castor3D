/* See LICENSE file in root folder */
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
