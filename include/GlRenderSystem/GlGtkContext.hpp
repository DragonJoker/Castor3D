/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GL_GtkContext___
#define ___GL_GtkContext___

#ifndef _WIN32
#if CASTOR_GTK
#	include <gtk/gtk.h>
#	include <gdk/gdk.h>
#	include <gdk/gdkx.h>

namespace Castor3D
{
	class GlContext : public Context, public MemoryTraced<GlContext>
    {
    protected:
        GLXContext  	m_glXContext;
		int				m_iGlXVersion;
		Display		*	m_pDisplay;
		GLXDrawable		m_drawable;
		GLXFBConfig	*	m_pFbConfig;

	public:
		GlContext();
        GlContext( RenderWindow * p_window);
        virtual ~GlContext();

		virtual bool Initialise();
        virtual void SetCurrent();
        virtual void EndCurrent();
        virtual void SwapBuffers();

        inline GLXContext GetContext() { return m_glXContext; }

	protected:
		virtual bool _createGlContext();
		virtual void _createGl3Context();
        virtual void _setMainContext();
    };
}
#endif
#endif

#endif
