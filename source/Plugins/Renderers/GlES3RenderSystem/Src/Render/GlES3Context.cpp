#include "Render/GlES3Context.hpp"

#if defined( CASTOR_PLATFORM_ANDROID )
#	include "Render/GlES3EGLContext.hpp"
#endif

#include "Render/GlES3RenderPipeline.hpp"
#include "Render/GlES3RenderSystem.hpp"

#include <Cache/MaterialCache.hpp>

#include <Mesh/Buffer/GeometryBuffers.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Render/RenderLoop.hpp>
#include <Render/RenderWindow.hpp>
#include <Shader/ShaderProgram.hpp>

using namespace Castor;
using namespace Castor3D;

namespace GlES3Render
{
	GlES3Context::GlES3Context( GlES3RenderSystem & p_renderSystem, OpenGlES3 & p_gl )
		: Context{ p_renderSystem }
		, Holder{ p_gl }
		, m_implementation{ std::make_unique< GlES3ContextImpl >( GetOpenGlES3(), this ) }
	{
	}

	GlES3Context::~GlES3Context()
	{
		m_implementation.reset();
	}

	GlES3ContextImpl & GlES3Context::GetImpl()
	{
		return *m_implementation;
	}

	bool GlES3Context::DoInitialise()
	{
		auto l_engine = m_window->GetEngine();
		auto l_renderSystem = static_cast< GlES3RenderSystem * >( l_engine->GetRenderSystem() );
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
				l_engine->GetMaterialCache().Initialise();

#if !defined( NDEBUG )

				GetOpenGlES3().InitialiseDebug();

#endif

				GetImpl().EndCurrent();
				Logger::LogInfo( cuT( "OpenGL Initialisation Ended" ) );
				Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
			}

			GetImpl().UpdateVSync( m_window->GetVSync() );
			l_engine->GetRenderLoop().UpdateVSync( m_window->GetVSync() );
		}
		else if ( !l_mainContext )
		{
			Logger::LogError( cuT( "OpenGL Initialisation Failed" ) );
			Logger::LogInfo( cuT( "***********************************************************************************************************************" ) );
		}

		return m_initialised;
	}

	void GlES3Context::DoCleanup()
	{
	}

	void GlES3Context::DoDestroy()
	{
		GetImpl().Cleanup();
	}

	void GlES3Context::DoSetCurrent()
	{
		GetImpl().SetCurrent();
	}

	void GlES3Context::DoEndCurrent()
	{
		GetImpl().EndCurrent();
	}

	void GlES3Context::DoSwapBuffers()
	{
		GetImpl().SwapBuffers();
	}
}
