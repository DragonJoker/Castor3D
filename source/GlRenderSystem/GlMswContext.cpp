#include "GlRenderSystem/PrecompiledHeader.hpp"

#if defined( _WIN32 )

#	include "GlRenderSystem/GlMswContext.hpp"
#	include "GlRenderSystem/GlContext.hpp"
#	include "GlRenderSystem/GlRenderSystem.hpp"
#	include "GlRenderSystem/OpenGl.hpp"

#	include <Castor3D/WindowHandle.hpp>
#	include <Castor3D/Material.hpp>

using namespace GlRender;
using namespace Castor3D;
using namespace Castor;

GlContextImpl :: GlContextImpl( OpenGl & p_gl, GlContext * p_pContext )
    :	m_hDC			( NULL			)
    ,   m_hContext		( NULL			)
	,	m_hWnd			( NULL			)
	,	m_pContext		( p_pContext	)
	,	m_bInitialised	( false			)
	,	m_gl			( p_gl			)
{
}

bool GlContextImpl :: Initialise( RenderWindow * p_window )
{
	GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
	GlContextSPtr l_pMainContext = std::static_pointer_cast< GlContext >( l_pRenderSystem->GetMainContext() );
	m_hDC = ::GetDC( p_window->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd() );
	bool l_bHasPF = false;

	if ( !l_pRenderSystem->IsInitialised() )
	{
		m_hContext = DoCreateDummyContext( p_window );
		SetCurrent();

		typedef const char * (PFNWGLGETEXTENSIONSSTRINGEXTPROC)();
		PFNWGLGETEXTENSIONSSTRINGEXTPROC * wglGetExtensionsStringEXT;
		wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC*)wglGetProcAddress( "wglGetExtensionsStringEXT" );

		if( wglGetExtensionsStringEXT() )
		{
			l_pRenderSystem->Initialise( str_utils::from_str( wglGetExtensionsStringEXT() ) );
		}
		else
		{
			l_pRenderSystem->Initialise( cuT( "" ) );
		}
		
		p_window->GetEngine()->GetMaterialManager().Initialise();
		EndCurrent();
		m_gl.DeleteContext( m_hContext );
		m_hContext = NULL;
	}

	if( !l_pMainContext )
	{
		if( p_window->IsUsingStereo() )
		{
			l_bHasPF = DoSelectStereoPixelFormat( p_window );
		}
		else
		{
			l_pRenderSystem->SetStereoAvailable( false );
			l_bHasPF = DoSelectPixelFormat( p_window );
		}
	}
	else
	{
		l_bHasPF = DoSelectPixelFormat( p_window );
	}

	if( l_bHasPF )
	{
		m_hContext = m_gl.CreateContext( m_hDC );

		if( l_pRenderSystem->GetOpenGlMajor() >= 3 )
		{
			m_bInitialised = DoCreateGl3Context( p_window );
		}
		else
		{
			if( l_pMainContext )
			{
				wglShareLists( m_hContext, l_pMainContext->GetImpl()->GetContext() );
			}

			m_bInitialised = true;
		}
	}
	else
	{
		Logger::LogError( str_utils::from_str( "No supported pixel format found, context creation failed" ), false );
	}

	if( m_bInitialised )
	{
#if !defined( NDEBUG )
		if( m_gl.HasDebugOutput() )
		{
			SetCurrent();
			m_gl.DebugMessageCallback( OpenGl::PFNGLDEBUGPROC( &OpenGl::StDebugLog ), &m_gl );
			m_gl.DebugMessageCallback( OpenGl::PFNGLDEBUGAMDPROC( &OpenGl::StDebugLogAMD ), &m_gl );
			m_gl.Enable( eGL_TWEAK_DEBUG_OUTPUT_SYNCHRONOUS );
			EndCurrent();
		}
#endif
		UpdateVSync( p_window->GetVSync() );
	}

	return m_bInitialised;
}

GlContextImpl :: ~GlContextImpl()
{
	m_gl.DeleteContext( m_hContext );
}

void GlContextImpl :: SetCurrent()
{
    m_gl.MakeCurrent( m_hDC, m_hContext );
}

void GlContextImpl :: EndCurrent()
{
    m_gl.MakeCurrent( NULL, NULL );
}

void GlContextImpl :: SwapBuffers()
{
    m_gl.SwapBuffers( m_hDC );
}

void GlContextImpl :: UpdateVSync( bool p_bEnable )
{
	SetCurrent();
	Logger::LogDebug( cuT( "GlContextImpl :: UpdateVSync" ) );

	if( p_bEnable )
	{
		m_gl.SwapInterval( 1 );
	}
	else
	{
		m_gl.SwapInterval( 0 );
	}

	EndCurrent();
}

bool GlContextImpl :: DoCreateGl3Context( RenderWindow * p_window )
{
	bool l_bReturn = false;

	try
	{
		GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
#if defined( NDEBUG ) || !C3DGL_LIMIT_TO_2_1
		if( m_gl.HasCreateContextAttribs() )
#else
		if( false )
#endif
		{
			HGLRC l_hContext = m_hContext;

			IntArray l_attribList;
			l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_MAJOR_VERSION	);l_attribList.push_back( l_pRenderSystem->GetOpenGlMajor()			);
			l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_MINOR_VERSION	);l_attribList.push_back( l_pRenderSystem->GetOpenGlMinor()			);
#if !defined( NDEBUG )
			l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_FLAGS			);l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_FORWARD_COMPATIBLE_BIT | eGL_CREATECONTEXT_ATTRIB_DEBUG_BIT	);
			l_attribList.push_back( eGL_PROFILE_ATTRIB_MASK					);l_attribList.push_back( eGL_PROFILE_ATTRIB_COMPATIBILITY_BIT				);
