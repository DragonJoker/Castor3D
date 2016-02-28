#if defined( __linux__ )

#	include "GlX11Context.hpp"
#	include "GlContext.hpp"
#	include "GlRenderSystem.hpp"
#	include "OpenGl.hpp"

#	include <unistd.h>
#	include <iostream>
#	include <cstdlib>
#	include <cstring>
#	include <cstdio>

#	include <PlatformWindowHandle.hpp>
#	include <Material.hpp>
#	include <MaterialManager.hpp>
#	include <RenderWindow.hpp>

#	include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GlRender;

typedef GLXFBConfig * ( FnGlXChooseFBConfigProc	)( Display *, int, int const *, int * );
typedef XVisualInfo * ( FnGlXGetVisualFromFBConfigProc	)( Display *, GLXFBConfig );

typedef FnGlXChooseFBConfigProc * PFnGlXChooseFBConfigProc;
typedef FnGlXGetVisualFromFBConfigProc * PFnGlXGetVisualFromFBConfigProc;

PFnGlXChooseFBConfigProc g_pfnGlXChooseFBConfig = nullptr;
PFnGlXGetVisualFromFBConfigProc g_pfnGlXGetVisualFromFBConfig = nullptr;

GlContextImpl::GlContextImpl( OpenGl & p_gl, GlContext * p_context )
	: Holder( p_gl )
	, m_display( nullptr )
	, m_glxVersion( 10 )
	, m_glxContext( nullptr )
	, m_drawable( None )
	, m_fbConfig( nullptr )
	, m_context( p_context )
	, m_initialised( false )
{
}

GlContextImpl::~GlContextImpl()
{
	GetOpenGl().DeleteContext( m_display, m_glxContext );
	XFree( m_fbConfig );
}

