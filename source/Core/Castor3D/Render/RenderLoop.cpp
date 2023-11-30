#include "Castor3D/Render/RenderLoop.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/StagedUploadData.hpp"
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

		if ( m_uploadData )
		{
			auto & device = m_renderSystem.getRenderDevice();
			auto data = m_reservedQueue;

			if ( !data )
			{
				data = device.graphicsData().release();
			}

			getEngine()->getFrameListenerCache().forEach( [&device, data]( FrameListener & listener )
				{
					listener.fireEvents( GpuEventType::ePreUpload, device, *data );
					listener.fireEvents( CpuEventType::ePreGpuStep );
				} );
			getEngine()->getFrameListenerCache().forEach( [&device, data]( FrameListener & listener )
				{
					listener.fireEvents( GpuEventType::ePreRender, device, *data );
					listener.fireEvents( CpuEventType::ePreCpuStep );
				} );
			getEngine()->getFrameListenerCache().forEach( [&device, data]( FrameListener & listener )
				{
					listener.fireEvents( GpuEventType::ePostRender, device, *data );
					listener.fireEvents( CpuEventType::ePostCpuStep );
				} );

			device->waitIdle();
			m_uploadData = {};
			m_ignored = 5;

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
					listener.flushEvents( CpuEventType::ePreGpuStep );
					listener.flushEvents( GpuEventType::ePreUpload );
					listener.flushEvents( GpuEventType::ePreRender );
					listener.flushEvents( GpuEventType::ePostRender );
					listener.flushEvents( CpuEventType::ePreCpuStep );
					listener.flushEvents( CpuEventType::ePostCpuStep );
				} );
		}
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
				listener.flushEvents( CpuEventType::ePreGpuStep );
				listener.flushEvents( CpuEventType::ePreCpuStep );
				listener.flushEvents( CpuEventType::ePostCpuStep );
				listener.flushEvents( GpuEventType::ePreUpload );
				listener.flushEvents( GpuEventType::ePreRender );
				listener.flushEvents( GpuEventType::ePostRender );
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
			bool first = m_ignored > 0;
			RenderInfo & info = m_debugOverlays->beginFrame();
			doProcessEvents( CpuEventType::ePreGpuStep );
			doGpuStep( info );
			doProcessEvents( CpuEventType::ePreCpuStep );
			doCpuStep( tslf );
			doProcessEvents( CpuEventType::ePostCpuStep );
			m_lastFrameTime = m_debugOverlays->endFrame( first );

			if ( m_ignored == 1 )
			{
				log::info << "Initialisation time: " << ( float( m_lastFrameTime.load().count() ) / 1000.0f ) << " ms." << std::endl;
			}

			m_ignored = std::max( m_ignored - 1, 0 );
		}
	}

	void RenderLoop::doProcessEvents( CpuEventType eventType )
	{
		auto block = m_timerCpuEvents[size_t( eventType )]->start();
		getEngine()->getFrameListenerCache().forEach( [eventType]( FrameListener & listener )
			{
				listener.fireEvents( eventType );
			} );
	}

	void RenderLoop::doProcessEvents( GpuEventType eventType
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

			m_uploadData = castor::makeUniqueDerived< UploadData, StagedUploadData >( device
				, "RenderLoop"
				, data->commandPool->createCommandBuffer( "RenderLoopUpload" ) );
			m_uploadFence = device->createFence( "RenderLoopUpload" );

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

		auto & uploadData = *m_uploadData;

		// Usually GPU initialisation
		doProcessEvents( GpuEventType::ePreUpload, device, *data );

		// GPU Update
		GpuUpdater updater{ device, info };
		getEngine()->update( updater );

		uploadData.begin();
		device.bufferPool->upload( uploadData );
		device.uboPool->upload( uploadData );
		getEngine()->upload( uploadData );

		for ( auto & buffer : m_shaderBuffers )
		{
			buffer->upload( uploadData );
		}

		for ( auto & window : windows )
		{
			window.second->upload( uploadData );
		}

		getEngine()->getRenderTargetCache().upload( uploadData );
		getEngine()->getTextureUnitCache().upload( uploadData );
		uploadData.process();
		auto used = uploadData.end( *data->queue );

		doProcessEvents( GpuEventType::ePreRender, device, *data );

		// Render
		toWait = getEngine()->getRenderTargetCache().render( device
			, info
			, *data->queue
			, { { *used.semaphore
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT } } );

		for ( auto & window : windows )
		{
			window.second->render( info
				, m_ignored > 0
				, toWait );
		}

		*used.used = toWait.empty();
		info.uploadSize = uint32_t( used.uploadSize );
		info.stagingBuffersCount = uint32_t( used.buffersCount );

		// Usually GPU cleanup
		doProcessEvents( GpuEventType::ePostRender, device, *data );

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
