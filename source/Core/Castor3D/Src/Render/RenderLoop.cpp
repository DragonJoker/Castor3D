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
	RenderLoop::RenderLoop( Engine & engine
		, uint32_t wantedFPS
		, bool isAsync )
		: OwnedBy< Engine >( engine )
		, m_wantedFPS{ wantedFPS }
		, m_frameTime{ 1000 / wantedFPS }
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_debugOverlays{ std::make_unique< DebugOverlays >( engine ) }
		, m_queueUpdater{ std::max( 2u, engine.getCpuInformations().getCoreCount() - ( isAsync ? 2u : 1u ) ) }
	{
		m_debugOverlays->initialise( getEngine()->getOverlayCache() );
	}

	RenderLoop::~RenderLoop()
	{
		m_debugOverlays.reset();
	}

	void RenderLoop::cleanup()
	{
		if ( m_renderSystem.hasMainDevice() )
		{
			m_renderSystem.getMainDevice().enable();
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
			{
				p_listener.fireEvents( EventType::ePreRender );
			} );
			getEngine()->getOverlayCache().updateRenderer();
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
			{
				p_listener.fireEvents( EventType::eQueueRender );
			} );
			m_renderSystem.getMainDevice().disable();
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

	void RenderLoop::createDevice( renderer::WindowHandle && handle
		, RenderWindow & window )
	{
		if ( !m_renderSystem.hasMainDevice() )
		{
			doCreateMainDevice( std::move( handle ), window );
		}
		else
		{
			doCreateDevice( std::move( handle ), window );
		}
	}

	void RenderLoop::showDebugOverlays( bool p_show )
	{
		m_debugOverlays->show( p_show );
	}

	void RenderLoop::enableVSync( bool p_enable )
	{
	}

	void RenderLoop::flushEvents()
	{
		getEngine()->getFrameListenerCache().forEach( []( FrameListener & listener )
		{
			listener.flushEvents( EventType::ePreRender );
			listener.flushEvents( EventType::eQueueRender );
			listener.flushEvents( EventType::ePostRender );
		} );
	}

	void RenderLoop::registerTimer( RenderPassTimer & timer )
	{
		m_debugOverlays->registerTimer( timer );
	}

	void RenderLoop::unregisterTimer( RenderPassTimer & timer )
	{
		m_debugOverlays->unregisterTimer( timer );
	}

	renderer::Device const * RenderLoop::doCreateDevice( renderer::WindowHandle && handle
		, RenderWindow & window )
	{
		try
		{
			auto device = m_renderSystem.createDevice( std::move( handle ) );

			if ( device )
			{
				window.setDevice( std::move( device ) );
			}

			return &window.getDevice();
		}
		catch ( castor::Exception & p_exc )
		{
			Logger::logError( cuT( "createContext - " ) + p_exc.getFullDescription() );
		}
		catch ( std::exception & p_exc )
		{
			Logger::logError( std::string( "createContext - " ) + p_exc.what() );
		}

		return nullptr;
	}

	void RenderLoop::doRenderFrame()
	{
		if ( m_renderSystem.hasMainDevice() )
		{
			RenderInfo & info = m_debugOverlays->beginFrame();
			doGpuStep( info );
			doCpuStep();
			m_debugOverlays->endFrame();
		}
	}

	void RenderLoop::doProcessEvents( EventType eventType )
	{
		getEngine()->getFrameListenerCache().forEach( [eventType]( FrameListener & listener )
		{
			listener.fireEvents( eventType );
		} );
	}

	void RenderLoop::doGpuStep( RenderInfo & info )
	{
		{
			auto guard = makeBlockGuard(
				[this]()
				{
					m_renderSystem.getMainDevice().enable();
				},
				[this]()
				{
					m_renderSystem.getMainDevice().disable();
				} );
			doProcessEvents( EventType::ePreRender );
			getEngine()->getMaterialCache().update();
			getEngine()->getOverlayCache().updateRenderer();
			getEngine()->getRenderTargetCache().render( info );
			doProcessEvents( EventType::eQueueRender );
		}

		getEngine()->getSceneCache().forEach( []( Scene & scene )
		{
			scene.getEngine()->getRenderWindowCache().forEach( []( RenderWindow & window )
			{
				window.render( true );
			} );
		} );

		m_debugOverlays->endGpuTask();
	}

	void RenderLoop::doCpuStep()
	{
		doProcessEvents( EventType::ePostRender );
		getEngine()->getSceneCache().forEach( []( Scene & scene )
		{
			scene.update();
		} );
		RenderQueueArray queues;
		getEngine()->getRenderTechniqueCache().forEach( [&queues]( RenderTechnique & technique )
		{
			technique.update( queues );
		} );
		doUpdateQueues( queues );
		getEngine()->getOverlayCache().update();
		m_debugOverlays->endCpuTask();
	}

	void RenderLoop::doUpdateQueues( RenderQueueArray & queues )
	{
		if ( queues.size() > m_queueUpdater.getCount() )
		{
			for ( auto & queue : queues )
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
			for ( auto & queue : queues )
			{
				queue.get().update();
			}
		}
	}
}