#else
			l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_FLAGS			);l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_FORWARD_COMPATIBLE_BIT	);
			l_attribList.push_back( eGL_PROFILE_ATTRIB_MASK					);l_attribList.push_back( eGL_PROFILE_ATTRIB_CORE_BIT						);
#endif
			l_attribList.push_back( 0										);

			GlContextSPtr l_pMainContext = std::static_pointer_cast< GlContext >( l_pRenderSystem->GetMainContext() );
			SetCurrent();

			if( l_pMainContext )
			{
				m_hContext = m_gl.CreateContextAttribs( m_hDC, l_pMainContext->GetImpl()->GetContext(), &l_attribList[0] );
			}
			else
			{
				m_hContext = m_gl.CreateContextAttribs( m_hDC, NULL, &l_attribList[0] );
			}

			EndCurrent();
			m_gl.DeleteContext( l_hContext );
			Logger::LogMessage( cuT( "GlContext :: GlContext - OpenGL 3.x context created" ) );
			l_bReturn = true;
		}
		else
		{	//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
			l_pRenderSystem->SetOpenGlVersion( 2, 1 );
			Logger::LogWarning( cuT( "GlContext :: GlContext - Can't create OpenGL 3.x context" ) );
			l_bReturn = true;
		}
	}
	catch( ... )
	{
		l_bReturn = false;
	}

	return l_bReturn;
}

HGLRC GlContextImpl :: DoCreateDummyContext( RenderWindow * p_window )
{
	HGLRC					l_hReturn		= NULL;
	int						l_iPixelFormat	= 0;
	PIXELFORMATDESCRIPTOR	l_pfd			= { 0 };

	l_pfd.nSize			= sizeof( PIXELFORMATDESCRIPTOR );
	l_pfd.nVersion		= 1;
	l_pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	l_pfd.iPixelType	= PFD_TYPE_RGBA;
	l_pfd.cColorBits	= BYTE( m_gl.Get( p_window->GetPixelFormat() ).Format );
	l_pfd.cDepthBits	= 16;
	l_pfd.iLayerType	= PFD_MAIN_PLANE;

	l_iPixelFormat = ::ChoosePixelFormat( m_hDC, &l_pfd );

	if( ::SetPixelFormat( m_hDC, l_iPixelFormat, &l_pfd ) )
	{
		l_hReturn = m_gl.CreateContext( m_hDC );
	}

	return l_hReturn;
}

bool GlContextImpl :: DoSelectPixelFormat( RenderWindow * p_window )
{
	bool					l_bReturn			= false;
	PIXELFORMATDESCRIPTOR	l_pfd				= { 0 };

	l_pfd.nSize			= sizeof( PIXELFORMATDESCRIPTOR );
	l_pfd.nVersion		= 1;
	l_pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	l_pfd.iPixelType	= PFD_TYPE_RGBA;
	l_pfd.cColorBits	= 32;
	l_pfd.cDepthBits	= 24;
	l_pfd.iLayerType	= PFD_MAIN_PLANE;

	if( p_window->IsUsingStereo() && p_window->GetEngine()->GetRenderSystem()->IsStereoAvailable() )
	{
		l_pfd.dwFlags	|= PFD_STEREO;
	}

	if( !l_bReturn )
	{
		int l_iPixelFormats[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		l_iPixelFormats[0] = ::ChoosePixelFormat( m_hDC, &l_pfd );
		l_bReturn = ::SetPixelFormat( m_hDC, l_iPixelFormats[0], &l_pfd ) == TRUE;
	}

	return l_bReturn;
}

bool GlContextImpl :: DoSelectStereoPixelFormat( RenderWindow * p_window )
{
	bool l_bReturn = false;
	GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
	PIXELFORMATDESCRIPTOR l_pfd = { 0 };
	l_pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
	int l_iPixelFormat = ::DescribePixelFormat( m_hDC, 1, sizeof( PIXELFORMATDESCRIPTOR ), &l_pfd );
	bool l_bStereoAvailable = false;

	while( l_iPixelFormat && !l_bStereoAvailable )
	{
		::DescribePixelFormat( m_hDC, l_iPixelFormat, sizeof( PIXELFORMATDESCRIPTOR ), &l_pfd );

		if( (l_pfd.dwFlags & PFD_STEREO) && (l_pfd.dwFlags & PFD_SUPPORT_OPENGL) && (l_pfd.dwFlags & PFD_DOUBLEBUFFER) && (l_pfd.dwFlags & PFD_DRAW_TO_WINDOW) && l_pfd.iPixelType == PFD_TYPE_RGBA && l_pfd.cColorBits == 32 && l_pfd.cDepthBits == 24 )
		{
			l_bStereoAvailable = true;
		}
		else
		{
			l_iPixelFormat--;
		}
	}

	if( l_bStereoAvailable )
	{
		l_bReturn = ::SetPixelFormat( m_hDC, l_iPixelFormat, &l_pfd ) == TRUE;

		if( !l_bReturn )
		{
			l_bStereoAvailable = false;
			l_bReturn = DoSelectPixelFormat( p_window );
		}
	}
	else
	{
		l_bReturn = DoSelectPixelFormat( p_window );
	}

	l_pRenderSystem->SetStereoAvailable( l_bStereoAvailable );
	return l_bReturn;
}
#endif
