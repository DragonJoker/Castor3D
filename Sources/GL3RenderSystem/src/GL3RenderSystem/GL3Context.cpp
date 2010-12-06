#include "PrecompiledHeader.h"

#include "GL3Context.h"
#include "GL3RenderSystem.h"

using namespace Castor3D;

GL3Context :: GL3Context( RenderWindow * p_window, void * p_win)
	:	GLContext()
{
	m_window = p_window;
#ifdef WIN32
	m_hWnd = (HWND)p_win;
#endif

	_createGLContext();
	_createGL3Context();
	_setMainContext();
}

GL3Context :: ~GL3Context()
{
}

void GL3Context :: _createGL3Context()
{
#ifdef WIN32
	HGLRC l_context = m_context;
#else
	GLXContext l_context = m_context;
#endif

	SetCurrent();

	if ( ! GL3RenderSystem::IsInitialised())
	{
		GL3RenderSystem::GetSingletonPtr()->Initialise();
		MaterialManager::Initialise();
	}

	int l_attribs[] =
	{
		GL_CONTEXT_MAJOR_VERSION, GL3RenderSystem::GetOpenGLMajor(),
		GL_CONTEXT_MINOR_VERSION, GL3RenderSystem::GetOpenGLMinor(),
		GL_CONTEXT_FLAGS_ARB, GL_FORWARD_COMPATIBLE_BIT,
		0
	};

	if (glCreateContextAttribs != NULL)
	{
		GLContext * l_mainContext = static_cast <GL3Context *>( RenderSystem::GetSingletonPtr()->GetMainContext());
#ifdef WIN32
		if (l_mainContext != NULL)
		{
			m_context = glCreateContextAttribs( m_hDC, l_mainContext->GetContext(), l_attribs);
		}
		else
		{
			m_context = glCreateContextAttribs( m_hDC, 0, l_attribs);
		}
#else
		int l_iScreen = DefaultScreen( m_display);
		int l_iNbConfigs = 0;
		GLXFBConfig * l_configs = glXGetFBConfigs( m_display, l_iScreen, & l_iNbConfigs);
		GLXFBConfig l_fbConfig = l_configs[m_iSelectedConfigIndex];
		if (l_mainContext != NULL)
		{
			m_context = glCreateContextAttribs( m_display, l_fbConfig, l_mainContext->GetContext());
		}
		else
		{
			m_context = glCreateContextAttribs( m_display, l_fbConfig, None, );
		}
#endif
		CheckGLError( CU_T( "GL3Context :: GL3Context - glCreateContextAttribs"));
		SetCurrent();
		glDeleteContext( l_context);
		CheckGLError( CU_T( "GL3Context :: GL3Context - wglDeleteContext"));
		Logger::LogWarning( CU_T( "GL3Context :: GL3Context - OpenGL 3.x context created"));
	}
	else
	{	//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
		Logger::LogWarning( CU_T( "GL3Context :: GL3Context - Can't create OpenGL 3.x context"));
	}

	EndCurrent();
}