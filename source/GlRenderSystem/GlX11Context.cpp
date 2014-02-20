#include "GlRenderSystem/PrecompiledHeader.hpp"

#if defined( __linux__ )

#	include "GlRenderSystem/GlX11Context.hpp"
#	include "GlRenderSystem/GlContext.hpp"
#	include "GlRenderSystem/GlRenderSystem.hpp"
#	include "GlRenderSystem/OpenGl.hpp"

#	include <unistd.h>
#	include <iostream>
#	include <cstdlib>
#	include <cstring>
#	include <cstdio>

#	include <Castor3D/WindowHandle.hpp>
#	include <Castor3D/Material.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GlRender;

typedef GLXFBConfig * (FnGlXChooseFBConfigProc			)( Display *, int, int const *,int * );
typedef XVisualInfo * (FnGlXGetVisualFromFBConfigProc	)( Display *, GLXFBConfig );

typedef FnGlXChooseFBConfigProc *			PFnGlXChooseFBConfigProc;
typedef FnGlXGetVisualFromFBConfigProc *	PFnGlXGetVisualFromFBConfigProc;

PFnGlXChooseFBConfigProc		g_pfnGlXChooseFBConfig			= NULL;
PFnGlXGetVisualFromFBConfigProc g_pfnGlXGetVisualFromFBConfig	= NULL;

GlContextImpl :: GlContextImpl( OpenGl & p_gl, GlContext * p_pContext )
	:	m_pDisplay		( NULL			)
	,	m_iGlXVersion	( 10			)
	,	m_glXContext	( NULL			)
	,	m_drawable		( None			)
	,	m_pFbConfig		( NULL			)
	,	m_pContext		( p_pContext	)
	,	m_bInitialised	( false			)
	,	m_gl			( p_gl			)
{
}

GlContextImpl :: ~GlContextImpl()
{
    m_gl.DeleteContext( m_pDisplay, m_glXContext );
    XFree( m_pFbConfig );
}

