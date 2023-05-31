#include "Castor3D/Render/RenderLoop.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Overlay/DebugOverlays.hpp"
#include "Castor3D/Render/RenderQueue.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/RenderTechnique.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/BlockGuard.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>

CU_ImplementSmartPtr( castor3d, RenderLoop )

namespace castor3d
{
	RenderLoop::RenderLoop( Engine & engine
		, uint32_t wantedFPS
		, bool isAsync )
		: castor::OwnedBy< Engine >( engine )
		, m_renderSystem{ *engine.getRenderSystem() }
		, m_wantedFPS{ wantedFPS }
		, m_frameTime{ 1000ull / wantedFPS }
		, m_debugOverlays{ std::make_unique< DebugOverlays >( engine ) }
		, m_uploadResources{ UploadResources{ { nullptr, nullptr }, nullptr }
			, UploadResources{ { nullptr, nullptr }, nullptr } }
		, m_uploadTimer{ castor::makeUnique< crg::FramePassTimer >( m_renderSystem.getRenderDevice().makeContext(), "Upload" ) }
		, m_timerCpuEvents{ castor::makeUnique< crg::FramePassTimer >( m_renderSystem.getRenderDevice().makeContext(), "Events/CPU/PreRender" )
			, castor::makeUnique< crg::FramePassTimer >( m_renderSystem.getRenderDevice().makeContext(), "Events/CPU/QueueRender" )
			, castor::makeUnique< crg::FramePassTimer >( m_renderSystem.getRenderDevice().makeContext(), "Events/CPU/PostRender" ) }
		, m_timerGpuEvents{ castor::makeUnique< crg::FramePassTimer >( m_renderSystem.getRenderDevice().makeContext(), "Events/GPU/PreRender" )
			, castor::makeUnique< crg::FramePassTimer >( m_renderSystem.getRenderDevice().makeContext(), "Events/GPU/QueueRender" )
			, castor::makeUnique< crg::FramePassTimer >( m_renderSystem.getRenderDevice().makeContext(), "Events/GPU/PostRender" ) }
	{
		{
			auto lock( castor::makeUniqueLock( m_debugOverlaysMtx ) );
			m_debugOverlays->initialise( getEngine()->getOverlayCache() );
		}
	}

	RenderLoop::~RenderLoop()
	{
		m_timerCpuEvents[0].reset();
		m_timerCpuEvents[1].reset();
		m_timerCpuEvents[2].reset();

		m_timerGpuEvents[0].reset();
		m_timerGpuEvents[1].reset();
		m_timerGpuEvents[2].reset();

		auto lock( castor::makeUniqueLock( m_debugOverlaysMtx ) );
		m_debugOverlays->cleanup();
		m_debugOverlays.reset();
	}

