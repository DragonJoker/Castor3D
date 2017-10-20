/*
See LICENSE file in root folder
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
