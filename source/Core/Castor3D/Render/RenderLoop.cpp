#include "Castor3D/Render/RenderLoop.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>

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
			auto guard = makeBlockGuard(
				[this]()
				{
					m_renderSystem.setCurrentRenderDevice( m_renderSystem.getMainRenderDevice().get() );
				},
				[this]()
				{
					m_renderSystem.setCurrentRenderDevice( nullptr );
				} );
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & listener )
				{
					listener.fireEvents( EventType::ePreRender );
				} );
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & listener )
				{
					listener.fireEvents( EventType::eQueueRender );
				} );
		}
		else
		{
			getEngine()->getFrameListenerCache().forEach( []( FrameListener & listener )
				{
					listener.flushEvents( EventType::ePreRender );
					listener.flushEvents( EventType::eQueueRender );
				} );
		}

		getEngine()->getFrameListenerCache().forEach( []( FrameListener & listener )
			{
				listener.fireEvents( EventType::ePostRender );
			} );
	}

	void RenderLoop::createDevice( ashes::WindowHandle handle
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

	bool RenderLoop::hasDebugOverlays()const
	{
		return m_debugOverlays->isShown();
	}

	RenderDeviceSPtr RenderLoop::doCreateDevice( ashes::WindowHandle handle
		, RenderWindow & window )
	{
		RenderDeviceSPtr result;

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
			m_lastFrameTime = m_debugOverlays->endFrame();
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
					m_renderSystem.setCurrentRenderDevice( m_renderSystem.getMainRenderDevice().get() );
				},
				[this]()
				{
					m_renderSystem.setCurrentRenderDevice( nullptr );
				} );
			doProcessEvents( EventType::ePreRender );
			getEngine()->getSceneCache().forEach( []( Scene & scene )
				{
					scene.getGeometryCache().uploadUbos();
					scene.getBillboardPools().uploadUbos();
					scene.getAnimatedObjectGroupCache().uploadUbos();
				} );
			getEngine()->getMaterialCache().update();
			getEngine()->getRenderTargetCache().render( info );
			doProcessEvents( EventType::eQueueRender );
		}

		getEngine()->getRenderWindowCache().forEach( []( RenderWindow & window )
			{
				window.render( true );
			} );

		{
			auto guard = makeBlockGuard(
				[this]()
				{
					m_renderSystem.setCurrentRenderDevice( m_renderSystem.getMainRenderDevice().get() );
				},
				[this]()
				{
					m_renderSystem.setCurrentRenderDevice( nullptr );
				} );
			m_debugOverlays->endGpuTask();
		}
	}

	void RenderLoop::doCpuStep()
	{
		doProcessEvents( EventType::ePostRender );
		getEngine()->getSceneCache().forEach( []( Scene & scene )
			{
				scene.update();
			} );
		std::vector< TechniqueQueues > techniquesQueues;
		getEngine()->getRenderTechniqueCache().forEach( [&techniquesQueues]( RenderTechnique & technique )
			{
				TechniqueQueues techniqueQueues;
				technique.update( techniqueQueues.queues );
				techniqueQueues.shadowMaps = technique.getShadowMaps();
				techniquesQueues.push_back( techniqueQueues );
			} );
		doUpdateQueues( techniquesQueues );
		m_debugOverlays->endCpuTask();
	}

	void RenderLoop::doUpdateQueues( std::vector< TechniqueQueues > & techniquesQueues )
	{
		for ( auto & techniqueQueues : techniquesQueues )
		{
			if ( techniqueQueues.queues.size() > m_queueUpdater.getCount() )
			{
				for ( auto & queue : techniqueQueues.queues )
				{
					m_queueUpdater.pushJob( [&queue, &techniqueQueues]()
						{
							queue.get().update( techniqueQueues.shadowMaps );
						} );
				}

				m_queueUpdater.waitAll( Milliseconds::max() );
			}
			else
			{
				for ( auto & queue : techniqueQueues.queues )
				{
					queue.get().update( techniqueQueues.shadowMaps );
				}
			}
		}
	}
}
