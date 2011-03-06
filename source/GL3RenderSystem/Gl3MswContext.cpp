#include "Gl3RenderSystem/PrecompiledHeader.h"

#include "Gl3RenderSystem/Gl3Context.h"
#include "Gl3RenderSystem/Gl3RenderSystem.h"

#ifdef _WIN32
using namespace Castor3D;

Gl3Context :: Gl3Context( RenderWindow * p_window)
    :	GlContext()
{
    m_pWindow = p_window;
    _createGlContext();
    _createGl3Context();
    _setMainContext();
}

Gl3Context :: ~Gl3Context()
{
}

void Gl3Context :: _createGl3Context()
{
    HGLRC l_context = m_hContext;
    SetCurrent();

    if ( ! Gl3RenderSystem::IsInitialised())
    {
        Gl3RenderSystem::GetSingletonPtr()->Initialise();
        Gl3RenderSystem::GetSingletonPtr()->GetSceneManager()->GetMaterialManager()->Initialise();
    }

    int l_attribs[] =
    {
        GL_CONTEXT_MAJOR_VERSION,	Gl3RenderSystem::GetOpenGlMajor(),
        GL_CONTEXT_MINOR_VERSION,	Gl3RenderSystem::GetOpenGlMinor(),
        GL_CONTEXT_FLAGS_ARB,			GL_FORWARD_COMPATIBLE_BIT,
        0
    };

    if (glCreateContextAttribs != NULL)
    {
        Gl3Context * l_mainContext = Gl3RenderSystem::GetSingletonPtr()->GetMainContext();

        if (l_mainContext != NULL)
        {
            m_hContext = glCreateContextAttribs( m_hDC, l_mainContext->GetContext(), l_attribs);
        }
        else
        {
            m_hContext = glCreateContextAttribs( m_hDC, 0, l_attribs);
        }

        EndCurrent();
        glDeleteContext( l_context);
        Logger::LogMessage( CU_T( "Gl3Context :: Gl3Context - OpenGL 3.x context created"));
    }
    else
    {	//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
        Logger::LogError( CU_T( "Gl3Context :: Gl3Context - Can't create OpenGL 3.x context"));
    }

    EndCurrent();
}
#endif
