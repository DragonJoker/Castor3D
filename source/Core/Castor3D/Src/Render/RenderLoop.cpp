#include "RenderLoop.hpp"

#include "Engine.hpp"
#include "ListenerCache.hpp"
#include "OverlayCache.hpp"
#include "SamplerCache.hpp"
#include "SceneCache.hpp"
#include "TargetCache.hpp"
#include "TechniqueCache.hpp"
#include "WindowCache.hpp"

#include "Pipeline.hpp"
#include "RenderSystem.hpp"
#include "RenderWindow.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Overlay/DebugOverlays.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
{
	static const char * C3D_MAIN_LOOP_EXISTS = "Render loop is already started";
	static const char * C3D_UNKNOWN_EXCEPTION = "Unknown exception";

	RenderLoop::RenderLoop( Engine & p_engine, uint32_t p_wantedFPS )
		: OwnedBy< Engine >( p_engine )
		, m_wantedFPS( p_wantedFPS )
		, m_frameTime( 1000 / p_wantedFPS )
		, m_renderSystem( *p_engine.GetRenderSystem() )
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
		if ( m_renderSystem.GetMainContext() )
		{
			m_renderSystem.GetMainContext()->SetCurrent();
			GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
			{
				p_listener.FireEvents( EventType::PreRender );
			} );
			GetEngine()->GetOverlayCache().UpdateRenderer();
			m_renderSystem.GetMainContext()->EndCurrent();
		}

		GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
		{
			p_listener.FireEvents( EventType::PostRender );
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
		if ( !m_renderSystem.GetMainContext() )
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

	void RenderLoop::DoGpuStep( uint32_t & p_vtxCount, uint32_t & p_fceCount, uint32_t & p_objCount, uint32_t & p_visible, uint32_t & p_particles )
	{
		m_renderSystem.GetMainContext()->SetCurrent();

		try
		{
			GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
			{
				p_listener.FireEvents( EventType::PreRender );
			} );

			GetEngine()->GetOverlayCache().UpdateRenderer();
			GetEngine()->GetRenderTargetCache().Render( m_frameTime, p_vtxCount, p_fceCount, p_objCount, p_visible, p_particles );
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

		GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
		{
			p_listener.FireEvents( EventType::QueueRender );
		} );

		m_renderSystem.GetMainContext()->EndCurrent();
		{
			GetEngine()->GetSceneCache().ForEach( []( Scene & p_scene )
			{
				p_scene.GetRenderWindowCache().ForEach( []( RenderWindow & p_window )
				{
					p_window.Render( true );
				} );
			} );
		}
		m_debugOverlays->EndGpuTask();
	}

	void RenderLoop::DoCpuStep()
	{
		GetEngine()->GetSceneCache().ForEach( []( Scene & p_scene )
		{
			p_scene.Update();
		} );
		GetEngine()->GetRenderTechniqueCache().ForEach( []( RenderTechnique & p_technique )
		{
			p_technique.Update();
		} );
		GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
		{
			p_listener.FireEvents( EventType::PostRender );
		} );
		GetEngine()->GetOverlayCache().Update();
		m_debugOverlays->EndCpuTask();
	}

	void RenderLoop::DoRenderFrame()
	{
		if ( m_renderSystem.GetMainContext() )
		{
			uint32_t l_vertices = 0;
			uint32_t l_faces = 0;
			uint32_t l_objects = 0;
			uint32_t l_visible = 0;
			uint32_t l_particles = 0;
			m_debugOverlays->StartFrame();
			DoGpuStep( l_vertices, l_faces, l_objects, l_visible, l_particles );
			DoCpuStep();
			m_debugOverlays->EndFrame( l_vertices, l_faces, l_objects, l_visible, l_particles );
		}
	}

	ContextSPtr RenderLoop::DoCreateContext( RenderWindow & p_window )
	{
		ContextSPtr l_context;

		try
		{
			l_context = m_renderSystem.CreateContext();

			if ( l_context && l_context->Initialise( &p_window ) )
			{
				p_window.SetContext( l_context );
			}
			else
			{
				l_context.reset();
			}
		}
		catch ( Castor::Exception & p_exc )
		{
			Logger::LogError( cuT( "CreateContext - " ) + p_exc.GetFullDescription() );
			l_context.reset();
		}
		catch ( std::exception & p_exc )
		{
			Logger::LogError( std::string( "CreateContext - " ) + p_exc.what() );
			l_context.reset();
		}

		return l_context;
	}
}
