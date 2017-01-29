#if defined( _WIN32 )

#include "Render/GlES3MswContext.hpp"

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3Context.hpp"
#include "Render/GlES3RenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Graphics/PixelFormat.hpp>
#include <Miscellaneous/Utils.hpp>

#include <Cache/MaterialCache.hpp>
#include <Miscellaneous/PlatformWindowHandle.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	namespace
	{
#if !defined( NDEBUG )

		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = int( GlES3ContextAttribute::eForwardCompatibleBit ) | int( GlES3ContextAttribute::eDebugBit );
		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = int( GlES3ProfileAttribute::eCoreBit );

#else

		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = int( GlES3ContextAttribute::eForwardCompatibleBit );
		static const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = int( GlES3ProfileAttribute::eCompatibilityBit );

#endif
	}

	GlES3ContextImpl::GlES3ContextImpl( OpenGlES3 & p_gl, GlES3Context * p_context )
		: m_hDC( nullptr )
		, m_hContext( nullptr )
		, m_hWnd( nullptr )
		, m_context( p_context )
		, m_initialised( false )
		, Holder( p_gl )
	{
	}

	GlES3ContextImpl::~GlES3ContextImpl()
	{
	}

	bool GlES3ContextImpl::Initialise( RenderWindow * p_window )
	{
		auto l_engine = p_window->GetEngine();
		auto l_renderSystem = static_cast< GlES3RenderSystem * >( l_engine->GetRenderSystem() );
		auto l_mainContext = std::static_pointer_cast< GlES3Context >( l_renderSystem->GetMainContext() );

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
				m_gpuInformations.RemoveFeature( GpuFeature::eStereo );
				l_bHasPF = DoSelectPixelFormat( l_colour, false );
			}
		}
		else if ( !l_isMain )
		{
			l_bHasPF = DoSelectPixelFormat( l_colour, l_stereo );
		}

		if ( l_bHasPF )
		{
			m_hContext = GetOpenGlES3().CreateContext( m_hDC );

			if ( GetOpenGlES3().GetVersion() >= 30 )
			{
				m_initialised = DoCreateGlES33Context();
			}
			else
			{
				GetOpenGlES3().DeleteContext( m_hContext );
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
			glTrack( GetOpenGlES3(), "GlES3ContextImpl", this );
		}

		return m_initialised;
	}

	void GlES3ContextImpl::Cleanup()
	{
		try
		{
			glUntrack( GetOpenGlES3(), this );

			if ( m_hDC )
			{
				GlES3RenderSystem * l_renderSystem = static_cast< GlES3RenderSystem * >( m_context->GetRenderSystem() );
				GlES3ContextSPtr l_mainContext = std::static_pointer_cast< GlES3Context >( l_renderSystem->GetMainContext() );

				if ( l_mainContext.get() == m_context || !l_mainContext || l_mainContext->GetImpl().m_hWnd != m_hWnd )
				{
					if ( !GetOpenGlES3().DeleteContext( m_hContext ) )
					{
						Logger::LogError( cuT( "GlES3ContextImpl::Cleanup - " ) +  System::GetLastErrorText() );
					}

					::ReleaseDC( m_hWnd, m_hDC );
				}
			}
		}
		catch ( ... )
		{
		}
	}

	void GlES3ContextImpl::SetCurrent()
	{
		GetOpenGlES3().MakeCurrent( m_hDC, m_hContext );
	}

	void GlES3ContextImpl::EndCurrent()
	{
		GetOpenGlES3().MakeCurrent( nullptr, nullptr );
	}

	void GlES3ContextImpl::SwapBuffers()
	{
		GetOpenGlES3().SwapBuffers( m_hDC );
	}

	void GlES3ContextImpl::UpdateVSync( bool p_enable )
	{
		SetCurrent();

		if ( p_enable )
		{
			GetOpenGlES3().SwapInterval( 1 );
		}
		else
		{
			GetOpenGlES3().SwapInterval( 0 );
		}

		EndCurrent();
	}

	void GlES3ContextImpl::DoInitialiseOpenGL( PixelFormat p_colour, bool p_stereo )
	{
		m_hContext = DoCreateDummyContext( p_colour, p_stereo );
		SetCurrent();
		typedef const char * ( PFNWGLGETEXTENSIONSSTRINGEXTPROC )( );
		PFNWGLGETEXTENSIONSSTRINGEXTPROC * wglGetExtensionsStringEXT;
		wglGetExtensionsStringEXT = ( PFNWGLGETEXTENSIONSSTRINGEXTPROC * )wglGetProcAddress( "wglGetExtensionsStringEXT" );

		if ( wglGetExtensionsStringEXT && wglGetExtensionsStringEXT() )
		{
			GetOpenGlES3().PreInitialise( string::string_cast< xchar >( wglGetExtensionsStringEXT() ) );
		}
		else
		{
			GetOpenGlES3().PreInitialise( String() );
		}

		EndCurrent();
		GetOpenGlES3().DeleteContext( m_hContext );
		m_hContext = nullptr;
	}

	HGLRC GlES3ContextImpl::DoCreateDummyContext( PixelFormat p_colour, bool p_stereo )
	{
		HGLRC l_hReturn = nullptr;

		if ( DoSelectPixelFormat( p_colour, p_stereo ) )
		{
			l_hReturn = GetOpenGlES3().CreateContext( m_hDC );
		}

		return l_hReturn;
	}

	bool GlES3ContextImpl::DoSelectPixelFormat( PixelFormat p_colour, bool p_stereo )
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

	bool GlES3ContextImpl::DoSelectStereoPixelFormat( PixelFormat p_colour )
	{
		bool l_return = false;
		GlES3RenderSystem * l_renderSystem = static_cast< GlES3RenderSystem * >( m_context->GetRenderSystem() );
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

		m_gpuInformations.UpdateFeature( GpuFeature::eStereo, l_bStereoAvailable );
		return l_return;
	}

	bool GlES3ContextImpl::DoCreateGlES33Context()
	{
		bool l_return = false;

		try
		{
			GlES3RenderSystem * l_renderSystem = static_cast< GlES3RenderSystem * >( m_context->GetRenderSystem() );

			if ( GetOpenGlES3().HasCreateContextAttribs() )
			{
				std::function< HGLRC( HDC hDC, HGLRC hShareContext, int const * attribList ) > glCreateContextAttribs;
				HGLRC l_hContext = m_hContext;
				int l_major = GetOpenGlES3().GetVersion() / 10;
				int l_minor = GetOpenGlES3().GetVersion() % 10;
				IntArray l_attribList
				{
					int( GlES3ContextAttribute::eMajorVersion ), l_major,
					int( GlES3ContextAttribute::eMinorVersion ), l_minor,
					int( GlES3ContextAttribute::eFlags ), C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS,
					int( GlES3ProfileAttribute::eMask ), C3D_GL_CONTEXT_CREATION_DEFAULT_MASK,
					0
				};
				GlES3ContextSPtr l_mainContext = std::static_pointer_cast< GlES3Context >( l_renderSystem->GetMainContext() );
				SetCurrent();

				if ( GetOpenGlES3().HasExtension( ARB_create_context ) )
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
				GetOpenGlES3().DeleteContext( l_hContext );
				l_return = m_hContext != nullptr;

				if ( l_return )
				{
					Logger::LogInfo( StringStream() << cuT( "GlES3Context::Create - " ) << l_major << cuT( "." ) << l_minor << cuT( " OpenGL context created." ) );
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
				GetOpenGlES3().DeleteContext( m_hContext );
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
