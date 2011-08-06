#include "GlRenderSystem/PrecompiledHeader.hpp"

#include "GlRenderSystem/GlContext.hpp"
#include "GlRenderSystem/GlRenderSystem.hpp"
#include "GlRenderSystem/OpenGl.hpp"

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
	m_bInitialised = true;
}

GlContext :: ~GlContext()
{
	OpenGl::DeleteContext( m_hContext);
}

void GlContext :: SetCurrent()
{
	CASTOR_TRACK;
    OpenGl::MakeCurrent( m_hDC, m_hContext);
}

void GlContext :: EndCurrent()
{
	CASTOR_TRACK;
    OpenGl::MakeCurrent( NULL, NULL);
}

void GlContext :: SwapBuffers()
{
	CASTOR_TRACK;
    OpenGl::SwapBuffers( m_hDC);
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
	GlContext * l_pMainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

	if (l_pMainContext == NULL)
	{
		m_hContext = OpenGl::CreateContext( m_hDC);
		RenderSystem::GetSingletonPtr()->SetMainContext( this);

		if ( ! GlRenderSystem::IsInitialised())
		{
			SetCurrent();
			GlRenderSystem::GetSingletonPtr()->Initialise();
			Root::GetSingleton()->GetMaterialManager()->Initialise();
			EndCurrent();
		}

		if (GlRenderSystem::GetOpenGlMajor() >= 3)
		{
			_createGl3Context();
		}
	}
	else
	{
		m_hContext = l_pMainContext->GetContext();
	}

	RenderSystem::GetSingletonPtr()->AddContext( this, m_pWindow);
}

void GlContext :: _createGl3Context()
{
	HGLRC l_context = m_hContext;
	SetCurrent();

	int l_attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB,	GlRenderSystem::GetOpenGlMajor(),
		WGL_CONTEXT_MINOR_VERSION_ARB,	GlRenderSystem::GetOpenGlMinor(),
		WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	if (OpenGl::HasCreateContextAttribs())
	{
		GlContext * l_mainContext = GlRenderSystem::GetSingletonPtr()->GetMainContext();

		if (l_mainContext)
		{
			m_hContext = OpenGl::CreateContextAttribs( m_hDC, l_mainContext->GetContext(), l_attribs);
		}
		else
		{
			m_hContext = OpenGl::CreateContextAttribs( m_hDC, 0, l_attribs);
		}

		EndCurrent();
		OpenGl::DeleteContext( l_context);
		Logger::LogMessage( cuT( "GlContext :: GlContext - OpenGL 3.x context created"));
	}
	else
	{	//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
		GlRenderSystem::SetOpenGlVersion( 2, 1);
		Logger::LogError( cuT( "GlContext :: GlContext - Can't create OpenGL 3.x context"));
	}

	EndCurrent();
}
#endif
