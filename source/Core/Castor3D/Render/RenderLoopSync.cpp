#include "Castor3D/Render/RenderLoopSync.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>

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
		CU_Exception( CALL_START_RENDERING );
	}

	void RenderLoopSync::renderSyncFrame()
	{
		if ( m_active )
		{
			try
			{
				doRenderFrame();
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
		CU_Exception( CALL_PAUSE_RENDERING );
	}

	void RenderLoopSync::resume()
	{
		CU_Exception( CALL_RESUME_RENDERING );
	}

	void RenderLoopSync::endRendering()
	{
		CU_Exception( CALL_END_RENDERING );
	}

	RenderDeviceSPtr RenderLoopSync::doCreateMainDevice( ashes::WindowHandle handle
		, RenderWindow & window )
	{
		auto result = doCreateDevice( std::move( handle ), window );

		if ( result )
		{
			m_renderSystem.setMainDevice( result );
			auto guard = makeBlockGuard(
				[this, &result]()
				{
					m_renderSystem.setCurrentRenderDevice( result.get() );
				},
				[this]()
				{
					m_renderSystem.setCurrentRenderDevice( nullptr );
				} );
			GpuInformations info;
			m_renderSystem.initialise( std::move( info ) );
		}

		return result;
	}
}
