#if defined( CASTOR_PLATFORM_LINUX )

#include "Render/GlX11Context.hpp"

#include "Common/OpenGl.hpp"
#include "Render/GlContext.hpp"
#include "Render/GlRenderSystem.hpp"

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <Log/Logger.hpp>

#include <Cache/MaterialCache.hpp>
#include <Miscellaneous/PlatformWindowHandle.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	namespace
	{
		std::function< GLXFBConfig *( Display *, int, int const *, int * ) > glXChooseFBConfig;
		std::function< XVisualInfo *( Display *, GLXFBConfig ) > glXGetVisualFromFBConfig;

#if !defined( NDEBUG )

		const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = int( GlContextAttribute::eForwardCompatibleBit ) | int( GlContextAttribute::eDebugBit );
		const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = int( GlProfileAttribute::eCoreBit );

#else

		const int C3D_GL_CONTEXT_CREATION_DEFAULT_FLAGS = int( GlContextAttribute::eForwardCompatibleBit );
		const int C3D_GL_CONTEXT_CREATION_DEFAULT_MASK = int( GlProfileAttribute::eCompatibilityBit );
#endif
	}

	GlContextImpl::GlContextImpl( OpenGl & p_gl, GlContext * p_context )
		: Holder( p_gl )
		, m_display( nullptr )
		, m_glxVersion( 10 )
		, m_glxContext( nullptr )
		, m_drawable( 0 )
		, m_fbConfig( nullptr )
		, m_context( p_context )
		, m_initialised( false )
	{
	}

	GlContextImpl::~GlContextImpl()
	{
	}

	bool GlContextImpl::Initialise( RenderWindow * p_window )
	{
		if ( !glXChooseFBConfig )
		{
			gl_api::GetFunction( cuT( "glXChooseFBConfig" ), glXChooseFBConfig );
			gl_api::GetFunction( cuT( "glXGetVisualFromFBConfig" ), glXGetVisualFromFBConfig );
		}

		if ( m_drawable == 0 )
		{
			m_drawable = p_window->GetHandle().GetInternal< IXWindowHandle >()->GetDrawable();
			m_display = p_window->GetHandle().GetInternal< IXWindowHandle >()->GetDisplay();
		}

		auto l_engine = p_window->GetEngine();
		auto l_renderSystem = static_cast< GlRenderSystem * >( l_engine->GetRenderSystem() );
		auto l_mainContext = std::static_pointer_cast< GlContext >( l_renderSystem->GetMainContext() );

		try
		{
			int l_screen = DefaultScreen( m_display );
			int l_major{ 0 };
			int l_minor{ 0 };
			bool l_ok = glXQueryVersion( m_display, &l_major, &l_minor );
			XVisualInfo * l_visualInfo = nullptr;

			if ( l_ok )
			{
				m_glxVersion = l_major * 10 + l_minor;
				Logger::LogDebug( StringStream() << cuT( "GlXContext::Create - glx version: " ) << m_glxVersion );
			}

			IntArray l_attribList
			{
				GLX_RENDER_TYPE, GLX_RGBA_BIT,
				GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
				GLX_DOUBLEBUFFER, GL_TRUE,
				GLX_RED_SIZE, 1,
				GLX_GREEN_SIZE, 1,
				GLX_BLUE_SIZE, 1,
				0
			};

			if ( p_window->IsUsingStereo() )
			{
				l_attribList.push_back( GLX_STEREO );
				l_attribList.push_back( 1 );
			}

			l_attribList.push_back( 0 );

			if ( glXChooseFBConfig )
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
					m_glxContext = glXCreateContext( m_display, l_visualInfo, l_mainContext->GetImpl().GetContext(), GL_TRUE );
				}
				else
				{
					m_glxContext = glXCreateContext( m_display, l_visualInfo, nullptr, GL_TRUE );
				}

				if ( m_glxContext )
				{
					if ( !l_renderSystem->IsInitialised() )
					{
						Logger::LogDebug( StringStream() << cuT( "Display: " ) << std::hex << cuT( "0x" ) << m_display );
						Logger::LogDebug( StringStream() << cuT( "Drawable: " ) << std::hex << cuT( "0x" ) << m_drawable );
						Logger::LogDebug( StringStream() << cuT( "Context: " ) << std::hex << cuT( "0x" ) << m_glxContext );
						glXMakeCurrent( m_display, m_drawable, m_glxContext );
						GetOpenGl().PreInitialise( String() );
						glXMakeCurrent( m_display, 0, nullptr );
					}

					if ( GetOpenGl().GetVersion() >= 30 )
					{
						m_initialised = DoCreateGl3Context( p_window );
					}
					else
					{
						m_initialised = true;
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
		}

		return m_initialised;
	}

	void GlContextImpl::Cleanup()
	{
		try
		{
			glUntrack( GetOpenGl(), this );
			GetOpenGl().DeleteContext( m_display, m_glxContext );
			XFree( m_fbConfig );
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
		GetOpenGl().MakeCurrent( m_display, 0, nullptr );
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

	XVisualInfo * GlContextImpl::DoCreateVisualInfoWithFBConfig( RenderWindow * p_window, IntArray p_arrayAttribs, int p_screen )
	{
		Logger::LogDebug( cuT( "GlXContext::Create - Using FBConfig" ) );
		XVisualInfo * l_return = nullptr;
		int l_result = 0;
		m_fbConfig = glXChooseFBConfig( m_display, p_screen, p_arrayAttribs.data(), &l_result );

		if ( !m_fbConfig )
		{

			// First try failed
			if ( p_window->IsUsingStereo() )
			{
				// Maybe because of stereo ? We try in mono
				m_gpuInformations.RemoveFeature( GpuFeature::eStereo );
				Logger::LogWarning( cuT( "GlXContext::Create - Stereo glXChooseFBConfig failed, using mono FB config" ) );

				IntArray l_attribList
				{
					GLX_RENDER_TYPE, GLX_RGBA_BIT,
					GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
					GLX_DOUBLEBUFFER, GL_TRUE,
					GLX_RED_SIZE, 1,
					GLX_GREEN_SIZE, 1,
					GLX_BLUE_SIZE, 1,
					0
				};
				m_fbConfig = glXChooseFBConfig( m_display, p_screen, l_attribList.data(), &l_result );

				if ( !m_fbConfig )
				{
					// Second try failed, we try a default FBConfig
					Logger::LogWarning( cuT( "GlXContext::Create - Mono glXChooseFBConfig failed, using default FB config" ) );
					int l_data = 0;
					m_fbConfig = glXChooseFBConfig( m_display, p_screen, &l_data, &l_result );

					if ( !m_fbConfig )
					{
						// Last FBConfig try failed
						Logger::LogWarning( cuT( "GlXContext::Create - Default glXChooseFBConfig failed" ) );
						l_return = DoCreateVisualInfoWithoutFBConfig( l_attribList, p_screen );
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
				int l_data = 0;
				m_fbConfig = glXChooseFBConfig( m_display, p_screen, &l_data, &l_result );

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
				m_gpuInformations.AddFeature( GpuFeature::eStereo );
				Logger::LogDebug( cuT( "GlXContext::Create - Stereo glXChooseFBConfig successful with detailed attributes" ) );
			}
			else
			{
				Logger::LogDebug( cuT( "GlXContext::Create - glXChooseFBConfig successful with detailed attributes" ) );
			}
		}

		if ( m_fbConfig )
		{
			l_return = glXGetVisualFromFBConfig( m_display, m_fbConfig[0] );

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

	XVisualInfo * GlContextImpl::DoCreateVisualInfoWithoutFBConfig( IntArray p_arrayAttribs, int p_screen )
	{
		Logger::LogInfo( cuT( "GlXContext::Create - Not using FBConfig" ) );
		XVisualInfo * l_return = glXChooseVisual( m_display, p_screen, p_arrayAttribs.data() );

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
			GetOpenGl().DeleteContext( m_display, m_glxContext );

			if ( l_mainContext )
			{
				m_glxContext = GetOpenGl().CreateContextAttribs( m_display, m_fbConfig[0], l_mainContext->GetImpl().GetContext(), true, l_attribList.data() );
			}
			else
			{
				m_glxContext = GetOpenGl().CreateContextAttribs( m_display, m_fbConfig[0], nullptr, true, l_attribList.data() );
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
		else
		{
			//It's not possible to make a GL 3.x and later context. Use the old style context (GL 2.1 and before)
			l_renderSystem->SetOpenGlVersion( 2, 1 );
			Logger::LogWarning( cuT( "GlContext::Create - Can't create OpenGL >= 3.x context, since glCreateContextAttribs is not supported." ) );
			l_return = true;
		}

		return l_return;
	}
}

#endif
