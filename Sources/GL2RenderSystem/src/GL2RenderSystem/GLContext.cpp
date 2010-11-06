#include "PrecompiledHeader.h"

#include "GLContext.h"
#include "GLRenderSystem.h"

using namespace Castor3D;

GLContext :: GLContext( RenderWindow * p_window, void * p_win)
	:	m_window( p_window),
#ifdef WIN32
		m_hWnd( (HWND)p_win)
#else
		m_initialised( false)
#endif
{
	GLContext * l_mainContext = GLRenderSystem::GetSingletonPtr()->GetMainContext();
#ifdef WIN32
	PIXELFORMATDESCRIPTOR l_pfd;
	memset( & l_pfd, 0, sizeof( l_pfd));
    l_pfd.nSize = sizeof( l_pfd);
    l_pfd.nVersion = 1;
    l_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    l_pfd.iPixelType = PFD_TYPE_RGBA;
    l_pfd.cColorBits = 24;
    l_pfd.cDepthBits = 16;
    l_pfd.iLayerType = PFD_MAIN_PLANE;

	m_HDC = GetDC( (HWND)p_window->GetHWnd());
    int iFormat = ChoosePixelFormat( m_HDC, & l_pfd);
    SetPixelFormat( m_HDC, iFormat, & l_pfd);

	m_HGLRC = GLCreateContext( m_HDC);

	if (l_mainContext != NULL)
	{
		if (wglShareLists( l_mainContext->GetHRC(), m_HGLRC) != TRUE)
		{
			Log::LogMessage( "Can't share context");
		}
	}
#endif

	if (l_mainContext == NULL)
	{
		GLRenderSystem::GetSingletonPtr()->SetMainContext( this);
	}
	Log::LogMessage( "GLContext :: GLContext - Main Context set");

	GLRenderSystem::GetSingletonPtr()->AddContext( this, m_window);
	Log::LogMessage( "GLContext :: GLContext - Context added");
}

GLContext :: ~GLContext()
{
//	GLRenderSystem *rs = static_cast <GLRenderSystem *>( Root::GetSingletonPtr()->GetRenderSystem());
//	rs->_unregisterContext(this);
}
#ifdef __GNUG__
void GLContext :: Initialise()
{
	if (m_initialised)
	{
		return;
	}
	int l_screen;
	GtkWidget * l_widget = static_cast <GtkWidget *>( m_window->GetHWnd());
	Log::LogMessage( "GLContext :: GLContext - Widget retrieved");
	m_display = GDK_DISPLAY_XDISPLAY( gtk_widget_get_display( l_widget));
	Log::LogMessage( "GLContext :: GLContext - Display retrieved");

	l_screen = DefaultScreen( m_display);
	Log::LogMessage( "GLContext :: GLContext - Default screen retrieved");
	int l_nbConfigs = 0;
	GLXFBConfig * l_configs = glXGetFBConfigs( m_display, l_screen, & l_nbConfigs);
	Log::LogMessage( "GLContext :: GLContext - Configs list retrieved");
	int i = 0;
	bool l_found = false;
	GLXFBConfig l_finalConfig;
	while (i < l_nbConfigs&&  ! l_found)
	{
		if (_checkConfig( l_configs[i]))
		{
			Log::LogMessage( "GLContext - config : %d seems valid", i);
			l_finalConfig = l_configs[i];
			l_found = true;
		}
		i++;
	}
	if ( ! l_found)
	{
		std::cerr << "No appropriate config found\n";
		exit( 1);
	}
	m_xWindow = GDK_WINDOW_XWINDOW( l_widget->window);
	Log::LogMessage( "GLContext :: GLContext - X Window retrieved");
	m_drawable = glXCreateWindow( m_display, l_finalConfig, m_xWindow, NULL);
	Log::LogMessage( "GLContext :: GLContext - GL Window created");
	GLContext * l_mainContext = static_cast <GLRenderSystem *>( RenderSystem::GetSingletonPtr())->GetMainContext();  
	if (l_mainContext != NULL)
	{
		m_context = GLCreateContext( m_display, l_finalConfig, GLX_RGBA_TYPE, l_mainContext->GetContext(), true);
	}
	else
	{
		m_context = GLCreateContext( m_display, l_finalConfig, GLX_RGBA_TYPE, None, true);
	}
	Log::LogMessage( "GLContext :: GLContext - Context created");
/*
	int l_screen;
	XVisualInfo * l_visualInfo;
	int l_attrs1[] = {GLX_RGBA, 1, None};
	int l_attrs2[] = {GLX_BUFFER_SIZE, 1, None};
	XSetWindowAttributes l_windowAttr;
	unsigned long l_mask;

	GtkWidget * l_widget = static_cast <GtkWidget *>( m_window->GetHWnd());
	Log::LogMessage( "GLContext :: GLContext - Widget retrieved");
	m_display = GDK_DISPLAY_XDISPLAY( gtk_widget_get_display( l_widget));
	Log::LogMessage( "GLContext :: GLContext - Display retrieved");

	l_screen = DefaultScreen( m_display);

	l_visualInfo = glXChooseVisual( m_display, l_screen, l_attrs1);
	if (l_visualInfo == NULL)
	{
		Log::LogMessage( "GLContext :: GLContext - no RGB visual found");
    	l_visualInfo = glXChooseVisual( m_display, l_screen, l_attrs2);
  	}
  	if (l_visualInfo == NULL)
	{
	Log::LogMessage( "GLContext :: GLContext - no good visual found");
    	exit( 1);
  	}
	Log::LogMessage( "GLContext :: GLContext - X Visual retrieved");

	l_windowAttr.background_pixel = 0;
  	l_windowAttr.border_pixel = 0;
  	l_windowAttr.colormap = XCreateColormap( m_display, RootWindow( m_display, l_screen),
				  							 l_visualInfo->visual, AllocNone);
	Log::LogMessage( "GLContext :: GLContext - Colour map retrieved");
  	l_windowAttr.event_mask = StructureNotifyMask | ExposureMask;
  	l_mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	m_xWindow = XCreateWindow( m_display, RootWindow( m_display, l_screen),
							   0, 0, 200, 200, 0, l_visualInfo->depth,
							   InputOutput, l_visualInfo->visual, l_mask,
							   & l_windowAttr);
	Log::LogMessage( "GLContext :: GLContext - X Window created");
  	XMapWindow( m_display, m_xWindow);
	Log::LogMessage( "GLContext :: GLContext - Window mapped");

	GLContext * l_mainContext = static_cast <GLRenderSystem *>( RenderSystem::GetSingletonPtr())->GetMainContext();  
	if (l_mainContext != NULL)
	{
		m_context = GLCreateContext( m_display, l_visualInfo, l_mainContext->GetContext(), true);ontext(), true);
	}
	else
	{
		m_context = GLCreateContext( m_display, l_visualInfo, NULL, true);
	}
*/
	m_initialised = true;
}

