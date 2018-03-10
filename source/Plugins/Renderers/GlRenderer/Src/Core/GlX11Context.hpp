/*
See LICENSE file in root folder
*/
#pragma once

#include "Core/GlContext.hpp"

#if RENDERLIB_XLIB
#	include "Miscellaneous/OpenGLLibrary.hpp"

#	include <X11/Xlib.h>
#	include <GL/glx.h>

namespace gl_renderer
{
	class X11Context
		: public Context
	{
	public:
		X11Context( PhysicalDevice const & gpu
			, renderer::ConnectionPtr && connection );
		~X11Context();

		void setCurrent()const override;
		void endCurrent()const override;
		void swapBuffers()const override;

		inline GLXContext getContext()
		{
			return m_glxContext;
		}

	private:
		XVisualInfo * doCreateVisualInfoWithFBConfig( std::vector< int > arrayAttribs, int screen );
		XVisualInfo * doCreateVisualInfoWithoutFBConfig( std::vector< int > arrayAttribs, int screen );
		bool doCreateGl3Context();

	protected:
		std::unique_ptr< OpenGLLibrary > m_opengl;
		GLXContext m_glxContext;
		int m_glxVersion;
		Display * m_display;
		GLXDrawable m_drawable;
		GLXFBConfig * m_fbConfig;
	};
}

#endif
