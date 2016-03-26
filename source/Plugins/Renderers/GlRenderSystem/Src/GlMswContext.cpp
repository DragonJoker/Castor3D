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

	GlContextImpl::GlContextImpl( OpenGl & p_gl, GlContext * p_context )
		: m_hDC( nullptr )
		, m_hContext( nullptr )
		, m_hWnd( nullptr )
		, m_context( p_context )
		, m_initialised( false )
		, Holder( p_gl )
	{
	}

	GlContextImpl::~GlContextImpl()
	{
	}

	bool GlContextImpl::Initialise( RenderWindow * p_window )
	{
		GlRenderSystem * l_renderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
		GlContextSPtr l_mainContext = std::static_pointer_cast< GlContext >( l_renderSystem->GetMainContext() );
		m_hWnd = p_window->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
		auto l_colour = p_window->GetPixelFormat();
		auto l_stereo = p_window->IsUsingStereo() && p_window->GetEngine()->GetRenderSystem()->IsStereoAvailable();
		bool l_isMain = false;

		if ( !l_mainContext )
		{
			Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
			Logger::LogInfo( cuT( "Initialising OpenGL" ) );
		}

		if ( !l_mainContext || l_mainContext->GetImpl()->m_hWnd != m_hWnd )
		{
			m_hDC = ::GetDC( m_hWnd );
		}
		else if ( l_mainContext->GetImpl()->m_hWnd == m_hWnd )
		{
			l_isMain = true;
			m_hContext = l_mainContext->GetImpl()->m_hContext;
			m_hDC = l_mainContext->GetImpl()->m_hDC;
		}

		if ( !l_renderSystem->IsInitialised() && !l_isMain )
		{
			DoInitialiseOpenGL( l_colour, l_stereo );
		}

		bool l_bHasPF = false;

		if ( !l_mainContext )
		{
			if ( l_stereo )
			{
				l_bHasPF = DoSelectStereoPixelFormat( l_colour );
			}
			else
			{
				l_renderSystem->SetStereoAvailable( false );
				l_bHasPF = DoSelectPixelFormat( l_colour, false );
			}
		}
		else if ( !l_isMain )
		{
			l_bHasPF = DoSelectPixelFormat( l_colour, l_stereo );
		}

		if ( l_bHasPF )
		{
			m_hContext = GetOpenGl().CreateContext( m_hDC );

			if ( GetOpenGl().GetVersion() >= 30 )
			{
				m_initialised = DoCreateGl3Context();
			}
			else
			{
				if ( l_mainContext )
				{
					wglShareLists( m_hContext, l_mainContext->GetImpl()->GetContext() );
				}

				m_initialised = true;
			}
		}
		else if ( !l_isMain )
		{
			Logger::LogError( cuT( "No supported pixel format found, context creation failed" ) );
		}
		else
		{
			m_initialised = true;
		}

		if ( m_initialised && !l_isMain )
		{
			glTrack( GetOpenGl(), "GlContextImpl", this );
			SetCurrent();
			l_renderSystem->Initialise();
			p_window->GetEngine()->GetMaterialManager().Initialise();
#if !defined( NDEBUG )

			if ( GetOpenGl().HasDebugOutput() )
			{
				GetOpenGl().DebugMessageCallback( OpenGl::PFNGLDEBUGPROC( &OpenGl::StDebugLog ), &GetOpenGl() );
				GetOpenGl().DebugMessageCallback( OpenGl::PFNGLDEBUGAMDPROC( &OpenGl::StDebugLogAMD ), &GetOpenGl() );
				GetOpenGl().Enable( eGL_TWEAK_DEBUG_OUTPUT_SYNCHRONOUS );
			}

#endif
			EndCurrent();
			UpdateVSync( p_window->GetVSync() );

			if ( !l_mainContext )
			{
				Logger::LogInfo( cuT( "OpenGL Initialisation Ended" ) );
				Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
			}
		}

		return m_initialised;
	}

	void GlContextImpl::Cleanup()
	{
		try
		{
			glUntrack( GetOpenGl(), this );

			if ( m_hDC )
			{
				GlRenderSystem * l_renderSystem = static_cast< GlRenderSystem * >( m_context->GetRenderSystem() );
				GlContextSPtr l_mainContext = std::static_pointer_cast< GlContext >( l_renderSystem->GetMainContext() );

				if ( l_mainContext.get() == m_context || !l_mainContext || l_mainContext->GetImpl()->m_hWnd != m_hWnd )
				{
					GetOpenGl().DeleteContext( m_hContext );
					::ReleaseDC( m_hWnd, m_hDC );
				}
			}
		}
		catch ( ... )
		{
		}
	}

	void GlContextImpl::SetCurrent()
	{
		GetOpenGl().MakeCurrent( m_hDC, m_hContext );
	}

	void GlContextImpl::EndCurrent()
	{
		GetOpenGl().MakeCurrent( nullptr, nullptr );
	}

	void GlContextImpl::SwapBuffers()
	{
		GetOpenGl().SwapBuffers( m_hDC );
	}

	void GlContextImpl::UpdateVSync( bool p_enable )
	{
		SetCurrent();

		if ( p_enable )
		{
			GetOpenGl().SwapInterval( 1 );
		}
		else
		{
			GetOpenGl().SwapInterval( 0 );
		}

		EndCurrent();
	}

	void GlContextImpl::DoInitialiseOpenGL( ePIXEL_FORMAT p_colour, bool p_stereo )
	{
		m_hContext = DoCreateDummyContext( p_colour, p_stereo );
		SetCurrent();
		typedef const char * ( PFNWGLGETEXTENSIONSSTRINGEXTPROC )( );
		PFNWGLGETEXTENSIONSSTRINGEXTPROC * wglGetExtensionsStringEXT;
		wglGetExtensionsStringEXT = ( PFNWGLGETEXTENSIONSSTRINGEXTPROC * )wglGetProcAddress( "wglGetExtensionsStringEXT" );

		if ( wglGetExtensionsStringEXT() )
		{
			GetOpenGl().PreInitialise( string::string_cast< xchar >( wglGetExtensionsStringEXT() ) );
		}
		else
		{
			GetOpenGl().PreInitialise( String() );
		}

		EndCurrent();
		GetOpenGl().DeleteContext( m_hContext );
		m_hContext = nullptr;
	}

	HGLRC GlContextImpl::DoCreateDummyContext( ePIXEL_FORMAT p_colour, bool p_stereo )
	{
		HGLRC l_hReturn = nullptr;

		if ( DoSelectPixelFormat( p_colour, p_stereo ) )
		{
			l_hReturn = GetOpenGl().CreateContext( m_hDC );
		}

		return l_hReturn;
	}

	bool GlContextImpl::DoSelectPixelFormat( ePIXEL_FORMAT p_colour, bool p_stereo )
	{
		bool l_return = false;
		PIXELFORMATDESCRIPTOR l_pfd = { 0 };
		l_pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		l_pfd.nVersion = 1;
		l_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		l_pfd.iPixelType = PFD_TYPE_RGBA;
		l_pfd.iLayerType = PFD_MAIN_PLANE;
		l_pfd.cColorBits = PF::GetBytesPerPixel( p_colour ) * 8;

		if ( p_stereo )
		{
			l_pfd.dwFlags |= PFD_STEREO;
		}

		int l_iPixelFormats = ::ChoosePixelFormat( m_hDC, &l_pfd );

		if ( l_iPixelFormats )
		{
			l_return = ::SetPixelFormat( m_hDC, l_iPixelFormats, &l_pfd ) != FALSE;

			if ( !l_return )
			{
				Castor::String l_error = Castor::System::GetLastErrorText();

				if ( !l_error.empty() )
				{
					Logger::LogError( cuT( "ChoosePixelFormat failed : " ) + l_error );
				}
				else
				{
					Logger::LogWarning( cuT( "ChoosePixelFormat failed" ) );
					l_return = true;
				}
			}
		}
		else
		{
			Logger::LogError( cuT( "SetPixelFormat failed : " ) + Castor::System::GetLastErrorText() );
		}

		return l_return;
	}

	bool GlContextImpl::DoSelectStereoPixelFormat( ePIXEL_FORMAT p_colour )
	{
		bool l_return = false;
		GlRenderSystem * l_renderSystem = static_cast< GlRenderSystem * >( m_context->GetRenderSystem() );
		PIXELFORMATDESCRIPTOR l_pfd = { 0 };
		l_pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		int l_iPixelFormat = ::DescribePixelFormat( m_hDC, 1, sizeof( PIXELFORMATDESCRIPTOR ), &l_pfd );
		bool l_bStereoAvailable = false;

		BYTE l_color = PF::GetBytesPerPixel( p_colour ) * 8;
		BYTE l_depth = 0;
		BYTE l_stencil = 0;

		while ( l_iPixelFormat && !l_bStereoAvailable )
		{
			::DescribePixelFormat( m_hDC, l_iPixelFormat, sizeof( PIXELFORMATDESCRIPTOR ), &l_pfd );

			if ( l_pfd.dwFlags & PFD_STEREO )
			{
				if ( ( l_pfd.dwFlags & PFD_SUPPORT_OPENGL )
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
			else
			{
				l_iPixelFormat--;
			}
		}

		if ( l_bStereoAvailable )
		{
			l_return = ::SetPixelFormat( m_hDC, l_iPixelFormat, &l_pfd ) == TRUE;

			if ( !l_return )
			{
				l_bStereoAvailable = false;
				l_return = DoSelectPixelFormat( p_colour, false );
			}
		}
		else
		{
			l_return = DoSelectPixelFormat( p_colour, true );
		}

		l_renderSystem->SetStereoAvailable( l_bStereoAvailable );
		return l_return;
	}

	bool GlContextImpl::DoCreateGl3Context()
	{
		bool l_return = false;

		try
		{
			GlRenderSystem * l_renderSystem = static_cast< GlRenderSystem * >( m_context->GetRenderSystem() );

			if ( GetOpenGl().HasCreateContextAttribs() )
			{
				std::function< HGLRC( HDC hDC, HGLRC hShareContext, int const * attribList ) > glCreateContextAttribs;
				HGLRC l_hContext = m_hContext;
				IntArray l_attribList;
				int l_major = GetOpenGl().GetVersion() / 10;
				int l_minor = GetOpenGl().GetVersion() % 10;
				l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_MAJOR_VERSION );
				l_attribList.push_back( l_major );
				l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_MINOR_VERSION );
				l_attribList.push_back( l_minor );
				l_attribList.push_back( eGL_CREATECONTEXT_ATTRIB_FLAGS );
				l_attribList.push_back( C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS );
				l_attribList.push_back( eGL_PROFILE_ATTRIB_MASK );
				l_attribList.push_back( C3D_GL_CONTEXT_CREATION_DEFAULT_MASK );
				l_attribList.push_back( 0 );
				GlContextSPtr l_mainContext = std::static_pointer_cast< GlContext >( l_renderSystem->GetMainContext() );
				SetCurrent();

				if ( GetOpenGl().HasExtension( ARB_create_context ) )
				{
					gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), glCreateContextAttribs );
				}
				else
				{
					gl_api::GetFunction( cuT( "wglCreateContextAttribsEXT" ), glCreateContextAttribs );
				}

				if ( l_mainContext )
				{
					m_hContext = glCreateContextAttribs( m_hDC, l_mainContext->GetImpl()->GetContext(), &l_attribList[0] );
				}
				else
				{
					m_hContext = glCreateContextAttribs( m_hDC, nullptr, &l_attribList[0] );
				}

				EndCurrent();
				GetOpenGl().DeleteContext( l_hContext );
				l_return = m_hContext != nullptr;

				if ( l_return )
				{
					Logger::LogInfo( StringStream() << cuT( "GlContext::Create - " ) << l_major << cuT( "." ) << l_minor << cuT( " OpenGL context created." ) );
				}
				else
				{
					Logger::LogError( StringStream() << cuT( "GlContext::Create - Failed to create a " ) << l_major << cuT( "." ) << l_minor << cuT( " OpenGL context." ) );
				}
			}
			else
			{
				//It's not possible to make a GL 3[0] context. Use the old style context (GL 2.1 and before)
				l_renderSystem->SetOpenGlVersion( 2, 1 );
				Logger::LogWarning( cuT( "GlContext::Create - Can't create OpenGL 3.x/4.x context, since glCreateContextAttribs is not supported." ) );
				l_return = true;
			}
		}
		catch ( ... )
		{
			l_return = false;
		}

		return l_return;
	}
}

#endif
