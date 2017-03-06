#include "Render/GlES3EGLContext.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )

#include "Common/OpenGlES3.hpp"
#include "Render/GlES3Context.hpp"
#include "Render/GlES3RenderSystem.hpp"

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

#ifndef EGL_OPENGL_ES3_BIT
#	define EGL_OPENGL_ES3_BIT 0x00000040
#endif

namespace GlES3Render
{
	GlES3ContextImpl::GlES3ContextImpl( OpenGlES3 & p_gl, GlES3Context * p_context )
		: Holder{ p_gl }
		, m_context{ p_context }
	{
	}

	GlES3ContextImpl::~GlES3ContextImpl()
	{
	}

	bool GlES3ContextImpl::Initialise( RenderWindow * p_window )
	{
		if ( m_window == 0 )
		{
			m_window = p_window->GetHandle().GetInternal< IEglWindowHandle >()->GetWindow();
			m_display = p_window->GetHandle().GetInternal< IEglWindowHandle >()->GetDisplay();
		}

		auto l_engine = p_window->GetEngine();
		auto l_renderSystem = static_cast< GlES3RenderSystem * >( l_engine->GetRenderSystem() );
		auto l_mainContext = std::static_pointer_cast< GlES3Context >( l_renderSystem->GetMainContext() );

		try
		{
			EGLint l_numConfigs{ 0 };
			IntArray l_attribList
			{
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_BLUE_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_RED_SIZE, 8,
				EGL_NONE
			};

			eglChooseConfig( m_display
				, l_attribList.data()
				, &m_eglConfig
				, 1
				, &l_numConfigs );

			if ( m_eglConfig )
			{
				EGLint l_format;
				eglGetConfigAttrib( m_display, m_eglConfig, EGL_NATIVE_VISUAL_ID, &l_format );
				ANativeWindow_setBuffersGeometry( m_window, 0, 0, l_format );

				m_eglSurface = eglCreateWindowSurface( m_display
					, m_eglConfig
					, m_window
					, nullptr );

				if ( m_eglSurface )
				{
					EGLint l_contextAttribs[]
					{
						EGL_CONTEXT_CLIENT_VERSION, 3,
						EGL_NONE
					};

					if ( l_mainContext )
					{
						m_eglContext = eglCreateContext( m_display
							, m_eglConfig
							, l_mainContext->GetImpl().GetContext()
							, l_contextAttribs );
					}
					else
					{
						m_eglContext = eglCreateContext( m_display
							, m_eglConfig
							, nullptr
							, l_contextAttribs );
					}
				}

				if ( m_eglContext )
				{
					if ( !l_renderSystem->IsInitialised() )
					{
						Logger::LogDebug( StringStream() << cuT( "Display: " ) << std::hex << cuT( "0x" ) << m_display );
						Logger::LogDebug( StringStream() << cuT( "Surface: " ) << std::hex << cuT( "0x" ) << m_eglSurface );
						Logger::LogDebug( StringStream() << cuT( "Context: " ) << std::hex << cuT( "0x" ) << m_eglContext );
						eglMakeCurrent( m_display, m_eglSurface, m_eglSurface, m_eglContext );
						GetOpenGlES3().PreInitialise( String() );
						eglMakeCurrent( m_display, 0, 0, nullptr );
					}

					m_initialised = true;
				}
			}
		}
		catch ( ... )
		{
			m_initialised = false;
		}

		if ( m_initialised )
		{
			glTrack( GetOpenGlES3(), "GlES3ContextImpl", this );
		}

		return m_initialised;
	}

	void GlES3ContextImpl::Cleanup()
	{
		try
		{
			if ( m_eglContext != EGL_NO_CONTEXT )
			{
				eglDestroyContext( m_display, m_eglContext );
			}

			if ( m_eglSurface != EGL_NO_SURFACE )
			{
				eglDestroySurface( m_display, m_eglSurface );
			}
		}
		catch ( ... )
		{
		}
	}

	void GlES3ContextImpl::SetCurrent()
	{
		GetOpenGlES3().MakeCurrent( m_display, m_eglSurface, m_eglSurface, m_eglContext );
	}

	void GlES3ContextImpl::EndCurrent()
	{
		GetOpenGlES3().MakeCurrent( m_display, 0, 0, nullptr );
	}

	void GlES3ContextImpl::SwapBuffers()
	{
		GetOpenGlES3().SwapBuffers( m_display, m_eglSurface );
	}

	void GlES3ContextImpl::UpdateVSync( bool p_enable )
	{
		SetCurrent();

		if ( p_enable )
		{
			GetOpenGlES3().SwapInterval( m_display, 1 );
		}
		else
		{
			GetOpenGlES3().SwapInterval( m_display, 0 );
		}

		EndCurrent();
	}
}

#endif