bool GlContextImpl::Initialise( RenderWindow * p_window )
{
	if ( !g_pfnGlXChooseFBConfig )
	{
		g_pfnGlXChooseFBConfig = PFnGlXChooseFBConfigProc( glXGetProcAddress( ( uint8_t const * )"glXChooseFBConfig" ) );
		g_pfnGlXGetVisualFromFBConfig = PFnGlXGetVisualFromFBConfigProc( glXGetProcAddress( ( uint8_t const * )"glXGetVisualFromFBConfig" ) );
	}

	if ( m_drawable == None )
	{
		m_drawable = p_window->GetHandle().GetInternal< IXWindowHandle >()->GetDrawable();
		m_display = p_window->GetHandle().GetInternal< IXWindowHandle >()->GetDisplay();
	}

	GlRenderSystem * l_renderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
	GlContextSPtr l_mainContext = std::static_pointer_cast< GlContext >( l_renderSystem->GetMainContext() );

	if ( !l_mainContext )
	{
		Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
		Logger::LogInfo( cuT( "Initialising OpenGL" ) );
	}

	try
	{
		int l_screen = DefaultScreen( m_display );
		int l_major, l_minor;
		bool l_ok = glXQueryVersion( m_display, &l_major, &l_minor );
		XVisualInfo * l_visualInfo = nullptr;

		if ( l_ok )
		{
			m_glxVersion = l_major * 10 + l_minor;
			Logger::LogDebug( StringStream() << cuT( "GlXContext::Create - glx version: " ) << m_glxVersion );
		}

		IntArray l_attribList;
		l_attribList.push_back( GLX_RENDER_TYPE );
		l_attribList.push_back( GLX_RGBA_BIT );
		l_attribList.push_back( GLX_DRAWABLE_TYPE );
		l_attribList.push_back( GLX_WINDOW_BIT );
		l_attribList.push_back( GLX_DOUBLEBUFFER );
		l_attribList.push_back( GL_TRUE	);
		l_attribList.push_back( GLX_RED_SIZE );
		l_attribList.push_back( 1 );
		l_attribList.push_back( GLX_GREEN_SIZE );
		l_attribList.push_back( 1 );
		l_attribList.push_back( GLX_BLUE_SIZE );
		l_attribList.push_back( 1 );

		if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_DEPTH32 )
		{
			l_attribList.push_back( GLX_DEPTH_SIZE );
			l_attribList.push_back( 32 );
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_DEPTH24 )
		{
			l_attribList.push_back( GLX_DEPTH_SIZE );
			l_attribList.push_back( 24 );
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_DEPTH24S8 )
		{
			l_attribList.push_back( GLX_DEPTH_SIZE );
			l_attribList.push_back( 24 );
			l_attribList.push_back( GLX_STENCIL_SIZE );
			l_attribList.push_back( 8 );
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_STENCIL8 )
		{
			l_attribList.push_back( GLX_STENCIL_SIZE );
			l_attribList.push_back( 8 );
		}
		else if ( p_window->GetDepthFormat() == ePIXEL_FORMAT_STENCIL1 )
		{
			l_attribList.push_back( GLX_STENCIL_SIZE );
			l_attribList.push_back( 1 );
		}

		if ( p_window->IsUsingStereo() )
		{
			l_attribList.push_back( GLX_STEREO );
			l_attribList.push_back( 1 );
		}

		l_attribList.push_back( None );

		if ( g_pfnGlXChooseFBConfig )
		{
			l_visualInfo = DoCreateVisualInfoWithFBConfig( p_window, l_attribList, l_screen );
		}
		else
		{
			l_visualInfo = DoCreateVisualInfoWithoutFBConfig( l_attribList, l_screen );
		}

		if ( l_visualInfo )
		{
			if ( l_mainContext )
			{
				m_glxContext = glXCreateContext( m_display, l_visualInfo, l_mainContext->GetImpl()->GetContext(), GL_TRUE );
			}
			else
			{
				m_glxContext = glXCreateContext( m_display, l_visualInfo, nullptr, GL_TRUE );
			}

			if ( !m_glxContext )
			{
				Logger::LogWarning( cuT( "GlXContext::Create - glXCreateContext failed" ) );
			}
			else
			{
				Logger::LogInfo( cuT( "GlXContext::Create - glXContext created" ) );

				if ( !l_renderSystem->IsInitialised() )
				{
					glXMakeCurrent( m_display, m_drawable, m_glxContext );
					GetOpenGl().PreInitialise( String() );
					glXMakeCurrent( m_display, None, nullptr );
				}

				if ( GetOpenGl().GetVersion() >= 30 )
				{
					m_initialised = DoCreateGl3Context( p_window );
				}
				else
				{
					m_initialised = true;
				}

				if ( !l_renderSystem->IsInitialised() )
				{
					glXMakeCurrent( m_display, m_drawable, m_glxContext );
					l_renderSystem->Initialise();
					p_window->GetEngine()->GetMaterialManager().Initialise();
					glXMakeCurrent( m_display, None, nullptr );
				}
			}

			XFree( l_visualInfo );
		}
	}
	catch ( ... )
	{
		m_initialised = false;
	}

	if ( m_initialised )
	{
		glTrack( GetOpenGl(), "GlContextImpl", this );
#if !defined( NDEBUG )

		if ( GetOpenGl().HasDebugOutput() )
		{
			SetCurrent();
			GetOpenGl().DebugMessageCallback( OpenGl::PFNGLDEBUGPROC( &OpenGl::StDebugLog ), &GetOpenGl() );
			GetOpenGl().DebugMessageCallback( OpenGl::PFNGLDEBUGAMDPROC( &OpenGl::StDebugLogAMD ), &GetOpenGl() );
//			GetOpenGl().Enable( eGL_TWEAK_DEBUG_OUTPUT_SYNCHRONOUS );
			EndCurrent();
		}

#endif
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
		GetOpenGl().DeleteContext( m_display, m_glxContext );
	}
	catch ( ... )
	{
	}
}

void GlContextImpl::SetCurrent()
{
	GetOpenGl().MakeCurrent( m_display, m_drawable, m_glxContext );
}

void GlContextImpl::EndCurrent()
{
	GetOpenGl().MakeCurrent( m_display, None, nullptr );
}