bool GlContextImpl :: Initialise( RenderWindow * p_pWindow )
{
	if( !g_pfnGlXChooseFBConfig )
	{
		g_pfnGlXChooseFBConfig			= PFnGlXChooseFBConfigProc(			glXGetProcAddress( (uint8_t const *)"glXChooseFBConfig"			) );
		g_pfnGlXGetVisualFromFBConfig	= PFnGlXGetVisualFromFBConfigProc(	glXGetProcAddress( (uint8_t const *)"glXGetVisualFromFBConfig"	) );
	}

	if( m_drawable == None )
	{
		m_drawable = p_pWindow->GetHandle().GetInternal< IXWindowHandle >()->GetDrawable();
		m_pDisplay = p_pWindow->GetHandle().GetInternal< IXWindowHandle >()->GetDisplay();
	}

	try
	{
		GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_pWindow->GetEngine()->GetRenderSystem() );
		GlContextSPtr l_pMainContext = std::static_pointer_cast< GlContext >( l_pRenderSystem->GetMainContext() );

		int l_iScreen = DefaultScreen( m_pDisplay );
		int l_iMajor, l_iMinor;
		bool l_bOk = glXQueryVersion( m_pDisplay, &l_iMajor, &l_iMinor );
		XVisualInfo * l_pVisualInfo = NULL;

		if( l_bOk )
		{
			m_iGlXVersion = l_iMajor * 10 + l_iMinor;
			Logger::LogDebug( cuT( "GlXContext :: Create - glx version : %i" ), m_iGlXVersion );
		}

		IntArray l_attribList;
		l_attribList.push_back( GLX_RENDER_TYPE		);l_attribList.push_back( GLX_RGBA_BIT		);
		l_attribList.push_back( GLX_DRAWABLE_TYPE	);l_attribList.push_back( GLX_WINDOW_BIT	);
		l_attribList.push_back( GLX_DOUBLEBUFFER	);l_attribList.push_back( GL_TRUE			);
		l_attribList.push_back( GLX_RED_SIZE		);l_attribList.push_back( 1					);
		l_attribList.push_back( GLX_GREEN_SIZE		);l_attribList.push_back( 1					);
		l_attribList.push_back( GLX_BLUE_SIZE		);l_attribList.push_back( 1					);

		if( p_pWindow->IsUsingStereo() )
		{
			l_attribList.push_back(	GLX_STEREO			);l_attribList.push_back( 1					);
		}

		l_attribList.push_back( 	None				);

		if( g_pfnGlXChooseFBConfig )
		{
			l_pVisualInfo = DoCreateVisualInfoWithFBConfig( p_pWindow, l_attribList, l_iScreen );
		}
		else
		{
			l_pVisualInfo = DoCreateVisualInfoWithoutFBConfig( l_attribList, l_iScreen );
		}

		if( l_pVisualInfo )
		{
			if( l_pMainContext )
			{
				m_glXContext = glXCreateContext( m_pDisplay, l_pVisualInfo, l_pMainContext->GetImpl()->GetContext(), GL_TRUE );
			}
			else
			{
				m_glXContext = glXCreateContext( m_pDisplay, l_pVisualInfo, NULL, GL_TRUE );
			}

			if( !m_glXContext )
			{
				Logger::LogWarning( str_utils::from_str( "GlXContext :: Create - glXCreateContext failed" ) );
			}
			else
			{
				Logger::LogMessage( str_utils::from_str( "GlXContext :: Create - glXContext created" ) );

				if ( !l_pRenderSystem->IsInitialised() )
				{
					glXMakeCurrent( m_pDisplay, m_drawable, m_glXContext );
					l_pRenderSystem->Initialise( cuT( "" ) );
					p_pWindow->GetEngine()->GetMaterialManager().Initialise();
					glXMakeCurrent( m_pDisplay, None, NULL );
				}

				if( l_pRenderSystem->GetOpenGlMajor() >= 3 )
				{
					m_bInitialised = DoCreateGl3Context( p_pWindow );
				}
				else
				{
					m_bInitialised = true;
				}
			}

			XFree( l_pVisualInfo );
		}
	}
	catch( ... )
	{
		m_bInitialised = false;
	}

	if( m_bInitialised )
	{
#if !defined( NDEBUG )
		if( m_gl.HasDebugOutput() )
		{
			SetCurrent();
			m_gl.DebugMessageCallback( OpenGl::PFNGLDEBUGPROC( &OpenGl::StDebugLog ), &m_gl );
			m_gl.DebugMessageCallback( OpenGl::PFNGLDEBUGAMDPROC( &OpenGl::StDebugLogAMD ), &m_gl );
#if defined( _WIN32 )
			m_gl.Enable( eGL_TWEAK_DEBUG_OUTPUT_SYNCHRONOUS );
#endif
			EndCurrent();
		}
#endif
		UpdateVSync( p_pWindow->GetVSync() );
	}

	return m_bInitialised;
}

void GlContextImpl :: SetCurrent()
{
	m_gl.MakeCurrent( m_pDisplay, m_drawable, m_glXContext );
}

void GlContextImpl :: EndCurrent()
{
	m_gl.MakeCurrent( m_pDisplay, None, nullptr );
}

void GlContextImpl :: SwapBuffers()
{
    m_gl.SwapBuffers( m_pDisplay, m_drawable );
}

void GlContextImpl :: UpdateVSync( bool p_bEnable )
{
	SetCurrent();

	if( p_bEnable )
	{
		m_gl.SwapInterval( m_pDisplay, m_drawable, 1 );
	}
	else
	{
		m_gl.SwapInterval( m_pDisplay, m_drawable, 0 );
	}

	EndCurrent();
}

