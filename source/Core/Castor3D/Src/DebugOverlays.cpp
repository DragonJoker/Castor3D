#include "DebugOverlays.hpp"

#include "Overlay.hpp"
#include "OverlayManager.hpp"
#include "TextOverlay.hpp"

#include <algorithm>

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		TextOverlaySPtr GetTextOverlay( OverlayManager & p_manager, String const & p_name )
		{
			TextOverlaySPtr l_return;
			OverlaySPtr l_overlay = p_manager.Find( p_name );

			if ( l_overlay )
			{
				l_return = l_overlay->GetTextOverlay();
			}

			return l_return;
		}
	}

	DebugOverlays::DebugOverlays()
		: m_visible( false )
		, m_frameIndex( 0 )
		, m_gpuTime( 0 )
		, m_cpuTime( 0 )
	{
	}

	DebugOverlays::~DebugOverlays()
	{
	}

	void DebugOverlays::Initialise( OverlayManager & p_manager )
	{
		OverlaySPtr l_panel = p_manager.Find( cuT( "DebugPanel" ) );
		m_debugPanel = l_panel;
		m_debugCpuTime = GetTextOverlay( p_manager, cuT( "DebugPanel-CpuTime-Value" ) );
		m_debugGpuTime = GetTextOverlay( p_manager, cuT( "DebugPanel-GpuTime-Value" ) );
		m_debugTotalTime = GetTextOverlay( p_manager, cuT( "DebugPanel-TotalTime-Value" ) );
		m_debugAverageFps = GetTextOverlay( p_manager, cuT( "DebugPanel-AverageFPS-Value" ) );
		m_debugVertexCount = GetTextOverlay( p_manager, cuT( "DebugPanel-VertexCount-Value" ) );
		m_debugFaceCount = GetTextOverlay( p_manager, cuT( "DebugPanel-FaceCount-Value" ) );
		m_debugObjectCount = GetTextOverlay( p_manager, cuT( "DebugPanel-ObjectCount-Value" ) );

		if ( l_panel )
		{
			l_panel->SetVisible( m_visible );
		}
	}

	void DebugOverlays::Cleanup()
	{
		m_debugPanel.reset();
		m_debugCpuTime.reset();
		m_debugGpuTime.reset();
		m_debugTotalTime.reset();
		m_debugAverageFps.reset();
		m_debugVertexCount.reset();
		m_debugFaceCount.reset();
		m_debugObjectCount.reset();
	}

	void DebugOverlays::StartFrame()
	{
		if ( m_visible )
		{
			m_gpuTime = 0;
			m_cpuTime = 0;
			m_taskTimer.TimeMs();
		}
	}

	void DebugOverlays::EndFrame( uint32_t p_vertices, uint32_t p_faces, uint32_t p_objects )
	{
		if ( m_visible )
		{
			double l_time = m_frameTimer.TimeMs();
			m_framesTimes[m_frameIndex] = l_time;
			TextOverlaySPtr l_txt = m_debugCpuTime.lock();

			if ( l_txt )
			{
				l_txt->SetCaption( string::to_string( int( m_cpuTime ) ) + cuT( " ms" ) );
			}

			l_txt = m_debugGpuTime.lock();

			if ( l_txt )
			{
				l_txt->SetCaption( string::to_string( int( m_gpuTime ) ) + cuT( " ms" ) );
			}

			l_txt = m_debugTotalTime.lock();

			if ( l_txt )
			{
				l_txt->SetCaption( string::to_string( int( l_time ) ) + cuT( " ms" ) );
			}

			l_txt = m_debugAverageFps.lock();

			if ( l_txt )
			{
				double l_avgTime = std::accumulate( m_framesTimes.begin(), m_framesTimes.end(), 0.0 ) / m_framesTimes.size();
				l_txt->SetCaption( string::to_string( int( 1000 / l_avgTime ) ) + cuT( " frames/s" ) );
			}

			l_txt = m_debugVertexCount.lock();

			if ( l_txt )
			{
				l_txt->SetCaption( string::to_string( p_vertices ) );
			}

			l_txt = m_debugFaceCount.lock();

			if ( l_txt )
			{
				l_txt->SetCaption( string::to_string( p_faces ) );
			}

			l_txt = m_debugObjectCount.lock();

			if ( l_txt )
			{
				l_txt->SetCaption( string::to_string( p_objects ) );
			}

			m_frameIndex = ++m_frameIndex % 100;
		}
	}

	void DebugOverlays::EndGpuTask()
	{
		if ( m_visible )
		{
			m_gpuTime += m_taskTimer.TimeMs();
		}
	}

	void DebugOverlays::EndCpuTask()
	{
		if ( m_visible )
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
			l_panel->SetVisible( m_visible );
		}
	}
}
