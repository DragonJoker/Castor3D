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

#pragma GCC diagnostic ignored "-Wuseless-cast"

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
			, nullptr
			, PanelBaseLevel ).lock()->getPanelOverlay() }
		, m_times{ std::make_unique< DebugPanelsT< castor::Nanoseconds > >( cuT( "Times" ), m_panel, cache ) }
		, m_fps{ std::make_unique< DebugPanelsT< float > >( cuT( "FPS" ), m_panel, cache ) }
		, m_counts{ std::make_unique< DebugPanelsT< uint32_t > >( cuT( "Counts" ), m_panel, cache ) }
	{
		m_panel->setPixelPosition( castor::Position{ 0, 0 } );
		m_panel->setPixelSize( castor::Size{ DebugPanelWidth, PanelHeight } );
		m_panel->setMaterial( m_cache.getEngine().findMaterial( cuT( "AlphaDarkBlue" ) ).lock().get() );
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
		m_panel->setPixelSize( castor::Size{ DebugPanelWidth, uint32_t( y ) } );
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

		m_panel->setPixelPosition( castor::Position{ PassMainPanelLeft, int32_t( PanelHeight + PanelHeight * index ) } );
		auto posX = 0;
		m_passName->setPixelPosition( castor::Position{ posX, 0 } );
		m_passName->setPixelSize( castor::Size{ PassNameWidth, PanelHeight } );
		posX += PassNameWidth;
		m_cpu.name->setPixelPosition( castor::Position{ posX, 0 } );
		m_cpu.name->setPixelSize( castor::Size{ CpuNameWidth, PanelHeight } );
		posX += CpuNameWidth;
		m_cpu.value->setPixelPosition( castor::Position{ posX, 0 } );
		m_cpu.value->setPixelSize( castor::Size{ CpuValueWidth, PanelHeight } );
		posX += CpuValueWidth;
		m_gpu.name->setPixelPosition( castor::Position{ posX, 0 } );
		m_gpu.name->setPixelSize( castor::Size{ GpuNameWidth, PanelHeight } );
		posX += GpuNameWidth;
		m_gpu.value->setPixelPosition( castor::Position{ posX, 0 } );
		m_gpu.value->setPixelSize( castor::Size{ GpuValueWidth, PanelHeight } );
		posX += GpuValueWidth;
		m_panel->setPixelSize( castor::Size{ uint32_t( posX ), PanelHeight } );

		m_passName->setFont( cuT( "Arial16" ) );
		m_cpu.name->setFont( cuT( "Arial10" ) );
		m_gpu.name->setFont( cuT( "Arial10" ) );
		m_cpu.value->setFont( cuT( "Arial10" ) );
		m_gpu.value->setFont( cuT( "Arial10" ) );

		m_passName->setCaption( castor::string::toU32String( name ) );
		m_cpu.name->setCaption( U"CPU:" );
		m_gpu.name->setCaption( U"GPU:" );

		auto & engine = cache.getEngine();
		m_panel->setMaterial( engine.findMaterial( cuT( "TransparentBlack" ) ).lock().get() );
		m_passName->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_cpu.name->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_gpu.name->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_cpu.value->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_gpu.value->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );

		m_passName->setVAlign( VAlign::eCenter );
		m_passName->setHAlign( HAlign::eLeft );

		m_panel->setVisible( true );
		m_passName->setVisible( true );
		m_cpu.name->setVisible( true );
		m_gpu.name->setVisible( true );
		m_cpu.value->setVisible( true );
		m_gpu.value->setVisible( true );
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

	void DebugOverlays::PassOverlays::compute()
	{
		m_cpu.time = 0_ns;
		m_gpu.time = 0_ns;

		for ( auto & timer : m_timers )
		{
			m_cpu.time += timer.first->getCpuTime();
			m_gpu.time += timer.first->getGpuTime();
			timer.first->reset();
		}
	}

	void DebugOverlays::PassOverlays::update( uint32_t & top )
	{
		if ( !m_cpu.value || !m_gpu.value )
		{
			return;
		}

		m_visible = m_cpu.time > 0_ns && m_gpu.time > 0_ns;

		if ( m_visible && m_detailed )
		{
			m_panel->setPixelPosition( castor::Position{ 0, int32_t( top ) } );
			m_cpu.value->setCaption( castor::string::toU32String( castor::string::toString( m_cpu.time ) ) );
			m_gpu.value->setCaption( castor::string::toU32String( castor::string::toString( m_gpu.time ) ) );
			top += PanelHeight;
		}

		m_panel->setVisible( m_visible && m_detailed );
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
			, nullptr
			, PanelBaseLevel ).lock()->getPanelOverlay();
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

		m_container->setPixelPosition( castor::Position{ PassPanelLeft, 0 } );
		m_firstLinePanel->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_name->setPixelPosition( castor::Position{ 0, 0 } );
		m_name->setPixelSize( castor::Size{ CategoryNameWidth, PanelHeight } );
		m_posX += CategoryNameWidth;
		m_cpu.name->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_cpu.name->setPixelSize( castor::Size{ CpuNameWidth, PanelHeight } );
		m_posX += CpuNameWidth;
		m_cpu.value->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_cpu.value->setPixelSize( castor::Size{ CpuValueWidth, PanelHeight } );
		m_posX += CpuValueWidth;
		m_gpu.name->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_gpu.name->setPixelSize( castor::Size{ GpuNameWidth, PanelHeight } );
		m_posX += GpuNameWidth;
		m_gpu.value->setPixelPosition( castor::Position{ m_posX, 0 } );
		m_gpu.value->setPixelSize( castor::Size{ GpuValueWidth, PanelHeight } );
		m_posX += GpuValueWidth;
		m_container->setPixelSize( castor::Size{ uint32_t( m_posX ), PanelHeight } );
		m_firstLinePanel->setPixelSize( castor::Size{ uint32_t( m_posX ), PanelHeight } );

		m_name->setFont( cuT( "Arial16" ) );
		m_cpu.name->setFont( cuT( "Arial10" ) );
		m_gpu.name->setFont( cuT( "Arial10" ) );
		m_cpu.value->setFont( cuT( "Arial10" ) );
		m_gpu.value->setFont( cuT( "Arial10" ) );

		m_name->setCaption( castor::string::toU32String( m_categoryName ) );
		m_cpu.name->setCaption( U"CPU:" );
		m_gpu.name->setCaption( U"GPU:" );

		auto & engine = cache.getEngine();
		m_container->setMaterial( engine.findMaterial( cuT( "AlphaDarkBlue" ) ).lock().get() );
		m_firstLinePanel->setMaterial( engine.findMaterial( cuT( "AlphaDarkBlue" ) ).lock().get() );
		m_name->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_cpu.name->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_gpu.name->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_cpu.value->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );
		m_gpu.value->setMaterial( engine.findMaterial( cuT( "White" ) ).lock().get() );

		m_name->setVAlign( VAlign::eCenter );
		m_name->setHAlign( HAlign::eLeft );

		m_container->setVisible( true );
		m_name->setVisible( true );
		m_cpu.name->setVisible( true );
		m_gpu.name->setVisible( true );
		m_cpu.value->setVisible( true );
		m_gpu.value->setVisible( true );
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

	void DebugOverlays::CategoryOverlays::compute()
	{
		m_cpu.time = 0_ns;
		m_gpu.time = 0_ns;

		for ( auto & pass : m_passes )
		{
			if ( pass )
			{
				pass->compute();
				m_cpu.time += pass->getCpuTime();
				m_gpu.time += pass->getGpuTime();
			}
		}
	}

	void DebugOverlays::CategoryOverlays::update( uint32_t & top )
	{
		if ( !m_cpu.value || !m_gpu.value )
		{
			return;
		}

		m_visible = m_cpu.time > 0_ns && m_gpu.time > 0_ns;

		if ( m_visible && m_parentVisible )
		{
			m_container->setPixelPosition( castor::Position{ PassPanelLeft, int32_t( top ) } );
			m_cpu.value->setCaption( castor::string::toU32String( castor::string::toString( m_cpu.time ) ) );
			m_gpu.value->setCaption( castor::string::toU32String( castor::string::toString( m_gpu.time ) ) );
			uint32_t height = PanelHeight;

			for ( auto & pass : m_passes )
			{
				if ( pass )
				{
					pass->update( height );
				}
			}

			m_container->setPixelSize( castor::Size{ uint32_t( m_posX ), height } );
			top += height;
		}

		m_container->setVisible( m_visible && m_parentVisible );
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
		m_parentVisible = visible;
		m_container->setVisible( m_visible && m_parentVisible );
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
		m_externalTime = m_frameTimer.getElapsed();
		return m_renderInfo;
	}

	uint32_t DebugOverlays::registerTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto realCategory = category.substr( 0u, category.find_last_of( cuT( '/' ) ) );
		auto & cache = getEngine()->getOverlayCache();
		auto ires = m_renderPasses.emplace( realCategory, CategoryOverlays{} );
		auto it = ires.first;

		if ( ires.second )
		{
			it->second = CategoryOverlays{ realCategory, cache, m_detailed };
			it->second.setVisible( m_visible );
		}

		it->second.addTimer( timer );
		m_dirty = true;
		return m_queriesCount++;
	}

	void DebugOverlays::unregisterTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
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

	void DebugOverlays::dumpFrameTimes( Parameters & params )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		params.add( "Average", m_averageTime );

		for ( auto & pass : m_renderPasses )
		{
			params.add( pass.first + " GPU", pass.second.getGpuTime() );
			params.add( pass.first + " CPU", pass.second.getCpuTime() );
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
			auto count = std::min( m_frameCount, uint64_t( m_framesTimes.size() ) );
			m_averageTime = std::accumulate( m_framesTimes.begin()
				, std::next( m_framesTimes.begin(), ptrdiff_t( count ) )
				, 0_ns ) / count;
			m_averageFps = 1000000.0f / float( std::chrono::duration_cast< castor::Microseconds >( m_averageTime ).count() );
			auto v = ( ++m_frameIndex ) % FrameSamplesCount;
			m_frameIndex = v;
		}

		auto result = std::chrono::duration_cast< castor::Microseconds >( m_totalTime );
		m_fps = 1000000.0f / float( result.count() );
		doCompute();

		if ( m_visible )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			uint32_t top{};

			for ( auto & pass : m_renderPasses )
			{
				pass.second.update( top );
			}

			m_debugPanel->update();
			getEngine()->getRenderSystem()->resetGpuTime();
		}

		fprintf( stdout
			, "\r%0.2f ms, %0.2f fps                           "
			, float( result.count() ) / 1000.0f
			, m_fps );
		m_frameTimer.getElapsed();

		return result;
	}

	void DebugOverlays::endGpuTask()
	{
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			for ( auto & renderPass : m_renderPasses )
			{
				renderPass.second.retrieveGpuTime();
			}
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
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			for ( auto & pass : m_renderPasses )
			{
				pass.second.setVisible( m_visible );
			}
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
			, m_renderInfo.totalVertexCount );
		m_debugPanel->addCountPanel( cuT( "TotalFaceCount" )
			, cuT( "Faces Count:" )
			, m_renderInfo.totalFaceCount );
		m_debugPanel->addCountPanel( cuT( "TotalObjectCount" )
			, cuT( "Objects Count:" )
			, m_renderInfo.totalObjectsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleVertexCount" )
			, cuT( "Visible Vertices Count:" )
			, m_renderInfo.visibleVertexCount );
		m_debugPanel->addCountPanel( cuT( "VisibleFaceCount" )
			, cuT( "Visible Faces Count:" )
			, m_renderInfo.visibleFaceCount );
		m_debugPanel->addCountPanel( cuT( "VisibleObjectCount" )
			, cuT( "Visible Objects Count:" )
			, m_renderInfo.visibleObjectsCount );
		m_debugPanel->addCountPanel( cuT( "ParticlesCount" )
			, cuT( "Particles Count:" )
			, m_renderInfo.particlesCount );
		m_debugPanel->addCountPanel( cuT( "LightCount" )
			, cuT( "Lights Count:" )
			, m_renderInfo.totalLightsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleLightCount" )
			, cuT( "Visible Lights Count:" )
			, m_renderInfo.visibleLightsCount );
		m_debugPanel->addCountPanel( cuT( "DrawCalls" )
			, cuT( "Draw calls:" )
			, m_renderInfo.drawCalls );
		m_debugPanel->updatePosition();
		m_debugPanel->setVisible( m_visible );
	}

	void DebugOverlays::doCompute()
	{
		m_gpuTotalTime = 0_ns;
		m_gpuClientTime = 0_ns;
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			for ( auto & pass : m_renderPasses )
			{
				pass.second.compute();
				m_gpuTotalTime += pass.second.getGpuTime();
				m_gpuClientTime += pass.second.getCpuTime();
			}
		}
	}

	//*********************************************************************************************
}
