#include "OpenGlCommon/PrecompiledHeader.h"

#include "OpenGlCommon/GlContext.h"
#include "OpenGlCommon/GlRenderSystem.h"

#ifndef _WIN32
#ifndef CASTOR_GTK

using namespace Castor3D;

GlContext :: GlContext()
    :	Context( NULL)
    ,   m_pDisplay( NULL)
{
}

GlContext :: GlContext( RenderWindow * p_window)
    :	Context( p_window)
    ,   m_pDisplay( NULL)
{
    _createGlContext();
    _setMainContext();
}

GlContext :: ~GlContext()
{
    glXDestroyContext( m_pDisplay, m_context);
    m_context = NULL;
}

void GlContext :: SetCurrent()
{
    glXMakeCurrent( m_pDisplay, reinterpret_cast<GLXDrawable>( m_pWindow->GetHandle()), m_context);
    CheckGlError( CU_T( "GlX11Context :: SetCurrent - glMakeCurrent"));
}

void GlContext :: EndCurrent()
{
    glXMakeCurrent( m_pDisplay, None, NULL);
}

void GlContext :: SwapBuffers()
{
    glXSwapBuffers( m_pDisplay, reinterpret_cast<GLXDrawable>( m_pWindow->GetHandle()));
}

GLXFBConfig * GlContext :: ChooseFBConfig( const GLint *attribList, GLint * nElements)
{
    Logger::LogMessage( "GlX11Context :: ChooseFBConfig - Start");
    glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glXGetProcAddressARB( (unsigned char *)"glXChooseFBConfig");
    GLXFBConfig * fbConfigs;

    fbConfigs = glXChooseFBConfig( m_pDisplay, DefaultScreen( m_pDisplay), attribList, nElements);

    Logger::LogMessage( "GlX11Context :: ChooseFBConfig - End");
    return fbConfigs;
}

XVisualInfo * GlContext :: GetVisualFromFBConfig( GLXFBConfig fbConfig)
{
    XVisualInfo *visualInfo;

    if (GLXEW_VERSION_1_3)
        visualInfo = glXGetVisualFromFBConfig( m_pDisplay, fbConfig);
    else
        visualInfo = glXGetVisualFromFBConfigSGIX( m_pDisplay, fbConfig);

    return visualInfo;
}

GLXFBConfig GlContext :: GetFBConfigFromVisualID( VisualID visualid)
{
    GLXFBConfig fbConfig = NULL;

    if (GLXEW_SGIX_fbconfig && glXGetFBConfigFromVisualSGIX)
    {
        XVisualInfo visualInfo;

        visualInfo.screen = DefaultScreen( m_pDisplay);
        visualInfo.depth = DefaultDepth( m_pDisplay, DefaultScreen( m_pDisplay));
        visualInfo.visualid = visualid;

        fbConfig = glXGetFBConfigFromVisualSGIX( m_pDisplay, & visualInfo);
    }

    if (! fbConfig)
    {
        int minAttribs[] = {
            GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT || GLX_PIXMAP_BIT,
            GLX_RENDER_TYPE,	GLX_RGBA_BIT,
            GLX_RED_SIZE,	   1,
            GLX_BLUE_SIZE,	  1,
            GLX_GREEN_SIZE,	 1,
            None
        };
        int nConfigs = 0;

        GLXFBConfig *fbConfigs = ChooseFBConfig(minAttribs, &nConfigs);

        for (int i = 0; i < nConfigs && ! fbConfig; i++)
        {
            XVisualInfo *visualInfo = GetVisualFromFBConfig(fbConfigs[i]);

            if (visualInfo->visualid == visualid)
                fbConfig = fbConfigs[i];

            XFree(visualInfo);
        }

        XFree(fbConfigs);
    }

    return fbConfig;
}

