#include "Castor3D/Render/RenderLoop.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/ListenerCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/SceneCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/TechniqueCache.hpp"
#include "Castor3D/Cache/WindowCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Overlay/DebugOverlays.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderLoop )

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
		, m_uploadResources
		{
			UploadResources{ { nullptr, nullptr }, nullptr },
			UploadResources{ { nullptr, nullptr }, nullptr },
		}
	{
		m_debugOverlays->initialise( getEngine()->getOverlayCache() );
	}

	RenderLoop::~RenderLoop()
	{
		m_debugOverlays->cleanup();
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
			auto & device = m_renderSystem.getCurrentRenderDevice();
			getEngine()->getFrameListenerCache().forEach( [&device]( FrameListener & listener )
				{
					listener.fireEvents( EventType::ePreRender, device );
					listener.fireEvents( EventType::ePreRender );
				} );
			getEngine()->getFrameListenerCache().forEach( [&device]( FrameListener & listener )
				{
					listener.fireEvents( EventType::eQueueRender, device );
					listener.fireEvents( EventType::eQueueRender );
				} );
			m_uploadResources =
			{
				UploadResources{ { nullptr, nullptr }, nullptr },
				UploadResources{ { nullptr, nullptr }, nullptr },
			};
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

	void RenderLoop::createDevice( RenderWindow & window )
	{
		if ( !m_renderSystem.hasMainDevice() )
		{
			doCreateMainDevice( window );
		}

		window.setDevice( m_renderSystem.getMainRenderDevice() );
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

	RenderDeviceSPtr RenderLoop::doCreateDevice( RenderWindow const & window )
	{
		RenderDeviceSPtr result;

		try
		{
			result = m_renderSystem.createDevice( window.getSurface() );
		}
		catch ( castor::Exception & exc )
		{
			log::error << cuT( "createContext - " ) << exc.getFullDescription() << std::endl;
		}
		catch ( std::exception & exc )
		{
			log::error << std::string( "createContext - " ) << exc.what() << std::endl;
		}

		return result;
	}

	void RenderLoop::doRenderFrame( castor::Milliseconds tslf )
	{
		if ( m_renderSystem.hasMainDevice() )
		{
			bool first = m_first;
			RenderInfo & info = m_debugOverlays->beginFrame();
			doGpuStep( info );
			doCpuStep( tslf );
			m_lastFrameTime = m_debugOverlays->endFrame();

			if ( first )
			{
				log::info << "Initialisation time: " << ( m_lastFrameTime.load().count() / 1000.0f ) << " ms." << std::endl;
			}
		}
	}

	void RenderLoop::doProcessEvents( EventType eventType )
	{
		getEngine()->getFrameListenerCache().forEach( [eventType]( FrameListener & listener )
			{
				listener.fireEvents( eventType );
			} );
	}

	void RenderLoop::doProcessEvents( EventType eventType
		, RenderDevice const & device )
	{
		getEngine()->getFrameListenerCache().forEach( [eventType, &device]( FrameListener & listener )
			{
				listener.fireEvents( eventType, device );
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
			auto & device = m_renderSystem.getCurrentRenderDevice();

			if ( !m_uploadResources[0].fence )
			{
				for ( auto & resources : m_uploadResources )
				{
					resources.commands =
					{
						device.graphicsCommandPool->createCommandBuffer( "RenderLoopUboUpload" ),
						device->createSemaphore( "RenderLoopUboUpload" ),
					};
					resources.fence = device->createFence();
				}
			}

			// Usually GPU initialisation
			doProcessEvents( EventType::ePreRender, device );
			doProcessEvents( EventType::ePreRender );

			// GPU Update
			GpuUpdater updater{ device, info };
			getEngine()->getMaterialCache().update( updater );
			getEngine()->getRenderTargetCache().update( updater );

			auto & uploadResources = m_uploadResources[m_currentUpdate];
			uploadResources.commands.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
			device.uboPools->upload( *uploadResources.commands.commandBuffer );
			uploadResources.commands.commandBuffer->end();
			device.graphicsQueue->submit( { *uploadResources.commands.commandBuffer }
				, {}
				, {}
				, {}
				, uploadResources.fence.get() );
			uploadResources.fence->wait( ashes::MaxTimeout );
			uploadResources.fence->reset();

			// Render
			getEngine()->getRenderTargetCache().render( device, info );

			// Usually GPU cleanup
			doProcessEvents( EventType::eQueueRender, device );
			doProcessEvents( EventType::eQueueRender );
		}

		for ( auto & window : getEngine()->getRenderWindows() )
		{
			window.second->render( m_first );
		}

		m_first = false;

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

	void RenderLoop::doCpuStep( castor::Milliseconds tslf )
	{
		doProcessEvents( EventType::ePostRender );
		CpuUpdater updater;
		updater.tslf = tslf;
		getEngine()->getMaterialCache().update( updater );
		getEngine()->getSceneCache().forEach( [&updater]( Scene & scene )
			{
				scene.update( updater );
			} );
		getEngine()->getRenderTargetCache().update( updater );
		std::vector< TechniqueQueues > techniquesQueues;
		getEngine()->getRenderTechniqueCache().forEach( [&updater, &techniquesQueues]( RenderTechnique & technique )
			{
				TechniqueQueues techniqueQueues;
				updater.queues = &techniqueQueues.queues;
				technique.update( updater );
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
