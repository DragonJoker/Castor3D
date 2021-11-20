#include "Castor3D/Overlay/DebugOverlays.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Core/Device.hpp>
#include <ashespp/Miscellaneous/QueryPool.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <iomanip>

CU_ImplementCUSmartPtr( castor3d, DebugOverlays )

namespace castor3d
{
	//*********************************************************************************************

	DebugOverlays::MainDebugPanel::MainDebugPanel( OverlayCache & cache )
		: m_cache{ cache }
		, m_panel{ m_cache.add( cuT( "MainDebugPanel" )
			, cache.getEngine()
			, OverlayType::ePanel
			, nullptr
			, nullptr ).lock()->getPanelOverlay() }
		, m_times{ std::make_unique< DebugPanelsT< castor::Nanoseconds > >( cuT( "Times" ), m_panel, cache ) }
		, m_fps{ std::make_unique< DebugPanelsT< float > >( cuT( "FPS" ), m_panel, cache ) }
		, m_counts{ std::make_unique< DebugPanelsT< uint32_t > >( cuT( "Counts" ), m_panel, cache ) }
	{
		auto & materials = m_cache.getEngine().getMaterialCache();
		m_panel->setPixelPosition( castor::Position{ 0, 0 } );
		m_panel->setPixelSize( castor::Size{ 320, 20 } );
		m_panel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ).lock().get() );
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
		m_panel->setPixelSize( castor::Size{ 320, uint32_t( y ) } );
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

	DebugOverlays::PassOverlays::PassOverlays( OverlayCache & cache
		, OverlayRPtr parent
		, castor::String const & category
		, castor::String const & name
		, uint32_t index
		, bool const & detailed )
		: m_cache{ cache }
		, m_detailed{ detailed }
		, m_name{ name }
	{
		auto baseName = cuT( "RenderPassOverlays-" ) + category + cuT( "-" ) + name;
		m_panel = cache.add( baseName
			, cache.getEngine()
			, OverlayType::ePanel
			, nullptr
			, parent ).lock()->getPanelOverlay();
		m_passName = cache.add( baseName + cuT( "_PassName" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_panel->getOverlay() ).lock()->getTextOverlay();
		m_cpu.name = cache.add( baseName + cuT( "_CPUName" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_panel->getOverlay() ).lock()->getTextOverlay();
		m_cpu.value = cache.add( baseName + cuT( "_CPUValue" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_panel->getOverlay() ).lock()->getTextOverlay();
		m_gpu.name = cache.add( baseName + cuT( "_GPUName" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_panel->getOverlay() ).lock()->getTextOverlay();
		m_gpu.value = cache.add( baseName + cuT( "_GPUValue" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_panel->getOverlay() ).lock()->getTextOverlay();

		m_panel->setPixelPosition( castor::Position{ 0, 20 + int32_t( 20 * index ) } );
		auto posX = 0;
		m_passName->setPixelPosition( castor::Position{ posX, 0 } );
		m_passName->setPixelSize( castor::Size{ 250, 20 } );
		posX += 250;
		m_cpu.name->setPixelPosition( castor::Position{ posX, 0 } );
		m_cpu.name->setPixelSize( castor::Size{ 30, 20 } );
		posX += 30;
		m_cpu.value->setPixelPosition( castor::Position{ posX, 0 } );
		m_cpu.value->setPixelSize( castor::Size{ 75, 20 } );
		posX += 75;
		m_gpu.name->setPixelPosition( castor::Position{ posX, 0 } );
		m_gpu.name->setPixelSize( castor::Size{ 30, 20 } );
		posX += 30;
		m_gpu.value->setPixelPosition( castor::Position{ posX, 0 } );
		m_gpu.value->setPixelSize( castor::Size{ 75, 20 } );
		posX += 75;
		m_panel->setPixelSize( castor::Size{ uint32_t( posX ), 20 } );

		m_passName->setFont( cuT( "Arial20" ) );
		m_cpu.name->setFont( cuT( "Arial10" ) );
		m_gpu.name->setFont( cuT( "Arial10" ) );
		m_cpu.value->setFont( cuT( "Arial10" ) );
		m_gpu.value->setFont( cuT( "Arial10" ) );

		m_passName->setCaption( name );
		m_cpu.name->setCaption( cuT( "CPU:" ) );
		m_gpu.name->setCaption( cuT( "GPU:" ) );

		auto & materials = cache.getEngine().getMaterialCache();
		m_panel->setMaterial( materials.find( cuT( "TransparentBlack" ) ).lock().get() );
		m_passName->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_cpu.name->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_gpu.name->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_cpu.value->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_gpu.value->setMaterial( materials.find( cuT( "White" ) ).lock().get() );

		m_passName->setVAlign( VAlign::eCenter );
		m_passName->setHAlign( HAlign::eLeft );

		m_panel->setVisible( m_detailed );
		m_passName->setVisible( m_detailed );
		m_cpu.name->setVisible( m_detailed );
		m_gpu.name->setVisible( m_detailed );
		m_cpu.value->setVisible( m_detailed );
		m_gpu.value->setVisible( m_detailed );
	}

	DebugOverlays::PassOverlays::~PassOverlays()
	{
		if ( m_cpu.name )
		{
			m_cache.remove( m_cpu.name->getOverlayName() );
			m_cache.remove( m_gpu.name->getOverlayName() );
			m_cache.remove( m_gpu.value->getOverlayName() );
			m_cache.remove( m_cpu.value->getOverlayName() );
			m_cache.remove( m_passName->getOverlayName() );
			m_cache.remove( m_panel->getOverlayName() );
		}
	}

	void DebugOverlays::PassOverlays::update()
	{
		m_cpu.time = 0_ns;
		m_gpu.time = 0_ns;

		for ( auto & timer : m_timers )
		{
			m_cpu.time += timer.first->getCpuTime();
			m_gpu.time += timer.first->getGpuTime();
			timer.first->reset();
		}

		m_cpu.value->setCaption( castor::string::toString( m_cpu.time ) );
		m_gpu.value->setCaption( castor::string::toString( m_gpu.time ) );

		m_panel->setVisible( m_detailed );
		m_passName->setVisible( m_detailed );
		m_cpu.name->setVisible( m_detailed );
		m_gpu.name->setVisible( m_detailed );
		m_cpu.value->setVisible( m_detailed );
		m_gpu.value->setVisible( m_detailed );
	}

	void DebugOverlays::PassOverlays::retrieveGpuTime()
	{
		for ( auto & timer : m_timers )
		{
			timer.first->retrieveGpuTime();
		}
	}

	void DebugOverlays::PassOverlays::addTimer( FramePassTimer & timer )
	{
		m_timers.emplace( &timer
			, timer.onDestroy.connect( [this]( FramePassTimer & obj )
				{
					removeTimer( obj );
				} ) );
	}

	bool DebugOverlays::PassOverlays::removeTimer( FramePassTimer & timer )
	{
		auto it = m_timers.find( &timer );

		if ( it != m_timers.end() )
		{
			m_timers.erase( it );
		}

		return m_timers.empty();
	}

	//*********************************************************************************************

	DebugOverlays::CategoryOverlays::CategoryOverlays()
	{
	}

	DebugOverlays::CategoryOverlays::CategoryOverlays( castor::String const & category
		, OverlayCache & cache
		, bool const & detailed )
		: m_cache{ &cache }
		, m_detailed{ &detailed }
		, m_categoryName{ category }
	{
		auto baseName = cuT( "RenderPassOverlays-" ) + m_categoryName;
		m_container = cache.add( baseName
			, cache.getEngine()
			, OverlayType::ePanel
			, nullptr
			, nullptr ).lock()->getPanelOverlay();
		m_firstLinePanel = cache.add( baseName + cuT( "_TitlePanel" )
			, cache.getEngine()
			, OverlayType::ePanel
			, nullptr
			, &m_container->getOverlay() ).lock()->getPanelOverlay();
		m_name = cache.add( baseName + cuT( "_TitleName" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_container->getOverlay() ).lock()->getTextOverlay();
		m_cpu.name = cache.add( baseName + cuT( "_CPUName" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_container->getOverlay() ).lock()->getTextOverlay();
		m_cpu.value = cache.add( baseName + cuT( "_CPUValue" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_container->getOverlay() ).lock()->getTextOverlay();
		m_gpu.name = cache.add( baseName + cuT( "_GPUName" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_container->getOverlay() ).lock()->getTextOverlay();
		m_gpu.value = cache.add( baseName + cuT( "_GPUValue" )
			, cache.getEngine()
			, OverlayType::eText
			, nullptr
			, &m_container->getOverlay() ).lock()->getTextOverlay();

		m_container->setPixelPosition( castor::Position{ 330, 0 } );
		m_firstLinePanel->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_name->setPixelPosition( castor::Position{ 0, 0 } );
		m_name->setPixelSize( castor::Size{ 250, 20 } );
		m_posX += 250;
		m_cpu.name->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_cpu.name->setPixelSize( castor::Size{ 30, 20 } );
		m_posX += 30;
		m_cpu.value->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_cpu.value->setPixelSize( castor::Size{ 75, 20 } );
		m_posX += 75;
		m_gpu.name->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_gpu.name->setPixelSize( castor::Size{ 30, 20 } );
		m_posX += 30;
		m_gpu.value->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_gpu.value->setPixelSize( castor::Size{ 75, 20 } );
		m_posX += 75;
		m_container->setPixelSize( castor::Size{ uint32_t( m_posX ), 20 } );
		m_firstLinePanel->setPixelSize( castor::Size{ uint32_t( m_posX ), 20 } );

		m_name->setFont( cuT( "Arial20" ) );
		m_cpu.name->setFont( cuT( "Arial10" ) );
		m_gpu.name->setFont( cuT( "Arial10" ) );
		m_cpu.value->setFont( cuT( "Arial10" ) );
		m_gpu.value->setFont( cuT( "Arial10" ) );

		m_name->setCaption( m_categoryName );
		m_cpu.name->setCaption( cuT( "CPU:" ) );
		m_gpu.name->setCaption( cuT( "GPU:" ) );

		auto & materials = cache.getEngine().getMaterialCache();
		m_container->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ).lock().get() );
		m_firstLinePanel->setMaterial( materials.find( cuT( "AlphaDarkBlue" ) ).lock().get() );
		m_name->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_cpu.name->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_gpu.name->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_cpu.value->setMaterial( materials.find( cuT( "White" ) ).lock().get() );
		m_gpu.value->setMaterial( materials.find( cuT( "White" ) ).lock().get() );

		m_name->setVAlign( VAlign::eCenter );
		m_name->setHAlign( HAlign::eLeft );

		m_container->setVisible( true );
		m_name->setVisible( true );
		m_cpu.name->setVisible( true );
		m_gpu.name->setVisible( true );
		m_cpu.value->setVisible( true );
		m_gpu.value->setVisible( true );
	}

	void DebugOverlays::CategoryOverlays::initialise( uint32_t offset )
	{
		if ( !m_container )
		{
			return;
		}

		m_container->setPixelPosition( castor::Position{ 330, int32_t( 20 * offset ) } );
		m_container->setPixelSize( castor::Size{ uint32_t( m_posX )
			, ( m_detailed ? getPanelCount() : 1u ) * 20u } );
	}

	DebugOverlays::CategoryOverlays::~CategoryOverlays()
	{
		m_passes.clear();

		if ( m_cpu.name )
		{
			m_cache->remove( m_cpu.name->getOverlayName() );
			m_cache->remove( m_gpu.name->getOverlayName() );
			m_cache->remove( m_gpu.value->getOverlayName() );
			m_cache->remove( m_cpu.value->getOverlayName() );
			m_cache->remove( m_name->getOverlayName() );
			m_cache->remove( m_firstLinePanel->getOverlayName() );
			m_cache->remove( m_container->getOverlayName() );
		}
	}

	void DebugOverlays::CategoryOverlays::addTimer( FramePassTimer & timer )
	{
		auto it = std::find_if( m_passes.begin()
			, m_passes.end()
			, [&timer]( auto const & lookup )
			{
				return lookup->getName() == timer.getName();
			} );

		if ( it == m_passes.end() )
		{
			auto index = uint32_t( m_passes.size() );
			m_passes.emplace_back( std::make_unique< PassOverlays >( *m_cache
				, &m_container->getOverlay()
				, m_categoryName
				, timer.getName()
				, index
				, *m_detailed ) );
			it = std::next( m_passes.begin()
				, ptrdiff_t( m_passes.size() - 1 ) );
		}

		( *it )->addTimer( timer );
	}

	bool DebugOverlays::CategoryOverlays::removeTimer( FramePassTimer & timer )
	{
		auto it = std::find_if( m_passes.begin()
			, m_passes.end()
			, [&timer]( auto const & lookup )
			{
				return lookup->getName() == timer.getName();
			} );

		if ( it != m_passes.end() )
		{
			( *it )->removeTimer( timer );
		}

		return m_passes.empty();
	}

	void DebugOverlays::CategoryOverlays::update()
	{
		m_cpu.time = 0_ns;
		m_gpu.time = 0_ns;

		for ( auto & pass : m_passes )
		{
			if ( pass )
			{
				pass->update();
				m_cpu.time += pass->getCpuTime();
				m_gpu.time += pass->getGpuTime();
			}
		}

		m_cpu.value->setCaption( castor::string::toString( m_cpu.time ) );
		m_gpu.value->setCaption( castor::string::toString( m_gpu.time ) );
	}

	void DebugOverlays::CategoryOverlays::retrieveGpuTime()
	{
		for ( auto & pass : m_passes )
		{
			if ( pass )
			{
				pass->retrieveGpuTime();
			}
		}
	}

	void DebugOverlays::CategoryOverlays::setVisible( bool visible )
	{
		m_container->setVisible( visible );
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
		if ( m_totalTime > 0_ns )
		{
			log::info << "Counts:\n"
				<< "  Average Frame Time: " << ( float( std::chrono::duration_cast< castor::Microseconds >( m_averageTime ).count() ) / 1000.0f ) << " ms\n"
				<< "  Average Frames per second: " <<  m_averageFps << std::endl;
		}

		m_debugPanel.reset();
	}

	RenderInfo & DebugOverlays::beginFrame()
	{
		if ( m_visible )
		{
			m_gpuTime = 0_ns;
			m_cpuTime = 0_ns;
			m_taskTimer.getElapsed();
		}

		m_renderInfo = RenderInfo{};

		if ( m_dirty )
		{
			uint32_t offset = 0u;

			for ( auto & renderPass : m_renderPasses )
			{
				renderPass.second.initialise( offset );
				offset += ( m_detailed
					? renderPass.second.getPanelCount()
					: 1u );
			}
		}

		m_externalTime = m_frameTimer.getElapsed();
		return m_renderInfo;
	}

	uint32_t DebugOverlays::registerTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		auto & cache = getEngine()->getOverlayCache();
		auto ires = m_renderPasses.emplace( category, CategoryOverlays{} );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second = CategoryOverlays{ category, cache, m_detailed };
			it->second.setVisible( m_visible );
		}

		it->second.addTimer( timer );
		m_dirty = true;
		return m_queriesCount++;
	}

	void DebugOverlays::unregisterTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		m_dirty = true;
		auto it = m_renderPasses.find( category );

		if ( it != m_renderPasses.end() )
		{
			if ( it->second.removeTimer( timer ) )
			{
				m_renderPasses.erase( it );
			}
		}
	}

	castor::Microseconds DebugOverlays::endFrame( bool first )
	{
		m_totalTime = m_frameTimer.getElapsed() + m_externalTime;

		if ( !first )
		{
			 // Prevent initialisation frame from being counted in average time.
			++m_frameCount;
			m_framesTimes[m_frameIndex] = m_totalTime;
			m_averageTime = std::accumulate( m_framesTimes.begin()
				, m_framesTimes.begin() + ptrdiff_t( std::min( m_frameCount, m_framesTimes.size() ) )
				, 0_ns ) / m_framesTimes.size();
			m_averageFps = 1000000.0f / float( std::chrono::duration_cast< castor::Microseconds >( m_averageTime ).count() );
			auto v = ( ++m_frameIndex ) % FRAME_SAMPLES_COUNT;
			m_frameIndex = v;
		}

		auto total = std::chrono::duration_cast< castor::Microseconds >( m_totalTime );
		m_fps = 1000000.0f / float( total.count() );

		if ( m_visible )
		{
			m_gpuTotalTime = 0_ns;
			m_gpuClientTime = 0_ns;

			for ( auto & pass : m_renderPasses )
			{
				pass.second.update();
				m_gpuTotalTime += pass.second.getGpuTime();
				m_gpuClientTime += pass.second.getCpuTime();
			}

			m_debugPanel->update();
			getEngine()->getRenderSystem()->resetGpuTime();
		}

		fprintf( stdout
			, "\r%0.2f ms, %0.2f fps                           "
			, float( total.count() ) / 1000.0f
			, m_fps );
		m_frameTimer.getElapsed();

		return total;
	}

	void DebugOverlays::endGpuTask()
	{
		for ( auto & renderPass : m_renderPasses )
		{
			renderPass.second.retrieveGpuTime();
		}

		m_gpuTime += m_taskTimer.getElapsed();
	}

	void DebugOverlays::endCpuTask()
	{
		m_cpuTime += m_taskTimer.getElapsed();
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
	}

	//*********************************************************************************************
}
