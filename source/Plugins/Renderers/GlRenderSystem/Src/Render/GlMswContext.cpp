#include <Config/PlatformConfig.hpp>

#if defined( CASTOR_PLATFORM_WINDOWS )

#include "Common/GlGetFunction.hpp"
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

using namespace castor3d;
using namespace castor;

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

	bool GlContextImpl::initialise( RenderWindow * p_window )
	{
		auto engine = p_window->getEngine();
		auto renderSystem = static_cast< GlRenderSystem * >( engine->getRenderSystem() );
		auto mainContext = std::static_pointer_cast< GlContext >( renderSystem->getMainContext() );

		m_hWnd = p_window->getHandle().getInternal< IMswWindowHandle >()->getHwnd();
		auto colour = p_window->getPixelFormat();
		auto stereo = p_window->isUsingStereo() && renderSystem->getGpuInformations().isStereoAvailable();
		bool isMain = false;

		if ( !mainContext || mainContext->getImpl().m_hWnd != m_hWnd )
		{
			m_hDC = ::GetDC( m_hWnd );
		}
		else if ( mainContext->getImpl().m_hWnd == m_hWnd )
		{
			isMain = true;
			m_hContext = mainContext->getImpl().m_hContext;
			m_hDC = mainContext->getImpl().m_hDC;
		}

		if ( !renderSystem->isInitialised() && !isMain )
		{
			doInitialiseOpenGL( colour, stereo );
		}

		bool bHasPF = false;

		if ( !mainContext )
		{
			if ( stereo )
			{
				bHasPF = doSelectStereoPixelFormat( colour );
			}
			else
			{
				m_gpuInformations.removeFeature( GpuFeature::eStereo );
				bHasPF = doSelectPixelFormat( colour, false );
			}
		}
		else if ( !isMain )
		{
			bHasPF = doSelectPixelFormat( colour, stereo );
		}

		if ( bHasPF )
		{
			m_hContext = getOpenGl().CreateContext( m_hDC );

			if ( getOpenGl().getVersion() >= 30 )
			{
				m_initialised = doCreateGl3Context();
			}
			else
			{
				getOpenGl().DeleteContext( m_hContext );
				CASTOR_EXCEPTION( cuT( "The supported OpenGL version is insufficient to run Castor3D" ) );
			}
		}
		else if ( !isMain )
		{
			Logger::logError( cuT( "No supported pixel format found, context creation failed" ) );
		}
		else
		{
			m_initialised = true;
		}

		if ( m_initialised && !isMain )
		{
			glTrack( getOpenGl(), "GlContextImpl", this );
		}

		return m_initialised;
	}

	void GlContextImpl::cleanup()
	{
		try
		{
			glUntrack( getOpenGl(), this );

			if ( m_hDC )
			{
				GlRenderSystem * renderSystem = static_cast< GlRenderSystem * >( m_context->getRenderSystem() );
				GlContextSPtr mainContext = std::static_pointer_cast< GlContext >( renderSystem->getMainContext() );

				if ( mainContext.get() == m_context || !mainContext || mainContext->getImpl().m_hWnd != m_hWnd )
				{
					if ( !getOpenGl().DeleteContext( m_hContext ) )
					{
						Logger::logError( cuT( "GlContextImpl::Cleanup - " ) +  System::getLastErrorText() );
					}

					::ReleaseDC( m_hWnd, m_hDC );
				}
			}
		}
		catch ( ... )
		{
		}
	}

	void GlContextImpl::setCurrent()
	{
		getOpenGl().MakeCurrent( m_hDC, m_hContext );
	}

	void GlContextImpl::endCurrent()
	{
		getOpenGl().MakeCurrent( nullptr, nullptr );
	}

	void GlContextImpl::swapBuffers()
	{
		getOpenGl().SwapBuffers( m_hDC );
	}

	void GlContextImpl::updateVSync( bool p_enable )
	{
		setCurrent();

		if ( p_enable )
		{
			getOpenGl().SwapInterval( 1 );
		}
		else
		{
			getOpenGl().SwapInterval( 0 );
		}

		endCurrent();
	}

	void GlContextImpl::doInitialiseOpenGL( PixelFormat p_colour, bool p_stereo )
	{
		m_hContext = doCreateDummyContext( p_colour, p_stereo );
		setCurrent();
		typedef const char * ( PFNWGLGETEXTENSIONSSTRINGEXTPROC )( );
		PFNWGLGETEXTENSIONSSTRINGEXTPROC * wglGetExtensionsStringEXT;
		wglGetExtensionsStringEXT = ( PFNWGLGETEXTENSIONSSTRINGEXTPROC * )wglGetProcAddress( "wglGetExtensionsStringEXT" );

		if ( wglGetExtensionsStringEXT && wglGetExtensionsStringEXT() )
		{
			getOpenGl().preInitialise( string::stringCast< xchar >( wglGetExtensionsStringEXT() ) );
		}
		else
		{
			getOpenGl().preInitialise( String() );
		}

		endCurrent();
		getOpenGl().DeleteContext( m_hContext );
		m_hContext = nullptr;
	}

	HGLRC GlContextImpl::doCreateDummyContext( PixelFormat p_colour, bool p_stereo )
	{
		HGLRC hReturn = nullptr;

		if ( doSelectPixelFormat( p_colour, p_stereo ) )
		{
			hReturn = getOpenGl().CreateContext( m_hDC );
		}

		return hReturn;
	}

	bool GlContextImpl::doSelectPixelFormat( PixelFormat p_colour, bool p_stereo )
	{
		bool result = false;
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.iLayerType = PFD_MAIN_PLANE;
		pfd.cColorBits = PF::getBytesPerPixel( p_colour ) * 8;

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
				castor::String error = castor::System::getLastErrorText();

				if ( !error.empty() )
				{
					Logger::logError( cuT( "ChoosePixelFormat failed : " ) + error );
				}
				else
				{
					Logger::logWarning( cuT( "ChoosePixelFormat failed" ) );
					result = true;
				}
			}
		}
		else
		{
			Logger::logError( cuT( "setPixelFormat failed : " ) + castor::System::getLastErrorText() );
		}

		return result;
	}

	bool GlContextImpl::doSelectStereoPixelFormat( PixelFormat p_colour )
	{
		bool result = false;
		GlRenderSystem * renderSystem = static_cast< GlRenderSystem * >( m_context->getRenderSystem() );
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
		int iPixelFormat = ::DescribePixelFormat( m_hDC, 1, sizeof( PIXELFORMATDESCRIPTOR ), &pfd );
		bool bStereoAvailable = false;

		BYTE color = PF::getBytesPerPixel( p_colour ) * 8;
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
				result = doSelectPixelFormat( p_colour, false );
			}
		}
		else
		{
			result = doSelectPixelFormat( p_colour, true );
		}

		m_gpuInformations.updateFeature( GpuFeature::eStereo, bStereoAvailable );
		return result;
	}

	bool GlContextImpl::doCreateGl3Context()
	{
		bool result = false;

		try
		{
			GlRenderSystem * renderSystem = static_cast< GlRenderSystem * >( m_context->getRenderSystem() );

			if ( getOpenGl().HasCreateContextAttribs() )
			{
				std::function< HGLRC( HDC hDC, HGLRC hShareContext, int const * attribList ) > glCreateContextAttribs;
				HGLRC hContext = m_hContext;
				int major = getOpenGl().getVersion() / 10;
				int minor = getOpenGl().getVersion() % 10;
				IntArray attribList
				{
					int( GlContextAttribute::eMajorVersion ), major,
					int( GlContextAttribute::eMinorVersion ), minor,
					int( GlContextAttribute::eFlags ), C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS,
					int( GlProfileAttribute::eMask ), C3D_GL_CONTEXT_CREATION_DEFAULT_MASK,
					0
				};
				GlContextSPtr mainContext = std::static_pointer_cast< GlContext >( renderSystem->getMainContext() );
				setCurrent();

				if ( getOpenGl().hasExtension( ARB_create_context ) )
				{
					gl_api::getFunction( cuT( "wglCreateContextAttribsARB" ), glCreateContextAttribs );
				}
				else
				{
					gl_api::getFunction( cuT( "wglCreateContextAttribsEXT" ), glCreateContextAttribs );
				}

				if ( mainContext )
				{
					m_hContext = glCreateContextAttribs( m_hDC, mainContext->getImpl().getContext(), attribList.data() );
				}
				else
				{
					m_hContext = glCreateContextAttribs( m_hDC, nullptr, attribList.data() );
				}

				endCurrent();
				getOpenGl().DeleteContext( hContext );
				result = m_hContext != nullptr;

				if ( result )
				{
					Logger::logInfo( StringStream() << cuT( "GlContext::create - " ) << major << cuT( "." ) << minor << cuT( " OpenGL context created." ) );
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
				getOpenGl().DeleteContext( m_hContext );
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
