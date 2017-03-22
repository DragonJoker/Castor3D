#include "RenderLoop.hpp"

#include "Engine.hpp"

#include "Overlay/DebugOverlays.hpp"
#include "Render/RenderWindow.hpp"

#include <Design/BlockGuard.hpp>

#include <future>

using namespace Castor;

namespace Castor3D
{
	RenderLoop::RenderLoop( Engine & p_engine, uint32_t p_wantedFPS, bool p_isAsync )
		: OwnedBy< Engine >( p_engine )
		, m_wantedFPS{ p_wantedFPS }
		, m_frameTime{ 1000 / p_wantedFPS }
		, m_renderSystem{ *p_engine.GetRenderSystem() }
		, m_debugOverlays{ std::make_unique< DebugOverlays >( p_engine ) }
		, m_queueUpdater{ std::max( 2u, p_engine.GetCpuInformations().CoreCount() - ( p_isAsync ? 2u : 1u ) ) }
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
				p_listener.FireEvents( EventType::ePreRender );
			} );
			GetEngine()->GetOverlayCache().UpdateRenderer();
			GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
			{
				p_listener.FireEvents( EventType::eQueueRender );
			} );
			m_renderSystem.GetMainContext()->EndCurrent();
		}
		else
		{
			GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
			{
				p_listener.FlushEvents( EventType::ePreRender );
			} );
			GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
			{
				p_listener.FlushEvents( EventType::eQueueRender );
			} );
		}

		GetEngine()->GetFrameListenerCache().ForEach( []( FrameListener & p_listener )
		{
			p_listener.FireEvents( EventType::ePostRender );
		} );
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

	void RenderLoop::DoRenderFrame()
	{
		if ( m_renderSystem.GetMainContext() )
		{
			RenderInfo l_info;
			m_debugOverlays->StartFrame();
			DoGpuStep( l_info );
			DoCpuStep();
			m_debugOverlays->EndFrame( l_info );
		}
	}

	void RenderLoop::DoProcessEvents( EventType p_eventType )
	{
		GetEngine()->GetFrameListenerCache().ForEach( [p_eventType]( FrameListener & p_listener )
		{
			p_listener.FireEvents( p_eventType );
		} );
	}

	void RenderLoop::DoGpuStep( RenderInfo & p_info )
	{
		{
			auto l_guard = make_block_guard(
				[this]()
				{
					m_renderSystem.GetMainContext()->SetCurrent();
				},
				[this]()
				{
					m_renderSystem.GetMainContext()->EndCurrent();
				} );
			DoProcessEvents( EventType::ePreRender );
			GetEngine()->GetOverlayCache().UpdateRenderer();
			GetEngine()->GetRenderTargetCache().Render( p_info );
			DoProcessEvents( EventType::eQueueRender );
		}

		GetEngine()->GetSceneCache().ForEach( []( Scene & p_scene )
		{
			p_scene.GetRenderWindowCache().ForEach( []( RenderWindow & p_window )
			{
				p_window.Render( true );
			} );
		} );

		m_debugOverlays->EndGpuTask();
	}

	void RenderLoop::DoCpuStep()
	{
		DoProcessEvents( EventType::ePostRender );
		GetEngine()->GetSceneCache().ForEach( []( Scene & p_scene )
		{
			p_scene.Update();
		} );
		RenderQueueArray l_queues;
		GetEngine()->GetRenderTechniqueCache().ForEach( [&l_queues]( RenderTechnique & p_technique )
		{
			p_technique.Update( l_queues );
		} );
		DoUpdateQueues( l_queues );
		GetEngine()->GetOverlayCache().Update();
		m_debugOverlays->EndCpuTask();
	}

	void RenderLoop::DoUpdateQueues( RenderQueueArray & p_queues )
	{
		if ( p_queues.size() > m_queueUpdater.GetCount() )
		{
			for ( auto & l_queue : p_queues )
			{
				m_queueUpdater.PushJob( [&l_queue]()
				{
					l_queue.get().Update();
				} );
			}

			m_queueUpdater.WaitAll( std::chrono::milliseconds::max() );
		}
		else
		{
			for ( auto & l_queue : p_queues )
			{
				l_queue.get().Update();
			}
		}
	}
}
