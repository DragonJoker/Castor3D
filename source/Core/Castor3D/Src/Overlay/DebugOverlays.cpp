#include "DebugOverlays.hpp"

#include "Engine.hpp"
#include "Overlay/PanelOverlay.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Render/RenderPassTimer.hpp"

#include <iomanip>

//*********************************************************************************************

namespace std
{
	std::ostream & operator<<( std::ostream & p_stream, castor::Nanoseconds const & p_duration )
	{
		p_stream << std::setprecision( 3 ) << ( p_duration.count() / 1000000.0_r ) << cuT( " ms" );
		return p_stream;
	}
}

//*********************************************************************************************

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

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

	//*********************************************************************************************

	DebugOverlays::DebugPanel::DebugPanel( String const & name
		, String const & label
		, uint32_t index
		, OverlayCache & cache )
		: m_cache{ cache }
	{
		auto baseName = cuT( "DebugPanel-" ) + name;
		m_panel = cache.add( baseName
			, OverlayType::ePanel
			, nullptr
			, nullptr )->getPanelOverlay();
		m_label = cache.add( baseName + cuT( "-Label" )
			, OverlayType::eText
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getTextOverlay();
		m_value = cache.add( baseName + cuT( "-Value" )
			, OverlayType::eText
			, nullptr
			, m_panel->getOverlay().shared_from_this() )->getTextOverlay();

		m_panel->setPixelPosition( Position{ 0, int( 20 * index ) } );
		m_label->setPixelPosition( Position{ 10, 0 } );
		m_value->setPixelPosition( Position{ 200, 0 } );

		m_panel->setPixelSize( Size{ 320, 20 } );
		m_label->setPixelSize( Size{ 190, 20 } );
		m_value->setPixelSize( Size{ 110, 20 } );

		m_label->setFont( cuT( "Arial20" ) );
		m_value->setFont( cuT( "Arial20" ) );

		m_label->setVAlign( VAlign::eCenter );
		m_value->setVAlign( VAlign::eCenter );

		auto & materials = cache.getEngine()->getMaterialCache();
		m_panel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
		m_label->setMaterial( materials.find( cuT( "White" ) ) );
		m_value->setMaterial( materials.find( cuT( "White" ) ) );

		m_label->setCaption( label );
	}

	DebugOverlays::DebugPanel::~DebugPanel()
	{
		m_cache.remove( m_value->getOverlay().getName() );
		m_cache.remove( m_label->getOverlay().getName() );
		m_cache.remove( m_panel->getOverlay().getName() );
		m_value.reset();
		m_label.reset();
		m_panel.reset();
	}

	void DebugOverlays::DebugPanel::setVisible( bool visible )
	{
		m_panel->setVisible( visible );
	}

	//*********************************************************************************************

	DebugOverlays::TimeDebugPanel::TimeDebugPanel( String const & name
		, String const & label
		, uint32_t index
		, OverlayCache & cache
		, Nanoseconds const & value )
		: DebugPanel{ name, label, index, cache }
		, m_v{ value }
	{
	}

	void DebugOverlays::TimeDebugPanel::update()
	{
		m_value->setCaption( StringStream{} << m_v );
	}

	//*********************************************************************************************

	DebugOverlays::CountDebugPanel::CountDebugPanel( String const & name
		, String const & label
		, uint32_t index
		, OverlayCache & cache
		, uint32_t const & value )
		: DebugPanel{ name, label, index, cache }
		, m_v{ value }
	{
	}

	void DebugOverlays::CountDebugPanel::update()
	{
		m_value->setCaption( StringStream{} << m_v );
	}

	//*********************************************************************************************

	DebugOverlays::FloatDebugPanel::FloatDebugPanel( String const & name
		, String const & label
		, uint32_t index
		, OverlayCache & cache
		, float const & value )
		: DebugPanel{ name, label, index, cache }
		, m_v{ value }
	{
	}

	void DebugOverlays::FloatDebugPanel::update()
	{
		m_value->setCaption( StringStream{} << std::setprecision( 4 ) << m_v );
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
		m_title = cache.add( baseName + cuT( "_TitleText" )
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
		m_panel->setPixelPosition( Position{ 400, int32_t( 40 * index ) } );
		m_titlePanel->setPixelPosition( Position{ 0, 0 } );
		m_title->setPixelPosition( Position{ 10, 0 } );
		m_cpuName->setPixelPosition( Position{ 10, 20 } );
		m_cpuValue->setPixelPosition( Position{ 45, 20 } );
		m_gpuName->setPixelPosition( Position{ 130, 20 } );
		m_gpuValue->setPixelPosition( Position{ 165, 20 } );

		m_panel->setPixelSize( Size{ 250, 40 } );
		m_titlePanel->setPixelSize( Size{ 250, 20 } );
		m_title->setPixelSize( Size{ 230, 20 } );
		m_cpuName->setPixelSize( Size{ 30, 20 } );
		m_cpuValue->setPixelSize( Size{ 75, 20 } );
		m_gpuName->setPixelSize( Size{ 30, 20 } );
		m_gpuValue->setPixelSize( Size{ 75, 20 } );

		m_title->setFont( cuT( "Arial20" ) );
		m_cpuName->setFont( cuT( "Arial10" ) );
		m_gpuName->setFont( cuT( "Arial10" ) );
		m_cpuValue->setFont( cuT( "Arial10" ) );
		m_gpuValue->setFont( cuT( "Arial10" ) );

		m_title->setCaption( category );
		m_cpuName->setCaption( cuT( "CPU:" ) );
		m_gpuName->setCaption( cuT( "GPU:" ) );

		auto & materials = cache.getEngine()->getMaterialCache();
		m_panel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
		m_titlePanel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ) );
		m_title->setMaterial( materials.find( cuT( "White" ) ) );
		m_cpuName->setMaterial( materials.find( cuT( "White" ) ) );
		m_gpuName->setMaterial( materials.find( cuT( "White" ) ) );
		m_cpuValue->setMaterial( materials.find( cuT( "White" ) ) );
		m_gpuValue->setMaterial( materials.find( cuT( "White" ) ) );

		m_panel->setVisible( true );
		m_titlePanel->setVisible( true );
		m_title->setVisible( true );
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
			m_cache.remove( m_title->getOverlayName() );
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
		m_debugPanels.clear();
	}

	void DebugOverlays::beginFrame()
	{
		if ( m_visible )
		{
			m_gpuTime = 0_ms;
			m_cpuTime = 0_ms;
			m_taskTimer.getElapsed();
		}

		m_externalTime = m_frameTimer.getElapsed();
	}

	void DebugOverlays::registerTimer( RenderPassTimer & timer )
	{
		auto & cache = getEngine()->getOverlayCache();
		auto it = m_renderPasses.find( timer.getCategory() );

		if ( it == m_renderPasses.end() )
		{
			it = m_renderPasses.emplace( timer.getCategory()
				, RenderPassOverlays{ timer.getCategory()
					, uint32_t( m_renderPasses.size() )
					, cache } ).first;
			it->second.setVisible( m_visible );
		}

		it->second.addTimer( std::ref( timer ) );
	}

	void DebugOverlays::unregisterTimer( RenderPassTimer & timer )
	{
		auto it = m_renderPasses.find( timer.getCategory() );

		if ( it != m_renderPasses.end() )
		{
			if ( it->second.removeTimer( timer ) )
			{
				m_renderPasses.erase( it );
			}
		}
	}

	void DebugOverlays::endFrame( RenderInfo const & info )
	{
		m_totalTime = m_frameTimer.getElapsed() + m_externalTime;

		if ( m_visible )
		{
			m_framesTimes[m_frameIndex] = m_totalTime;
			m_averageTime = std::accumulate( m_framesTimes.begin(), m_framesTimes.end(), 0_ns ) / m_framesTimes.size();
			m_averageFps = 1000000.0f / std::chrono::duration_cast< std::chrono::microseconds >( m_averageTime ).count();
			m_gpuTotalTime = 0_ns;
			m_totalVertexCount = info.m_totalVertexCount;
			m_totalFaceCount = info.m_totalFaceCount;
			m_totalObjectsCount = info.m_totalObjectsCount;
			m_visibleObjectsCount = info.m_visibleObjectsCount;
			m_particlesCount = info.m_particlesCount;
			m_totalLightsCount = info.m_totalLightsCount;
			m_visibleLightsCount = info.m_visibleLightsCount;

			for ( auto & pass : m_renderPasses )
			{
				m_gpuTotalTime += pass.second.update();
			}

			m_gpuClientTime = m_gpuTime - m_gpuTotalTime;

			for ( auto & panel : m_debugPanels )
			{
				panel->update();
			}


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

		for ( auto & panel : m_debugPanels )
		{
			panel->setVisible( m_visible );
		}

		for ( auto & pass : m_renderPasses )
		{
			pass.second.setVisible( m_visible );
		}
	}

	void DebugOverlays::doCreateDebugPanel( OverlayCache & cache )
	{
		m_debugPanels.push_back( std::make_unique< TimeDebugPanel >( cuT( "CpuTime" )
			, cuT( "CPU Time:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_cpuTime ) );
		m_debugPanels.push_back( std::make_unique< TimeDebugPanel >( cuT( "GpuClientTime" )
			, cuT( "GPU Client Time:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_gpuClientTime ) );
		m_debugPanels.push_back( std::make_unique< TimeDebugPanel >( cuT( "GpuServerTime" )
			, cuT( "GPU Server Time:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_gpuTotalTime ) );
		m_debugPanels.push_back( std::make_unique< TimeDebugPanel >( cuT( "ExternalTime" )
			, cuT( "External Time:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_externalTime ) );
		m_debugPanels.push_back( std::make_unique< TimeDebugPanel >( cuT( "TotalTime" )
			, cuT( "Total Time:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_totalTime ) );
		m_debugPanels.push_back( std::make_unique< FloatDebugPanel >( cuT( "AverageFPS" )
			, cuT( "Average FPS:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_averageFps ) );
		m_debugPanels.push_back( std::make_unique< TimeDebugPanel >( cuT( "AverageTime" )
			, cuT( "Average Time:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_averageTime ) );
		m_debugPanels.push_back( std::make_unique< CountDebugPanel >( cuT( "VertexCount" )
			, cuT( "Vertices Count:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_totalVertexCount ) );
		m_debugPanels.push_back( std::make_unique< CountDebugPanel >( cuT( "FaceCount" )
			, cuT( "Faces Count:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_totalFaceCount ) );
		m_debugPanels.push_back( std::make_unique< CountDebugPanel >( cuT( "ObjectCount" )
			, cuT( "Objects Count:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_totalObjectsCount ) );
		m_debugPanels.push_back( std::make_unique< CountDebugPanel >( cuT( "VisibleObjectCount" )
			, cuT( "Visible Objects Count:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_visibleObjectsCount ) );
		m_debugPanels.push_back( std::make_unique< CountDebugPanel >( cuT( "ParticlesCount" )
			, cuT( "Particles Count:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_particlesCount ) );
		m_debugPanels.push_back( std::make_unique< CountDebugPanel >( cuT( "LightCount" )
			, cuT( "Lights Count:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_totalLightsCount ) );
		m_debugPanels.push_back( std::make_unique< CountDebugPanel >( cuT( "VisibleLightCount" )
			, cuT( "Visible Lights Count:" )
			, uint32_t( m_debugPanels.size() )
			, cache
			, m_visibleLightsCount ) );

		for ( auto & panel : m_debugPanels )
		{
			panel->setVisible( m_visible );
		}
	}

	//*********************************************************************************************
}
