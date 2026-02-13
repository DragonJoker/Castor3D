#include "Castor3D/Overlay/DebugOverlays.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Gui/Layout/LayoutBox.hpp"
#include "Castor3D/Overlay/Overlay.hpp"

#include <RenderGraph/FramePassTimer.hpp>

#include <iomanip>

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( c3d, DebugOverlays )

namespace c3d
{
	//*********************************************************************************************

	namespace dbgovl
	{
		static String toString( Nanoseconds const & duration )
		{
			StringStream stream;
			stream << std::setprecision( 3 ) << ( float( duration.count() ) / 1000000.0f ) << cuT( " ms" );
			return stream.str();
		}

		static ControlsManager & getControlsManager( Engine & engine )
		{
			return static_cast< ControlsManager & >( *engine.getUserInputListener() );
		}
	}

	//*********************************************************************************************

	DebugOverlays::DebugPanel::DebugPanel( String const & name
		, String const & label
		, Engine & engine
		, PanelCtrl & parent
		, Function< String() > value )
		: m_engine{ engine }
		, m_v{ c3d::move( value ) }
	{
		auto & manager = *parent.getControlsManager();
		auto panelStyle = parent.getStyle().getStyle< PanelStyle >( cuT( "Entry" ) );
		auto labelStyle = panelStyle->getStyle< StaticStyle >( cuT( "Label" ) );
		auto valueStyle = panelStyle->getStyle< StaticStyle >( cuT( "Value" ) );
		m_panel = manager.registerControlT( makeUnique< PanelCtrl >( nullptr
			, name
			, panelStyle
			, &parent
			, Position{}
			, Size{ DebugLineWidth, PanelHeight } ) );
		m_label = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "Label" )
			, labelStyle
			, m_panel
			, toUtf8U32String( label )
			, Position{ 0, 2 }
			, Size{ DebugLabelWidth, PanelHeight } ) );
		m_value = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "Value" )
			, valueStyle
			, m_panel
			, U""
			, Position{ DebugLabelWidth, 2 }
			, Size{ DebugValueWidth, PanelHeight } ) );

		manager.create( m_panel );
		manager.create( m_label );
		manager.create( m_value );

		parent.getLayout()->addControl( *m_panel
			, LayoutItemFlags{}
				.padLeft( DebugPanelWidth - DebugLineWidth ) );
	}

	DebugOverlays::DebugPanel::DebugPanel( DebugPanel && rhs )noexcept
		: m_engine{ rhs.m_engine }
		, m_v{ c3d::move( rhs.m_v ) }
		, m_panel{ c3d::move( rhs.m_panel ) }
		, m_label{ c3d::move( rhs.m_label ) }
		, m_value{ c3d::move( rhs.m_value ) }
	{
		rhs.m_panel = {};
		rhs.m_label = {};
		rhs.m_value = {};
	}

	DebugOverlays::DebugPanel::~DebugPanel()noexcept
	{
		if ( m_panel )
		{
			auto & layout = *static_cast< LayoutControl & >( *m_panel->getParent() ).getLayout();
			layout.removeControl( *m_panel );

			auto & manager = *m_engine.getControlsManager();
			manager.destroy( m_label );
			manager.destroy( m_value );
			manager.destroy( m_panel );

			manager.unregisterControl( *m_label );
			manager.unregisterControl( *m_value );
			manager.unregisterControl( *m_panel );

			m_panel = {};
			m_label = {};
			m_value = {};
		}
	}

	void DebugOverlays::DebugPanel::update()
	{
		m_value->setCaption( toUtf8U32String( m_v() ) );
	}

	//*********************************************************************************************

	DebugOverlays::DebugPanels::DebugPanels( String const & title
		, Engine & engine
		, PanelCtrl & parent )
		: m_engine{ engine }
	{
		auto & manager = dbgovl::getControlsManager( m_engine );
		auto containerStyle = manager.getStyle< ExpandablePanelStyle >( cuT( "Debug/Main/Container" ) );
		auto titleStyle = containerStyle->getHeaderStyle().getStyle< StaticStyle >( cuT( "Title" ) );
		m_panel = manager.registerControlT( makeUnique< ExpandablePanelCtrl >( nullptr
			, title
			, containerStyle
			, &parent
			, Position{}
			, Size{ DebugPanelWidth, PanelHeight }
			, PanelHeight
			, true ) );
		m_panel->getContent()->setLayout( makeUniqueDerived< Layout, LayoutBox >( *m_panel->getContent() ) );
		m_title = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "Title" )
			, titleStyle
			, m_panel->getHeader()
			, toUtf8U32String( title )
			, Position{ 0, 3 }
			, Size{ DebugPanelWidth - PanelHeight, PanelHeight } ) );

		m_title->setHAlign( HAlign::eCenter );

		manager.create( m_panel );
		manager.create( m_title );

		parent.getLayout()->addControl( *m_panel );
	}

	DebugOverlays::DebugPanels::DebugPanels( DebugPanels && rhs )noexcept
		: m_engine{ rhs.m_engine }
		, m_panel{ c3d::move( rhs.m_panel ) }
		, m_title{ c3d::move( rhs.m_title ) }
		, m_panels{ c3d::move( rhs.m_panels ) }
	{
		rhs.m_panel = {};
		rhs.m_title = {};
	}

	DebugOverlays::DebugPanels::~DebugPanels()noexcept
	{
		m_panels.clear();

		if ( m_panel )
		{
			auto & layout = *static_cast< LayoutControl & >( *m_panel->getParent() ).getLayout();
			layout.removeControl( *m_panel );

			auto & manager = *m_engine.getControlsManager();
			manager.destroy( m_title );
			manager.destroy( m_panel );

			manager.unregisterControl( *m_title );
			manager.unregisterControl( *m_panel );

			m_panel = {};
			m_title = {};
		}
	}

	void DebugOverlays::DebugPanels::update()
	{
		for ( auto & panel : m_panels )
		{
			panel.update();
		}
	}

	uint32_t DebugOverlays::DebugPanels::updatePosition( uint32_t y )
	{
		uint32_t height{};

		if ( m_panel->isExpanded() )
		{
			height = uint32_t( PanelHeight * ( m_panels.size() + 1u ) );
			m_panel->setSize( { DebugPanelWidth, height } );
		}

		return y + height;
	}

	void DebugOverlays::DebugPanels::add( String const & name
		, String const & label
		, Function< String() > value )
	{
		m_panels.emplace_back( name
			, label
			, m_engine
			, *m_panel->getContent()
			, value );
	}

	//*********************************************************************************************

	DebugOverlays::MainDebugPanel::MainDebugPanel( Engine & engine, String const & name )
		: m_engine{ engine }
	{
		auto & manager = dbgovl::getControlsManager( m_engine );
		m_panel = manager.registerControlT( makeUnique< PanelCtrl >( nullptr
			, cuT( "Debug/" ) + name
			, manager.getStyle< PanelStyle >( cuT( "Debug/Main" ) )
			, nullptr
			, Position{}
			, Size{ DebugPanelWidth, PanelHeight }
			, ControlFlagType( ControlFlag::eAlwaysOnTop ) ) );
		m_panel->setLayout( makeUniqueDerived< Layout, LayoutBox >( *m_panel ) );
		manager.create( m_panel );

		m_times = makeRawUnique< DebugPanels >( cuT( "Times" ), m_engine, *m_panel );
		m_fps = makeRawUnique< DebugPanels >( cuT( "FPS" ), m_engine, *m_panel );
		m_counts = makeRawUnique< DebugPanels >( cuT( "Counts" ), m_engine, *m_panel );
		m_stats = makeRawUnique< DebugPanels >( cuT( "Allocations" ), m_engine, *m_panel );
	}

	DebugOverlays::MainDebugPanel::~MainDebugPanel()noexcept
	{
		m_stats.reset();
		m_counts.reset();
		m_fps.reset();
		m_times.reset();

		auto & manager = *m_engine.getControlsManager();
		manager.destroy( m_panel );

		manager.unregisterControl( *m_panel );

		m_panel = {};
	}

	void DebugOverlays::MainDebugPanel::update()
	{
		m_times->update();
		m_fps->update();
		m_counts->update();
		m_stats->update();
	}

	void DebugOverlays::MainDebugPanel::setVisible( bool visible )
	{
		m_panel->setVisible( visible );
	}

	void DebugOverlays::MainDebugPanel::addTimePanel( String const & name
		, String const & label
		, Nanoseconds const & value )
	{
		auto v = &value;
		m_times->add( name
			, label
			, [v]() { return dbgovl::toString( *v ); } );
		doUpdatePosition();
	}

	void DebugOverlays::MainDebugPanel::addCountPanel( String const & name
		, String const & label
		, uint32_t const & value )
	{
		auto v = &value;
		m_counts->add( name
			, label
			, [v]() { return string::toString( *v ); } );
		doUpdatePosition();
	}

	void DebugOverlays::MainDebugPanel::addStatsPanel( String const & name
		, String const & label
		, AllocationStats const & value )
	{
		auto v = &value;
		m_stats->add( name
			, label
			, [v]() { return string::toString( v->total - v->available ); } );
		doUpdatePosition();
	}

	void DebugOverlays::MainDebugPanel::addFpsPanel( String const & name
		, String const & label
		, float const & value )
	{
		auto v = &value;
		m_fps->add( name
			, label
			, [v]() { return string::toString( *v ); } );
		doUpdatePosition();
	}

	void DebugOverlays::MainDebugPanel::doUpdatePosition()
	{
		uint32_t y = m_times->updatePosition( 0u );
		y = m_fps->updatePosition( y );
		y = m_counts->updatePosition( y );
		y = m_stats->updatePosition( y );
		m_panel->setSize( Size{ DebugPanelWidth, y } );
	}

	//*********************************************************************************************

	void DebugOverlays::PassTime::addTime( Nanoseconds v )
	{
		++count;
		accumulator -= times[index];
		accumulator += v;
		times[index] = v;
		auto realCount = std::min( count, uint64_t( times.size() ) );
		average = accumulator / realCount;
		index = ( index + 1u ) % SamplesCount;
	}

	//*********************************************************************************************

	DebugOverlays::PassOverlays::PassOverlays( Engine & engine
		, PanelCtrl & parent
		, String const & name
		, uint32_t leftOffset
		, HdrRgbColour const & colour )
		: m_parent{ &parent }
		, m_name{ name }
	{
		auto & manager = dbgovl::getControlsManager( engine );
		auto panelStyle = manager.getStyle< PanelStyle >( cuT( "Debug/RenderPasses/Pass" ) );
		auto nameStyle = panelStyle->getStyle< StaticStyle >( cuT( "Name" ) );
		auto counterStyle = panelStyle->getStyle< StaticStyle >( cuT( "Counter" ) );

		panelStyle = &static_cast< PanelStyle & >( *panelStyle->clone( cuT( "Debug/RenderPasses/" ) + name ) );
		panelStyle->setBackgroundMaterial( createMaterial( engine, name, colour ) );

		auto maxWidth = CategoryLineWidth - leftOffset;
		m_panel = manager.registerControlT( makeUnique< PanelCtrl >( nullptr
			, m_name + cuT( "Pass" )
			, panelStyle
			, m_parent
			, Position{}
			, Size{ maxWidth, PanelHeight } ) );
		int32_t posX{};
		auto nameWidth = CategoryNameWidth - leftOffset;
		m_passName = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "PassName" )
			, nameStyle
			, m_panel
			, toUtf8U32String( m_name )
			, Position{ posX, 2 }
			, Size{ nameWidth, PanelHeight } ) );
		posX += nameWidth;
		m_cpu.name = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "CPUName" )
			, counterStyle
			, m_panel
			, U"CPU:"
			, Position{ posX, 0 }
			, Size{ CpuNameWidth, PanelHeight } ) );
		posX += CpuNameWidth;
		m_cpu.value = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "CPUValue" )
			, counterStyle
			, m_panel
			, U""
			, Position{ posX, 0 }
			, Size{ CpuValueWidth, PanelHeight } ) );
		posX += CpuValueWidth;
		m_gpu.name = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "GPUName" )
			, counterStyle
			, m_panel
			, U"GPU:"
			, Position{ posX, 0 }
			, Size{ GpuNameWidth, PanelHeight } ) );
		posX += GpuNameWidth;
		m_gpu.value = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "GPUValue" )
			, counterStyle
			, m_panel
			, U""
			, Position{ posX, 0 }
			, Size{ GpuValueWidth, PanelHeight } ) );

		m_cpu.name->setVAlign( VAlign::eCenter );
		m_gpu.name->setVAlign( VAlign::eCenter );
		m_cpu.value->setVAlign( VAlign::eCenter );
		m_gpu.value->setVAlign( VAlign::eCenter );

		m_panel->setVisible( true );
		m_passName->setVisible( true );
		m_cpu.name->setVisible( true );
		m_gpu.name->setVisible( true );
		m_cpu.value->setVisible( true );
		m_gpu.value->setVisible( true );

		manager.create( m_panel );
		manager.create( m_passName );
		manager.create( m_cpu.name );
		manager.create( m_gpu.name );
		manager.create( m_cpu.value );
		manager.create( m_gpu.value );

		m_parent->getLayout()->addControl( *m_panel, LayoutItemFlags{}.padLeft( leftOffset ) );
	}

	DebugOverlays::PassOverlays::PassOverlays( PassOverlays && rhs )noexcept
		: m_parent{ c3d::move( rhs.m_parent ) }
		, m_name{ c3d::move( rhs.m_name ) }
		, m_timers{ c3d::move( rhs.m_timers ) }
		, m_panel{ c3d::move( rhs.m_panel ) }
		, m_passName{ c3d::move( rhs.m_passName ) }
		, m_cpu{ c3d::move( rhs.m_cpu ) }
		, m_gpu{ c3d::move( rhs.m_gpu ) }
	{
		rhs.m_parent = {};
		rhs.m_panel = {};
		rhs.m_passName = {};
		rhs.m_cpu.name = {};
		rhs.m_cpu.value = {};
		rhs.m_gpu.name = {};
		rhs.m_gpu.value = {};
	}

	DebugOverlays::PassOverlays::~PassOverlays()noexcept
	{
		if ( m_panel )
		{
			m_parent->getLayout()->removeControl( *m_panel );

			m_panel->setVisible( false );
			m_passName->setVisible( false );
			m_cpu.name->setVisible( false );
			m_gpu.name->setVisible( false );
			m_cpu.value->setVisible( false );
			m_gpu.value->setVisible( false );

			auto & manager = *m_panel->getControlsManager();
			manager.destroy( m_gpu.name );
			manager.destroy( m_gpu.value );
			manager.destroy( m_cpu.name );
			manager.destroy( m_cpu.value );
			manager.destroy( m_passName );
			manager.destroy( m_panel );

			manager.unregisterControl( *m_gpu.name );
			manager.unregisterControl( *m_gpu.value );
			manager.unregisterControl( *m_cpu.name );
			manager.unregisterControl( *m_cpu.value );
			manager.unregisterControl( *m_passName );
			manager.unregisterControl( *m_panel );

			m_gpu.name = {};
			m_gpu.value = {};
			m_cpu.name = {};
			m_cpu.value = {};
			m_passName = {};
			m_panel = {};
		}
	}

	void DebugOverlays::PassOverlays::compute()
	{
		Nanoseconds cpuTime{};
		Nanoseconds gpuTime{};

		for ( auto const & [timer, _] : m_timers )
		{
			if ( timer->getScope() != crg::TimerScope::eGraph )
			{
				cpuTime += timer->getCpuTime();
				gpuTime += timer->getGpuTime();
			}

			timer->reset();
		}

		m_cpu.time.addTime( cpuTime );
		m_gpu.time.addTime( gpuTime );
	}

	void DebugOverlays::PassOverlays::update( uint32_t & top )
	{
		m_cpu.value->setCaption( toUtf8U32String( dbgovl::toString( m_cpu.time.average ) ) );
		m_gpu.value->setCaption( toUtf8U32String( dbgovl::toString( m_gpu.time.average ) ) );
		top += PanelHeight;
	}

	void DebugOverlays::PassOverlays::retrieveGpuTime()const
	{
		for ( auto const & [timer, _] : m_timers )
		{
			timer->retrieveGpuTime();
		}
	}

	void DebugOverlays::PassOverlays::addTimer( FramePassTimer & timer )
	{
		m_timers.try_emplace( &timer
			, timer.onDestroy.connect( [this]( FramePassTimer & obj )
				{
					removeTimer( obj );
				} ) );
	}

	bool DebugOverlays::PassOverlays::removeTimer( FramePassTimer & timer )
	{
		if ( auto it = m_timers.find( &timer );
			it != m_timers.end() )
		{
			m_timers.erase( it );
		}

		return m_timers.empty();
	}

	//*********************************************************************************************

	DebugOverlays::CategoryOverlays::CategoryOverlays() = default;

	DebugOverlays::CategoryOverlays::CategoryOverlays( String const & category
		, Engine & engine
		, PanelCtrl & parent
		, uint32_t leftOffset
		, bool expanded )
		: m_engine{ &engine }
		, m_parent{ &parent }
		, m_categoryName{ category }
		, m_leftOffset{ leftOffset }
		, m_posX{ int( m_leftOffset ) }
	{
		auto & manager = dbgovl::getControlsManager( *m_engine );
		auto containerStyle = manager.getStyle< ExpandablePanelStyle >( cuT( "Debug/RenderPasses/Category" ) );
		auto nameStyle = containerStyle->getHeaderStyle().getStyle< StaticStyle >( cuT( "Name" ) );
		auto counterStyle = containerStyle->getHeaderStyle().getStyle< StaticStyle >( cuT( "Counter" ) );
		m_container = manager.registerControlT( makeUnique< ExpandablePanelCtrl >( nullptr
			, m_categoryName
			, containerStyle
			, m_parent
			, Position{}
			, Size{ CategoryLineWidth, PanelHeight }
			, PanelHeight
			, expanded ) );
		m_container->getContent()->setLayout( makeUniqueDerived< Layout, LayoutBox >( *m_container->getContent() ) );
		auto nameWidth = CategoryNameWidth - m_leftOffset;
		m_name = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "Title" )
			, nameStyle
			, m_container->getHeader()
			, toUtf8U32String( m_categoryName )
			, Position{ m_posX, 2 }
			, Size{ nameWidth, PanelHeight } ) );
		m_posX += nameWidth;
		m_cpu.name = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "CPUName" )
			, counterStyle
			, m_container->getHeader()
			,  U"CPU:"
			, Position{ m_posX, 0 }
			, Size{ CpuNameWidth, PanelHeight } ) );
		m_posX += CpuNameWidth;
		m_cpu.value = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "CPUValue" )
			, counterStyle
			, m_container->getHeader()
			,  U""
			, Position{ m_posX, 0 }
			, Size{ CpuValueWidth, PanelHeight } ) );
		m_posX += CpuValueWidth;
		m_gpu.name = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "GPUName" )
			, counterStyle
			, m_container->getHeader()
			,  U"GPU:"
			, Position{ m_posX, 0 }
			, Size{ GpuNameWidth, PanelHeight } ) );
		m_posX += GpuNameWidth;
		m_gpu.value = manager.registerControlT( makeUnique< StaticCtrl >( nullptr
			, cuT( "GPUValue" )
			, counterStyle
			, m_container->getHeader()
			, U""
			, Position{ m_posX, 0 }
			, Size{ GpuValueWidth, PanelHeight } ) );
		m_posX += GpuValueWidth;

		m_cpu.name->setVAlign( VAlign::eCenter );
		m_gpu.name->setVAlign( VAlign::eCenter );
		m_cpu.value->setVAlign( VAlign::eCenter );
		m_gpu.value->setVAlign( VAlign::eCenter );

		m_container->setVisible( true );
		m_name->setVisible( true );
		m_cpu.name->setVisible( true );
		m_gpu.name->setVisible( true );
		m_cpu.value->setVisible( true );
		m_gpu.value->setVisible( true );

		manager.create( m_container );
		manager.create( m_name );
		manager.create( m_cpu.name );
		manager.create( m_gpu.name );
		manager.create( m_cpu.value );
		manager.create( m_gpu.value );

		m_parent->getLayout()->addControl( *m_container );
	}

	DebugOverlays::CategoryOverlays & DebugOverlays::CategoryOverlays::operator=( CategoryOverlays && rhs )noexcept
	{
		m_engine = c3d::move( rhs.m_engine );
		m_parent = c3d::move( rhs.m_parent );
		m_categoryName = c3d::move( rhs.m_categoryName );
		m_leftOffset = rhs.m_leftOffset;
		m_posX = rhs.m_posX;
		m_passes = c3d::move( rhs.m_passes );
		m_categories = c3d::move( rhs.m_categories );
		m_container = c3d::move( rhs.m_container );
		m_name = c3d::move( rhs.m_name );
		m_cpu = c3d::move( rhs.m_cpu );
		m_gpu = c3d::move( rhs.m_gpu );

		rhs.m_engine = {};
		rhs.m_parent = {};
		rhs.m_leftOffset = {};
		rhs.m_posX = {};
		rhs.m_container = {};
		rhs.m_name = {};
		rhs.m_cpu.name = {};
		rhs.m_cpu.value = {};
		rhs.m_gpu.name = {};
		rhs.m_gpu.value = {};

		return *this;
	}

	DebugOverlays::CategoryOverlays::CategoryOverlays( CategoryOverlays && rhs )noexcept
		: m_engine{ c3d::move( rhs.m_engine ) }
		, m_parent{ c3d::move( rhs.m_parent ) }
		, m_categoryName{ c3d::move( rhs.m_categoryName ) }
		, m_leftOffset{ rhs.m_leftOffset }
		, m_posX{ rhs.m_posX }
		, m_passes{ c3d::move( rhs.m_passes ) }
		, m_categories{ c3d::move( rhs.m_categories ) }
		, m_container{ c3d::move( rhs.m_container ) }
		, m_name{ c3d::move( rhs.m_name ) }
		, m_cpu{ c3d::move( rhs.m_cpu ) }
		, m_gpu{ c3d::move( rhs.m_gpu ) }
	{
		rhs.m_engine = {};
		rhs.m_parent = {};
		rhs.m_leftOffset = {};
		rhs.m_posX = {};
		rhs.m_container = {};
		rhs.m_name = {};
		rhs.m_cpu.name = {};
		rhs.m_cpu.value = {};
		rhs.m_gpu.name = {};
		rhs.m_gpu.value = {};
	}

	DebugOverlays::CategoryOverlays::~CategoryOverlays()noexcept
	{
		if ( m_container )
		{
			m_parent->getLayout()->removeControl( *m_container );

			m_container->setVisible( false );
			m_name->setVisible( false );
			m_cpu.name->setVisible( false );
			m_gpu.name->setVisible( false );
			m_cpu.value->setVisible( false );
			m_gpu.value->setVisible( false );

			m_passes.clear();
			m_categories.clear();

			auto & manager = dbgovl::getControlsManager( *m_engine );
			manager.destroy( m_cpu.value );
			manager.destroy( m_gpu.value );
			manager.destroy( m_cpu.name );
			manager.destroy( m_gpu.name );
			manager.destroy( m_name );
			manager.destroy( m_container );

			manager.unregisterControl( *m_cpu.value );
			manager.unregisterControl( *m_gpu.value );
			manager.unregisterControl( *m_cpu.name );
			manager.unregisterControl( *m_gpu.name );
			manager.unregisterControl( *m_name );
			manager.unregisterControl( *m_container );

			m_cpu.value = {};
			m_gpu.value = {};
			m_cpu.name = {};
			m_gpu.name = {};
			m_name = {};
			m_container = {};
		}
	}

	void DebugOverlays::CategoryOverlays::addTimer( String const & name
		, StringArray & categories
		, FramePassTimer & timer )
	{
		if ( categories.empty() )
		{
			auto it = std::find_if( m_passes.begin()
				, m_passes.end()
				, [&name]( auto const & lookup )
				{
					return lookup->getName() == name;
				} );

			try
			{
				if ( it == m_passes.end() )
				{
					auto passOverlays = makeRawUnique< PassOverlays >( *m_engine
						, *m_container->getContent()
						, name
						, m_leftOffset + SubpassOffset
						, HdrRgbColour::fromComponents( timer.getColour()[0], timer.getColour()[1], timer.getColour()[2] ) );
					m_passes.push_back( c3d::move( passOverlays ) );
					it = std::next( m_passes.begin()
						, ptrdiff_t( m_passes.size() - 1 ) );
				}

				( *it )->addTimer( timer );
			}
			catch ( std::exception & exc )
			{
				log::warn << exc.what() << std::endl;
			}
		}
		else
		{
			auto current = categories.back();
			categories.pop_back();
			auto it = std::find_if( m_categories.begin()
				, m_categories.end()
				, [&current]( auto & lookup )
				{
					return lookup->getName() == current;
				} );

			try
			{
				if ( it == m_categories.end() )
				{
					m_categories.emplace_back( makeRawUnique< CategoryOverlays >( current
						, *m_engine
						, *m_container->getContent()
						, m_leftOffset + SubpassOffset ) );
					it = std::next( m_categories.begin()
						, ptrdiff_t( m_categories.size() - 1 ) );
				}

				( *it )->addTimer( name, categories, timer );
			}
			catch ( std::exception & exc )
			{
				log::warn << exc.what() << std::endl;
			}
		}
	}

	bool DebugOverlays::CategoryOverlays::removeTimer( String const & name
		, StringArray & categories
		, FramePassTimer & timer )
	{
		if ( categories.empty() )
		{
			auto it = std::find_if( m_passes.begin()
				, m_passes.end()
				, [&name]( auto & lookup )
				{
					return lookup->getName() == name;
				} );

			if ( it != m_passes.end() )
			{
				if ( ( *it )->removeTimer( timer ) )
				{
					auto pass = c3d::move( *it );
					m_passes.erase( it );
				}
			}
			else
			{
				log::warn << "DebugOverlays::removeTimer: Couldn't find pass [" << name << "]" << std::endl;
			}
		}
		else
		{
			auto current = categories.back();
			categories.pop_back();
			auto it = std::find_if( m_categories.begin()
				, m_categories.end()
				, [&current]( auto & lookup )
				{
					return lookup->getName() == current;
				} );

			if ( it != m_categories.end() )
			{
				if ( ( *it )->removeTimer( name, categories, timer ) )
				{
					auto category = c3d::move( *it );
					m_categories.erase( it );
				}
			}
			else
			{
				log::warn << "DebugOverlays::removeTimer: Couldn't find category [" << current << "]" << std::endl;
			}
		}

		return m_categories.empty() && m_passes.empty();
	}

	void DebugOverlays::CategoryOverlays::compute()
	{
		Nanoseconds cpuTime{};
		Nanoseconds gpuTime{};

		for ( auto const & pass : m_passes )
		{
			if ( pass )
			{
				pass->compute();
				cpuTime += pass->getCpuTime();
				gpuTime += pass->getGpuTime();
			}
		}

		for ( auto const & cat : m_categories )
		{
			cat->compute();
			cpuTime += cat->getCpuTime();
			gpuTime += cat->getGpuTime();
		}

		m_cpu.time.addTime( cpuTime );
		m_gpu.time.addTime( gpuTime );
	}

	void DebugOverlays::CategoryOverlays::update( uint32_t & top )
	{
		if ( m_container->isExpanded() )
		{
			uint32_t height = PanelHeight;

			for ( auto const & pass : m_passes )
			{
				if ( pass )
					pass->update( height );
			}

			for ( auto const & cat : m_categories )
				cat->update( height );

			top += height;
			m_container->setSize( { CategoryLineWidth, height } );
		}
		else
		{
			top += PanelHeight;
		}

		m_cpu.value->setCaption( toUtf8U32String( dbgovl::toString( m_cpu.time.average ) ) );
		m_gpu.value->setCaption( toUtf8U32String( dbgovl::toString( m_gpu.time.average ) ) );
	}

	void DebugOverlays::CategoryOverlays::retrieveGpuTime()const
	{
		for ( auto const & pass : m_passes )
		{
			if ( pass )
			{
				pass->retrieveGpuTime();
			}
		}

		for ( auto const & cat : m_categories )
		{
			cat->retrieveGpuTime();
		}
	}

	PanelCtrl * DebugOverlays::CategoryOverlays::getContainer()const
	{
		return m_container->getContent();
	}

	void DebugOverlays::CategoryOverlays::dumpFrameTimes( String prefix
		, Parameters & params )const
	{
		if ( !prefix.empty() )
		{
			prefix += cuT( "/" );
		}

		prefix += getName();

		for ( auto & pass : m_passes )
		{
			params.add( prefix + pass->getName() + cuT( " GPU" ), pass->getGpuTime() );
			params.add( prefix + pass->getName() + cuT( " CPU" ), pass->getCpuTime() );
		}

		for ( auto & cat : m_categories )
		{
			cat->dumpFrameTimes( prefix, params );
		}
	}

	//*********************************************************************************************

	DebugOverlays::DebugOverlays( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_allocations{ *engine.getRenderDevice() }
	{
		doCreateMainDebugPanel();
		doCreateRenderPassesDebugPanel();
	}

	void DebugOverlays::cleanup()
	{
		if ( m_totalTime > 0_ns )
		{
			log::info << cuT( "Counts:\n" )
				<< cuT( "  Average Frame Time: " ) << ( float( std::chrono::duration_cast< Microseconds >( m_frameTime.average ).count() ) / 1000.0f ) << cuT( " ms\n" )
				<< cuT( "  Average Frames per second: " ) <<  m_averageFps << std::endl;
		}

		m_debugPanel.reset();
	}

	RenderInfo & DebugOverlays::beginFrame()
	{
		if ( m_visible )
		{
			m_cpuTime = 0_ns;
			m_gpuTime = 0_ns;
			m_taskTimer.getElapsed();
		}

		m_renderInfo = RenderInfo{};
		m_allocations = DeviceCounts{ *getEngine()->getRenderDevice() };
		m_externalTime = m_frameTimer.getElapsed();
		return m_renderInfo;
	}

	void DebugOverlays::registerTimer( String const & category
		, FramePassTimer & timer )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		auto words = string::split( category, cuT( "/" ), 0xFFFFFFFF, false );

		if ( words.empty() )
		{
			return;
		}

		auto last = words.back();
		words.pop_back();
		std::reverse( words.begin(), words.end() );
		m_renderPasses.addTimer( last, words, timer );
		m_dirty = true;
	}

	void DebugOverlays::unregisterTimer( String const & category
		, FramePassTimer & timer )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		auto words = string::split( category, cuT( "/" ), 0xFFFFFFFF, false );

		if ( words.empty() )
		{
			return;
		}

		auto last = words.back();
		words.pop_back();
		std::reverse( words.begin(), words.end() );
		m_renderPasses.removeTimer( last, words, timer );
		m_dirty = true;
	}

	void DebugOverlays::dumpFrameTimes( Parameters & params )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		params.add( cuT( "Average" ), m_frameTime.average );
		m_renderPasses.dumpFrameTimes( String{}, params );
	}

	Microseconds DebugOverlays::endFrame( bool first )
	{
		m_totalTime = m_frameTimer.getElapsed();

		if ( !first )
		{
			 // Prevent initialisation frame from being counted in average time.
			m_frameTime.addTime( m_totalTime );
			m_averageFps = 1000000.0f / float( std::chrono::duration_cast< Microseconds >( m_frameTime.average ).count() );
		}

		auto result = std::chrono::duration_cast< Microseconds >( m_totalTime );
		m_fps = 1000000.0f / float( result.count() );
		doCompute();

		if ( m_visible )
		{
			auto lock( makeUniqueLock( m_mutex ) );
			uint32_t top{};
			m_renderPasses.update( top );

			if ( m_debugPanel )
			{
				m_debugPanel->update();
			}
		}

		m_debugTime = m_frameTimer.getElapsed();
		return result;
	}

	void DebugOverlays::endGpuTasks()
	{
		auto lock( makeUniqueLock( m_mutex ) );
		m_renderPasses.retrieveGpuTime();
	}

	void DebugOverlays::endCpuTask()
	{
		m_cpuTime += m_taskTimer.getElapsed();
	}

	void DebugOverlays::show( bool show )
	{
		m_visible = show;

		if ( m_debugPanel )
		{
			m_debugPanel->setVisible( m_visible );
		}

		if ( m_passesContainer )
		{
			m_passesContainer->setVisible( m_visible );
		}
	}

	void DebugOverlays::doCreateRenderPassesDebugPanel()
	{
		auto & engine = *getEngine();
		auto & manager = dbgovl::getControlsManager( engine );
		m_passesContainer = manager.registerControlT( makeUnique< PanelCtrl >( nullptr
			, cuT( "Debug/RenderPasses" )
			, manager.getPanelStyle( cuT( "Debug/RenderPasses" ) )
			, nullptr
			, Position{ PassPanelLeft, 0 }
			, Size{ CategoryLineWidth + 20u, 600u }
			, ControlFlagType( ControlFlag::eAlwaysOnTop ) | ControlFlagType( ScrollBarFlag::eVertical ) ) );
		m_passesContainer->setLayout( makeUniqueDerived< Layout, LayoutBox >( *m_passesContainer ) );
		manager.create( m_passesContainer );

		m_passesContainer->setVisible( m_visible );

		m_renderPasses = { cuT( "Passes" )
			, engine
			, *m_passesContainer
			, 0u
			, true };
	}

	void DebugOverlays::doCreateMainDebugPanel()
	{
		m_debugPanel = makeRawUnique< MainDebugPanel >( *getEngine(), cuT( "Main" ) );
		m_debugPanel->addTimePanel( cuT( "CpuTime" )
			, cuT( "CPU:" )
			, m_cpuTime );
		m_debugPanel->addTimePanel( cuT( "GpuTime" )
			, cuT( "GPU:" )
			, m_gpuTime );
		m_debugPanel->addTimePanel( cuT( "DebugTime" )
			, cuT( "Debug:" )
			, m_debugTime );
		m_debugPanel->addTimePanel( cuT( "ExternalTime" )
			, cuT( "External:" )
			, m_externalTime );
		m_debugPanel->addTimePanel( cuT( "TotalTime" )
			, cuT( "Total:" )
			, m_totalTime );
		m_debugPanel->addTimePanel( cuT( "AverageTime" )
			, cuT( "Average:" )
			, m_frameTime.average );
		m_debugPanel->addFpsPanel( cuT( "FPS" )
			, cuT( "Last:" )
			, m_fps );
		m_debugPanel->addFpsPanel( cuT( "AverageFPS" )
			, cuT( "Average:" )
			, m_averageFps );
		m_debugPanel->addCountPanel( cuT( "VisibleVertexCount" )
			, cuT( "Vertices:" )
			, m_renderInfo.visible.vertexCount );
		m_debugPanel->addCountPanel( cuT( "VisibleFaceCount" )
			, cuT( "Faces:" )
			, m_renderInfo.visible.faceCount );
		m_debugPanel->addCountPanel( cuT( "VisibleObjectCount" )
			, cuT( "Objects:" )
			, m_renderInfo.visible.objectCount );
		m_debugPanel->addCountPanel( cuT( "VisibleBillboardCount" )
			, cuT( "Billboards:" )
			, m_renderInfo.visible.billboardCount );
		m_debugPanel->addCountPanel( cuT( "ParticlesCount" )
			, cuT( "Particles:" )
			, m_renderInfo.particlesCount );
		m_debugPanel->addCountPanel( cuT( "VisibleLightCount" )
			, cuT( "Lights:" )
			, m_renderInfo.visible.lightsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleOverlaysCount" )
			, cuT( "Overlays:" )
			, m_renderInfo.visibleOverlaysCount );
		m_debugPanel->addCountPanel( cuT( "VisibleOverlayQuadsCount" )
			, cuT( "2D Quads:" )
			, m_renderInfo.visibleOverlayQuadsCount );
		m_debugPanel->addCountPanel( cuT( "DrawCalls" )
			, cuT( "Draw calls:" )
			, m_renderInfo.drawCalls );
		m_debugPanel->addCountPanel( cuT( "UploadSize" )
			, cuT( "Upload Size:" )
			, m_renderInfo.uploadSize );
		m_debugPanel->addCountPanel( cuT( "StagingBuffersCount" )
			, cuT( "Upload Buffers:" )
			, m_renderInfo.stagingBuffersCount );
		m_debugPanel->addStatsPanel( cuT( "RawBuffers" )
			, cuT( "Raw Buffers:" )
			, m_allocations.bufferAllocated );
		m_debugPanel->addStatsPanel( cuT( "VertexBuffers" )
			, cuT( "Vertex Buffers:" )
			, m_allocations.vertexAllocated );
		m_debugPanel->addStatsPanel( cuT( "IndexBuffers" )
			, cuT( "Index Buffers:" )
			, m_allocations.indexAllocated );
		m_debugPanel->addStatsPanel( cuT( "SubmeshBuffers" )
			, cuT( "Submesh Buffers:" )
			, m_allocations.geometryAllocated );
		m_debugPanel->addStatsPanel( cuT( "UniformBuffers" )
			, cuT( "Uniform Buffers:" )
			, m_allocations.uboAllocated );
		m_debugPanel->setVisible( m_visible );
	}

	void DebugOverlays::doCompute()
	{
		auto lock( makeUniqueLock( m_mutex ) );
		m_renderPasses.compute();
		m_gpuTime += m_renderPasses.getGpuTime();
		m_cpuTime -= m_gpuTime;
	}

	//*********************************************************************************************
}
