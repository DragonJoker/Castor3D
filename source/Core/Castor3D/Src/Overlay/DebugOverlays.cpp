#include "DebugOverlays.hpp"

#include "Engine.hpp"
#include "Cache/OverlayCache.hpp"

#include "TextOverlay.hpp"

#include "Render/RenderSystem.hpp"

#include <algorithm>
#include <iomanip>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		TextOverlaySPtr GetTextOverlay( OverlayCache & p_cache, String const & p_name )
		{
			TextOverlaySPtr l_return;
			OverlaySPtr l_overlay = p_cache.Find( p_name );

			if ( l_overlay )
			{
				l_return = l_overlay->GetTextOverlay();
			}

			return l_return;
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
		OverlaySPtr l_panel = p_cache.Find( cuT( "DebugPanel" ) );
		m_debugPanel = l_panel;
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

		if ( l_panel )
		{
			l_panel->SetVisible( m_valid && m_visible );
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
			m_gpuTime = 0;
			m_cpuTime = 0;
			m_taskTimer.TimeMs();
			m_externalTime = m_frameTimer.TimeMs();
		}
	}

	void DebugOverlays::EndFrame( uint32_t p_vertices, uint32_t p_faces, uint32_t p_objects, uint32_t p_visible, uint32_t p_particles )
	{
		if ( m_valid && m_visible )
		{
			double l_time = m_frameTimer.TimeMs() + m_externalTime;
			m_debugTimer.TimeMs();
			m_framesTimes[m_frameIndex] = l_time;
			m_debugTotalTime->SetCaption( StringStream() << std::setprecision( 3 ) << ( l_time ) << cuT( " ms" ) );
			m_debugCpuTime->SetCaption( StringStream() << std::setprecision( 3 ) << m_cpuTime << cuT( " ms" ) );
			m_externTime->SetCaption( StringStream() << std::setprecision( 3 ) << m_externalTime << cuT( " ms" ) );
			m_debugVertexCount->SetCaption( string::to_string( p_vertices ) );
			m_debugFaceCount->SetCaption( string::to_string( p_faces ) );
			m_debugObjectCount->SetCaption( string::to_string( p_objects ) );
			m_debugVisibleObjectCount->SetCaption( string::to_string( p_visible ) );
			m_debugParticlesCount->SetCaption( string::to_string( p_particles ) );

			m_debugGpuClientTime->SetCaption( StringStream() << std::setprecision( 3 ) << m_gpuTime << cuT( " ms" ) );
			m_debugGpuServerTime->SetCaption( StringStream() << std::setprecision( 3 ) << ( GetEngine()->GetRenderSystem()->GetGpuTime().count() / 1000.0 ) << cuT( " ms" ) );
			GetEngine()->GetRenderSystem()->ResetGpuTime();

			l_time = std::accumulate( m_framesTimes.begin(), m_framesTimes.end(), 0.0 ) / m_framesTimes.size();
			m_debugAverageFps->SetCaption( StringStream() << std::setprecision( 3 ) << 1000.0 / l_time << cuT( " frames/s" ) );
			m_debugAverageTime->SetCaption( StringStream() << std::setprecision( 3 ) << l_time << cuT( " ms" ) );

			m_frameIndex = ++m_frameIndex % FRAME_SAMPLES_COUNT;
			l_time = m_debugTimer.TimeMs();
			m_debugTime->SetCaption( StringStream() << std::setprecision( 3 ) << l_time << cuT( " ms" ) );

			m_frameTimer.TimeMs();
		}
	}

	void DebugOverlays::EndGpuTask()
	{
		if ( m_valid && m_visible )
		{
			m_gpuTime += m_taskTimer.TimeMs();
		}
	}

	void DebugOverlays::EndCpuTask()
	{
		if ( m_valid && m_visible )
		{
			m_cpuTime += m_taskTimer.TimeMs();
		}
	}

	void DebugOverlays::Show( bool p_show )
	{
		m_visible = p_show;
		OverlaySPtr l_panel = m_debugPanel.lock();

		if ( l_panel )
		{
			l_panel->SetVisible( m_valid && m_visible );
		}
	}
}