GLXFBConfig GlContext :: GetFBConfigFromDrawable( GLXDrawable drawable, unsigned int * width, unsigned int * height)
{
    Logger::LogMessage( "GlX11Context :: GetFBConfigFromDrawable - Start");

    GLXFBConfig fbConfig = 0;

    int fbConfigAttrib[] = {
        GLX_FBCONFIG_ID, 0,
        None
    };

    static int visual_attribs[] =
    {
      GLX_X_RENDERABLE    , True,
      GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
      GLX_RED_SIZE        , 8,
      GLX_GREEN_SIZE      , 8,
      GLX_BLUE_SIZE       , 8,
      GLX_ALPHA_SIZE      , 8,
      GLX_DEPTH_SIZE      , 16,
      GLX_STENCIL_SIZE    , 8,
      GLX_DOUBLEBUFFER    , True,
      //GLX_SAMPLE_BUFFERS  , 1,
      //GLX_SAMPLES         , 4,
      None
    };


    GLXFBConfig *fbConfigs;
    int nElements = 0;
    GLXFBConfig * fbc = ChooseFBConfig( visual_attribs, & nElements);

    if (fbc == NULL)
    {
        Logger::LogError( "GlX11Context :: GetFBConfigFromDrawable - Can't retrieve a FB config");
    }

    Logger::LogMessage( "GlX11Context :: GetFBConfigFromDrawable - %i", nElements);
/*
    if (nElements)
    {
        fbConfig = fbConfigs[0];
        XFree (fbConfigs);

        if (width != NULL)
        {
            glXQueryDrawable( m_pDisplay, drawable, GLX_WIDTH, width);
        }

        if (height != NULL)
        {
            glXQueryDrawable( m_pDisplay, drawable, GLX_HEIGHT, height);
        }
    }

    if (! fbConfig && GLXEW_SGIX_fbconfig)
    {
        Logger::LogMessage( CU_T( "GlX11Context :: GetFBConfigFromDrawable - GLXEW_SGIX_fbconfig && ! fbConfig - drawable : %i"), (int)drawable);
        XWindowAttributes windowAttrib;

        if (XGetWindowAttributes( m_pDisplay, drawable, & windowAttrib))
        {
            Logger::LogMessage( CU_T( "GlX11Context :: GetFBConfigFromDrawable - X Window attributes retrieved"));
            VisualID visualid = XVisualIDFromVisual(windowAttrib.visual);

            fbConfig = GetFBConfigFromVisualID(visualid);
            Logger::LogMessage( CU_T( "GlX11Context :: GetFBConfigFromDrawable - fbConfig : %i"), fbConfig);

            if (width != NULL)
            {
                *width = windowAttrib.width;
            }

            if (height != NULL)
            {
                *height = windowAttrib.height;
            }
        }
    }
    else
    {
        Logger::LogMessage( CU_T( "GlX11Context :: GetFBConfigFromDrawable - GLXEW_SGIX_fbconfig : %i - fbConfig - %i"), GLXEW_SGIX_fbconfig, fbConfig);
    }
*/
    return fbConfig;
}

Display * GlContext :: GetGLDisplay()
{
    if (m_pDisplay == NULL)
    {
        glXGetCurrentDisplay = (PFNGLXGETCURRENTDISPLAYPROC)glXGetProcAddressARB( (unsigned char *)"glXGetCurrentDisplay");

        m_pDisplay = glXGetCurrentDisplay();

        if (m_pDisplay == NULL)
        {
            m_pDisplay = XOpenDisplay( 0);
        }

        if (m_pDisplay == NULL)
        {
            Logger::LogError( CU_T( "GlX11Context :: GetGLDisplay - Couldn`t open X display ") + String( (const char *)XDisplayName( 0)));
        }
        else
        {
            Logger::LogMessage( CU_T( "GlX11Context :: GetGLDisplay - Opened X display [") + String( (const char *)XDisplayName( 0)) + CU_T( "]"));
        }
    }

    return m_pDisplay;
}

void GlContext :: _createGlContext()
{
    GetGLDisplay();
    GLXFBConfig l_pFBConfig = GetFBConfigFromDrawable( reinterpret_cast<GLXDrawable>( m_pWindow->GetHandle()), NULL, NULL);

    if (l_pFBConfig != NULL)
    {
        GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

        if (l_pMainContext != NULL)
        {
            m_context = glXCreateNewContext( m_pDisplay, l_pFBConfig, GLX_RGBA_TYPE, l_pMainContext->GetContext(), GL_TRUE);
        }
        else
        {
            m_context = glXCreateNewContext( m_pDisplay, l_pFBConfig, GLX_RGBA_TYPE, NULL, GL_TRUE);
        }
    }
    else
    {
        Logger::LogError( CU_T( "GlX11Context :: Create - Can't retrieve GLXFBConfig"));
    }
}

void GlContext :: _setMainContext()
{
    GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

    if (l_pMainContext == NULL)
    {
        RenderSystem::GetSingletonPtr()->SetMainContext( this);
    }

    Logger::LogMessage( CU_T( "GlX11Context :: Create - Main Context set"));
}

#endif
#endif
