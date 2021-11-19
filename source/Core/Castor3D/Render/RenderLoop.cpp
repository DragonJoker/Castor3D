#include "Castor3D/Render/RenderLoop.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Overlay/DebugOverlays.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Technique/RenderTechnique.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementCUSmartPtr( castor3d, RenderLoop )

using namespace castor;

namespace castor3d
{
	RenderLoop::RenderLoop( Engine & engine
		, uint32_t wantedFPS
		, bool isAsync )
		: OwnedBy< Engine >( engine )
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_wantedFPS{ wantedFPS }
		, m_frameTime{ 1000ull / wantedFPS }
		, m_debugOverlays{ std::make_unique< DebugOverlays >( engine ) }
		, m_uploadResources
		{
			UploadResources{ { nullptr, nullptr }, nullptr },
			UploadResources{ { nullptr, nullptr }, nullptr },
		}
	{
		auto lock( makeUniqueLock( m_debugOverlaysMtx ) );
		m_debugOverlays->initialise( getEngine()->getOverlayCache() );
	}

	RenderLoop::~RenderLoop()
	{
		auto lock( makeUniqueLock( m_debugOverlaysMtx ) );
		m_debugOverlays->cleanup();
		m_debugOverlays.reset();
	}

	void RenderLoop::cleanup()
	{
		if ( m_queueData )
		{
			auto & device = m_renderSystem.getRenderDevice();
			auto data = m_queueData;
			m_queueData = nullptr;
			getEngine()->getFrameListenerCache().forEach( [&device, data]( FrameListener & listener )
				{
					listener.fireEvents( EventType::ePreRender, device, *data );
					listener.fireEvents( EventType::ePreRender );
				} );
			getEngine()->getFrameListenerCache().forEach( [&device, data]( FrameListener & listener )
				{
					listener.fireEvents( EventType::eQueueRender, device, *data );
					listener.fireEvents( EventType::eQueueRender );
				} );
			for ( auto & uploadResources : m_uploadResources )
			{
				uploadResources.fence->wait( ashes::MaxTimeout );
				uploadResources.commands = {};
				uploadResources.fence.reset();
				uploadResources.used = false;
			}
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

	uint32_t RenderLoop::registerTimer( castor::String const & category
		, crg::FramePassTimer & timer )
	{
		auto lock( makeUniqueLock( m_debugOverlaysMtx ) );
		return m_debugOverlays->registerTimer( category, timer );
	}

	void RenderLoop::unregisterTimer( castor::String const & category
		, crg::FramePassTimer & timer )
	{
		auto lock( makeUniqueLock( m_debugOverlaysMtx ) );
		m_debugOverlays->unregisterTimer( category, timer );
	}

	bool RenderLoop::hasDebugOverlays()const
	{
		return m_debugOverlays->isShown();
	}

	castor::Nanoseconds RenderLoop::getAvgFrameTime()const
	{
		return m_debugOverlays->getAvgFrameTime();
	}

	void RenderLoop::doRenderFrame( castor::Milliseconds tslf )
	{
		if ( m_renderSystem.hasDevice() )
		{
			bool first = m_first;
			RenderInfo & info = m_debugOverlays->beginFrame();
			doGpuStep( info );
			doCpuStep( tslf );
			m_lastFrameTime = m_debugOverlays->endFrame( first );

			if ( first )
			{
				log::info << "Initialisation time: " << ( float( m_lastFrameTime.load().count() ) / 1000.0f ) << " ms." << std::endl;
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
		, RenderDevice const & device
		, QueueData const & queueData )
	{
		getEngine()->getFrameListenerCache().forEach( [eventType, &device, &queueData]( FrameListener & listener )
			{
				listener.fireEvents( eventType, device, queueData );
			} );
	}

	void RenderLoop::doGpuStep( RenderInfo & info )
	{
		auto & windows = getEngine()->getRenderWindows();
		crg::SemaphoreWaitArray toWait;
		auto & device = m_renderSystem.getRenderDevice();

		if ( !m_queueData )
		{
			// Run this initialisation here, to make sure we are in the render loop thread.
			m_queueData = device.reserveGraphicsData();

			for ( auto & resources : m_uploadResources )
			{
				resources.commands =
				{
					m_queueData->commandPool->createCommandBuffer( "RenderLoopUboUpload" ),
					device->createSemaphore( "RenderLoopUboUpload" ),
				};
				resources.fence = device->createFence( VK_FENCE_CREATE_SIGNALED_BIT );
			}
		}

		auto & uploadResources = m_uploadResources[m_currentUpdate];

		// Usually GPU initialisation
		doProcessEvents( EventType::ePreRender, device, *m_queueData );
		doProcessEvents( EventType::ePreRender );

		// GPU Update
		GpuUpdater updater{ device, info };
		getEngine()->update( updater );

		uploadResources.fence->wait( ashes::MaxTimeout );
		uploadResources.fence->reset();
		uploadResources.commands.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		device.uboPools->upload( *uploadResources.commands.commandBuffer );
		uploadResources.commands.commandBuffer->end();
		m_queueData->queue->submit( *uploadResources.commands.commandBuffer
			, ( uploadResources.used
				? VkSemaphore{ VK_NULL_HANDLE }
				: *uploadResources.commands.semaphore )
			, ( uploadResources.used
				? VkPipelineStageFlagBits{}
				: VK_PIPELINE_STAGE_TRANSFER_BIT )
			, *uploadResources.commands.semaphore
			, *uploadResources.fence );

		// Render
		toWait = getEngine()->getRenderTargetCache().render( device
			, info
			, *m_queueData->queue
			, { { *uploadResources.commands.semaphore
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } } );

		for ( auto & window : windows )
		{
			window.second->render( info
				, m_first
				, toWait );
		}

		m_first = false;
		uploadResources.used = toWait.empty();

		// Usually GPU cleanup
		doProcessEvents( EventType::eQueueRender, device, *m_queueData );
		doProcessEvents( EventType::eQueueRender );

		m_debugOverlays->endGpuTask();
	}

	void RenderLoop::doCpuStep( castor::Milliseconds tslf )
	{
		doProcessEvents( EventType::ePostRender );
		CpuUpdater updater;
		updater.tslf = tslf;
		getEngine()->update( updater );

		for ( auto & techniqueQueues : updater.techniquesQueues )
		{
			for ( auto & queue : techniqueQueues.queues )
			{
				queue.get().update( techniqueQueues.shadowMaps );
			}
		}

		m_debugOverlays->endCpuTask();
	}
}