bool GLContext :: _checkConfig( const GLXFBConfig & p_config)
{
	if ( ! _checkConfigAttrib( p_config, GLX_X_RENDERABLE, 1))
	{
		return false;
	}
	if ( ! _checkConfigAttrib( p_config, GLX_DOUBLEBUFFER, 1))
	{
		return false;
	}
	return true;
}

bool GLContext :: _checkConfigAttrib( const GLXFBConfig & p_config, int p_attrib,
									  int p_condition)
{
	int l_attributeRes;
	if (glXGetFBConfigAttrib( m_display, p_config, p_attrib, & l_attributeRes) != 0)
	{
		return false;
	}
	return l_attributeRes == p_condition;
}
#endif

void GLContext :: SetCurrent()
{
#ifdef WIN32
	GLMakeCurrent( m_HDC, m_HGLRC);
#else
#	ifdef __GNUG__
	if ( ! m_initialised)
	{
		Initialise();
	}
	GLMakeCurrent( m_display, m_drawable, m_drawable, m_context);
#	endif
#endif
}

void GLContext :: EndCurrent()
{
#ifdef WIN32
     GLMakeCurrent( NULL, NULL);
#else
#	ifdef __GNUG__  
	GLMakeCurrent( m_display, None, None, NULL);
#	endif
#endif
}

GLContext * GLContext :: Clone()const
{
#ifdef WIN32
	HGLRC l_newHGLRC = GLCreateContext( m_HDC);
	
	if (l_newHGLRC == 0)
	{
		Log::LogMessage( "Can't create context");
		return NULL;
	}

	HGLRC l_oldHGLRC = GLGetCurrentContext();
	HDC l_oldHDC = wglGetCurrentDC();
	wglMakeCurrent( NULL, NULL);

    if ( ! wglShareLists( m_HGLRC, l_newHGLRC))
	{
		wglDeleteContext( l_newHGLRC);
		Log::LogMessage( "Can't share lists");
	}
	// restore old context
	wglMakeCurrent( l_oldHDC, l_oldHGLRC);
	

	return new GLContext( m_window, l_newHGLRC);
#endif
	return NULL;
}