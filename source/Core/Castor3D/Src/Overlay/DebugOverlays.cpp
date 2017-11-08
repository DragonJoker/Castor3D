#include "DebugOverlays.hpp"

#include "Engine.hpp"
#include "Overlay/PanelOverlay.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Render/RenderPassTimer.hpp"

#include <iomanip>

namespace std
{
	std::ostream & operator<<( std::ostream & p_stream, castor::Nanoseconds const & p_duration )
	{
		p_stream << std::setprecision( 3 ) << ( p_duration.count() / 1000000.0_r ) << cuT( " ms" );
		return p_stream;
	}
}

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextOverlaySPtr getTextOverlay( OverlayCache & cache, String const & name )
		{
			TextOverlaySPtr result;
			OverlaySPtr overlay = cache.find( name );

			if ( overlay )
			{
				result = overlay->getTextOverlay();
			}

			return result;
		}
	}

	DebugOverlays::DebugOverlays( Engine & engine )
		: OwnedBy< Engine >( engine )
	{
	}

	DebugOverlays::~DebugOverlays()
	{
	}

	void DebugOverlays::initialise( OverlayCache & cache )
	{
		OverlaySPtr panel = cache.find( cuT( "DebugPanel" ) );
		m_debugPanel = panel;
		m_debugCpuTime = getTextOverlay( cache, cuT( "DebugPanel-CpuTime-Value" ) );
		m_debugGpuClientTime = getTextOverlay( cache, cuT( "DebugPanel-GpuClientTime-Value" ) );
		m_debugGpuServerTime = getTextOverlay( cache, cuT( "DebugPanel-GpuServerTime-Value" ) );
		m_debugTotalTime = getTextOverlay( cache, cuT( "DebugPanel-TotalTime-Value" ) );
		m_debugAverageFps = getTextOverlay( cache, cuT( "DebugPanel-AverageFPS-Value" ) );
		m_debugAverageTime = getTextOverlay( cache, cuT( "DebugPanel-AverageTime-Value" ) );
		m_debugVertexCount = getTextOverlay( cache, cuT( "DebugPanel-VertexCount-Value" ) );
		m_debugFaceCount = getTextOverlay( cache, cuT( "DebugPanel-FaceCount-Value" ) );
		m_debugObjectCount = getTextOverlay( cache, cuT( "DebugPanel-ObjectCount-Value" ) );
		m_debugVisibleObjectCount = getTextOverlay( cache, cuT( "DebugPanel-VisibleObjectCount-Value" ) );
		m_debugParticlesCount = getTextOverlay( cache, cuT( "DebugPanel-ParticlesCount-Value" ) );
		m_debugLightCount = getTextOverlay( cache, cuT( "DebugPanel-LightCount-Value" ) );
		m_debugVisibleLightCount = getTextOverlay( cache, cuT( "DebugPanel-VisibleLightCount-Value" ) );
		m_debugTime = getTextOverlay( cache, cuT( "DebugPanel-DebugTime-Value" ) );
		m_externTime = getTextOverlay( cache, cuT( "DebugPanel-ExternalTime-Value" ) );

		m_valid = m_debugCpuTime
			&& m_debugGpuClientTime
			&& m_debugGpuServerTime
			&& m_debugTotalTime
			&& m_debugAverageFps
			&& m_debugAverageTime
			&& m_debugVertexCount
			&& m_debugFaceCount
			&& m_debugObjectCount
			&& m_debugVisibleObjectCount
			&& m_debugParticlesCount
			&& m_debugTime
			&& m_externTime;

		if ( panel )
		{
			panel->setVisible( m_valid && m_visible );
		}
	}

	void DebugOverlays::cleanup()
	{
		m_debugPanel.reset();
		m_debugCpuTime.reset();
		m_debugGpuClientTime.reset();
		m_debugGpuServerTime.reset();
		m_debugTotalTime.reset();
		m_debugAverageFps.reset();
		m_debugAverageTime.reset();
		m_debugVertexCount.reset();
		m_debugFaceCount.reset();
		m_debugObjectCount.reset();
		m_debugVisibleObjectCount.reset();
		m_debugParticlesCount.reset();
		m_debugTime.reset();
		m_externTime.reset();
	}

	void DebugOverlays::beginFrame()
	{
		if ( m_valid && m_visible )
		{
			m_gpuTime = 0_ms;
			m_cpuTime = 0_ms;
			m_taskTimer.getElapsed();
		}

		m_externalTime = m_frameTimer.getElapsed();
	}

	void DebugOverlays::registerTimer( RenderPassTimer const & timer )
	{
		auto & cache = getEngine()->getOverlayCache();
		
		auto it = m_renderPasses.find( timer.getCategory() );

		if ( it == m_renderPasses.end() )
		{
			auto pair = m_renderPasses.emplace( timer.getCategory(), RenderPassOverlays{} );
			it = pair.first;

			if ( pair.second )
			{
				auto & overlays = it->second;
				auto baseName = cuT( "RenderPassOverlays " ) + string::toString( int( m_renderPasses.size() ) ) + cuT( " " ) + timer.getName();
				overlays.m_panel = cache.add( baseName
					, OverlayType::ePanel
					, nullptr
					, nullptr )->getPanelOverlay();
				overlays.m_titlePanel = cache.add( baseName + cuT( "_Title" )
					, OverlayType::ePanel
					, nullptr
					, overlays.m_panel->getOverlay().shared_from_this() )->getPanelOverlay();
				overlays.m_title = cache.add( baseName + cuT( "_TitleText" )
					, OverlayType::eText
					, nullptr
					, overlays.m_titlePanel->getOverlay().shared_from_this() )->getTextOverlay();
				overlays.m_cpuName = cache.add( baseName + cuT( "_CPUName" )
					, OverlayType::eText
					, nullptr
					, overlays.m_panel->getOverlay().shared_from_this() )->getTextOverlay();
				overlays.m_cpuValue = cache.add( baseName + cuT( "_CPUValue" )
					, OverlayType::eText
					, nullptr
					, overlays.m_panel->getOverlay().shared_from_this() )->getTextOverlay();
				overlays.m_gpuName = cache.add( baseName + cuT( "_GPUName" )
					, OverlayType::eText
					, nullptr
					, overlays.m_panel->getOverlay().shared_from_this() )->getTextOverlay();
				overlays.m_gpuValue = cache.add( baseName + cuT( "_GPUValue" )
					, OverlayType::eText
					, nullptr
					, overlays.m_panel->getOverlay().shared_from_this() )->getTextOverlay();
				overlays.m_panel->setPixelPosition( Position{ 400, int32_t( 40 * ( m_renderPasses.size() - 1 ) ) } );
				overlays.m_titlePanel->setPixelPosition( Position{ 0, 0 } );
				overlays.m_title->setPixelPosition( Position{ 10, 0 } );
				overlays.m_cpuName->setPixelPosition( Position{ 10, 20 } );
				overlays.m_cpuValue->setPixelPosition( Position{ 45, 20 } );
				overlays.m_gpuName->setPixelPosition( Position{ 130, 20 } );
				overlays.m_gpuValue->setPixelPosition( Position{ 165, 20 } );

				overlays.m_panel->setPixelSize( Size{ 250, 40 } );
				overlays.m_titlePanel->setPixelSize( Size{ 250, 20 } );
				overlays.m_title->setPixelSize( Size{ 230, 20 } );
				overlays.m_cpuName->setPixelSize( Size{ 30, 20 } );
				overlays.m_cpuValue->setPixelSize( Size{ 75, 20 } );
				overlays.m_gpuName->setPixelSize( Size{ 30, 20 } );
				overlays.m_gpuValue->setPixelSize( Size{ 75, 20 } );

				overlays.m_title->setFont( cuT( "Arial20" ) );
				overlays.m_cpuName->setFont( cuT( "Arial10" ) );
				overlays.m_gpuName->setFont( cuT( "Arial10" ) );
				overlays.m_cpuValue->setFont( cuT( "Arial10" ) );
				overlays.m_gpuValue->setFont( cuT( "Arial10" ) );

				overlays.m_title->setCaption( timer.getName() );
				overlays.m_cpuName->setCaption( cuT( "CPU:" ) );
				overlays.m_gpuName->setCaption( cuT( "GPU:" ) );

				auto & materials = getEngine()->getMaterialCache();
				overlays.m_panel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
				overlays.m_titlePanel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
				overlays.m_title->setMaterial( materials.find( cuT( "White" ) ) );
				overlays.m_cpuName->setMaterial( materials.find( cuT( "White" ) ) );
				overlays.m_gpuName->setMaterial( materials.find( cuT( "White" ) ) );
				overlays.m_cpuValue->setMaterial( materials.find( cuT( "White" ) ) );
				overlays.m_gpuValue->setMaterial( materials.find( cuT( "White" ) ) );

				overlays.m_panel->setVisible( m_visible );
				overlays.m_titlePanel->setVisible( true );
				overlays.m_title->setVisible( true );
				overlays.m_cpuName->setVisible( true );
				overlays.m_gpuName->setVisible( true );
				overlays.m_cpuValue->setVisible( true );
				overlays.m_gpuValue->setVisible( true );
			}
		}

		it->second.m_timers.emplace_back( std::ref( timer ) );
	}

	void DebugOverlays::unregisterTimer( RenderPassTimer const & timer )
	{
		auto itC = m_renderPasses.find( timer.getCategory() );

		if ( itC != m_renderPasses.end() )
		{
			auto & overlays = itC->second;
			auto it = std::find_if( overlays.m_timers.begin()
				, overlays.m_timers.end()
				, [&timer]( auto const & lookup )
				{
					return &lookup.get() == &timer;
				} );

			if ( it != overlays.m_timers.end() )
			{
				overlays.m_timers.erase( it );

				if ( overlays.m_timers.empty() )
				{
					auto & cache = getEngine()->getOverlayCache();
					cache.remove( overlays.m_cpuName->getOverlayName() );
					cache.remove( overlays.m_gpuName->getOverlayName() );
					cache.remove( overlays.m_gpuValue->getOverlayName() );
					cache.remove( overlays.m_cpuValue->getOverlayName() );
					cache.remove( overlays.m_title->getOverlayName() );
					cache.remove( overlays.m_titlePanel->getOverlayName() );
					cache.remove( overlays.m_panel->getOverlayName() );
					m_renderPasses.erase( itC );
				}
			}
		}
	}

	void DebugOverlays::endFrame( RenderInfo const & info )
	{
		auto totalTime = m_frameTimer.getElapsed() + m_externalTime;

		if ( m_valid && m_visible )
		{
			m_debugTimer.getElapsed();
			m_framesTimes[m_frameIndex] = totalTime;
			m_debugTotalTime->setCaption( StringStream() << totalTime );
			m_debugCpuTime->setCaption( StringStream() << m_cpuTime );
			m_externTime->setCaption( StringStream() << m_externalTime );
			m_debugVertexCount->setCaption( string::toString( info.m_totalVertexCount ) );
			m_debugFaceCount->setCaption( string::toString( info.m_totalFaceCount ) );
			m_debugObjectCount->setCaption( string::toString( info.m_totalObjectsCount ) );
			m_debugVisibleObjectCount->setCaption( string::toString( info.m_visibleObjectsCount ) );
			m_debugParticlesCount->setCaption( string::toString( info.m_particlesCount ) );
			m_debugLightCount->setCaption( string::toString( info.m_totalLightsCount ) );
			m_debugVisibleLightCount->setCaption( string::toString( info.m_visibleLightsCount ) );

			auto time = std::accumulate( m_framesTimes.begin(), m_framesTimes.end(), 0_ns ) / m_framesTimes.size();
			m_debugAverageFps->setCaption( StringStream() << std::setprecision( 4 ) << 1000000.0_r / std::chrono::duration_cast< std::chrono::microseconds >( time ).count() << cuT( " fps" ) );
			m_debugAverageTime->setCaption( StringStream() << time );

			auto gpuTotal = 0_ns;

			for ( auto & pass : m_renderPasses )
			{
				Nanoseconds cpu{ 0 };
				Nanoseconds gpu{ 0 };

				for ( auto & timer : pass.second.m_timers )
				{
					cpu += timer.get().getCpuTime();
					gpu += timer.get().getGpuTime();
				}

				pass.second.m_cpuValue->setCaption( StringStream{} << cpu );
				pass.second.m_gpuValue->setCaption( StringStream{} << gpu );
				gpuTotal += gpu;
			}

			m_debugGpuClientTime->setCaption( StringStream() << ( m_gpuTime - gpuTotal ) );
			m_debugGpuServerTime->setCaption( StringStream() << gpuTotal );
			getEngine()->getRenderSystem()->resetGpuTime();

			m_frameIndex = ++m_frameIndex % FRAME_SAMPLES_COUNT;
			time = m_debugTimer.getElapsed();
			m_debugTime->setCaption( StringStream() << time );

			m_frameTimer.getElapsed();
		}

#if defined( NDEBUG )

		auto total = std::chrono::duration_cast< std::chrono::microseconds >( totalTime );
		fprintf( stdout
			, "\r%0.7f ms, %0.7f fps"
			, total.count() / 1000.0f
			, ( 1000000.0_r / total.count() ) );

#else

		std::cout << "\rTime: " << std::setw( 7 ) << totalTime;
		std::cout << " - FPS: " << std::setw( 7 ) << std::setprecision( 4 ) << ( 1000000.0_r / std::chrono::duration_cast< std::chrono::microseconds >( totalTime ).count() );

#endif
	}

	void DebugOverlays::endGpuTask()
	{
		if ( m_valid && m_visible )
		{
			m_gpuTime += m_taskTimer.getElapsed();
		}
	}

	void DebugOverlays::endCpuTask()
	{
		if ( m_valid && m_visible )
		{
			m_cpuTime += m_taskTimer.getElapsed();
		}
	}

	void DebugOverlays::show( bool show )
	{
		m_visible = show;
		OverlaySPtr panel = m_debugPanel.lock();

		if ( panel )
		{
			panel->setVisible( m_valid && m_visible );
		}

		for ( auto & pass : m_renderPasses )
		{
			pass.second.m_panel->setVisible( m_valid && m_visible );
		}
	}
}