void GlContextImpl::SwapBuffers()
{
	GetOpenGl().SwapBuffers( m_display, m_drawable );
}

void GlContextImpl::UpdateVSync( bool p_enable )
{
	SetCurrent();

	if ( p_enable )
	{
		GetOpenGl().SwapInterval( m_display, m_drawable, 1 );
	}
	else
	{
		GetOpenGl().SwapInterval( m_display, m_drawable, 0 );
	}

	EndCurrent();
}

XVisualInfo * GlContextImpl::DoCreateVisualInfoWithFBConfig( RenderWindow * p_window, IntArray & p_arrayAttribs, int p_screen )
{
	Logger::LogDebug( cuT( "GlXContext::Create - Using FBConfig" ) );
	XVisualInfo * l_return = nullptr;
	int l_result = 0;
	m_fbConfig = g_pfnGlXChooseFBConfig( m_display, p_screen, &p_arrayAttribs[0], &l_result );

	if ( !m_fbConfig )
	{
		int l_data = 0;

		// First try failed
		if ( p_window->IsUsingStereo() )
		{
			// Maybe because of stereo ? We try in mono
			p_window->GetEngine()->GetRenderSystem()->SetStereoAvailable( false );
			Logger::LogWarning( cuT( "GlXContext::Create - Stereo glXChooseFBConfig failed, using mono FB config" ) );
			p_arrayAttribs.clear();
			p_arrayAttribs.push_back( GLX_RENDER_TYPE	);
			p_arrayAttribs.push_back( GLX_RGBA_BIT	);
			p_arrayAttribs.push_back( GLX_DRAWABLE_TYPE	);
			p_arrayAttribs.push_back( GLX_WINDOW_BIT	);
			p_arrayAttribs.push_back( GLX_DOUBLEBUFFER	);
			p_arrayAttribs.push_back( GL_TRUE	);
			p_arrayAttribs.push_back( GLX_RED_SIZE	);
			p_arrayAttribs.push_back( 1	);
			p_arrayAttribs.push_back( GLX_GREEN_SIZE	);
			p_arrayAttribs.push_back( 1	);
			p_arrayAttribs.push_back( GLX_BLUE_SIZE	);
			p_arrayAttribs.push_back( 1	);
			p_arrayAttribs.push_back( None	);
			m_fbConfig = g_pfnGlXChooseFBConfig( m_display, p_screen, &p_arrayAttribs[0], &l_result );

			if ( !m_fbConfig )
			{
				// Second try failed, we try a default FBConfig
				Logger::LogWarning( cuT( "GlXContext::Create - Mono glXChooseFBConfig failed, using default FB config" ) );
				m_fbConfig = g_pfnGlXChooseFBConfig( m_display, p_screen, &l_data, &l_result );

				if ( !m_fbConfig )
				{
					// Last FBConfig try failed
					Logger::LogWarning( cuT( "GlXContext::Create - Default glXChooseFBConfig failed" ) );
					l_return = DoCreateVisualInfoWithoutFBConfig( p_arrayAttribs, p_screen );
				}
				else
				{
					Logger::LogDebug( cuT( "GlXContext::Create - Default glXChooseFBConfig successful" ) );
				}
			}
			else
			{
				Logger::LogDebug( cuT( "GlXContext::Create - Mono glXChooseFBConfig successful with detailed attributes" ) );
			}
		}
		else
		{
			// Can't be because of stereo, we try a default FBConfig
			Logger::LogWarning( cuT( "GlXContext::Create - glXChooseFBConfig failed, using default FB config" ) );
			m_fbConfig = g_pfnGlXChooseFBConfig( m_display, p_screen, &l_data, &l_result );

			if ( !m_fbConfig )
			{
				// Last FBConfig try failed, we try from XVisualInfo
				Logger::LogWarning( cuT( "GlXContext::Create - Default glXChooseFBConfig failed" ) );
				l_return = DoCreateVisualInfoWithoutFBConfig( p_arrayAttribs, p_screen );
			}
			else
			{
				Logger::LogDebug( cuT( "GlXContext::Create - Default glXChooseFBConfig successful" ) );
			}
		}
	}
	else
	{
		if ( p_window->IsUsingStereo() )
		{
			p_window->GetEngine()->GetRenderSystem()->SetStereoAvailable( true );
			Logger::LogDebug( cuT( "GlXContext::Create - Stereo glXChooseFBConfig successful with detailed attributes" ) );
		}
		else
		{
			Logger::LogDebug( cuT( "GlXContext::Create - glXChooseFBConfig successful with detailed attributes" ) );
		}
	}

	if ( m_fbConfig )
	{
		l_return = g_pfnGlXGetVisualFromFBConfig( m_display, m_fbConfig[0] );

		if ( !l_return )
		{
			Logger::LogError( cuT( "GlXContext::Create - glXGetVisualFromFBConfig failed" ) );
		}
		else
		{
			Logger::LogDebug( cuT( "GlXContext::Create - GlXGetVisualFromFBConfig successful" ) );
		}
	}

	return l_return;
}

