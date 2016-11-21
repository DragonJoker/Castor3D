#if defined( _WIN32 )

#include "Render/GlMswContext.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlContext.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Graphics/PixelFormat.hpp>
#include <Miscellaneous/Utils.hpp>

#include <MaterialCache.hpp>
#include <Miscellaneous/PlatformWindowHandle.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	namespace
	{
#if !defined( NDEBUG )

		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = int( GlContextAttribute::eForwardCompatibleBit ) | int( GlContextAttribute::eDebugBit );
		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = int( GlProfileAttribute::eCoreBit );

#else

		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = int( GlContextAttribute::eForwardCompatibleBit );
		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = int( GlProfileAttribute::eCompatibilityBit );

#endif
	}

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
		auto l_engine = p_window->GetEngine();
		auto l_renderSystem = static_cast< GlRenderSystem * >( l_engine->GetRenderSystem() );
		auto l_mainContext = std::static_pointer_cast< GlContext >( l_renderSystem->GetMainContext() );

		m_hWnd = p_window->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
		auto l_colour = p_window->GetPixelFormat();
		auto l_stereo = p_window->IsUsingStereo() && l_renderSystem->GetGpuInformations().IsStereoAvailable();
		bool l_isMain = false;

		if ( !l_mainContext || l_mainContext->GetImpl().m_hWnd != m_hWnd )
		{
			m_hDC = ::GetDC( m_hWnd );
		}
		else if ( l_mainContext->GetImpl().m_hWnd == m_hWnd )
		{
			l_isMain = true;
			m_hContext = l_mainContext->GetImpl().m_hContext;
			m_hDC = l_mainContext->GetImpl().m_hDC;
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
				m_gpuInformations.RemoveFeature( GpuFeature::Stereo );
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
				GetOpenGl().DeleteContext( m_hContext );
				CASTOR_EXCEPTION( cuT( "The supported OpenGL version is insufficient to run Castor3D" ) );
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

				if ( l_mainContext.get() == m_context || !l_mainContext || l_mainContext->GetImpl().m_hWnd != m_hWnd )
				{
					if ( !GetOpenGl().DeleteContext( m_hContext ) )
					{
						Logger::LogError( cuT( "GlContextImpl::Cleanup - " ) +  System::GetLastErrorText() );
					}

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

	void GlContextImpl::DoInitialiseOpenGL( PixelFormat p_colour, bool p_stereo )
	{
		m_hContext = DoCreateDummyContext( p_colour, p_stereo );
		SetCurrent();
		typedef const char * ( PFNWGLGETEXTENSIONSSTRINGEXTPROC )( );
		PFNWGLGETEXTENSIONSSTRINGEXTPROC * wglGetExtensionsStringEXT;
		wglGetExtensionsStringEXT = ( PFNWGLGETEXTENSIONSSTRINGEXTPROC * )wglGetProcAddress( "wglGetExtensionsStringEXT" );

		if ( wglGetExtensionsStringEXT && wglGetExtensionsStringEXT() )
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

	HGLRC GlContextImpl::DoCreateDummyContext( PixelFormat p_colour, bool p_stereo )
	{
		HGLRC l_hReturn = nullptr;

		if ( DoSelectPixelFormat( p_colour, p_stereo ) )
		{
			l_hReturn = GetOpenGl().CreateContext( m_hDC );
		}

		return l_hReturn;
	}

	bool GlContextImpl::DoSelectPixelFormat( PixelFormat p_colour, bool p_stereo )
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

	bool GlContextImpl::DoSelectStereoPixelFormat( PixelFormat p_colour )
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

		m_gpuInformations.UpdateFeature( GpuFeature::Stereo, l_bStereoAvailable );
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
				int l_major = GetOpenGl().GetVersion() / 10;
				int l_minor = GetOpenGl().GetVersion() % 10;
				IntArray l_attribList
				{
					int( GlContextAttribute::eMajorVersion ), l_major,
					int( GlContextAttribute::eMinorVersion ), l_minor,
					int( GlContextAttribute::eFlags ), C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS,
					int( GlProfileAttribute::eMask ), C3D_GL_CONTEXT_CREATION_DEFAULT_MASK,
					0
				};
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
					m_hContext = glCreateContextAttribs( m_hDC, l_mainContext->GetImpl().GetContext(), l_attribList.data() );
				}
				else
				{
					m_hContext = glCreateContextAttribs( m_hDC, nullptr, l_attribList.data() );
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
					StringStream l_error;
					l_error << cuT( "Failed to create a " ) << l_major << cuT( "." ) << l_minor << cuT( " OpenGL context." );
					CASTOR_EXCEPTION( l_error.str() );
				}
			}
			else
			{
				GetOpenGl().DeleteContext( m_hContext );
				CASTOR_EXCEPTION( cuT( "Can't create OpenGL >= 3.x context, since glCreateContextAttribs function is not supported." ) );
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
