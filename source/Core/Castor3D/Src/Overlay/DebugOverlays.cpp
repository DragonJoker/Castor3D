#include "DebugOverlays.hpp"

#include "Engine.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Overlay/PanelOverlay.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Render/RenderPassTimer.hpp"

#include <Core/Device.hpp>
#include <Miscellaneous/QueryPool.hpp>

#include <iomanip>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		String getFullName( RenderPassTimer & timer )
		{
			return timer.getCategory() == timer.getName()
				? timer.getCategory()
				: timer.getCategory() + cuT( ": " ) + timer.getName();
		}
	}

	//*********************************************************************************************

	DebugOverlays::MainDebugPanel::MainDebugPanel( OverlayCache & cache )
		: m_cache{ cache }
		, m_panel{ m_cache.add( cuT( "MainDebugPanel" )
			, OverlayType::ePanel
			, nullptr
			, nullptr )->getPanelOverlay() }
		, m_times{ std::make_unique< DebugPanels< castor::Nanoseconds > >( cuT( "Times" ), m_panel, cache ) }
		, m_fps{ std::make_unique< DebugPanels< float > >( cuT( "FPS" ), m_panel, cache ) }
		, m_counts{ std::make_unique< DebugPanels< uint32_t > >( cuT( "Counts" ), m_panel, cache ) }
	{
		auto & materials = m_cache.getEngine()->getMaterialCache();
		m_panel->setPixelPosition( Position{ 0, 0 } );
		m_panel->setPixelSize( Size{ 320, 20 } );
		m_panel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
		m_panel->setVisible( true );
	}

	DebugOverlays::MainDebugPanel::~MainDebugPanel()
	{
		m_times.reset();
		m_fps.reset();
		m_counts.reset();

		if ( m_panel )
		{
			m_cache.remove( m_panel->getOverlay().getName() );
			m_panel.reset();
		}
	}

	void DebugOverlays::MainDebugPanel::update()
	{
		m_times->update();
		m_fps->update();
		m_counts->update();
	}

	void DebugOverlays::MainDebugPanel::setVisible( bool visible )
	{
		m_panel->setVisible( visible );
	}

	void DebugOverlays::MainDebugPanel::updatePosition()
	{
		int y = m_times->updatePosition( 0 );
		y = m_fps->updatePosition( y );
		y = m_counts->updatePosition( y );
		m_panel->setPixelSize( Size{ 320, uint32_t( y ) } );
	}

	void DebugOverlays::MainDebugPanel::addTimePanel( castor::String const & name
		, castor::String const & label
		, castor::Nanoseconds const & value )
	{
		m_times->add( name
			, label
			, value );
	}

	void DebugOverlays::MainDebugPanel::addCountPanel( castor::String const & name
		, castor::String const & label
		, uint32_t const & value )
	{
		m_counts->add( name
			, label
			, value );
	}

	void DebugOverlays::MainDebugPanel::addFpsPanel( castor::String const & name
		, castor::String const & label
		, float const & value )
	{
		m_fps->add( name
			, label
			, value );
	}

	//*********************************************************************************************

	DebugOverlays::RenderPassOverlays::RenderPassOverlays( castor::String const & category
		, uint32_t index
		, OverlayCache & cache )
		: m_cache{ cache }
	{
		auto baseName = cuT( "RenderPassOverlays-" ) + category;
		m_panel = cache.add( baseName
			, OverlayType::ePanel
			, nullptr
			, nullptr )->getPanelOverlay();
		m_titlePanel = cache.add( baseName + cuT( "_Title" )
			, OverlayType::ePanel
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getPanelOverlay();
		m_titleText = cache.add( baseName + cuT( "_TitleText" )
			, OverlayType::eText
			, nullptr
			, m_titlePanel->getOverlay().shared_from_this() )->getTextOverlay();
		m_cpuName = cache.add( baseName + cuT( "_CPUName" )
			, OverlayType::eText
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getTextOverlay();
		m_cpuValue = cache.add( baseName + cuT( "_CPUValue" )
			, OverlayType::eText
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getTextOverlay();
		m_gpuName = cache.add( baseName + cuT( "_GPUName" )
			, OverlayType::eText
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getTextOverlay();
		m_gpuValue = cache.add( baseName + cuT( "_GPUValue" )
			, OverlayType::eText
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getTextOverlay();

		m_panel->setPixelPosition( Position{ 330, int32_t( 40 * index ) } );
		m_titlePanel->setPixelPosition( Position{ 0, 0 } );
		m_titleText->setPixelPosition( Position{ 10, 0 } );
		m_cpuName->setPixelPosition( Position{ 10, 20 } );
		m_cpuValue->setPixelPosition( Position{ 45, 20 } );
		m_gpuName->setPixelPosition( Position{ 130, 20 } );
		m_gpuValue->setPixelPosition( Position{ 165, 20 } );

		m_panel->setPixelSize( Size{ 250, 40 } );
		m_titlePanel->setPixelSize( Size{ 250, 20 } );
		m_titleText->setPixelSize( Size{ 230, 20 } );
		m_cpuName->setPixelSize( Size{ 30, 20 } );
		m_cpuValue->setPixelSize( Size{ 75, 20 } );
		m_gpuName->setPixelSize( Size{ 30, 20 } );
		m_gpuValue->setPixelSize( Size{ 75, 20 } );

		m_titleText->setFont( cuT( "Arial20" ) );
		m_cpuName->setFont( cuT( "Arial10" ) );
		m_gpuName->setFont( cuT( "Arial10" ) );
		m_cpuValue->setFont( cuT( "Arial10" ) );
		m_gpuValue->setFont( cuT( "Arial10" ) );

		m_titleText->setCaption( category );
		m_cpuName->setCaption( cuT( "CPU:" ) );
		m_gpuName->setCaption( cuT( "GPU:" ) );

		auto & materials = cache.getEngine()->getMaterialCache();
		m_panel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
		m_titlePanel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
		m_titleText->setMaterial( materials.find( cuT( "White" ) ) );
		m_cpuName->setMaterial( materials.find( cuT( "White" ) ) );
		m_gpuName->setMaterial( materials.find( cuT( "White" ) ) );
		m_cpuValue->setMaterial( materials.find( cuT( "White" ) ) );
		m_gpuValue->setMaterial( materials.find( cuT( "White" ) ) );

		m_titleText->setVAlign( VAlign::eCenter );
		m_titleText->setHAlign( HAlign::eCenter );

		m_panel->setVisible( true );
		m_titlePanel->setVisible( true );
		m_titleText->setVisible( true );
		m_cpuName->setVisible( true );
		m_gpuName->setVisible( true );
		m_cpuValue->setVisible( true );
		m_gpuValue->setVisible( true );
	}

	DebugOverlays::RenderPassOverlays::~RenderPassOverlays()
	{
		if ( m_cpuName )
		{
			m_cache.remove( m_cpuName->getOverlayName() );
			m_cache.remove( m_gpuName->getOverlayName() );
			m_cache.remove( m_gpuValue->getOverlayName() );
			m_cache.remove( m_cpuValue->getOverlayName() );
			m_cache.remove( m_titleText->getOverlayName() );
			m_cache.remove( m_titlePanel->getOverlayName() );
			m_cache.remove( m_panel->getOverlayName() );
		}
	}

	void DebugOverlays::RenderPassOverlays::addTimer( RenderPassTimer & timer )
	{
		m_timers.emplace_back( std::ref( timer ) );
	}

	bool DebugOverlays::RenderPassOverlays::removeTimer( RenderPassTimer & timer )
	{
		auto it = std::find_if( m_timers.begin()
			, m_timers.end()
			, [&timer]( auto const & lookup )
		{
			return &lookup.get() == &timer;
		} );

		if ( it != m_timers.end() )
		{
			m_timers.erase( it );
		}

		return m_timers.empty();
	}

	Nanoseconds DebugOverlays::RenderPassOverlays::update()
	{
		Nanoseconds cpu{ 0 };
		Nanoseconds gpu{ 0 };

		for ( auto & timer : m_timers )
		{
			cpu += timer.get().getCpuTime();
			gpu += timer.get().getGpuTime();
			timer.get().reset();
		}

		m_cpuValue->setCaption( StringStream{} << cpu );
		m_gpuValue->setCaption( StringStream{} << gpu );
		return gpu;
	}

	void DebugOverlays::RenderPassOverlays::setVisible( bool visible )
	{
		m_panel->setVisible( visible );
	}

	//*********************************************************************************************

	DebugOverlays::DebugOverlays( Engine & engine )
		: OwnedBy< Engine >( engine )
	{
	}

	DebugOverlays::~DebugOverlays()
	{
	}

	void DebugOverlays::initialise( OverlayCache & cache )
	{
		doCreateDebugPanel( cache );
	}

	void DebugOverlays::cleanup()
	{
		m_debugPanel.reset();
	}

	RenderInfo & DebugOverlays::beginFrame()
	{
		if ( m_visible )
		{
			m_gpuTime = 0_ms;
			m_cpuTime = 0_ms;
			m_taskTimer.getElapsed();
		}

		m_externalTime = m_frameTimer.getElapsed();
		m_renderInfo = RenderInfo{};
		return m_renderInfo;
	}

	uint32_t DebugOverlays::registerTimer( RenderPassTimer & timer )
	{
		auto & cache = getEngine()->getOverlayCache();
		auto fullName = getFullName( timer );
		auto it = m_renderPasses.find( fullName );

		if ( it == m_renderPasses.end() )
		{
			it = m_renderPasses.emplace( fullName
				, RenderPassOverlays{ fullName
					, uint32_t( m_renderPasses.size() )
					, cache } ).first;
			it->second.setVisible( m_visible );
		}

		it->second.addTimer( std::ref( timer ) );
		return m_queriesCount++;
	}

	void DebugOverlays::unregisterTimer( RenderPassTimer & timer )
	{
		auto fullName = getFullName( timer );
		auto it = m_renderPasses.find( fullName );

		if ( it != m_renderPasses.end() )
		{
			if ( it->second.removeTimer( timer ) )
			{
				m_renderPasses.erase( it );
			}
		}
	}

	void DebugOverlays::endFrame()
	{
		m_totalTime = m_frameTimer.getElapsed() + m_externalTime;

		if ( m_visible )
		{
			m_framesTimes[m_frameIndex] = m_totalTime;
			m_averageTime = std::accumulate( m_framesTimes.begin(), m_framesTimes.end(), 0_ns ) / m_framesTimes.size();
			m_averageFps = 1000000.0f / std::chrono::duration_cast< std::chrono::microseconds >( m_averageTime ).count();
			m_fps = 1000000.0f / std::chrono::duration_cast< std::chrono::microseconds >( m_totalTime ).count();
			m_gpuTotalTime = 0_ns;

			for ( auto & pass : m_renderPasses )
			{
				m_gpuTotalTime += pass.second.update();
			}

			m_gpuClientTime = m_gpuTime - m_gpuTotalTime;
			m_debugPanel->update();
			getEngine()->getRenderSystem()->resetGpuTime();

			m_frameIndex = ++m_frameIndex % FRAME_SAMPLES_COUNT;
			m_frameTimer.getElapsed();
		}

#if defined( NDEBUG )

		auto total = std::chrono::duration_cast< std::chrono::microseconds >( m_totalTime );
		fprintf( stdout
			, "\r%0.7f ms, %0.7f fps"
			, total.count() / 1000.0f
			, ( 1000000.0_r / total.count() ) );

#else

		std::cout << "\rTime: " << std::setw( 7 ) << m_totalTime;
		std::cout << " - FPS: " << std::setw( 7 ) << std::setprecision( 4 ) << ( 1000000.0_r / std::chrono::duration_cast< std::chrono::microseconds >( m_totalTime ).count() );

#endif
	}

	void DebugOverlays::endGpuTask()
	{
		if ( m_visible )
		{
			m_gpuTime += m_taskTimer.getElapsed();
		}
	}

	void DebugOverlays::endCpuTask()
	{
		if ( m_visible )
		{
			m_cpuTime += m_taskTimer.getElapsed();
		}
	}

	void DebugOverlays::show( bool show )
	{
		m_visible = show;
		m_debugPanel->setVisible( m_visible );

		for ( auto & pass : m_renderPasses )
		{
			pass.second.setVisible( m_visible );
		}
	}

	void DebugOverlays::doCreateDebugPanel( OverlayCache & cache )
	{
		m_debugPanel = std::make_unique< MainDebugPanel >( cache );
		m_debugPanel->addTimePanel( cuT( "CpuTime" )
			, cuT( "CPU Time:" )
			, m_cpuTime );
		m_debugPanel->addTimePanel( cuT( "GpuClientTime" )
			, cuT( "GPU Client Time:" )
			, m_gpuClientTime );
		m_debugPanel->addTimePanel( cuT( "GpuServerTime" )
			, cuT( "GPU Server Time:" )
			, m_gpuTotalTime );
		m_debugPanel->addTimePanel( cuT( "ExternalTime" )
			, cuT( "External Time:" )
			, m_externalTime );
		m_debugPanel->addTimePanel( cuT( "TotalTime" )
			, cuT( "Total Time:" )
			, m_totalTime );
		m_debugPanel->addTimePanel( cuT( "AverageTime" )
			, cuT( "Average Time:" )
			, m_averageTime );
		m_debugPanel->addFpsPanel( cuT( "FPS" )
			, cuT( "FPS:" )
			, m_fps );
		m_debugPanel->addFpsPanel( cuT( "AverageFPS" )
			, cuT( "Average FPS:" )
			, m_averageFps );
		m_debugPanel->addCountPanel( cuT( "TotalVertexCount" )
			, cuT( "Vertices Count:" )
			, m_renderInfo.m_totalVertexCount );
		m_debugPanel->addCountPanel( cuT( "TotalFaceCount" )
			, cuT( "Faces Count:" )
			, m_renderInfo.m_totalFaceCount );
		m_debugPanel->addCountPanel( cuT( "TotalObjectCount" )
			, cuT( "Objects Count:" )
			, m_renderInfo.m_totalObjectsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleVertexCount" )
			, cuT( "Visible Vertices Count:" )
			, m_renderInfo.m_visibleVertexCount );
		m_debugPanel->addCountPanel( cuT( "VisibleFaceCount" )
			, cuT( "Visible Faces Count:" )
			, m_renderInfo.m_visibleFaceCount );
		m_debugPanel->addCountPanel( cuT( "VisibleObjectCount" )
			, cuT( "Visible Objects Count:" )
			, m_renderInfo.m_visibleObjectsCount );
		m_debugPanel->addCountPanel( cuT( "ParticlesCount" )
			, cuT( "Particles Count:" )
			, m_renderInfo.m_particlesCount );
		m_debugPanel->addCountPanel( cuT( "LightCount" )
			, cuT( "Lights Count:" )
			, m_renderInfo.m_totalLightsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleLightCount" )
			, cuT( "Visible Lights Count:" )
			, m_renderInfo.m_visibleLightsCount );
		m_debugPanel->addCountPanel( cuT( "DrawCalls" )
			, cuT( "Draw calls:" )
			, m_renderInfo.m_drawCalls );
		m_debugPanel->updatePosition();
		m_debugPanel->setVisible( m_visible );

		getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
			, [this]()
			{
				m_queries = getEngine()->getRenderSystem()->getCurrentDevice()->createQueryPool( renderer::QueryType::eTimestamp
					, m_queriesCount
					, 0u );
			} ) );
	}

	//*********************************************************************************************
}
