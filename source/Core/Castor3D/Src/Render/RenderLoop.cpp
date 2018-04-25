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
			m_renderSystem.getMainDevice()->enable();
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
			{
				p_listener.fireEvents( EventType::ePreRender );
			} );
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & p_listener )
			{
				p_listener.fireEvents( EventType::eQueueRender );
			} );
			m_renderSystem.getMainDevice()->disable();
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

	uint32_t RenderLoop::registerTimer( RenderPassTimer & timer )
	{
		return m_debugOverlays->registerTimer( timer );
	}

	void RenderLoop::unregisterTimer( RenderPassTimer & timer )
	{
		m_debugOverlays->unregisterTimer( timer );
	}

	renderer::DevicePtr RenderLoop::doCreateDevice( renderer::WindowHandle && handle
		, RenderWindow & window )
	{
		renderer::DevicePtr result;

		try
		{
			result = m_renderSystem.createDevice( std::move( handle ) );

			if ( result )
			{
				window.setDevice( result );
			}
		}
		catch ( castor::Exception & exc )
		{
			Logger::logError( cuT( "createContext - " ) + exc.getFullDescription() );
		}
		catch ( std::exception & exc )
		{
			Logger::logError( std::string( "createContext - " ) + exc.what() );
		}

		return result;
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
					m_renderSystem.getMainDevice()->enable();
				},
				[this]()
				{
					m_renderSystem.getMainDevice()->disable();
				} );
			doProcessEvents( EventType::ePreRender );
			getEngine()->getSceneCache().forEach( []( Scene & scene )
			{
				scene.getGeometryCache().uploadUbos();
				scene.getBillboardListCache().uploadUbos();
				scene.getAnimatedObjectGroupCache().uploadUbos();
			} );
			getEngine()->getMaterialCache().update();
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
