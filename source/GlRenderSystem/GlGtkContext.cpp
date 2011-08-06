#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlContext.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"

#ifndef _WIN32
#	if CASTOR_GTK
#include <GL/glx.h>
#include <GL/gl.h>
#include <unistd.h>
#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace Castor3D;
typedef GLXFBConfig * (* glXChooseFBConfigProc)( Display *, int, int const *,int *);
typedef XVisualInfo * ( * glXGetVisualFromFBConfigProc)( Display *, GLXFBConfig);

glXChooseFBConfigProc GlXChooseFBConfig = nullptr;
glXGetVisualFromFBConfigProc GlXGetVisualFromFBConfig = nullptr;

GlContext :: GlContext()
	:	Context( nullptr)
	,	m_pDisplay( nullptr)
	,	m_iGlXVersion( 10)
	,	m_glXContext( nullptr)
	,	m_drawable( None)
	,	m_pFbConfig( nullptr)
{
}

GlContext :: GlContext( RenderWindow * p_window)
    :	Context( p_window)
	,	m_pDisplay( nullptr)
	,	m_iGlXVersion( 10)
	,	m_glXContext( nullptr)
	,	m_drawable( None)
	,	m_pFbConfig( nullptr)
{
}

GlContext :: ~GlContext()
{
    glXDestroyContext( m_pDisplay, m_glXContext);
    XFree( m_pFbConfig);
}

bool GlContext :: Initialise()
{
	GtkWidget * l_pWidget = static_cast<GtkWidget *>( m_pWindow->GetHandle());

	if ( ! m_bInitialised && m_drawable == None && l_pWidget->window)
	{
		m_drawable = GDK_WINDOW_XID( l_pWidget->window);
		m_bInitialised = _createGlContext();
	}

	return m_bInitialised;
}

void GlContext :: SetCurrent()
{
	OpenGl::MakeCurrent( m_pDisplay, m_drawable, m_glXContext), cuT( "GlGtkContext :: SetCurrent - glMakeCurrent"));
}

void GlContext :: EndCurrent()
{
	OpenGl::MakeCurrent( m_pDisplay, None, nullptr);
}

void GlContext :: SwapBuffers()
{
    OpenGl::SwapBuffers( m_pDisplay, m_drawable);
}

