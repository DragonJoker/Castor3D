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
#ifndef ___GL_X11_CONTEXT_H___
#define ___GL_X11_CONTEXT_H___

#if defined( __linux__ )
#include "Common/GlHolder.hpp"
#include "Common/OpenGl.hpp"

#include <Miscellaneous/GpuInformations.hpp>

namespace GlRender
{
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

		inline GLXContext GetContext()
		{
			return m_glxContext;
		}

		inline Castor3D::GpuInformations && GetGpuInformations()
		{
			return std::move( m_gpuInformations );
		}

	private:
		XVisualInfo * DoCreateVisualInfoWithFBConfig( Castor3D::RenderWindow * p_window, Castor::IntArray p_arrayAttribs, int p_screen );
		XVisualInfo * DoCreateVisualInfoWithoutFBConfig( Castor::IntArray p_arrayAttribs, int p_screen );
		bool DoCreateGl3Context( Castor3D::RenderWindow * p_window );

	protected:
		GLXContext m_glxContext;
		int m_glxVersion;
		Display * m_display;
		GLXDrawable m_drawable;
		GLXFBConfig * m_fbConfig;
		GlContext * m_context;
		bool m_initialised;
		Castor3D::GpuInformations m_gpuInformations;
	};
}
#endif

#endif
