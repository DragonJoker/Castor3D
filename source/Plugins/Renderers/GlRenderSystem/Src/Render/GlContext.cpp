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

using namespace castor;
using namespace castor3d;

namespace GlRender
{
	GlContext::GlContext( GlRenderSystem & renderSystem, OpenGl & p_gl )
		: Context{ renderSystem }
		, Holder{ p_gl }
		, m_glRenderSystem{ &renderSystem }
		, m_implementation{ std::make_unique< GlContextImpl >( getOpenGl(), this ) }
	{
	}

	GlContext::~GlContext()
	{
		m_implementation.reset();
	}

	GlContextImpl & GlContext::getImpl()
	{
		return *m_implementation;
	}

	bool GlContext::doInitialise()
	{
		auto engine = m_window->getEngine();
		auto renderSystem = static_cast< GlRenderSystem * >( engine->getRenderSystem() );
		auto mainContext = renderSystem->getMainContext();

		if ( !mainContext )
		{
			Logger::logInfo( cuT( "***********************************************************************************************************************" ) );
			Logger::logInfo( cuT( "Initialising OpenGL" ) );
		}

		m_initialised = getImpl().initialise( m_window );

		if ( m_initialised )
		{
			if ( !mainContext )
			{
				getImpl().setCurrent();
				renderSystem->initialise( std::move( getImpl().getGpuInformations() ) );
				engine->getMaterialCache().initialise( m_window->getScene()->getMaterialsType() );

#if !defined( NDEBUG )

				getOpenGl().initialiseDebug();

#endif

				getImpl().endCurrent();
				Logger::logInfo( cuT( "OpenGL Initialisation Ended" ) );
				Logger::logInfo( cuT( "***********************************************************************************************************************" ) );
			}

			getImpl().updateVSync( m_window->getVSync() );
			engine->getRenderLoop().updateVSync( m_window->getVSync() );

			if ( m_glRenderSystem->getOpenGlMajor() < 3
				|| ( m_glRenderSystem->getOpenGlMajor() == 3 && m_glRenderSystem->getOpenGlMinor() < 3 ) )
			{
				CASTOR_EXCEPTION( cuT( "The supported OpenGL version is insufficient to run Castor3D (OpenGL 3.3 is needed, at least)" ) );
			}

			getImpl().setCurrent();
			renderSystem->getOpenGl().Enable( GlTweak::eSeamlessCubeMaps );
			getImpl().endCurrent();
		}
		else if ( !mainContext )
		{
			Logger::logError( cuT( "OpenGL Initialisation Failed" ) );
			Logger::logInfo( cuT( "***********************************************************************************************************************" ) );
		}

		return m_initialised;
	}

	void GlContext::doCleanup()
	{
	}

	void GlContext::doDestroy()
	{
		getImpl().cleanup();
	}

	void GlContext::doSetCurrent()
	{
		getImpl().setCurrent();
	}

	void GlContext::doEndCurrent()
	{
		getImpl().endCurrent();
	}

	void GlContext::doSwapBuffers()
	{
		getImpl().swapBuffers();
	}

	void GlContext::doMemoryBarrier( MemoryBarriers const & p_barriers )
	{
		getOpenGl().MemoryBarrier( getOpenGl().get( p_barriers ) );
	}
}
