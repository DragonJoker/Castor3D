#include "RenderLoop.hpp"

#include "Engine.hpp"

#include "Overlay/DebugOverlays.hpp"
#include "Render/RenderWindow.hpp"
#include "Technique/RenderTechnique.hpp"

#include <Design/BlockGuard.hpp>

#include <future>

using namespace castor;

namespace castor3d
{
	RenderLoop::RenderLoop( Engine & engine, uint32_t p_wantedFPS, bool p_isAsync )
		: OwnedBy< Engine >( engine )
		, m_wantedFPS{ p_wantedFPS }
		, m_frameTime{ 1000 / p_wantedFPS }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_debugOverlays{ std::make_unique< DebugOverlays >( engine ) }
		, m_queueUpdater{ std::max( 2u, engine.getCpuInformations().getCoreCount() - ( p_isAsync ? 2u : 1u ) ) }
	{
		m_debugOverlays->initialise( getEngine()->getOverlayCache() );
	}

	RenderLoop::~RenderLoop()
	{
		m_debugOverlays.reset();
	}

	void RenderLoop::cleanup()
	{
		if ( m_renderSystem.getMainContext() )
		{
			m_renderSystem.getMainContext()->setCurrent();
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
			{
				p_listener.fireEvents( EventType::ePreRender );
			} );
			getEngine()->getOverlayCache().updateRenderer();
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
			{
				p_listener.fireEvents( EventType::eQueueRender );
			} );
			m_renderSystem.getMainContext()->endCurrent();
		}
		else
		{
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
			{
				p_listener.flushEvents( EventType::ePreRender );
				p_listener.flushEvents( EventType::eQueueRender );
			} );
		}

		getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
		{
			p_listener.fireEvents( EventType::ePostRender );
		} );
	}

	void RenderLoop::createContext( RenderWindow & p_window )
	{
		if ( !m_renderSystem.getMainContext() )
		{
			doCreateMainContext( p_window );
		}
		else
		{
			doCreateContext( p_window );
		}
	}

	void RenderLoop::showDebugOverlays( bool p_show )
	{
		m_debugOverlays->show( p_show );
	}

	void RenderLoop::updateVSync( bool p_enable )
	{
	}

	void RenderLoop::flushEvents()
	{
		getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
		{
			p_listener.flushEvents( EventType::ePreRender );
			p_listener.flushEvents( EventType::eQueueRender );
			p_listener.flushEvents( EventType::ePostRender );
		} );
	}

	void RenderLoop::registerTimer( RenderPassTimer const & timer )
	{
		m_debugOverlays->registerTimer( timer );
	}

	void RenderLoop::unregisterTimer( RenderPassTimer const & timer )
	{
		m_debugOverlays->unregisterTimer( timer );
	}

	ContextSPtr RenderLoop::doCreateContext( RenderWindow & p_window )
	{
		ContextSPtr context;

		try
		{
			context = m_renderSystem.createContext();

			if ( context && context->initialise( &p_window ) )
			{
				p_window.setContext( context );
			}
			else
			{
				context.reset();
			}
		}
		catch ( castor::Exception & p_exc )
		{
			Logger::logError( cuT( "createContext - " ) + p_exc.getFullDescription() );
			context.reset();
		}
		catch ( std::exception & p_exc )
		{
			Logger::logError( std::string( "createContext - " ) + p_exc.what() );
			context.reset();
		}

		return context;
	}

	void RenderLoop::doRenderFrame()
	{
		if ( m_renderSystem.getMainContext() )
		{
			RenderInfo info;
			m_debugOverlays->beginFrame();
			doGpuStep( info );
			doCpuStep();
			m_debugOverlays->endFrame( info );
		}
	}

	void RenderLoop::doProcessEvents( EventType p_eventType )
	{
		getEngine()->getFrameListenerCache().forEach( [p_eventType]( FrameListener & p_listener )
		{
			p_listener.fireEvents( p_eventType );
		} );
	}

	void RenderLoop::doGpuStep( RenderInfo & p_info )
	{
		{
			auto guard = makeBlockGuard(
				[this]()
				{
					m_renderSystem.getMainContext()->setCurrent();
				},
				[this]()
				{
					m_renderSystem.getMainContext()->endCurrent();
				} );
			doProcessEvents( EventType::ePreRender );
			getEngine()->getMaterialCache().update();
			getEngine()->getOverlayCache().updateRenderer();
			getEngine()->getRenderTargetCache().render( p_info );
			doProcessEvents( EventType::eQueueRender );
		}

		getEngine()->getSceneCache().forEach( []( Scene & p_scene )
		{
			p_scene.getEngine()->getRenderWindowCache().forEach( []( RenderWindow & p_window )
			{
				p_window.render( true );
			} );
		} );

		m_debugOverlays->endGpuTask();
	}

	void RenderLoop::doCpuStep()
	{
		doProcessEvents( EventType::ePostRender );
		getEngine()->getSceneCache().forEach( []( Scene & p_scene )
		{
			p_scene.update();
		} );
		RenderQueueArray queues;
		getEngine()->getRenderTechniqueCache().forEach( [&queues]( RenderTechnique & p_technique )
		{
			p_technique.update( queues );
		} );
		doUpdateQueues( queues );
		getEngine()->getOverlayCache().update();
		m_debugOverlays->endCpuTask();
	}

	void RenderLoop::doUpdateQueues( RenderQueueArray & p_queues )
	{
		if ( p_queues.size() > m_queueUpdater.getCount() )
		{
			for ( auto & queue : p_queues )
			{
				m_queueUpdater.pushJob( [&queue]()
				{
					queue.get().update();
				} );
			}

			m_queueUpdater.waitAll( Milliseconds::max() );
		}
		else
		{
			for ( auto & queue : p_queues )
			{
				queue.get().update();
			}
		}
	}
}