XVisualInfo * GlContextImpl :: DoCreateVisualInfoWithFBConfig( RenderWindow * p_pWindow, IntArray & p_arrayAttribs, int p_iScreen )
{
	Logger::LogDebug( cuT( "GlXContext :: Create - Using FBConfig" ) );
	XVisualInfo *	l_pReturn = NULL;
	int				l_iData = 0;
	int				l_iResult;

	m_pFbConfig = g_pfnGlXChooseFBConfig( m_pDisplay, p_iScreen, &p_arrayAttribs[0], &l_iResult );

	if( !m_pFbConfig )
	{
		// First try failed
		if( p_pWindow->IsUsingStereo() )
		{
			// Maybe because of stereo ? We try in mono
			p_pWindow->GetEngine()->GetRenderSystem()->SetStereoAvailable( false );
			Logger::LogWarning( cuT( "GlXContext :: Create - Stereo glXChooseFBConfig failed, using mono FB config" ) );

			p_arrayAttribs.clear();
			p_arrayAttribs.push_back( GLX_RENDER_TYPE	);p_arrayAttribs.push_back( GLX_RGBA_BIT	);
			p_arrayAttribs.push_back( GLX_DRAWABLE_TYPE	);p_arrayAttribs.push_back( GLX_WINDOW_BIT	);
			p_arrayAttribs.push_back( GLX_DOUBLEBUFFER	);p_arrayAttribs.push_back( GL_TRUE			);
			p_arrayAttribs.push_back( GLX_RED_SIZE		);p_arrayAttribs.push_back( 1				);
			p_arrayAttribs.push_back( GLX_GREEN_SIZE	);p_arrayAttribs.push_back( 1				);
			p_arrayAttribs.push_back( GLX_BLUE_SIZE		);p_arrayAttribs.push_back( 1				);
			p_arrayAttribs.push_back( None				);

			m_pFbConfig = g_pfnGlXChooseFBConfig( m_pDisplay, p_iScreen, &p_arrayAttribs[0], &l_iResult );

			if( !m_pFbConfig )
			{
				// Second try failed, we try a default FBConfig
				Logger::LogWarning( cuT( "GlXContext :: Create - Mono glXChooseFBConfig failed, using default FB config" ) );
				m_pFbConfig = g_pfnGlXChooseFBConfig( m_pDisplay, p_iScreen, &l_iData, &l_iResult );

				if( !m_pFbConfig )
				{
					// Last FBConfig try failed
					Logger::LogWarning( str_utils::from_str( "GlXContext :: Create - Default glXChooseFBConfig failed" ) );
					l_pReturn = DoCreateVisualInfoWithoutFBConfig( p_arrayAttribs, p_iScreen );
				}
				else
				{
					Logger::LogDebug( str_utils::from_str( "GlXContext :: Create - Default glXChooseFBConfig successful" ) );
				}
			}
			else
			{
				Logger::LogDebug( str_utils::from_str( "GlXContext :: Create - Mono glXChooseFBConfig successful with detailed attributes" ) );
			}
		}
		else
		{
			// Can't be because of stereo, we try a default FBConfig
			Logger::LogWarning( cuT( "GlXContext :: Create - glXChooseFBConfig failed, using default FB config" ) );
			m_pFbConfig = g_pfnGlXChooseFBConfig( m_pDisplay, p_iScreen, &l_iData, &l_iResult );

			if( !m_pFbConfig )
			{
				// Last FBConfig try failed, we try from XVisualInfo
				Logger::LogWarning( str_utils::from_str( "GlXContext :: Create - Default glXChooseFBConfig failed" ) );
				l_pReturn = DoCreateVisualInfoWithoutFBConfig( p_arrayAttribs, p_iScreen );
			}
			else
			{
				Logger::LogDebug( str_utils::from_str( "GlXContext :: Create - Default glXChooseFBConfig successful" ) );
			}
		}
	}
	else
	{
		if( p_pWindow->IsUsingStereo() )
		{
			p_pWindow->GetEngine()->GetRenderSystem()->SetStereoAvailable( true );
			Logger::LogDebug( str_utils::from_str( "GlXContext :: Create - Stereo glXChooseFBConfig successful with detailed attributes" ) );
		}
		else
		{
			Logger::LogDebug( str_utils::from_str( "GlXContext :: Create - glXChooseFBConfig successful with detailed attributes" ) );
		}
	}

	if( m_pFbConfig )
	{
		l_pReturn = g_pfnGlXGetVisualFromFBConfig( m_pDisplay, m_pFbConfig[0] );

		if( !l_pReturn )
		{
			Logger::LogError( str_utils::from_str( "GlXContext :: Create - glXGetVisualFromFBConfig failed" ) );
		}
		else
		{
			Logger::LogDebug( str_utils::from_str( "GlXContext :: Create - GlXGetVisualFromFBConfig successful" ) );
		}
	}

	return l_pReturn;
}