	void RenderLoop::cleanup()
	{
		unregisterTimer( "Events/CPU/PreRender", *m_timerCpuEvents[0] );
		unregisterTimer( "Events/CPU/QueueRender", *m_timerCpuEvents[1] );
		unregisterTimer( "Events/CPU/PostRender", *m_timerCpuEvents[2] );

		unregisterTimer( "Events/GPU/PreRender", *m_timerGpuEvents[0] );
		unregisterTimer( "Events/GPU/QueueRender", *m_timerGpuEvents[1] );
		unregisterTimer( "Events/GPU/PostRender", *m_timerGpuEvents[2] );

		if ( m_uploadTimer )
		{
			auto & device = m_renderSystem.getRenderDevice();
			auto data = m_reservedQueue;

			if ( !data )
			{
				data = device.graphicsData().release();
			}

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
				uploadResources.waitCount = 1u;
			}

			m_uploadTimer = nullptr;

			if ( m_reservedQueue )
			{
				device.unreserveGraphicsData( m_reservedQueue );
				m_reservedQueue = nullptr;
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

	void RenderLoop::showDebugOverlays( bool show )
	{
		m_debugOverlays->show( show );
	}

	void RenderLoop::enableVSync( bool enable )
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

	void RenderLoop::registerTimer( castor::String const & category
		, crg::FramePassTimer & timer )
	{
		auto lock( castor::makeUniqueLock( m_debugOverlaysMtx ) );
		m_debugOverlays->registerTimer( category, timer );
	}

	void RenderLoop::unregisterTimer( castor::String const & category
		, crg::FramePassTimer & timer )
	{
		auto lock( castor::makeUniqueLock( m_debugOverlaysMtx ) );
		m_debugOverlays->unregisterTimer( category, timer );
	}

	void RenderLoop::registerBuffer( ShaderBuffer const & buffer )
	{
		auto lock( castor::makeUniqueLock( m_shaderBuffersMtx ) );
		m_shaderBuffers.insert( &buffer );
	}

	void RenderLoop::unregisterBuffer( ShaderBuffer const & buffer )
	{
		auto lock( castor::makeUniqueLock( m_shaderBuffersMtx ) );
		m_shaderBuffers.erase( &buffer );
	}

	bool RenderLoop::hasDebugOverlays()const
	{
		return m_debugOverlays->isShown();
	}

	void RenderLoop::dumpFrameTimes( Parameters & params )const
	{
		params.add( "Last", std::chrono::duration_cast< castor::Nanoseconds >( getLastFrameTime() ) );
		m_debugOverlays->dumpFrameTimes( params );
	}

	castor::Nanoseconds RenderLoop::getAvgFrameTime()const
	{
		return m_debugOverlays->getAvgFrameTime();
	}

	void RenderLoop::doRenderFrame( castor::Milliseconds tslf )
	{
		if ( m_renderSystem.hasDevice() )
		{
			bool first = m_ignored == 5;
			RenderInfo & info = m_debugOverlays->beginFrame();
			doProcessEvents( EventType::ePreRender );
			doGpuStep( info );
			doProcessEvents( EventType::eQueueRender );
			doCpuStep( tslf );
			doProcessEvents( EventType::ePostRender );
			m_lastFrameTime = m_debugOverlays->endFrame( m_ignored > 0 );

			if ( first )
			{
				log::info << "Initialisation time: " << ( float( m_lastFrameTime.load().count() ) / 1000.0f ) << " ms." << std::endl;
			}

			m_ignored = std::max( m_ignored - 1, 0 );
		}
	}

	void RenderLoop::doProcessEvents( EventType eventType )
	{
		auto block = m_timerCpuEvents[size_t( eventType )]->start();
		getEngine()->getFrameListenerCache().forEach( [eventType]( FrameListener & listener )
			{
				listener.fireEvents( eventType );
			} );
	}

	void RenderLoop::doProcessEvents( EventType eventType
		, RenderDevice const & device
		, QueueData const & queueData )
	{
		auto block = m_timerGpuEvents[size_t( eventType )]->start();
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
		auto data = m_reservedQueue;

		if ( m_ignored == 5 )
		{
			// Run this initialisation here, to make sure we are in the render loop thread.
			m_reservedQueue = device.graphicsQueueSize() > 1
				? device.reserveGraphicsData()
				: nullptr;

			if ( !m_reservedQueue )
			{
				data = device.graphicsData().release();
			}
			else
			{
				data = m_reservedQueue;
			}

			for ( auto & resources : m_uploadResources )
			{
				resources.commands =
				{
					data->commandPool->createCommandBuffer( "RenderLoopUboUpload" ),
					device->createSemaphore( "RenderLoopUboUpload" ),
				};
				resources.fence = device->createFence( VK_FENCE_CREATE_SIGNALED_BIT );
			}

			registerTimer( "Upload", *m_uploadTimer );

			registerTimer( "Events/CPU/PreRender", *m_timerCpuEvents[0] );
			registerTimer( "Events/CPU/QueueRender", *m_timerCpuEvents[1] );
			registerTimer( "Events/CPU/PostRender", *m_timerCpuEvents[2] );

			registerTimer( "Events/GPU/PreRender", *m_timerGpuEvents[0] );
			registerTimer( "Events/GPU/QueueRender", *m_timerGpuEvents[1] );
			registerTimer( "Events/GPU/PostRender", *m_timerGpuEvents[2] );
		}

		if ( !data )
		{
			data = device.graphicsData().release();
		}

		auto & uploadResources = m_uploadResources[m_currentUpdate];

		// Usually GPU initialisation
		doProcessEvents( EventType::ePreRender, device, *data );

		// GPU Update
		GpuUpdater updater{ device, info };
		getEngine()->update( updater );

		uploadResources.fence->wait( ashes::MaxTimeout );
		uploadResources.waitCount--;
		CU_Require( uploadResources.waitCount == 0u );
		uploadResources.fence->reset();
		uploadResources.commands.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		uploadResources.commands.commandBuffer->beginDebugBlock( { "Buffers Upload"
			, makeFloatArray( getEngine()->getNextRainbowColour() ) } );
		m_uploadTimer->beginPass( *uploadResources.commands.commandBuffer );

		device.bufferPool->upload( *uploadResources.commands.commandBuffer );
		device.uboPool->upload( *uploadResources.commands.commandBuffer );
		device.geometryPools->upload( *uploadResources.commands.commandBuffer );
		device.vertexPools->upload( *uploadResources.commands.commandBuffer );
		getEngine()->upload( *uploadResources.commands.commandBuffer );

		for ( auto & buffer : m_shaderBuffers )
		{
			buffer->upload( *uploadResources.commands.commandBuffer );
		}

		for ( auto & window : windows )
		{
			window.second->upload( *uploadResources.commands.commandBuffer );
		}

		getEngine()->getRenderTargetCache().upload( *uploadResources.commands.commandBuffer );

		m_uploadTimer->endPass( *uploadResources.commands.commandBuffer );
		uploadResources.commands.commandBuffer->endDebugBlock();
		uploadResources.commands.commandBuffer->end();
		m_uploadTimer->notifyPassRender();
		data->queue->submit( *uploadResources.commands.commandBuffer
			, ( uploadResources.used
				? VkSemaphore{ VK_NULL_HANDLE }
				: *uploadResources.commands.semaphore )
			, ( uploadResources.used
				? VkPipelineStageFlagBits{}
				: VK_PIPELINE_STAGE_TRANSFER_BIT )
			, *uploadResources.commands.semaphore
			, *uploadResources.fence );
		uploadResources.waitCount++;
		CU_Require( uploadResources.waitCount == 1u );

		// Render
		toWait = getEngine()->getRenderTargetCache().render( device
			, info
			, *data->queue
			, { { *uploadResources.commands.semaphore
			, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } } );

		for ( auto & window : windows )
		{
			window.second->render( info
				, m_ignored == 5
				, toWait );
		}

		uploadResources.used = toWait.empty();

		// Usually GPU cleanup
		doProcessEvents( EventType::eQueueRender, device, *data );

		m_debugOverlays->endGpuTasks();

		if ( !m_reservedQueue )
		{
			device.putGraphicsData( data );
		}
	}

	void RenderLoop::doCpuStep( castor::Milliseconds tslf )
	{
		CpuUpdater updater;
		updater.tslf = tslf;
		getEngine()->update( updater );

		for ( auto & techniqueQueues : updater.techniquesQueues )
		{
			for ( auto & queue : techniqueQueues.queues )
			{
				queue.get().update( techniqueQueues.shadowMaps, techniqueQueues.shadowBuffer );
			}
		}

		m_debugOverlays->endCpuTask();
	}
}
