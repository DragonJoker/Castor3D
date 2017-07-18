#include <Config/PlatformConfig.hpp>

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Render/GlMswContext.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlContext.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Graphics/PixelFormat.hpp>
#include <Miscellaneous/Utils.hpp>

#include <Cache/MaterialCache.hpp>
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
		auto engine = p_window->GetEngine();
		auto renderSystem = static_cast< GlRenderSystem * >( engine->GetRenderSystem() );
		auto mainContext = std::static_pointer_cast< GlContext >( renderSystem->GetMainContext() );

		m_hWnd = p_window->GetHandle().GetInternal< IMswWindowHandle >()->GetHwnd();
		auto colour = p_window->GetPixelFormat();
		auto stereo = p_window->IsUsingStereo() && renderSystem->GetGpuInformations().IsStereoAvailable();
		bool isMain = false;

		if ( !mainContext || mainContext->GetImpl().m_hWnd != m_hWnd )
		{
			m_hDC = ::GetDC( m_hWnd );
		}
		else if ( mainContext->GetImpl().m_hWnd == m_hWnd )
		{
			isMain = true;
			m_hContext = mainContext->GetImpl().m_hContext;
			m_hDC = mainContext->GetImpl().m_hDC;
		}

		if ( !renderSystem->IsInitialised() && !isMain )
		{
			DoInitialiseOpenGL( colour, stereo );
		}

		bool bHasPF = false;

		if ( !mainContext )
		{
			if ( stereo )
			{
				bHasPF = DoSelectStereoPixelFormat( colour );
			}
			else
			{
				m_gpuInformations.RemoveFeature( GpuFeature::eStereo );
				bHasPF = DoSelectPixelFormat( colour, false );
			}
		}
		else if ( !isMain )
		{
			bHasPF = DoSelectPixelFormat( colour, stereo );
		}

		if ( bHasPF )
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
		else if ( !isMain )
		{
			Logger::LogError( cuT( "No supported pixel format found, context creation failed" ) );
		}
		else
		{
			m_initialised = true;
		}

		if ( m_initialised && !isMain )
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
				GlRenderSystem * renderSystem = static_cast< GlRenderSystem * >( m_context->GetRenderSystem() );
				GlContextSPtr mainContext = std::static_pointer_cast< GlContext >( renderSystem->GetMainContext() );

				if ( mainContext.get() == m_context || !mainContext || mainContext->GetImpl().m_hWnd != m_hWnd )
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
		HGLRC hReturn = nullptr;

		if ( DoSelectPixelFormat( p_colour, p_stereo ) )
		{
			hReturn = GetOpenGl().CreateContext( m_hDC );
		}

		return hReturn;
	}

	bool GlContextImpl::DoSelectPixelFormat( PixelFormat p_colour, bool p_stereo )
	{
		bool result = false;
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.iLayerType = PFD_MAIN_PLANE;
		pfd.cColorBits = PF::GetBytesPerPixel( p_colour ) * 8;

		if ( p_stereo )
		{
			pfd.dwFlags |= PFD_STEREO;
		}

		int iPixelFormats = ::ChoosePixelFormat( m_hDC, &pfd );

		if ( iPixelFormats )
		{
			result = ::SetPixelFormat( m_hDC, iPixelFormats, &pfd ) != FALSE;

			if ( !result )
			{
				Castor::String error = Castor::System::GetLastErrorText();

				if ( !error.empty() )
				{
					Logger::LogError( cuT( "ChoosePixelFormat failed : " ) + error );
				}
				else
				{
					Logger::LogWarning( cuT( "ChoosePixelFormat failed" ) );
					result = true;
				}
			}
		}
		else
		{
			Logger::LogError( cuT( "SetPixelFormat failed : " ) + Castor::System::GetLastErrorText() );
		}

		return result;
	}

	bool GlContextImpl::DoSelectStereoPixelFormat( PixelFormat p_colour )
	{
		bool result = false;
		GlRenderSystem * renderSystem = static_cast< GlRenderSystem * >( m_context->GetRenderSystem() );
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		int iPixelFormat = ::DescribePixelFormat( m_hDC, 1, sizeof( PIXELFORMATDESCRIPTOR ), &pfd );
		bool bStereoAvailable = false;

		BYTE color = PF::GetBytesPerPixel( p_colour ) * 8;
		BYTE depth = 0;
		BYTE stencil = 0;

		while ( iPixelFormat && !bStereoAvailable )
		{
			::DescribePixelFormat( m_hDC, iPixelFormat, sizeof( PIXELFORMATDESCRIPTOR ), &pfd );

			if ( pfd.dwFlags & PFD_STEREO )
			{
				if ( ( pfd.dwFlags & PFD_SUPPORT_OPENGL )
						&& ( pfd.dwFlags & PFD_DOUBLEBUFFER )
						&& ( pfd.dwFlags & PFD_DRAW_TO_WINDOW )
						&& pfd.iPixelType == PFD_TYPE_RGBA
						&& pfd.cColorBits == color
						&& pfd.cDepthBits == depth
						&& pfd.cStencilBits == stencil )
				{
					bStereoAvailable = true;
				}
				else
				{
					iPixelFormat--;
				}
			}
			else
			{
				iPixelFormat--;
			}
		}

		if ( bStereoAvailable )
		{
			result = ::SetPixelFormat( m_hDC, iPixelFormat, &pfd ) == TRUE;

			if ( !result )
			{
				bStereoAvailable = false;
				result = DoSelectPixelFormat( p_colour, false );
			}
		}
		else
		{
			result = DoSelectPixelFormat( p_colour, true );
		}

		m_gpuInformations.UpdateFeature( GpuFeature::eStereo, bStereoAvailable );
		return result;
	}

	bool GlContextImpl::DoCreateGl3Context()
	{
		bool result = false;

		try
		{
			GlRenderSystem * renderSystem = static_cast< GlRenderSystem * >( m_context->GetRenderSystem() );

			if ( GetOpenGl().HasCreateContextAttribs() )
			{
				std::function< HGLRC( HDC hDC, HGLRC hShareContext, int const * attribList ) > glCreateContextAttribs;
				HGLRC hContext = m_hContext;
				int major = GetOpenGl().GetVersion() / 10;
				int minor = GetOpenGl().GetVersion() % 10;
				IntArray attribList
				{
					int( GlContextAttribute::eMajorVersion ), major,
					int( GlContextAttribute::eMinorVersion ), minor,
					int( GlContextAttribute::eFlags ), C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS,
					int( GlProfileAttribute::eMask ), C3D_GL_CONTEXT_CREATION_DEFAULT_MASK,
					0
				};
				GlContextSPtr mainContext = std::static_pointer_cast< GlContext >( renderSystem->GetMainContext() );
				SetCurrent();

				if ( GetOpenGl().HasExtension( ARB_create_context ) )
				{
					gl_api::GetFunction( cuT( "wglCreateContextAttribsARB" ), glCreateContextAttribs );
				}
				else
				{
					gl_api::GetFunction( cuT( "wglCreateContextAttribsEXT" ), glCreateContextAttribs );
				}

				if ( mainContext )
				{
					m_hContext = glCreateContextAttribs( m_hDC, mainContext->GetImpl().GetContext(), attribList.data() );
				}
				else
				{
					m_hContext = glCreateContextAttribs( m_hDC, nullptr, attribList.data() );
				}

				EndCurrent();
				GetOpenGl().DeleteContext( hContext );
				result = m_hContext != nullptr;

				if ( result )
				{
					Logger::LogInfo( StringStream() << cuT( "GlContext::Create - " ) << major << cuT( "." ) << minor << cuT( " OpenGL context created." ) );
				}
				else
				{
					StringStream error;
					error << cuT( "Failed to create a " ) << major << cuT( "." ) << minor << cuT( " OpenGL context." );
					CASTOR_EXCEPTION( error.str() );
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
			result = false;
		}

		return result;
	}
}

#endif
