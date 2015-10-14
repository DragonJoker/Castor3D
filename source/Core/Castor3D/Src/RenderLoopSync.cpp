#include "RenderLoopSync.hpp"

#include "RenderSystem.hpp"

namespace Castor3D
{
	static const char * CALL_START_RENDERING = "Can't call StartRendering in a synchronous render loop";
	static const char * CALL_END_RENDERING = "Can't call EndRendering in a synchronous render loop";

	RenderLoopSync::RenderLoopSync( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS )
		: RenderLoop( p_engine, p_renderSystem, p_wantedFPS )
	{
	}

	RenderLoopSync::~RenderLoopSync()
	{
		// No render loop so we clean the render system ourselves with that single call.
		DoRenderOneFrame();
	}

	void RenderLoopSync::DoStartRendering()
	{
		CASTOR_ASSERT( false );
		CASTOR_EXCEPTION( CALL_START_RENDERING );
	}

	void RenderLoopSync::DoRenderSyncFrame()
	{
		DoRenderOneFrame();
	}

	void RenderLoopSync::DoEndRendering()
	{
		CASTOR_ASSERT( false );
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