XVisualInfo * GlContextImpl::DoCreateVisualInfoWithoutFBConfig( IntArray & p_arrayAttribs, int p_screen )
{
	Logger::LogInfo( cuT( "GlXContext::Create - Not using FBConfig" ) );
	XVisualInfo * l_return = glXChooseVisual( m_display, p_screen, &p_arrayAttribs[0] );

	if ( !l_return )
	{
		Logger::LogError( cuT( "GlXContext::Create - glXChooseVisual failed" ) );
	}

	return l_return;
}

bool GlContextImpl::DoCreateGl3Context( Castor3D::RenderWindow * p_window )
{
	bool l_return = false;
	GlRenderSystem * l_renderSystem = static_cast< GlRenderSystem * >( p_window->GetEngine()->GetRenderSystem() );
	GlContextSPtr l_mainContext = std::static_pointer_cast< GlContext >( l_renderSystem->GetMainContext() );

	if ( GetOpenGl().HasCreateContextAttribs() )
	{
		Logger::LogInfo( StringStream() << cuT( "GlXContext::Create - Using OpenGL " ) << ( GetOpenGl().GetVersion() / 10 ) << cuT( "." ) << ( GetOpenGl().GetVersion() % 10 ) );
		int l_major = GetOpenGl().GetVersion() / 10;
		int l_minor = GetOpenGl().GetVersion() % 10;
		IntArray l_arrayAttribs;
		l_arrayAttribs.push_back( GLX_CONTEXT_MAJOR_VERSION_ARB	);
		l_arrayAttribs.push_back( l_major );
		l_arrayAttribs.push_back( GLX_CONTEXT_MINOR_VERSION_ARB	);
		l_arrayAttribs.push_back( l_minor );
		l_arrayAttribs.push_back( GLX_CONTEXT_FLAGS_ARB	);
		l_arrayAttribs.push_back( GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB	);
		l_arrayAttribs.push_back( None	);
		glXMakeCurrent( m_display, None, nullptr );
		GetOpenGl().DeleteContext( m_display, m_glxContext );

		if ( l_mainContext )
		{
			m_glxContext = GetOpenGl().CreateContextAttribs( m_display, m_fbConfig[0], l_mainContext->GetImpl()->GetContext(), true, &l_arrayAttribs[0] );
		}
		else
		{
			m_glxContext = GetOpenGl().CreateContextAttribs( m_display, m_fbConfig[0], nullptr, true, &l_arrayAttribs[0] );
		}

		l_return = m_glxContext != nullptr;

		if ( l_return )
		{
			Logger::LogInfo( StringStream() << cuT( "GlContext::Create - " ) << l_major << cuT( "." ) << l_minor << cuT( " OpenGL context created." ) );
		}
		else
		{
			Logger::LogError( StringStream() << cuT( "GlContext::Create - Failed to create a " ) << l_major << cuT( "." ) << l_minor << cuT( " OpenGL context." ) );
		}
	}

	return l_return;
}
#endif
