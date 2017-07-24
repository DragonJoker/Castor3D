#include "DebugOverlays.hpp"

#include "Engine.hpp"
#include "PanelOverlay.hpp"
#include "TextOverlay.hpp"

#include <iomanip>

namespace std
{
	std::ostream & operator<<( std::ostream & p_stream, Castor::Nanoseconds const & p_duration )
	{
		p_stream << std::setprecision( 3 ) << ( p_duration.count() / 1000000.0_r ) << cuT( " ms" );
		return p_stream;
	}
}

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		TextOverlaySPtr GetTextOverlay( OverlayCache & cache, String const & name )
		{
			TextOverlaySPtr result;
			OverlaySPtr overlay = cache.Find( name );

			if ( overlay )
			{
				result = overlay->GetTextOverlay();
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

	void DebugOverlays::Initialise( OverlayCache & cache )
	{
		OverlaySPtr panel = cache.Find( cuT( "DebugPanel" ) );
		m_debugPanel = panel;
		m_debugCpuTime = GetTextOverlay( cache, cuT( "DebugPanel-CpuTime-Value" ) );
		m_debugGpuClientTime = GetTextOverlay( cache, cuT( "DebugPanel-GpuClientTime-Value" ) );
		m_debugGpuServerTime = GetTextOverlay( cache, cuT( "DebugPanel-GpuServerTime-Value" ) );
		m_debugTotalTime = GetTextOverlay( cache, cuT( "DebugPanel-TotalTime-Value" ) );
		m_debugAverageFps = GetTextOverlay( cache, cuT( "DebugPanel-AverageFPS-Value" ) );
		m_debugAverageTime = GetTextOverlay( cache, cuT( "DebugPanel-AverageTime-Value" ) );
		m_debugVertexCount = GetTextOverlay( cache, cuT( "DebugPanel-VertexCount-Value" ) );
		m_debugFaceCount = GetTextOverlay( cache, cuT( "DebugPanel-FaceCount-Value" ) );
		m_debugObjectCount = GetTextOverlay( cache, cuT( "DebugPanel-ObjectCount-Value" ) );
		m_debugVisibleObjectCount = GetTextOverlay( cache, cuT( "DebugPanel-VisibleObjectCount-Value" ) );
		m_debugParticlesCount = GetTextOverlay( cache, cuT( "DebugPanel-ParticlesCount-Value" ) );
		m_debugTime = GetTextOverlay( cache, cuT( "DebugPanel-DebugTime-Value" ) );
		m_externTime = GetTextOverlay( cache, cuT( "DebugPanel-ExternalTime-Value" ) );

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
			panel->SetVisible( m_valid && m_visible );
		}
	}

	void DebugOverlays::Cleanup()
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

	void DebugOverlays::StartFrame()
	{
		if ( m_valid && m_visible )
		{
			m_gpuTime = 0_ms;
			m_cpuTime = 0_ms;
			m_taskTimer.Time();
		}

		m_externalTime = m_frameTimer.Time();
	}

	void DebugOverlays::DoUpdateOverlays( OverlayCache & cache
		, RenderTimes const & times
		, uint32_t index )
	{
		if ( index >= m_renderPasses.size() )
		{
			RenderPassOverlays overlays;
			auto baseName = cuT( "RenderPassOverlays" ) + string::to_string( index );
			overlays.m_panel = cache.Add( baseName
				, OverlayType::ePanel
				, nullptr
				, nullptr )->GetPanelOverlay();
			overlays.m_title = cache.Add( baseName + cuT( "_Title" )
				, OverlayType::eText
				, nullptr
				, overlays.m_panel->GetOverlay().shared_from_this() )->GetTextOverlay();
			overlays.m_cpuName = cache.Add( baseName + cuT( "_CPUName" )
				, OverlayType::eText
				, nullptr
				, overlays.m_panel->GetOverlay().shared_from_this() )->GetTextOverlay();
			overlays.m_cpuValue = cache.Add( baseName + cuT( "_CPUValue" )
				, OverlayType::eText
				, nullptr
				, overlays.m_panel->GetOverlay().shared_from_this() )->GetTextOverlay();
			overlays.m_gpuName = cache.Add( baseName + cuT( "_GPUName" )
				, OverlayType::eText
				, nullptr
				, overlays.m_panel->GetOverlay().shared_from_this() )->GetTextOverlay();
			overlays.m_gpuValue = cache.Add( baseName + cuT( "_GPUValue" )
				, OverlayType::eText
				, nullptr
				, overlays.m_panel->GetOverlay().shared_from_this() )->GetTextOverlay();
			overlays.m_panel->SetPixelPosition( Position{ 400, int32_t( 60 * index ) } );
			overlays.m_title->SetPixelPosition( Position{ 10, 0 } );
			overlays.m_cpuName->SetPixelPosition( Position{ 10, 20 } );
			overlays.m_gpuName->SetPixelPosition( Position{ 10, 40 } );
			overlays.m_cpuValue->SetPixelPosition( Position{ 110, 20 } );
			overlays.m_gpuValue->SetPixelPosition( Position{ 110, 40 } );

			overlays.m_panel->SetPixelSize( Size{ 250, 60 } );
			overlays.m_title->SetPixelSize( Size{ 230, 20 } );
			overlays.m_cpuName->SetPixelSize( Size{ 100, 20 } );
			overlays.m_gpuName->SetPixelSize( Size{ 100, 20 } );
			overlays.m_cpuValue->SetPixelSize( Size{ 130, 20 } );
			overlays.m_gpuValue->SetPixelSize( Size{ 130, 20 } );

			overlays.m_title->SetFont( cuT( "Arial10" ) );
			overlays.m_cpuName->SetFont( cuT( "Arial10" ) );
			overlays.m_gpuName->SetFont( cuT( "Arial10" ) );
			overlays.m_cpuValue->SetFont( cuT( "Arial10" ) );
			overlays.m_gpuValue->SetFont( cuT( "Arial10" ) );

			overlays.m_cpuName->SetCaption( cuT( "CPU Time:" ) );
			overlays.m_gpuName->SetCaption( cuT( "GPU Time:" ) );

			auto & materials = GetEngine()->GetMaterialCache();
			overlays.m_panel->SetMaterial( materials.Find( cuT( "AlphaDarkBlue" ) ) );
			overlays.m_title->SetMaterial( materials.Find( cuT( "White" ) ) );
			overlays.m_cpuName->SetMaterial( materials.Find( cuT( "White" ) ) );
			overlays.m_gpuName->SetMaterial( materials.Find( cuT( "White" ) ) );
			overlays.m_cpuValue->SetMaterial( materials.Find( cuT( "White" ) ) );
			overlays.m_gpuValue->SetMaterial( materials.Find( cuT( "White" ) ) );

			overlays.m_panel->SetVisible( true );
			overlays.m_title->SetVisible( true );
			overlays.m_cpuName->SetVisible( true );
			overlays.m_gpuName->SetVisible( true );
			overlays.m_cpuValue->SetVisible( true );
			overlays.m_gpuValue->SetVisible( true );

			m_renderPasses.emplace_back( std::move( overlays ) );
		}

		auto & overlays = m_renderPasses[index];
		overlays.m_title->SetCaption( times.m_name );
		overlays.m_cpuValue->SetCaption( StringStream{} << times.m_cpu );
		overlays.m_gpuValue->SetCaption( StringStream{} << times.m_gpu );
	}

	void DebugOverlays::EndFrame( RenderInfo const & info )
	{
		auto totalTime = m_frameTimer.Time() + m_externalTime;

		if ( m_valid && m_visible )
		{
			m_debugTimer.Time();
			m_framesTimes[m_frameIndex] = totalTime;
			m_debugTotalTime->SetCaption( StringStream() << totalTime );
			m_debugCpuTime->SetCaption( StringStream() << m_cpuTime );
			m_externTime->SetCaption( StringStream() << m_externalTime );
			m_debugVertexCount->SetCaption( string::to_string( info.m_totalVertexCount ) );
			m_debugFaceCount->SetCaption( string::to_string( info.m_totalFaceCount ) );
			m_debugObjectCount->SetCaption( string::to_string( info.m_totalObjectsCount ) );
			m_debugVisibleObjectCount->SetCaption( string::to_string( info.m_visibleObjectsCount ) );
			m_debugParticlesCount->SetCaption( string::to_string( info.m_particlesCount ) );

			auto serverTime = GetEngine()->GetRenderSystem()->GetGpuTime();
			m_debugGpuClientTime->SetCaption( StringStream() << ( m_gpuTime - serverTime ) );
			m_debugGpuServerTime->SetCaption( StringStream() << serverTime );
			GetEngine()->GetRenderSystem()->ResetGpuTime();

			auto time = std::accumulate( m_framesTimes.begin(), m_framesTimes.end(), 0_ns ) / m_framesTimes.size();
			m_debugAverageFps->SetCaption( StringStream() << std::setprecision( 4 ) << 1000000.0_r / std::chrono::duration_cast< std::chrono::microseconds >( time ).count() << cuT( " fps" ) );
			m_debugAverageTime->SetCaption( StringStream() << time );

			auto & cache = GetEngine()->GetOverlayCache();
			uint32_t index = 0u;

			for ( auto & pass : info.m_times )
			{
				DoUpdateOverlays( cache, pass, index++ );
			}

			m_frameIndex = ++m_frameIndex % FRAME_SAMPLES_COUNT;
			time = m_debugTimer.Time();
			m_debugTime->SetCaption( StringStream() << time );

			m_frameTimer.Time();
		}

		std::cout << "\rTime: " << std::setw( 7 ) << totalTime;
		std::cout << " - FPS: " << std::setw( 7 ) << std::setprecision( 4 ) << ( 1000000.0_r / std::chrono::duration_cast< std::chrono::microseconds >( totalTime ).count() );
	}

	void DebugOverlays::EndGpuTask()
	{
		if ( m_valid && m_visible )
		{
			m_gpuTime += m_taskTimer.Time();
		}
	}

	void DebugOverlays::EndCpuTask()
	{
		if ( m_valid && m_visible )
		{
			m_cpuTime += m_taskTimer.Time();
		}
	}

	void DebugOverlays::Show( bool show )
	{
		m_visible = show;
		OverlaySPtr panel = m_debugPanel.lock();

		if ( panel )
		{
			panel->SetVisible( m_valid && m_visible );
		}
	}
}
