#include "Render/GlContext.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include "Render/GlMswContext.hpp"
#elif defined( CASTOR_PLATFORM_LINUX )
#	include "Render/GlX11Context.hpp"
#endif

#include "Render/GlRenderPipeline.hpp"
#include "Render/GlRenderSystem.hpp"

#include <Cache/MaterialCache.hpp>

#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlRender
{
	GlContext::GlContext( GlRenderSystem & renderSystem, OpenGl & p_gl )
		: Context{ renderSystem }
		, Holder{ p_gl }
		, m_glRenderSystem{ &renderSystem }
		, m_implementation{ std::make_unique< GlContextImpl >( GetOpenGl(), this ) }
	{
	}

	GlContext::~GlContext()
	{
		m_implementation.reset();
	}

	GlContextImpl & GlContext::GetImpl()
	{
		return *m_implementation;
	}

	bool GlContext::DoInitialise()
	{
		auto engine = m_window->GetEngine();
		auto renderSystem = static_cast< GlRenderSystem * >( engine->GetRenderSystem() );
		auto mainContext = renderSystem->GetMainContext();

		if ( !mainContext )
		{
			Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
			Logger::LogInfo( cuT( "Initialising OpenGL" ) );
		}

		m_initialised = GetImpl().Initialise( m_window );

		if ( m_initialised )
		{
			if ( !mainContext )
			{
				GetImpl().SetCurrent();
				renderSystem->Initialise( std::move( GetImpl().GetGpuInformations() ) );
				engine->GetMaterialCache().Initialise();

#if !defined( NDEBUG )

				GetOpenGl().InitialiseDebug();

#endif

				GetImpl().EndCurrent();
				Logger::LogInfo( cuT( "OpenGL Initialisation Ended" ) );
				Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
			}

			GetImpl().UpdateVSync( m_window->GetVSync() );
			engine->GetRenderLoop().UpdateVSync( m_window->GetVSync() );

			if ( m_glRenderSystem->GetOpenGlMajor() < 4 )
			{
				CASTOR_EXCEPTION( cuT( "The supported OpenGL version is insufficient to run Castor3D (OpenGL 4.2 is needed, at least)" ) );
			}

			GetImpl().SetCurrent();
			renderSystem->GetOpenGl().Enable( GlTweak::eSeamlessCubeMaps );
			GetImpl().EndCurrent();
		}
		else if ( !mainContext )
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

	void GlContext::DoBarrier( MemoryBarriers const & p_barriers )
	{
		GetOpenGl().MemoryBarrier( GetOpenGl().Get( p_barriers ) );
	}
}
