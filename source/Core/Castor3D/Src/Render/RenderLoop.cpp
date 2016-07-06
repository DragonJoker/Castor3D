#include "RenderLoop.hpp"

#include "BlendStateCache.hpp"
#include "Engine.hpp"
#include "ListenerCache.hpp"
#include "OverlayCache.hpp"
#include "SamplerCache.hpp"
#include "SceneCache.hpp"
#include "RenderTargetCache.hpp"
#include "TechniqueCache.hpp"
#include "RenderWindowCache.hpp"

#include "Pipeline.hpp"
#include "RenderSystem.hpp"

#include "Overlay/DebugOverlays.hpp"

using namespace Castor;

namespace Castor3D
{
	static const char * C3D_MAIN_LOOP_EXISTS = "Render loop is already started";
	static const char * C3D_UNKNOWN_EXCEPTION = "Unknown exception";

	RenderLoop::RenderLoop( Engine & p_engine, RenderSystem * p_renderSystem, uint32_t p_wantedFPS )
		: OwnedBy< Engine >( p_engine )
		, m_wantedFPS( p_wantedFPS )
		, m_frameTime( 1000 / p_wantedFPS )
		, m_renderSystem( p_renderSystem )
		, m_debugOverlays( std::make_unique< DebugOverlays >( p_engine ) )
	{
		m_debugOverlays->Initialise( GetEngine()->GetOverlayCache() );
	}

	RenderLoop::~RenderLoop()
	{
		m_debugOverlays.reset();
	}

	void RenderLoop::Cleanup()
	{
		if (m_renderSystem->GetMainContext ())
		{
			m_renderSystem->GetMainContext()->SetCurrent ();
			GetEngine()->GetListenerCache().ForEach( []( FrameListener & p_listener )
			{
				p_listener.FireEvents( eEVENT_TYPE_PRE_RENDER );
			} );
			GetEngine()->GetOverlayCache().UpdateRenderer();
			m_renderSystem->GetMainContext()->EndCurrent();
		}

		GetEngine()->GetListenerCache().ForEach( []( FrameListener & p_listener )
		{
			p_listener.FireEvents( eEVENT_TYPE_POST_RENDER );
		} );
	}

	void RenderLoop::StartRendering()
	{
		DoStartRendering();
	}

	void RenderLoop::Pause()
	{
		DoPause();
	}

	void RenderLoop::Resume()
	{
		DoResume();
	}

	void RenderLoop::RenderSyncFrame()
	{
		DoRenderSyncFrame();
	}

	void RenderLoop::EndRendering()
	{
		DoEndRendering();
	}

	uint32_t RenderLoop::GetFrameTime()
	{
		return m_frameTime;
	}

	uint32_t RenderLoop::GetWantedFps()
	{
		return m_wantedFPS;
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

	void RenderLoop::UpdateVSync( bool p_enable )
	{
	}

	void RenderLoop::DoCpuUpdate()
	{
		GetEngine()->GetRenderTechniqueCache().ForEach( []( RenderTechnique & p_technique )
		{
			p_technique.Update();
		} );
		GetEngine()->GetSceneCache().ForEach( []( Scene & p_scene )
		{
			p_scene.Update();
		} );
		GetEngine()->GetOverlayCache().Update();
		m_debugOverlays->EndCpuTask();
	}

	void RenderLoop::DoGpuStep( uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount )
	{
		m_renderSystem->GetMainContext()->SetCurrent();

		try
		{
			GetEngine()->GetListenerCache().ForEach( []( FrameListener & p_listener )
			{
				p_listener.FireEvents( eEVENT_TYPE_PRE_RENDER );
			} );

			GetEngine()->GetOverlayCache().UpdateRenderer();
			GetEngine()->GetTargetCache().Render( m_frameTime, p_vtxCount, p_fceCount, p_objCount );
		}
		catch ( Exception & p_exc )
		{
			Logger::LogError( p_exc.GetFullDescription() );
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( p_exc.what() );
		}
		catch ( ... )
		{
			Logger::LogError( C3D_UNKNOWN_EXCEPTION );
		}

		m_renderSystem->GetMainContext()->EndCurrent();
		{
			GetEngine()->GetSceneCache().ForEach( []( Scene & p_scene )
			{
				p_scene.GetWindowCache().ForEach( []( RenderWindow & p_window )
				{
					p_window.Render( true );
				} );
			} );
		}
		m_debugOverlays->EndGpuTask();
	}

	void RenderLoop::DoCpuStep()
	{
		GetEngine()->GetListenerCache().ForEach( []( FrameListener & p_listener )
		{
			p_listener.FireEvents( eEVENT_TYPE_POST_RENDER );
		} );

		m_debugOverlays->EndCpuTask();
	}

	void RenderLoop::DoRenderFrame()
	{
		if ( m_renderSystem->GetMainContext() )
		{
			uint32_t l_vertices = 0;
			uint32_t l_faces = 0;
			uint32_t l_objects = 0;
			m_debugOverlays->StartFrame();
			DoCpuUpdate();
			DoGpuStep( l_vertices, l_faces, l_objects );
			DoCpuStep();
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
