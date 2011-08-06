#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlContext.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"

#ifndef _WIN32
#if ! CASTOR_GTK

using namespace Castor3D;

GlContext :: GlContext()
    :	Context( nullptr)
    ,   m_pDisplay( nullptr)
{
}

GlContext :: GlContext( RenderWindow * p_window)
    :	Context( p_window)
    ,   m_pDisplay( nullptr)
{
    _createGlContext();
    _setMainContext();
}

GlContext :: ~GlContext()
{
    OpenGl::DestroyContext( m_pDisplay, m_glXContext);
    m_glXContext = nullptr;
}

bool GlContext :: Initialise()
{
	GtkWidget * l_pWidget = static_cast<GtkWidget *>( m_pWindow->GetHandle());

	if (m_drawable == None && l_pWidget->window)
	{
		m_drawable = GDK_WINDOW_XID( l_pWidget->window);
		m_bInitialised = _createGlContext();
	}

	return m_bInitialised;
}

void GlContext :: SetCurrent()
{
	CheckGlError( OpenGl::MakeCurrent( m_pDisplay, m_drawable, m_glXContext), cuT( "GlContext :: SetCurrent - glMakeCurrent"));
}

void GlContext :: EndCurrent()
{
	OpenGl::MakeCurrent( m_pDisplay, None, nullptr);
}

void GlContext :: SwapBuffers()
{
	OpenGl::SwapBuffers( m_pDisplay, m_drawable);
}

void GlContext :: _createGlContext()
{
	GtkWidget * l_pGtkWidget = static_cast<GtkWidget *>( m_pWindow->GetHandle());
	GdkDisplay * l_pGtkDisplay = gtk_widget_get_display( l_pGtkWidget);
	m_pDisplay = gdk_x11_display_get_xdisplay( l_pGtkDisplay);
	GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

	if (l_pMainContext)
	{
		m_glXContext = l_pMainContext->GetContext();
	}
	else
	{
		int l_iScreen = DefaultScreen( m_pDisplay);
		int l_iMajor, l_iMinor;
		bool l_bOk = OpenGl::QueryVersion( m_pDisplay, & l_iMajor, & l_iMinor);

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
			CheckGlError( m_pFbConfig = GlXChooseFBConfig( m_pDisplay, l_iScreen, & l_iData, & l_iResult), cuT( "GlGtkContext :: Create - glXChooseFBConfig failed"));
			Logger::LogMessage( cuT( "Coucou 1"));

			if (m_pFbConfig == NULL)
			{
				Logger::LogError( String( "GlGtkContext :: Create - glXChooseFBConfig failed"));
			}

			l_pVisualInfo = GlXGetVisualFromFBConfig( m_pDisplay, m_pFbConfig[0]);
			Logger::LogMessage( cuT( "Coucou 2"));

			if (l_pVisualInfo == NULL)
			{
				Logger::LogError( String( "GlContext :: Create - glXGetVisualFromFBConfig failed"));
			}

			GLXContext l_glXContext = OpenGl::CreateContext( m_pDisplay, l_pVisualInfo, None, GL_TRUE);
			Logger::LogMessage( cuT( "Coucou 3"));

			if (l_glXContext == NULL)
			{
				Logger::LogError( String("GlContext :: Create - glXCreateContext failed"));
			}

			if (GlXCreateContextAttribsARB == NULL)
			{
				GlXCreateContextAttribsARB =  (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
			}

			Logger::LogMessage( cuT( "Coucou 4"));

			if (GlXCreateContextAttribsARB)
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
				OpenGl::DestroyContext( m_pDisplay, l_glXContext);
				m_glXContext = OpenGl::CreateContextAttribsARB( m_pDisplay, m_pFbConfig[0], nullptr, true, l_ctxAttribs);
			}
			else
			{
				m_glXContext = l_glXContext;
			}

			Logger::LogMessage( cuT( "Coucou 5"));

			if ( ! GlRenderSystem::IsInitialised())
			{
				SetCurrent();
				GlRenderSystem::GetSingletonPtr()->Initialise();
				GlRenderSystem::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->Initialise();
				EndCurrent();
			}
		}
		else
		{
			Logger::LogMessage( cuT( "GlContext :: GlContext - Not using FBConfig"));
			CheckGlError( l_pVisualInfo = OpenGl::ChooseVisual( m_pDisplay, l_iScreen, l_attribList), cuT( "GlContext :: Create - glXChooseVisual failed"));

			if (l_pVisualInfo == NULL)
			{
				Logger::LogError( String( "GlContext :: Create - glXChooseVisual failed"));
			}

			CheckGlError( m_glXContext = OpenGl::CreateContext( m_pDisplay, l_pVisualInfo, None, GL_TRUE), cuT( "GlGtkContext :: Create - glXCreateContext"));
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

		CheckGlError( glXMakeCurrent( m_pDisplay, 0, 0), cuT( "Gl3X11Context :: Gl3X11Context - glXMakeCurrent failed"));
		CheckGlError( glXDestroyContext( m_pDisplay, l_context), cuT( "Gl3X11Context :: Gl3X11Context - glXDestroyContext failed"));

		int l_attribs[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, GlRenderSystem::GetOpenGlMajor(),
			GLX_CONTEXT_MINOR_VERSION_ARB, GlRenderSystem::GetOpenGlMinor(),
			GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			0
		};

		if (m_pDisplay)
		{
			CheckGlError( m_glXContext = glXCreateContextAttribsARB( m_pDisplay, m_pFbConfig[0], nullptr, False, l_attribs), cuT( "Gl3X11Context :: Gl3X11Context - glCreateContextAttribs failed"));
		}
		else
		{
			CheckGlError( m_glXContext = glXCreateContextAttribsARB( m_pDisplay, nullptr, nullptr, False, l_attribs), cuT( "Gl3X11Context :: Gl3X11Context - glCreateContextAttribs failed"));
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
		RenderSystem::GetSingletonPtr()->SetMainContext( this);
	}

	Logger::LogMessage( cuT( "GlContext :: Create - Main Context set"));
}

#endif
#endif
