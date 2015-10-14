#include "RenderLoop.hpp"

#include "BlendStateManager.hpp"
#include "DebugOverlays.hpp"
#include "Engine.hpp"
#include "OverlayManager.hpp"
#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "SamplerManager.hpp"
#include "TargetManager.hpp"
#include "WindowManager.hpp"

using namespace Castor;

namespace Castor3D
{
	static const char * C3D_MAIN_LOOP_EXISTS = "Render loop is already started";

	RenderLoop::RenderLoop( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS )
		: OwnedBy< Engine >( p_engine )
		, m_wantedFPS( p_wantedFPS )
		, m_frameTime( 1.0 / p_wantedFPS )
		, m_renderSystem( p_renderSystem )
		, m_debugOverlays( std::make_unique< DebugOverlays >() )
	{
		m_debugOverlays->Initialise( GetOwner()->GetOverlayManager() );
	}

	RenderLoop::~RenderLoop()
	{
		m_debugOverlays.reset();
	}

	void RenderLoop::StartRendering()
	{
		DoStartRendering();
	}

	void RenderLoop::RenderSyncFrame()
	{
		DoRenderSyncFrame();
	}

	void RenderLoop::EndRendering()
	{
		DoEndRendering();
	}

	double RenderLoop::GetFrameTime()
	{
		return m_frameTime;
	}

	void RenderLoop::CreateContext( RenderWindow & p_window )
	{
		if ( !m_renderSystem->GetMainContext() )
		{
			DoCreateMainContext( p_window );
		}
		else
		{
			DoCreateContext( p_window );
		}
	}

	void RenderLoop::ShowDebugOverlays( bool p_show )
	{
		m_debugOverlays->Show( p_show );
	}

	void RenderLoop::DoGpuStep( uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount )
	{
		PreciseTimer l_timer;
		m_renderSystem->GetMainContext()->SetCurrent();

		for ( auto && l_listener : GetOwner()->GetFrameListeners() )
		{
			l_listener.second->FireEvents( eEVENT_TYPE_PRE_RENDER );
		}

		GetOwner()->GetOverlayManager().Update();
		GetOwner()->GetTargetManager().Render( m_frameTime, p_vtxCount, p_fceCount, p_objCount );
		m_renderSystem->GetMainContext()->EndCurrent();
		GetOwner()->GetWindowManager().Render( true );
	}

	void RenderLoop::DoCpuStep()
	{
		for ( auto && l_listener : GetOwner()->GetFrameListeners() )
		{
			l_listener.second->FireEvents( eEVENT_TYPE_POST_RENDER );
		}
	}

	void RenderLoop::DoRenderOneFrame()
	{
		if ( m_renderSystem->GetMainContext() )
		{
			uint32_t l_vertices = 0;
			uint32_t l_faces = 0;
			uint32_t l_objects = 0;
			m_debugOverlays->StartFrame();
			DoGpuStep( l_vertices, l_faces, l_objects );
			m_debugOverlays->EndGpuTask();
			DoCpuStep();
			m_debugOverlays->EndCpuTask();
			m_debugOverlays->EndFrame( l_vertices, l_faces, l_objects );
		}
	}

	ContextSPtr RenderLoop::DoCreateContext( RenderWindow & p_window )
	{
		ContextSPtr l_context = m_renderSystem->CreateContext();

		if ( l_context && l_context->Initialise( &p_window ) )
		{
			p_window.SetContext( l_context );
		}
		else
		{
			l_context.reset();
		}

		return l_context;
	}
}