bool GlContext :: _createGlContext()
{
	GtkWidget * l_pGtkWidget = static_cast<GtkWidget *>( m_pWindow->GetHandle());
	GdkDisplay * l_pGtkDisplay = gtk_widget_get_display( l_pGtkWidget);
	m_pDisplay = gdk_x11_display_get_xdisplay( l_pGtkDisplay);
	GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

	if (l_pMainContext)
	{
		Logger::LogMessage( cuT( "GlContext :: GlContext - Not first context"));
		m_glXContext = l_pMainContext->GetContext();
		m_bInitialised = true;
	}
	else
	{
		int l_iScreen = DefaultScreen( m_pDisplay);
		int l_iMajor, l_iMinor;
		bool l_bOk = glXQueryVersion( m_pDisplay, & l_iMajor, & l_iMinor);

		if (l_bOk)
		{
			m_iGlXVersion = l_iMajor * 10 + l_iMinor;
			Logger::LogMessage( cuT( "GlContext :: GlContext - glx version : %i"), m_iGlXVersion);
		}

		int l_attribList[] =
		{
			GLX_RENDER_TYPE, 	GLX_RGBA_BIT,
			GLX_DRAWABLE_TYPE, 	GLX_WINDOW_BIT,
			GLX_DOUBLEBUFFER,	true,
			GLX_RED_SIZE,		1,
			GLX_GREEN_SIZE,		1,
			GLX_BLUE_SIZE,		1,
			None,
		};

		XVisualInfo	*	l_pVisualInfo;

		if (GlXChooseFBConfig == NULL)
		{
			GlXChooseFBConfig = (glXChooseFBConfigProc)glXGetProcAddress((const GLubyte*)"glXChooseFBConfig");
			GlXGetVisualFromFBConfig = (glXGetVisualFromFBConfigProc)glXGetProcAddress((const GLubyte*)"glXGetVisualFromFBConfig");
		}

		if (GlXChooseFBConfig)
		{
			Logger::LogMessage( cuT( "GlContext :: GlContext - Using FBConfig"));
			int l_iData = 0;
			int l_iResult;
			m_pFbConfig = GlXChooseFBConfig( m_pDisplay, l_iScreen, & l_iData, & l_iResult);

			if (m_pFbConfig == NULL)
			{
				Logger::LogError( String( "GlGtkContext :: Create - glXChooseFBConfig failed"));
			}

			l_pVisualInfo = GlXGetVisualFromFBConfig( m_pDisplay, m_pFbConfig[0]);

			if (l_pVisualInfo == NULL)
			{
				Logger::LogError( String( "GlContext :: Create - glXGetVisualFromFBConfig failed"));
			}

			GLXContext l_glXContext = OpenGl::CreateContext( m_pDisplay, l_pVisualInfo, None, GL_TRUE);

			if (l_glXContext == NULL)
			{
				Logger::LogError( String("GlContext :: Create - glXCreateContext failed"));
			}

			if (OpenGl::HasCreateContextAttribs())
			{
				Logger::LogMessage( cuT("GlContext :: Create - Use OpenGL 3.x"));

				static int l_ctxAttribs[] =
				{
					GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
					GLX_CONTEXT_MINOR_VERSION_ARB, 3,
					GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
					None
				};

				OpenGl::MakeCurrent( m_pDisplay, 0, 0);
				OpenGl::DeleteContext( m_pDisplay, l_glXContext);
				m_glXContext = OpenGl::CreateContextAttribs( m_pDisplay, m_pFbConfig[0], nullptr, true, l_ctxAttribs);
			}
			else
			{
				m_glXContext = l_glXContext;
			}

			RenderSystem::GetSingletonPtr()->SetMainContext( this);

			if ( ! GlRenderSystem::IsInitialised())
			{
				SetCurrent();
				GlRenderSystem::GetSingletonPtr()->Initialise();
				Root::GetSingleton()->GetMaterialManager()->Initialise();
				EndCurrent();
			}
		}
		else
		{
			Logger::LogMessage( cuT( "GlContext :: GlContext - Not using FBConfig"));
			l_pVisualInfo = glXChooseVisual( m_pDisplay, l_iScreen, l_attribList);

			if (l_pVisualInfo == NULL)
			{
				Logger::LogError( String( "GlContext :: Create - glXChooseVisual failed"));
			}

			m_glXContext = OpenGl::CreateContext( m_pDisplay, l_pVisualInfo, None, GL_TRUE);
		}

		if (m_glXContext == NULL)
		{
			Logger::LogError( String("GlContext :: Create - glXCreateContext failed"));
		}
		else
		{
			Logger::LogMessage( String("GlContext :: Create - glXContext created"));
		}

		XFree( l_pVisualInfo);
	}

	return true;
}

void GlContext :: _createGl3Context()
{
	GLXContext l_context = m_glXContext;

	if (glXCreateContextAttribsARB)
	{
		GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

		OpenGl::MakeCurrent( m_pDisplay, 0, 0);
		OpenGl::DestroyContext( m_pDisplay, l_context);

		int l_attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, GlRenderSystem::GetOpenGlMajor(),
			GLX_CONTEXT_MINOR_VERSION_ARB, GlRenderSystem::GetOpenGlMinor(),
			GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};

		if (m_pDisplay)
		{
			m_glXContext = OpenGl::CreateContextAttribs( m_pDisplay, m_pFbConfig[0], nullptr, False, l_attribs);
		}
		else
		{
			m_glXContext = OpenGl::CreateContextAttribs( m_pDisplay, nullptr, nullptr, False, l_attribs);
		}
		Logger::LogMessage( cuT( "Gl3X11Context :: Gl3X11Context - OpenGL 3.x context created"));
	}
	else
	{	//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
		GlRenderSystem::SetOpenGlVersion( 2, 1);
		Logger::LogError( cuT( "Gl3X11Context :: Gl3X11Context - Can't create OpenGL 3.x context"));
	}

	EndCurrent();
}

void GlContext :: _setMainContext()
{
    GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

    if (l_pMainContext == NULL)
    {
    }

    Logger::LogMessage( cuT( "GlContext :: Create - Main Context set"));
}
#	endif
#endif
