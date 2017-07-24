#include "DebugOverlays.hpp"

#include "Engine.hpp"
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
		TextOverlaySPtr GetTextOverlay( OverlayCache & p_cache, String const & p_name )
		{
			TextOverlaySPtr result;
			OverlaySPtr overlay = p_cache.Find( p_name );

			if ( overlay )
			{
				result = overlay->GetTextOverlay();
			}

			return result;
		}
	}

	DebugOverlays::DebugOverlays( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
	{
	}

	DebugOverlays::~DebugOverlays()
	{
	}

	void DebugOverlays::Initialise( OverlayCache & p_cache )
	{
		OverlaySPtr panel = p_cache.Find( cuT( "DebugPanel" ) );
		m_debugPanel = panel;
		m_debugCpuTime = GetTextOverlay( p_cache, cuT( "DebugPanel-CpuTime-Value" ) );
		m_debugGpuClientTime = GetTextOverlay( p_cache, cuT( "DebugPanel-GpuClientTime-Value" ) );
		m_debugGpuServerTime = GetTextOverlay( p_cache, cuT( "DebugPanel-GpuServerTime-Value" ) );
		m_debugTotalTime = GetTextOverlay( p_cache, cuT( "DebugPanel-TotalTime-Value" ) );
		m_debugAverageFps = GetTextOverlay( p_cache, cuT( "DebugPanel-AverageFPS-Value" ) );
		m_debugAverageTime = GetTextOverlay( p_cache, cuT( "DebugPanel-AverageTime-Value" ) );
		m_debugVertexCount = GetTextOverlay( p_cache, cuT( "DebugPanel-VertexCount-Value" ) );
		m_debugFaceCount = GetTextOverlay( p_cache, cuT( "DebugPanel-FaceCount-Value" ) );
		m_debugObjectCount = GetTextOverlay( p_cache, cuT( "DebugPanel-ObjectCount-Value" ) );
		m_debugVisibleObjectCount = GetTextOverlay( p_cache, cuT( "DebugPanel-VisibleObjectCount-Value" ) );
		m_debugParticlesCount = GetTextOverlay( p_cache, cuT( "DebugPanel-ParticlesCount-Value" ) );
		m_debugTime = GetTextOverlay( p_cache, cuT( "DebugPanel-DebugTime-Value" ) );
		m_externTime = GetTextOverlay( p_cache, cuT( "DebugPanel-ExternalTime-Value" ) );

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

	void DebugOverlays::EndFrame( RenderInfo const & p_info )
	{
		auto totalTime = m_frameTimer.Time() + m_externalTime;

		if ( m_valid && m_visible )
		{
			m_debugTimer.Time();
			m_framesTimes[m_frameIndex] = totalTime;
			m_debugTotalTime->SetCaption( StringStream() << totalTime );
			m_debugCpuTime->SetCaption( StringStream() << m_cpuTime );
			m_externTime->SetCaption( StringStream() << m_externalTime );
			m_debugVertexCount->SetCaption( string::to_string( p_info.m_totalVertexCount ) );
			m_debugFaceCount->SetCaption( string::to_string( p_info.m_totalFaceCount ) );
			m_debugObjectCount->SetCaption( string::to_string( p_info.m_totalObjectsCount ) );
			m_debugVisibleObjectCount->SetCaption( string::to_string( p_info.m_visibleObjectsCount ) );
			m_debugParticlesCount->SetCaption( string::to_string( p_info.m_particlesCount ) );

			auto serverTime = GetEngine()->GetRenderSystem()->GetGpuTime();
			m_debugGpuClientTime->SetCaption( StringStream() << ( m_gpuTime - serverTime ) );
			m_debugGpuServerTime->SetCaption( StringStream() << serverTime );
			GetEngine()->GetRenderSystem()->ResetGpuTime();

			auto time = std::accumulate( m_framesTimes.begin(), m_framesTimes.end(), 0_ns ) / m_framesTimes.size();
			m_debugAverageFps->SetCaption( StringStream() << std::setprecision( 4 ) << 1000000.0_r / std::chrono::duration_cast< std::chrono::microseconds >( time ).count() << cuT( " fps" ) );
			m_debugAverageTime->SetCaption( StringStream() << time );

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

	void DebugOverlays::Show( bool p_show )
	{
		m_visible = p_show;
		OverlaySPtr panel = m_debugPanel.lock();

		if ( panel )
		{
			panel->SetVisible( m_valid && m_visible );
		}
	}
}
