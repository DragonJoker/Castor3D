#include "Castor3D/Overlay/DebugOverlays.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"
#include "Castor3D/Gui/Controls/CtrlPanel.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Gui/Layout/LayoutBox.hpp"
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

CU_ImplementSmartPtr( castor3d, DebugOverlays )

//*********************************************************************************************

namespace std
{
	inline std::ostream & operator<<( std::ostream & stream, castor::Nanoseconds const & duration )
	{
		stream << std::setprecision( 3 ) << ( float( duration.count() ) / 1000000.0f ) << cuT( " ms" );
		return stream;
	}
}

namespace castor3d
{
	//*********************************************************************************************

	namespace dbgovl
	{
		static ControlsManager & getControlsManager( Engine & engine )
		{
			return static_cast< ControlsManager & >( *engine.getUserInputListener() );
		}
	}

	//*********************************************************************************************

	DebugOverlays::DebugPanel::DebugPanel( castor::String const & name
		, castor::String const & label
		, Engine & engine
		, PanelCtrl & parent
		, std::function< castor::String() > value )
		: m_engine{ engine }
		, m_v{ value }
	{
		auto & manager = *parent.getControlsManager();
		auto panelStyle = parent.getStyle().getStyle< PanelStyle >( "Entry" );
		auto labelStyle = panelStyle->getStyle< StaticStyle >( "Label" );
		auto valueStyle = panelStyle->getStyle< StaticStyle >( "Value" );
		m_panel = manager.registerControlT( castor::makeUnique< PanelCtrl >( nullptr
			, name
			, panelStyle
			, &parent
			, castor::Position{}
			, castor::Size{ DebugLineWidth, PanelHeight } ) );
		m_label = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "Label"
			, labelStyle
			, m_panel
			, castor::string::toU32String( label )
			, castor::Position{ 0, 2 }
			, castor::Size{ DebugLabelWidth, PanelHeight } ) );
		m_value = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "Value"
			, valueStyle
			, m_panel
			, U""
			, castor::Position{ DebugLabelWidth, 2 }
			, castor::Size{ DebugValueWidth, PanelHeight } ) );

		manager.create( m_panel );
		manager.create( m_label );
		manager.create( m_value );

		parent.getLayout()->addControl( *m_panel
			, LayoutItemFlags{}
				.padLeft( DebugPanelWidth - DebugLineWidth ) );
	}

	DebugOverlays::DebugPanel::DebugPanel( DebugPanel && rhs )noexcept
		: m_engine{ rhs.m_engine }
		, m_v{ std::move( rhs.m_v ) }
		, m_panel{ std::move( rhs.m_panel ) }
		, m_label{ std::move( rhs.m_label ) }
		, m_value{ std::move( rhs.m_value ) }
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
		m_value->setCaption( castor::string::toU32String( m_v() ) );
	}

	//*********************************************************************************************

	DebugOverlays::DebugPanels::DebugPanels( castor::String const & title
		, Engine & engine
		, PanelCtrl & parent )
		: m_engine{ engine }
	{
		auto & manager = dbgovl::getControlsManager( m_engine );
		auto containerStyle = manager.getStyle< ExpandablePanelStyle >( "Debug/Main/Container" );
		auto titleStyle = containerStyle->getHeaderStyle().getStyle< StaticStyle >( "Title" );
		m_panel = manager.registerControlT( castor::makeUnique< ExpandablePanelCtrl >( nullptr
			, title
			, containerStyle
			, &parent
			, castor::Position{}
			, castor::Size{ DebugPanelWidth, PanelHeight }
			, PanelHeight
			, true ) );
		m_panel->getContent()->setLayout( castor::makeUniqueDerived< Layout, LayoutBox >( *m_panel->getContent() ) );
		m_title = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "Title"
			, titleStyle
			, m_panel->getHeader()
			, castor::string::toU32String( title )
			, castor::Position{ 0, 3 }
			, castor::Size{ DebugPanelWidth - PanelHeight, PanelHeight } ) );

		m_title->setHAlign( HAlign::eCenter );

		manager.create( m_panel );
		manager.create( m_title );

		parent.getLayout()->addControl( *m_panel );
	}

	DebugOverlays::DebugPanels::DebugPanels( DebugPanels && rhs )noexcept
		: m_engine{ rhs.m_engine }
		, m_panel{ std::move( rhs.m_panel ) }
		, m_title{ std::move( rhs.m_title ) }
		, m_panels{ std::move( rhs.m_panels ) }
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

	void DebugOverlays::DebugPanels::add( castor::String const & name
		, castor::String const & label
		, std::function< castor::String() > value )
	{
		m_panels.emplace_back( name
			, label
			, m_engine
			, *m_panel->getContent()
			, value );
	}

	//*********************************************************************************************

	DebugOverlays::MainDebugPanel::MainDebugPanel( Engine & engine )
		: m_engine{ engine }
	{
		auto & manager = dbgovl::getControlsManager( m_engine );
		m_panel = manager.registerControlT( castor::makeUnique< PanelCtrl >( nullptr
			, cuT( "Debug/Main" )
			, manager.getStyle< PanelStyle >( "Debug/Main" )
			, nullptr
			, castor::Position{}
			, castor::Size{ DebugPanelWidth, PanelHeight }
			, ControlFlagType( ControlFlag::eAlwaysOnTop ) ) );
		m_panel->setLayout( castor::makeUniqueDerived< Layout, LayoutBox >( *m_panel ) );
		manager.create( m_panel );

		m_times = std::make_unique< DebugPanels >( cuT( "Times" ), m_engine, *m_panel );
		m_fps = std::make_unique< DebugPanels >( cuT( "FPS" ), m_engine, *m_panel );
		m_counts = std::make_unique< DebugPanels >( cuT( "Counts" ), m_engine, *m_panel );
	}

	DebugOverlays::MainDebugPanel::~MainDebugPanel()noexcept
	{
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
	}

	void DebugOverlays::MainDebugPanel::setVisible( bool visible )
	{
		m_panel->setVisible( visible );
	}

	void DebugOverlays::MainDebugPanel::addTimePanel( castor::String const & name
		, castor::String const & label
		, castor::Nanoseconds const & value )
	{
		auto v = &value;
		m_times->add( name
			, label
			, [v]() { return castor::string::toString( *v ); } );
		doUpdatePosition();
	}

	void DebugOverlays::MainDebugPanel::addCountPanel( castor::String const & name
		, castor::String const & label
		, uint32_t const & value )
	{
		auto v = &value;
		m_counts->add( name
			, label
			, [v]() { return castor::string::toString( *v ); } );
		doUpdatePosition();
	}

	void DebugOverlays::MainDebugPanel::addFpsPanel( castor::String const & name
		, castor::String const & label
		, float const & value )
	{
		auto v = &value;
		m_fps->add( name
			, label
			, [v]() { return castor::string::toString( *v ); } );
		doUpdatePosition();
	}

	void DebugOverlays::MainDebugPanel::doUpdatePosition()
	{
		uint32_t y = m_times->updatePosition( 0u );
		y = m_fps->updatePosition( y );
		y = m_counts->updatePosition( y );
		m_panel->setSize( castor::Size{ DebugPanelWidth, y } );
	}

	//*********************************************************************************************

	DebugOverlays::PassOverlays::PassOverlays( Engine & engine
		, PanelCtrl & parent
		, castor::String const & name
		, uint32_t leftOffset
		, uint32_t index )
		: m_parent{ &parent }
		, m_name{ name }
	{
		auto & manager = dbgovl::getControlsManager( engine );
		auto panelStyle = manager.getStyle< PanelStyle >( "Debug/RenderPasses/Pass" );
		auto nameStyle = panelStyle->getStyle< StaticStyle >( "Name" );
		auto counterStyle = panelStyle->getStyle< StaticStyle >( "Counter" );
		auto maxWidth = CategoryLineWidth - leftOffset;
		m_panel = manager.registerControlT( castor::makeUnique< PanelCtrl >( nullptr
			, m_name + "Pass"
			, panelStyle
			, m_parent
			, castor::Position{}
			, castor::Size{ maxWidth, PanelHeight } ) );
		int32_t posX{};
		auto nameWidth = CategoryNameWidth - leftOffset;
		m_passName = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "PassName"
			, nameStyle
			, m_panel
			, castor::string::toU32String( m_name )
			, castor::Position{ posX, 2 }
			, castor::Size{ nameWidth, PanelHeight } ) );
		posX += nameWidth;
		m_cpu.name = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "CPUName"
			, counterStyle
			, m_panel
			, U"CPU:"
			, castor::Position{ posX, 0 }
			, castor::Size{ CpuNameWidth, PanelHeight } ) );
		posX += CpuNameWidth;
		m_cpu.value = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "CPUValue"
			, counterStyle
			, m_panel
			, U""
			, castor::Position{ posX, 0 }
			, castor::Size{ CpuValueWidth, PanelHeight } ) );
		posX += CpuValueWidth;
		m_gpu.name = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "GPUName"
			, counterStyle
			, m_panel
			, U"GPU:"
			, castor::Position{ posX, 0 }
			, castor::Size{ GpuNameWidth, PanelHeight } ) );
		posX += GpuNameWidth;
		m_gpu.value = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "GPUValue"
			, counterStyle
			, m_panel
			, U""
			, castor::Position{ posX, 0 }
			, castor::Size{ GpuValueWidth, PanelHeight } ) );

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
		: m_parent{ std::move( rhs.m_parent ) }
		, m_name{ std::move( rhs.m_name ) }
		, m_visible{ std::move( rhs.m_visible ) }
		, m_timers{ std::move( rhs.m_timers ) }
		, m_panel{ std::move( rhs.m_panel ) }
		, m_passName{ std::move( rhs.m_passName ) }
		, m_cpu{ std::move( rhs.m_cpu ) }
		, m_gpu{ std::move( rhs.m_gpu ) }
	{
		rhs.m_parent = {};
		rhs.m_visible = {};
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
		m_visible = m_cpu.time > 0_ns && m_gpu.time > 0_ns;

		if ( m_panel->isVisible() != m_visible )
		{
			m_panel->setVisible( m_visible );
		}

		if ( !m_cpu.value
			|| !m_gpu.value
			|| !m_visible )
		{
			return;
		}

		m_cpu.value->setCaption( castor::string::toU32String( m_cpu.time ) );
		m_gpu.value->setCaption( castor::string::toU32String( m_gpu.time ) );
		top += PanelHeight;
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
		: m_visible{ false }
	{
	}

	DebugOverlays::CategoryOverlays::CategoryOverlays( castor::String const & category
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
		auto containerStyle = manager.getStyle< ExpandablePanelStyle >( "Debug/RenderPasses/Category" );
		auto nameStyle = containerStyle->getHeaderStyle().getStyle< StaticStyle >( "Name" );
		auto counterStyle = containerStyle->getHeaderStyle().getStyle< StaticStyle >( "Counter" );
		m_container = manager.registerControlT( castor::makeUnique< ExpandablePanelCtrl >( nullptr
			, m_categoryName
			, containerStyle
			, m_parent
			, castor::Position{}
			, castor::Size{ CategoryLineWidth, PanelHeight }
			, PanelHeight
			, expanded ) );
		m_container->getContent()->setLayout( castor::makeUniqueDerived< Layout, LayoutBox >( *m_container->getContent() ) );
		auto nameWidth = CategoryNameWidth - m_leftOffset;
		m_name = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "Title"
			, nameStyle
			, m_container->getHeader()
			, castor::string::toU32String( m_categoryName )
			, castor::Position{ m_posX, 2 }
			, castor::Size{ nameWidth, PanelHeight } ) );
		m_posX += nameWidth;
		m_cpu.name = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "CPUName"
			, counterStyle
			, m_container->getHeader()
			,  U"CPU:"
			, castor::Position{ m_posX, 0 }
			, castor::Size{ CpuNameWidth, PanelHeight } ) );
		m_posX += CpuNameWidth;
		m_cpu.value = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "CPUValue"
			, counterStyle
			, m_container->getHeader()
			,  U""
			, castor::Position{ m_posX, 0 }
			, castor::Size{ CpuValueWidth, PanelHeight } ) );
		m_posX += CpuValueWidth;
		m_gpu.name = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "GPUName"
			, counterStyle
			, m_container->getHeader()
			,  U"GPU:"
			, castor::Position{ m_posX, 0 }
			, castor::Size{ GpuNameWidth, PanelHeight } ) );
		m_posX += GpuNameWidth;
		m_gpu.value = manager.registerControlT( castor::makeUnique< StaticCtrl >( nullptr
			, "GPUValue"
			, counterStyle
			, m_container->getHeader()
			, U""
			, castor::Position{ m_posX, 0 }
			, castor::Size{ GpuValueWidth, PanelHeight } ) );
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
		m_engine = std::move( rhs.m_engine );
		m_parent = std::move( rhs.m_parent );
		m_categoryName = std::move( rhs.m_categoryName );
		m_leftOffset = std::move( rhs.m_leftOffset );
		m_posX = std::move( rhs.m_posX );
		m_visible = std::move( rhs.m_visible );
		m_parentVisible = std::move( rhs.m_parentVisible );
		m_passes = std::move( rhs.m_passes );
		m_categories = std::move( rhs.m_categories );
		m_container = std::move( rhs.m_container );
		m_name = std::move( rhs.m_name );
		m_cpu = std::move( rhs.m_cpu );
		m_gpu = std::move( rhs.m_gpu );

		rhs.m_engine = {};
		rhs.m_parent = {};
		rhs.m_leftOffset = {};
		rhs.m_posX = {};
		rhs.m_visible = {};
		rhs.m_parentVisible = {};
		rhs.m_container = {};
		rhs.m_name = {};
		rhs.m_cpu.name = {};
		rhs.m_cpu.value = {};
		rhs.m_gpu.name = {};
		rhs.m_gpu.value = {};

		return *this;
	}

	DebugOverlays::CategoryOverlays::CategoryOverlays( CategoryOverlays && rhs )noexcept
		: m_engine{ std::move( rhs.m_engine ) }
		, m_parent{ std::move( rhs.m_parent ) }
		, m_categoryName{ std::move( rhs.m_categoryName ) }
		, m_leftOffset{ std::move( rhs.m_leftOffset ) }
		, m_posX{ std::move( rhs.m_posX ) }
		, m_visible{ std::move( rhs.m_visible ) }
		, m_parentVisible{ std::move( rhs.m_parentVisible ) }
		, m_passes{ std::move( rhs.m_passes ) }
		, m_categories{ std::move( rhs.m_categories ) }
		, m_container{ std::move( rhs.m_container ) }
		, m_name{ std::move( rhs.m_name ) }
		, m_cpu{ std::move( rhs.m_cpu ) }
		, m_gpu{ std::move( rhs.m_gpu ) }
	{
		rhs.m_engine = {};
		rhs.m_parent = {};
		rhs.m_leftOffset = {};
		rhs.m_posX = {};
		rhs.m_visible = {};
		rhs.m_parentVisible = {};
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

	void DebugOverlays::CategoryOverlays::addTimer( castor::String const & name
		, castor::StringArray & categories
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

			if ( it == m_passes.end() )
			{
				auto index = uint32_t( m_passes.size() );
				m_passes.emplace_back( std::make_unique< PassOverlays >( *m_engine
					, *m_container->getContent()
					, name
					, m_leftOffset + 5u
					, index ) );
				it = std::next( m_passes.begin()
					, ptrdiff_t( m_passes.size() - 1 ) );
			}

			( *it )->addTimer( timer );
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

			if ( it == m_categories.end() )
			{
				m_categories.emplace_back( std::make_unique< CategoryOverlays >( current
					, *m_engine
					, *m_container->getContent()
					, m_leftOffset + 5u ) );
				it = std::next( m_categories.begin()
					, ptrdiff_t( m_categories.size() - 1 ) );
			}

			( *it )->addTimer( name, categories, timer );
		}
	}

	bool DebugOverlays::CategoryOverlays::removeTimer( castor::String const & name
		, castor::StringArray & categories
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
					auto pass = std::move( *it );
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
					auto category = std::move( *it );
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

		for ( auto & catIt : m_categories )
		{
			catIt->compute();
			m_cpu.time += catIt->getCpuTime();
			m_gpu.time += catIt->getGpuTime();
		}
	}

	void DebugOverlays::CategoryOverlays::update( uint32_t & top )
	{
		m_visible = m_cpu.time > 0_ns && m_gpu.time > 0_ns;

		if ( m_container->isVisible() != ( m_visible && m_parentVisible ) )
		{
			m_container->setVisible( m_visible && m_parentVisible );
		}

		if ( !m_cpu.value
			|| !m_gpu.value
			|| !m_container->isVisible() )
		{
			return;
		}

		m_cpu.value->setCaption( castor::string::toU32String( m_cpu.time ) );
		m_gpu.value->setCaption( castor::string::toU32String( m_gpu.time ) );

		if ( m_container->isExpanded() )
		{
			uint32_t height = PanelHeight;

			for ( auto & pass : m_passes )
			{
				if ( pass )
				{
					pass->update( height );
				}
			}

			for ( auto & catIt : m_categories )
			{
				catIt->update( height );
			}

			top += height;
			m_container->setSize( { CategoryLineWidth, height } );
		}
		else
		{
			top += PanelHeight;
		}
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

		for ( auto & catIt : m_categories )
		{
			catIt->retrieveGpuTime();
		}
	}

	void DebugOverlays::CategoryOverlays::setVisible( bool visible )
	{
		m_parentVisible = visible;
		m_container->setVisible( m_visible && m_parentVisible );
	}

	PanelCtrl * DebugOverlays::CategoryOverlays::getContainer()const
	{
		return m_container->getContent();
	}

	void DebugOverlays::CategoryOverlays::dumpFrameTimes( castor::String prefix
		, Parameters & params )const
	{
		if ( !prefix.empty() )
		{
			prefix += "/";
		}

		prefix += getName();

		for ( auto & pass : m_passes )
		{
			params.add( prefix + pass->getName() + " GPU", pass->getGpuTime() );
			params.add( prefix + pass->getName() + " CPU", pass->getCpuTime() );
		}

		for ( auto & cat : m_categories )
		{
			cat->dumpFrameTimes( prefix, params );
		}
	}

	//*********************************************************************************************

	DebugOverlays::DebugOverlays( Engine & engine )
		: OwnedBy< Engine >( engine )
	{
		doCreateMainDebugPanel();
		doCreateRenderPassesDebugPanel();
	}

	DebugOverlays::~DebugOverlays()
	{
	}

	void DebugOverlays::initialise( OverlayCache & cache )
	{
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

	void DebugOverlays::registerTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto words = castor::string::split( category, "/", 0xFFFFFFFF, false );

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

	void DebugOverlays::unregisterTimer( castor::String const & category
		, FramePassTimer & timer )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto words = castor::string::split( category, "/", 0xFFFFFFFF, false );

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
		auto lock( castor::makeUniqueLock( m_mutex ) );
		params.add( "Average", m_averageTime );
		m_renderPasses.dumpFrameTimes( castor::String{}, params );
	}

	castor::Microseconds DebugOverlays::endFrame( bool first )
	{
		m_totalTime = m_frameTimer.getElapsed();

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
			m_renderPasses.update( top );

			if ( m_debugPanel )
			{
				m_debugPanel->update();
			}

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
			m_renderPasses.retrieveGpuTime();
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
		m_passesContainer = manager.registerControlT( castor::makeUnique< PanelCtrl >( nullptr
			, "Debug/RenderPasses"
			, manager.getPanelStyle( "Debug/RenderPasses" )
			, nullptr
			, castor::Position{ PassPanelLeft, 0 }
			, castor::Size{ CategoryLineWidth, 600u }
			, ControlFlagType( ControlFlag::eAlwaysOnTop ) ) );
		m_passesContainer->setLayout( castor::makeUniqueDerived< Layout, LayoutBox >( *m_passesContainer ) );
		manager.create( m_passesContainer );

		m_passesContainer->setVisible( m_visible );

		m_renderPasses = { "Passes"
			, engine
			, *m_passesContainer
			, 0u
			, true };
	}

	void DebugOverlays::doCreateMainDebugPanel()
	{
		m_debugPanel = std::make_unique< MainDebugPanel >( *getEngine() );
		m_debugPanel->addTimePanel( cuT( "CpuTime" )
			, cuT( "CPU:" )
			, m_cpuTime );
		m_debugPanel->addTimePanel( cuT( "GpuClientTime" )
			, cuT( "GPU Client:" )
			, m_gpuClientTime );
		m_debugPanel->addTimePanel( cuT( "GpuServerTime" )
			, cuT( "GPU Server:" )
			, m_gpuTotalTime );
		m_debugPanel->addTimePanel( cuT( "ExternalTime" )
			, cuT( "External:" )
			, m_externalTime );
		m_debugPanel->addTimePanel( cuT( "TotalTime" )
			, cuT( "Total:" )
			, m_totalTime );
		m_debugPanel->addTimePanel( cuT( "AverageTime" )
			, cuT( "Average:" )
			, m_averageTime );
		m_debugPanel->addFpsPanel( cuT( "FPS" )
			, cuT( "Last:" )
			, m_fps );
		m_debugPanel->addFpsPanel( cuT( "AverageFPS" )
			, cuT( "Average:" )
			, m_averageFps );
		m_debugPanel->addCountPanel( cuT( "TotalVertexCount" )
			, cuT( "Vertices:" )
			, m_renderInfo.totalVertexCount );
		m_debugPanel->addCountPanel( cuT( "TotalFaceCount" )
			, cuT( "Faces:" )
			, m_renderInfo.totalFaceCount );
		m_debugPanel->addCountPanel( cuT( "TotalObjectCount" )
			, cuT( "Objects:" )
			, m_renderInfo.totalObjectsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleVertexCount" )
			, cuT( "Visible Vertices:" )
			, m_renderInfo.visibleVertexCount );
		m_debugPanel->addCountPanel( cuT( "VisibleFaceCount" )
			, cuT( "Visible Faces:" )
			, m_renderInfo.visibleFaceCount );
		m_debugPanel->addCountPanel( cuT( "VisibleObjectCount" )
			, cuT( "Visible Objects:" )
			, m_renderInfo.visibleObjectsCount );
		m_debugPanel->addCountPanel( cuT( "ParticlesCount" )
			, cuT( "Particles:" )
			, m_renderInfo.particlesCount );
		m_debugPanel->addCountPanel( cuT( "LightCount" )
			, cuT( "All Lights:" )
			, m_renderInfo.totalLightsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleLightCount" )
			, cuT( "Visible Lights:" )
			, m_renderInfo.visibleLightsCount );
		m_debugPanel->addCountPanel( cuT( "VisibleOverlaysCount" )
			, cuT( "Visible Overlays:" )
			, m_renderInfo.visibleOverlaysCount );
		m_debugPanel->addCountPanel( cuT( "VisibleOverlayQuadsCount" )
			, cuT( "Visible 2D Quads:" )
			, m_renderInfo.visibleOverlayQuadsCount );
		m_debugPanel->addCountPanel( cuT( "DrawCalls" )
			, cuT( "Draw calls:" )
			, m_renderInfo.drawCalls );
		m_debugPanel->setVisible( m_visible );
	}

	void DebugOverlays::doCompute()
	{
		m_gpuTotalTime = 0_ns;
		m_gpuClientTime = 0_ns;
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			m_renderPasses.compute();
			m_gpuTotalTime += m_renderPasses.getGpuTime();
			m_gpuClientTime += m_renderPasses.getCpuTime();
		}
	}

	//*********************************************************************************************
}
