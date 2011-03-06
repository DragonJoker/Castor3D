#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlContext.h"
#include "OpenGlCommon/GlRenderSystem.h"

#ifdef _WIN32

using namespace Castor3D;

GlContext :: GlContext()
    :	Context( NULL)
    ,	m_hWnd( NULL)
    ,   m_hContext( NULL)
{
}

GlContext :: GlContext( RenderWindow * p_window)
    :	Context( p_window)
    ,	m_hWnd( NULL)
    ,   m_hContext( NULL)
{
    _createGlContext();
    _setMainContext();
}

GlContext :: ~GlContext()
{
}

void GlContext :: SetCurrent()
{
    CheckGlError( wglMakeCurrent( m_hDC, m_hContext), CU_T( "GlMswContext :: SetCurrent - glMakeCurrent"));
}

void GlContext :: EndCurrent()
{
    wglMakeCurrent( NULL, NULL);
}

void GlContext :: SwapBuffers()
{
    ::SwapBuffers( m_hDC);
}

void GlContext :: _createGlContext()
{
    PIXELFORMATDESCRIPTOR l_pfd;
    memset( & l_pfd, 0, sizeof( l_pfd));
    l_pfd.nSize = sizeof( l_pfd);
    l_pfd.nVersion = 1;
    l_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    l_pfd.iPixelType = PFD_TYPE_RGBA;
    l_pfd.cColorBits = GlEnum::Get( m_pWindow->GetPixelFormat()).Format;
    l_pfd.cDepthBits = 16;
    l_pfd.iLayerType = PFD_MAIN_PLANE;

    m_hDC = ::GetDC( static_cast <HWND>( m_pWindow->GetHandle()));
    int iFormat = ChoosePixelFormat( m_hDC, & l_pfd);
    SetPixelFormat( m_hDC, iFormat, & l_pfd);

    m_hContext = glCreateContext( m_hDC);
}

void GlContext :: _setMainContext()
{
    GlContext * l_mainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

    if (l_mainContext != NULL)
    {
        if (glShareLists( l_mainContext->GetContext(), m_hContext) != TRUE)
        {
            Logger::LogWarning( CU_T( "Can't share context"));
        }
    }
    else
    {
        RenderSystem::GetSingletonPtr()->SetMainContext( this);
    }

    Logger::LogMessage( CU_T( "GLContext :: GLContext - Main Context set"));
    RenderSystem::GetSingletonPtr()->AddContext( this, m_pWindow);
    Logger::LogMessage( CU_T( "GLContext :: GLContext - Context added"));
}
#endif
