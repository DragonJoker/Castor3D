#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3Context.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"

#ifndef _WIN32
#	ifndef	CASTOR_GTK
using namespace Castor3D;

Gl3Context :: Gl3Context( RenderWindow * p_window)
    :   GlContext()
{
    m_pWindow = p_window;
    _createGlContext();
    _createGl3Context();
    _setMainContext();
}

Gl3Context :: ~Gl3Context()
{
}

void Gl3Context :: _createGlContext()
{
    GlContext::_createGlContext();
}

void Gl3Context :: _setMainContext()
{
    GlContext::_setMainContext();
}

void Gl3Context :: _createGl3Context()
{
    if (m_pDisplay != NULL)
    {
        GLXContext l_context = m_context;
        SetCurrent();

        if ( ! Gl3RenderSystem::IsInitialised())
        {
            Gl3RenderSystem::GetSingletonPtr()->Initialise();
            Gl3RenderSystem::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->Initialise();
        }

        int l_attribs[] =
        {
            GL_CONTEXT_MAJOR_VERSION, Gl3RenderSystem::GetOpenGlMajor(),
            GL_CONTEXT_MINOR_VERSION, Gl3RenderSystem::GetOpenGlMinor(),
            GL_CONTEXT_FLAGS_ARB, GL_FORWARD_COMPATIBLE_BIT,
            0
        };

        if (glCreateContextAttribs != NULL)
        {
            Gl3Context * l_pMainContext = Gl3RenderSystem::GetSingletonPtr()->GetMainContext();

            if (l_pMainContext != NULL)
            {
                m_context = glCreateContextAttribs( m_pDisplay, NULL, l_pMainContext->GetContext(), True, l_attribs);
            }
            else
            {
                m_context = glCreateContextAttribs( m_pDisplay, NULL, NULL, False, l_attribs);
            }

            CheckGlError( CU_T( "Gl3X11Context :: Gl3X11Context - glCreateContextAttribs"));
            SetCurrent();
            glDeleteContext( m_pDisplay, l_context);
            CheckGlError( CU_T( "Gl3X11Context :: Gl3X11Context - wglDeleteContext"));
            Logger::LogMessage( CU_T( "Gl3X11Context :: Gl3X11Context - OpenGL 3.x context created"));
        }
        else
        {	//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
            Logger::LogError( CU_T( "Gl3X11Context :: Gl3X11Context - Can't create OpenGL 3.x context"));
        }

        EndCurrent();
    }
}
#endif
#endif
