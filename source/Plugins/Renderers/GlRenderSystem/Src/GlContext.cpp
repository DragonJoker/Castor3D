#include "GlContext.hpp"

#if defined( _WIN32 )
#	include "GlMswContext.hpp"
#elif defined( __linux__ )
#	include "GlX11Context.hpp"
#endif

#include "GlPipeline.hpp"
#include "GlRenderSystem.hpp"

#include <GeometryBuffers.hpp>
#include <IndexBuffer.hpp>
#include <MaterialManager.hpp>
#include <RenderLoop.hpp>
#include <RenderWindow.hpp>
#include <ShaderProgram.hpp>
#include <VertexBuffer.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlContext::GlContext( GlRenderSystem & p_renderSystem, OpenGl & p_gl )
		: Context{ p_renderSystem }
		, Holder{ p_gl }
		, m_glRenderSystem{ &p_renderSystem }
		, m_implementation{ std::make_unique< GlContextImpl >( GetOpenGl(), this ) }
	{
		m_pipeline = std::make_unique< GlPipeline >( GetOpenGl(), *this );
	}

	GlContext::~GlContext()
	{
		m_pipeline.reset();
		m_implementation.reset();
	}

	GlContextImpl & GlContext::GetImpl()
	{
		return *m_implementation;
	}

	bool GlContext::DoInitialise()
	{
		auto l_engine = m_window->GetEngine();
		auto l_renderSystem = static_cast< GlRenderSystem * >( l_engine->GetRenderSystem() );
		auto l_mainContext = l_renderSystem->GetMainContext();

		if ( !l_mainContext )
		{
			Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
			Logger::LogInfo( cuT( "Initialising OpenGL" ) );
		}

		m_initialised = GetImpl().Initialise( m_window );

		if ( m_initialised )
		{
			if ( !l_mainContext )
			{
				GetImpl().SetCurrent();
				l_renderSystem->Initialise( std::move( GetImpl().GetGpuInformations() ) );
				l_engine->GetMaterialManager().Initialise();

#if !defined( NDEBUG )

				if ( GetOpenGl().HasDebugOutput() )
				{
					GetOpenGl().DebugMessageCallback( OpenGl::PFNGLDEBUGPROC( &OpenGl::StDebugLog ), &GetOpenGl() );
					GetOpenGl().DebugMessageCallback( OpenGl::PFNGLDEBUGAMDPROC( &OpenGl::StDebugLogAMD ), &GetOpenGl() );
					GetOpenGl().Enable( eGL_TWEAK_DEBUG_OUTPUT_SYNCHRONOUS );
				}

#endif

				GetImpl().EndCurrent();
				Logger::LogInfo( cuT( "OpenGL Initialisation Ended" ) );
				Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
			}

			GetImpl().UpdateVSync( m_window->GetVSync() );
			l_engine->GetRenderLoop().UpdateVSync( m_window->GetVSync() );

			if ( m_glRenderSystem->GetOpenGlMajor() < 3 )
			{
				CASTOR_EXCEPTION( cuT( "The supported OpenGL version is insufficient to run Castor3D" ) );
			}
		}
		else if ( !l_mainContext )
		{
			Logger::LogError( cuT( "OpenGL Initialisation Failed" ) );
			Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
		}

		return m_initialised;
	}

	void GlContext::DoCleanup()
	{
	}

	void GlContext::DoDestroy()
	{
		GetImpl().Cleanup();
	}

	void GlContext::DoSetCurrent()
	{
		GetImpl().SetCurrent();
	}

	void GlContext::DoEndCurrent()
	{
		GetImpl().EndCurrent();
	}

	void GlContext::DoSwapBuffers()
	{
		GetImpl().SwapBuffers();
	}
}
