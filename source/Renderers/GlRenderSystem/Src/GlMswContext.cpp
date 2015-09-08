#if defined( _WIN32 )

#include "GlMswContext.hpp"
#include "GlContext.hpp"
#include "GlRenderSystem.hpp"
#include "OpenGl.hpp"

#include <PlatformWindowHandle.hpp>
#include <Material.hpp>
#include <RenderWindow.hpp>
#include <Logger.hpp>
#include <PixelFormat.hpp>
#include <Utils.hpp>

#include <MaterialManager.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
#if !defined( NDEBUG )
	static const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = eGL_CREATECONTEXT_ATTRIB_FORWARD_COMPATIBLE_BIT | eGL_CREATECONTEXT_ATTRIB_DEBUG_BIT;
	static const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = eGL_PROFILE_ATTRIB_CORE_BIT;
#else
	static const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = eGL_CREATECONTEXT_ATTRIB_FORWARD_COMPATIBLE_BIT;
	static const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = eGL_PROFILE_ATTRIB_COMPATIBILITY_BIT;
#endif

	GlContextImpl::GlContextImpl( OpenGl & p_gl, GlContext * p_pContext )
		: m_hDC( NULL )
		, m_hContext( NULL )
		, m_hWnd( NULL )
		, m_pContext( p_pContext )
		, m_bInitialised( false )
		, m_gl( p_gl )
	{
	}

	bool GlContextImpl::Initialise( RenderWindow * p_window )
	{
		GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
		GlContextSPtr l_pMainContext = std::static_pointer_cast< GlContext >( l_pRenderSystem->GetMainContext() );
		m_hDC = ::GetDC( p_window->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd() );
		bool l_bHasPF = false;

		if ( !l_pRenderSystem->IsInitialised() )
		{
			m_hContext = DoCreateDummyContext( p_window );
			SetCurrent();
			typedef const char * ( PFNWGLGETEXTENSIONSSTRINGEXTPROC )();
			PFNWGLGETEXTENSIONSSTRINGEXTPROC * wglGetExtensionsStringEXT;
			wglGetExtensionsStringEXT = ( PFNWGLGETEXTENSIONSSTRINGEXTPROC * )wglGetProcAddress( "wglGetExtensionsStringEXT" );

			if ( wglGetExtensionsStringEXT() )
			{
				m_gl.PreInitialise( str_utils::from_str( wglGetExtensionsStringEXT() ) );
			}
			else
			{
				m_gl.PreInitialise( String() );
			}

			EndCurrent();
			m_gl.DeleteContext( m_hContext );
			m_hContext = NULL;
		}

		if ( !l_pMainContext )
		{
			if ( p_window->IsUsingStereo() )
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

		if ( l_bHasPF )
		{
			m_hContext = m_gl.CreateContext( m_hDC );

			if ( m_gl.GetVersion() >= 30 )
			{
				m_bInitialised = DoCreateGl3Context( p_window );
			}
			else
			{
				if ( l_pMainContext )
				{
					wglShareLists( m_hContext, l_pMainContext->GetImpl()->GetContext() );
				}

				m_bInitialised = true;
			}
		}
		else
		{
			Logger::LogError( str_utils::from_str( "No supported pixel format found, context creation failed" ) );
		}

		if ( m_bInitialised )
		{
			glTrack( m_gl, GlContextImpl, this );
			SetCurrent();
			l_pRenderSystem->Initialise();
			p_window->GetEngine()->GetMaterialManager().Initialise();
#if !defined( NDEBUG )

			if ( m_gl.HasDebugOutput() )
			{
				m_gl.DebugMessageCallback( OpenGl::PFNGLDEBUGPROC( &OpenGl::StDebugLog ), &m_gl );
				m_gl.DebugMessageCallback( OpenGl::PFNGLDEBUGAMDPROC( &OpenGl::StDebugLogAMD ), &m_gl );
				m_gl.Enable( eGL_TWEAK_DEBUG_OUTPUT_SYNCHRONOUS );
			}

#endif
			EndCurrent();
			UpdateVSync( p_window->GetVSync() );
		}

		return m_bInitialised;
	}

	GlContextImpl::~GlContextImpl()
	{
	}

	void GlContextImpl::Cleanup()
	{
		try
		{
			glUntrack( m_gl, this );
			m_gl.DeleteContext( m_hContext );
		}
		catch ( ... )
		{
		}
	}

	void GlContextImpl::SetCurrent()
	{
		m_gl.MakeCurrent( m_hDC, m_hContext );
	}

	void GlContextImpl::EndCurrent()
	{
		m_gl.MakeCurrent( NULL, NULL );
	}

	void GlContextImpl::SwapBuffers()
	{
		m_gl.SwapBuffers( m_hDC );
	}

	void GlContextImpl::UpdateVSync( bool p_bEnable )
	{
		SetCurrent();
		Logger::LogDebug( cuT( "GlContextImpl::UpdateVSync" ) );

		if ( p_bEnable )
		{
			m_gl.SwapInterval( 1 );
		}
		else
		{
			m_gl.SwapInterval( 0 );
		}

		EndCurrent();
	}

	bool GlContextImpl::DoCreateGl3Context( RenderWindow * p_window )
	{
		bool l_bReturn = false;

		try
		{
			GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );

			if ( m_gl.HasCreateContextAttribs() )
			{
				std::function< HGLRC( HDC hDC, HGLRC hShareContext, int const * attribList ) > glCreateContextAttribs;
				HGLRC l_hContext = m_hContext;
				IntArray l_attribList;
				l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_MAJOR_VERSION );
				l_attribList.push_back( m_gl.GetVersion() / 10 );
				l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_MINOR_VERSION );
				l_attribList.push_back( m_gl.GetVersion() % 10 );
				l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_FLAGS );
				l_attribList.push_back( C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS );
				l_attribList.push_back( eGL_PROFILE_ATTRIB_MASK );
				l_attribList.push_back( C3D_GL_CONTEXT_CREATION_DEFAULT_MASK );
				l_attribList.push_back( 0	);
				GlContextSPtr l_pMainContext = std::static_pointer_cast< GlContext >( l_pRenderSystem->GetMainContext() );
				SetCurrent();

				if ( m_gl.HasExtension( ARB_create_context ) )
				{
					gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), glCreateContextAttribs );
				}
				else
				{
					gl_api::GetFunction( cuT( "wglCreateContextAttribsEXT" ), glCreateContextAttribs );
				}

				if ( l_pMainContext )
				{
					m_hContext = glCreateContextAttribs( m_hDC, l_pMainContext->GetImpl()->GetContext(), &l_attribList[0] );
				}
				else
				{
					m_hContext = glCreateContextAttribs( m_hDC, NULL, &l_attribList[0] );
				}

				EndCurrent();
				m_gl.DeleteContext( l_hContext );
				Logger::LogInfo( cuT( "GlContext::GlContext - OpenGL 3.x context created" ) );
				l_bReturn = m_hContext != NULL;
			}
			else
			{
				//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
				l_pRenderSystem->SetOpenGlVersion( 2, 1 );
				Logger::LogWarning( cuT( "GlContext::GlContext - Can't create OpenGL 3.x context" ) );
				l_bReturn = true;
			}
		}
		catch ( ... )
		{
			l_bReturn = false;
		}

		return l_bReturn;
	}

	HGLRC GlContextImpl::DoCreateDummyContext( RenderWindow * p_window )
	{
		HGLRC l_hReturn = NULL;

		if ( DoSelectPixelFormat( p_window ) )
		{
			l_hReturn = m_gl.CreateContext( m_hDC );
		}

		return l_hReturn;
	}

	bool GlContextImpl::DoSelectPixelFormat( RenderWindow * p_window )
	{
		bool l_bReturn = false;
		PIXELFORMATDESCRIPTOR l_pfd = { 0 };
		l_pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		l_pfd.nVersion = 1;
		l_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		l_pfd.iPixelType = PFD_TYPE_RGBA;
		l_pfd.iLayerType = PFD_MAIN_PLANE;
		l_pfd.cColorBits = PF::GetBytesPerPixel( p_window->GetPixelFormat() ) * 8;

		if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_DEPTH16 )
		{
			l_pfd.cDepthBits = 16;
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_DEPTH32 )
		{
			l_pfd.cDepthBits = 32;
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_DEPTH24 )
		{
			l_pfd.cDepthBits = 24;
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_DEPTH24S8 )
		{
			l_pfd.cDepthBits = 24;
			l_pfd.cStencilBits = 8;
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_STENCIL8 )
		{
			l_pfd.cStencilBits = 8;
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_STENCIL1 )
		{
			l_pfd.cStencilBits = 1;
		}

		if ( p_window->IsUsingStereo() && p_window->GetEngine()->GetRenderSystem()->IsStereoAvailable() )
		{
			l_pfd.dwFlags	|= PFD_STEREO;
		}

		int l_iPixelFormats = ::ChoosePixelFormat( m_hDC, &l_pfd );

		if ( l_iPixelFormats )
		{
			l_bReturn = ::SetPixelFormat( m_hDC, l_iPixelFormats, &l_pfd ) != FALSE;

			if ( !l_bReturn )
			{
				Castor::String l_error = Castor::System::GetLastErrorText();

				if ( !l_error.empty() )
				{
					Logger::LogError( cuT( "ChoosePixelFormat failed : " ) + l_error );
				}
				else
				{
					Logger::LogWarning( cuT( "ChoosePixelFormat failed" ) );
					l_bReturn = true;
				}
			}
		}
		else
		{
			Logger::LogError( cuT( "SetPixelFormat failed : " ) + Castor::System::GetLastErrorText() );
		}

		return l_bReturn;
	}

	bool GlContextImpl::DoSelectStereoPixelFormat( RenderWindow * p_window )
	{
		bool l_bReturn = false;
		GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
		PIXELFORMATDESCRIPTOR l_pfd = { 0 };
		l_pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		int l_iPixelFormat = ::DescribePixelFormat( m_hDC, 1, sizeof( PIXELFORMATDESCRIPTOR ), &l_pfd );
		bool l_bStereoAvailable = false;

		while ( l_iPixelFormat && !l_bStereoAvailable )
		{
			BYTE l_color = PF::GetBytesPerPixel( p_window->GetPixelFormat() ) * 8;
			BYTE l_depth = 0;
			BYTE l_stencil = 0;

			if ( p_window->GetPixelFormat() == ePIXEL_FORMAT_DEPTH16 )
			{
				l_depth = 16;
			}
			else if ( p_window->GetPixelFormat() == ePIXEL_FORMAT_DEPTH32 )
			{
				l_depth = 32;
			}
			else if ( p_window->GetPixelFormat() == ePIXEL_FORMAT_DEPTH24 )
			{
				l_depth = 24;
			}
			else if ( p_window->GetPixelFormat() == ePIXEL_FORMAT_DEPTH24S8 )
			{
				l_depth = 24;
				l_stencil = 8;
			}
			else if ( p_window->GetPixelFormat() == ePIXEL_FORMAT_STENCIL8 )
			{
				l_stencil = 8;
			}
			else if ( p_window->GetPixelFormat() == ePIXEL_FORMAT_STENCIL1 )
			{
				l_stencil = 1;
			}

			::DescribePixelFormat( m_hDC, l_iPixelFormat, sizeof( PIXELFORMATDESCRIPTOR ), &l_pfd );

			if ( ( l_pfd.dwFlags & PFD_STEREO )
					&& ( l_pfd.dwFlags & PFD_SUPPORT_OPENGL )
					&& ( l_pfd.dwFlags & PFD_DOUBLEBUFFER )
					&& ( l_pfd.dwFlags & PFD_DRAW_TO_WINDOW )
					&& l_pfd.iPixelType == PFD_TYPE_RGBA
					&& l_pfd.cColorBits == l_color
					&& l_pfd.cDepthBits == l_depth
					&& l_pfd.cStencilBits == l_stencil )
			{
				l_bStereoAvailable = true;
			}
			else
			{
				l_iPixelFormat--;
			}
		}

		if ( l_bStereoAvailable )
		{
			l_bReturn = ::SetPixelFormat( m_hDC, l_iPixelFormat, &l_pfd ) == TRUE;

			if ( !l_bReturn )
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
}

#endif
