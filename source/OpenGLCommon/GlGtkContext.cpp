#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlContext.h"
#include "OpenGlCommon/GlRenderSystem.h"

#ifndef _WIN32
#	ifdef CASTOR_GTK

using namespace Castor3D;

GlContext :: GlContext()
    :	Context( NULL)
{
}

GlContext :: GlContext( RenderWindow * p_window)
    :	Context( p_window)
{
	_createGlContext();
	_setMainContext();
}

GlContext :: ~GlContext()
{
    glXDestroyContext( GDK_DISPLAY(), m_context);
    m_context = NULL;
}

void GlContext :: SetCurrent()
{
    glXMakeCurrent( GDK_DISPLAY(), reinterpret_cast<GLXDrawable>( m_pWindow->GetHandle()), m_context);
    CheckGlError( CU_T( "GlGtkContext :: SetCurrent - glMakeCurrent"));
}

void GlContext :: EndCurrent()
{
    glXMakeCurrent( GDK_DISPLAY(), None, NULL);
}

void GlContext :: SwapBuffers()
{
    glXSwapBuffers( GDK_DISPLAY(), reinterpret_cast<GLXDrawable>( m_pWindow->GetHandle()));
}

void * GlContext :: ChooseGLFBC()
{
    int data[1];
    data[0] = 0;

    int returned;

    if (glXChooseFBConfig != NULL)
    {
		return glXChooseFBConfig( GDK_DISPLAY(), DefaultScreen(GDK_DISPLAY()),
								  data, & returned);
    }
    else
    {
    	Logger::LogError( CU_T( "GlContext :: ChooseGLFBC - No glXChooseFBConfig"));
    	throw false;
    }
}

void GlContext :: _createGlContext()
{
	m_fbc = (GLXFBConfig * )ChooseGLFBC();

	if (m_fbc != NULL)
	{
        GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

		m_context = glXCreateNewContext( GDK_DISPLAY(), m_fbc[0], GLX_RGBA_TYPE,
										 l_pMainContext != NULL ? l_pMainContext->GetContext() : None,
										 GL_TRUE );

		if ( ! m_context)
		{
			Logger::LogError( CU_T("GlGtkContext :: Create - Couldn't create OpenGl context") );
		}
		else
		{
			m_vi = glXGetVisualFromFBConfig( GDK_DISPLAY(), m_fbc[0]);
		}
	}
	else
	{
		Logger::LogError( "GlGtkContext :: Create - Can't retrieve FB Config");
	}
}

void GlContext :: _setMainContext()
{
    GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

    if (l_pMainContext == NULL)
    {
        RenderSystem::GetSingletonPtr()->SetMainContext( this);
    }

    Logger::LogMessage( CU_T( "GlGtkContext :: Create - Main Context set"));
}
#	endif
#endif