XVisualInfo * GlContextImpl :: DoCreateVisualInfoWithoutFBConfig( IntArray & p_arrayAttribs, int p_iScreen )
{
	Logger::LogMessage( cuT( "GlXContext :: Create - Not using FBConfig" ) );
	XVisualInfo	* l_pReturn = glXChooseVisual( m_pDisplay, p_iScreen, &p_arrayAttribs[0] );

	if( !l_pReturn )
	{
		Logger::LogError( str_utils::from_str( "GlXContext :: Create - glXChooseVisual failed") );
	}

	return l_pReturn;
}

bool GlContextImpl :: DoCreateGl3Context( Castor3D::RenderWindow * p_pWindow )
{
	bool l_bReturn = false;
#if !C3DGL_LIMIT_TO_2_1
	GlRenderSystem * l_pRenderSystem = static_cast< GlRenderSystem * >( p_pWindow->GetEngine()->GetRenderSystem() );
	GlContextSPtr l_pMainContext = std::static_pointer_cast< GlContext >( l_pRenderSystem->GetMainContext() );

	if( m_gl.HasCreateContextAttribs() )
	{
		Logger::LogMessage( cuT("GlXContext :: Create - Using OpenGL %i.%i" ), l_pRenderSystem->GetOpenGlMajor(), l_pRenderSystem->GetOpenGlMinor() );

		IntArray l_arrayAttribs;
		l_arrayAttribs.push_back( GLX_CONTEXT_MAJOR_VERSION_ARB	);l_arrayAttribs.push_back( l_pRenderSystem->GetOpenGlMajor()		);
		l_arrayAttribs.push_back( GLX_CONTEXT_MINOR_VERSION_ARB	);l_arrayAttribs.push_back( l_pRenderSystem->GetOpenGlMinor()		);
		l_arrayAttribs.push_back( GLX_CONTEXT_FLAGS_ARB			);l_arrayAttribs.push_back( GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB	);
		l_arrayAttribs.push_back( None							);

		glXMakeCurrent( m_pDisplay, None, NULL );
		m_gl.DeleteContext( m_pDisplay, m_glXContext );

		if( l_pMainContext )
		{
			m_glXContext = m_gl.CreateContextAttribs( m_pDisplay, m_pFbConfig[0], l_pMainContext->GetImpl()->GetContext(), true, &l_arrayAttribs[0] );
		}
		else
		{
			m_glXContext = m_gl.CreateContextAttribs( m_pDisplay, m_pFbConfig[0], NULL, true, &l_arrayAttribs[0] );
		}

		if( !m_glXContext )
		{
			Logger::LogWarning( str_utils::from_str( "GlXContext :: Create - glXCreateContextAttribs failed" ) );
			l_bReturn = false;
		}
		else
		{
			Logger::LogDebug( str_utils::from_str( "GlXContext :: Create - glXContext 3.x/4.x compatible created" ) );
			l_bReturn = true;
		}
	}
#else
	l_bReturn = true;
#endif
	return l_bReturn;
}
#endif
