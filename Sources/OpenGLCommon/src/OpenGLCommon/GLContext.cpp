#include "PrecompiledHeader.h"

#include "OpenGLCommon/GLContext.h"

using namespace Castor3D;

GLContext :: GLContext()
	:	Context( NULL),
#ifdef WIN32
		m_hWnd( NULL)
#else
		m_initialised( false)
#endif
{
}

GLContext :: GLContext( RenderWindow * p_window, void * p_win)
	:	Context( p_window),
#ifdef WIN32
		m_hWnd( (HWND)p_win)
#endif
{
	_createGLContext();
	_setMainContext();
}

GLContext :: ~GLContext()
{
}
#ifdef __GNUG__
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
	glMakeCurrent( m_hDC, m_context);
#else
#	ifdef __GNUG__
	glMakeCurrent( m_display, m_drawable, m_drawable, m_context);
#	endif
#endif
	CheckGLError( CU_T( "GLContext :: SetCurrent - glMakeCurrent"));
}

void GLContext :: EndCurrent()
{
#ifdef WIN32
     glMakeCurrent( NULL, NULL);
#else
#	ifdef __GNUG__  
	glMakeCurrent( m_display, None, None, NULL);
#	endif
#endif
}

void GLContext :: _createGLContext()
{
#ifdef WIN32
	PIXELFORMATDESCRIPTOR l_pfd;
	memset( & l_pfd, 0, sizeof( l_pfd));
	l_pfd.nSize = sizeof( l_pfd);
	l_pfd.nVersion = 1;
	l_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	l_pfd.iPixelType = PFD_TYPE_RGBA;
	l_pfd.cColorBits = GetPixelFormatBits( m_window->GetPixelFormat());
	l_pfd.cDepthBits = 16;
	l_pfd.iLayerType = PFD_MAIN_PLANE;

	m_hDC = ::GetDC( static_cast <HWND>( m_window->GetHWnd()));
	int iFormat = ChoosePixelFormat( m_hDC, & l_pfd);
	SetPixelFormat( m_hDC, iFormat, & l_pfd);

	m_context = glCreateContext( m_hDC);
#else
	int l_screen;
	GtkWidget * l_widget = static_cast <GtkWidget *>( m_window->GetHWnd());
	m_display = GDK_DISPLAY_XDISPLAY( gtk_widget_get_display( l_widget));
	Logger::LogMessage( CU_T( "GLContext :: GLContext - Display retrieved"));

	l_screen = DefaultScreen( m_display);
	Logger::LogMessage( CU_T( "GLContext :: GLContext - Default screen retrieved"));
	int l_nbConfigs = 0;
	GLXFBConfig * l_configs = glXGetFBConfigs( m_display, l_screen, & l_nbConfigs);
	Logger::LogMessage( CU_T( "GLContext :: GLContext - Configs list retrieved"));
	int i = 0;
	GLXFBConfig l_finalConfig;
	m_iSelectedConfigIndex = -1;

	while (i < l_nbConfigs && m_iSelectedConfigIndex < 0)
	{
		if (_checkConfig( l_configs[i]))
		{
			Logger::LogMessage( CU_T( "GLContext - config : %d seems valid", i));
			l_finalConfig = l_configs[i];
			m_iSelectedConfigIndex = i;
		}

		i++;
	}

	if ( ! l_found)
	{
		Logger::LogError << "No appropriate config found\n";
		exit( 1);
	}

	m_xWindow = GDK_WINDOW_XWINDOW( l_widget->window);
	Logger::LogMessage( CU_T( "GLContext :: GLContext - X Window retrieved"));
	m_drawable = glXCreateWindow( m_display, l_finalConfig, m_xWindow, NULL);
	Logger::LogMessage( CU_T( "GLContext :: GLContext - GL Window created"));
	GLContext * l_mainContext = static_cast <GLRenderSystem *>( RenderSystem::GetSingletonPtr())->GetMainContext();  

	if (l_mainContext != NULL)
	{
		m_context = GLCreateContext( m_display, l_finalConfig, GLX_RGBA_TYPE, l_mainContext->GetContext(), true);
	}
	else
	{
		m_context = GLCreateContext( m_display, l_finalConfig, GLX_RGBA_TYPE, None, true);
	}

	Logger::LogMessage( CU_T( "GLContext :: GLContext - Context created"));
#endif
}

void GLContext :: _setMainContext()
{
	GLContext * l_mainContext = static_cast<GLContext *>( RenderSystem::GetSingletonPtr()->GetMainContext());
#ifdef WIN32
	if (l_mainContext != NULL)
	{
		if (glShareLists( l_mainContext->GetContext(), m_context) != TRUE)
		{
			Logger::LogWarning( CU_T( "Can't share context"));
		}
	}
#endif
	if (l_mainContext == NULL)
	{
		RenderSystem::GetSingletonPtr()->SetMainContext( this);
	}

	Logger::LogMessage( CU_T( "GLContext :: GLContext - Main Context set"));
	RenderSystem::GetSingletonPtr()->AddContext( this, m_window);
	Logger::LogMessage( CU_T( "GLContext :: GLContext - Context added"));
}