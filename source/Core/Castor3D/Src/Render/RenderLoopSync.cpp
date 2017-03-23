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

	RenderLoopSync::RenderLoopSync( Engine & p_engine, uint32_t p_wantedFPS )
		: RenderLoop( p_engine, p_wantedFPS, true )
		, m_active( true )
	{
	}

	RenderLoopSync::~RenderLoopSync()
	{
		Cleanup();
		m_renderSystem.Cleanup();
	}

	void RenderLoopSync::StartRendering()
	{
		CASTOR_EXCEPTION( CALL_START_RENDERING );
	}

	void RenderLoopSync::RenderSyncFrame()
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

	void RenderLoopSync::Pause()
	{
		CASTOR_EXCEPTION( CALL_PAUSE_RENDERING );
	}

	void RenderLoopSync::Resume()
	{
		CASTOR_EXCEPTION( CALL_RESUME_RENDERING );
	}

	void RenderLoopSync::EndRendering()
	{
		CASTOR_EXCEPTION( CALL_END_RENDERING );
	}

	ContextSPtr RenderLoopSync::DoCreateMainContext( RenderWindow & p_window )
	{
		ContextSPtr l_return = DoCreateContext( p_window );

		if ( l_return )
		{
			m_renderSystem.SetMainContext( l_return );
		}

		return l_return;
	}
}
