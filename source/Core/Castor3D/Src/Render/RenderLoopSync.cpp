#include "RenderLoopSync.hpp"

#include "RenderSystem.hpp"

#include <Core/Exception.hpp>

using namespace castor;

namespace castor3d
{
	static const char * CALL_START_RENDERING = "Can't call beginRendering in a synchronous render loop";
	static const char * CALL_END_RENDERING = "Can't call endRendering in a synchronous render loop";
	static const char * CALL_PAUSE_RENDERING = "Can't call Pause in a synchronous render loop";
	static const char * CALL_RESUME_RENDERING = "Can't call Resume in a synchronous render loop";
	static const char * RLS_UNKNOWN_EXCEPTION = "Unknown exception";

	RenderLoopSync::RenderLoopSync( Engine & engine
		, uint32_t wantedFPS )
		: RenderLoop( engine, wantedFPS, true )
		, m_active( true )
	{
	}

	RenderLoopSync::~RenderLoopSync()
	{
		cleanup();
		m_renderSystem.cleanup();
	}

	void RenderLoopSync::beginRendering()
	{
		CASTOR_EXCEPTION( CALL_START_RENDERING );
	}

	void RenderLoopSync::renderSyncFrame()
	{
		if ( m_active )
		{
			try
			{
				doRenderFrame();
			}
			catch ( renderer::Exception & exc )
			{
				Logger::logError( String{ cuT( "RenderLoop - " ) } +exc.what() );
				m_active = false;
			}
			catch ( castor::Exception & exc )
			{
				Logger::logError( exc.getFullDescription() );
				m_active = false;
			}
			catch ( std::exception & exc )
			{
				Logger::logError( exc.what() );
				m_active = false;
			}
			catch ( ... )
			{
				Logger::logError( RLS_UNKNOWN_EXCEPTION );
				m_active = false;
			}
		}
	}

	void RenderLoopSync::pause()
	{
		CASTOR_EXCEPTION( CALL_PAUSE_RENDERING );
	}

	void RenderLoopSync::resume()
	{
		CASTOR_EXCEPTION( CALL_RESUME_RENDERING );
	}

	void RenderLoopSync::endRendering()
	{
		CASTOR_EXCEPTION( CALL_END_RENDERING );
	}

	renderer::DevicePtr RenderLoopSync::doCreateMainDevice( renderer::WindowHandle && handle
		, RenderWindow & window )
	{
		auto result = doCreateDevice( std::move( handle ), window );

		if ( result )
		{
			m_renderSystem.setMainDevice( result );
			result->enable();
			GpuInformations info;
			m_renderSystem.initialise( std::move( info ) );
			result->disable();
		}

		return result;
	}
}
