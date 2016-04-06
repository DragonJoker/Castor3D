#include "RenderLoopSync.hpp"

#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	static const char * CALL_START_RENDERING = "Can't call StartRendering in a synchronous render loop";
	static const char * CALL_END_RENDERING = "Can't call EndRendering in a synchronous render loop";
	static const char * CALL_PAUSE_RENDERING = "Can't call Pause in a synchronous render loop";
	static const char * CALL_RESUME_RENDERING = "Can't call Resume in a synchronous render loop";
	static const char * RLS_UNKNOWN_EXCEPTION = "Unknown exception";

	RenderLoopSync::RenderLoopSync( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS )
		: RenderLoop( p_engine, p_renderSystem, p_wantedFPS )
		, m_active( true )
	{
	}

	RenderLoopSync::~RenderLoopSync()
	{
		Cleanup();
		m_renderSystem->Cleanup();
	}

	void RenderLoopSync::DoStartRendering()
	{
		CASTOR_EXCEPTION( CALL_START_RENDERING );
	}

	void RenderLoopSync::DoRenderSyncFrame()
	{
		if ( m_active )
		{
			try
			{
				DoRenderFrame();
			}
			catch ( Exception & p_exc )
			{
				Logger::LogError( p_exc.GetFullDescription() );
				m_active = false;
			}
			catch ( std::exception & p_exc )
			{
				Logger::LogError( p_exc.what() );
				m_active = false;
			}
			catch ( ... )
			{
				Logger::LogError( RLS_UNKNOWN_EXCEPTION );
				m_active = false;
			}
		}
	}

	void RenderLoopSync::DoPause()
	{
		CASTOR_EXCEPTION( CALL_PAUSE_RENDERING );
	}

	void RenderLoopSync::DoResume()
	{
		CASTOR_EXCEPTION( CALL_RESUME_RENDERING );
	}

	void RenderLoopSync::DoEndRendering()
	{
		CASTOR_EXCEPTION( CALL_END_RENDERING );
	}

	ContextSPtr RenderLoopSync::DoCreateMainContext( RenderWindow & p_window )
	{
		ContextSPtr l_return = DoCreateContext( p_window );

		if ( l_return )
		{
			m_renderSystem->SetMainContext( l_return );
		}

		return l_return;
	}
}
