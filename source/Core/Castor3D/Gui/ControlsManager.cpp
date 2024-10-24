#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Gui/Gui_Parsers.hpp"
#include "Castor3D/Gui/Controls/CtrlControl.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlComboBox.hpp"
#include "Castor3D/Gui/Controls/CtrlEdit.hpp"
#include "Castor3D/Gui/Controls/CtrlExpandablePanel.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"
#include "Castor3D/Gui/Controls/CtrlScrollBar.hpp"
#include "Castor3D/Gui/Controls/CtrlSlider.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"
#include "Castor3D/Gui/Layout/Layout.hpp"
#include "Castor3D/Gui/Theme/StyleButton.hpp"
#include "Castor3D/Gui/Theme/StyleComboBox.hpp"
#include "Castor3D/Gui/Theme/StyleEdit.hpp"
#include "Castor3D/Gui/Theme/StyleExpandablePanel.hpp"
#include "Castor3D/Gui/Theme/StyleListBox.hpp"
#include "Castor3D/Gui/Theme/StylePanel.hpp"
#include "Castor3D/Gui/Theme/StyleSlider.hpp"
#include "Castor3D/Gui/Theme/StyleStatic.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

CU_ImplementSmartPtr( castor3d, ControlsManager )
CU_ImplementSmartPtr( castor3d, GuiContext )

namespace castor3d
{
	namespace ctrlmgr
	{
		using LockType = castor::UniqueLock< castor::Mutex >;

		template< typename StyleT >
		StyleT * getThemeStyle( castor::String const & name
			, castor::StringMap< ThemeUPtr > const & themes )
		{
			StyleT * style{};
			auto themeIt = std::find_if( themes.begin()
				, themes.end()
				, [&name, &style]( auto const & lookup )
				{
					if ( name.find( lookup.first + cuT( "/" ) ) == 0u )
					{
						style = lookup.second->template getStyle< StyleT >( name.substr( lookup.first.size() + 1u ) );
					}

					return style != nullptr;
				} );
			return ( themeIt != themes.end() )
				? style
				: nullptr;
		}

		template< typename ConnectionT >
		void removeElem( Control const & control
			, castor::Map< Control const *, ConnectionT > & map )
		{
			if ( auto it = map.find( &control );
				it != map.end() )
			{
				map.erase( it );
			}
		}
	}

	castor::String ControlsManager::Name = cuT( "c3d.gui" );

	ControlsManager::ControlsManager( Engine & engine )
		: UserInputListener{ engine, Name }
		, StylesHolder{ castor::String{}, engine }
	{
	}
	
	void ControlsManager::setSize( castor::Size const & size )
	{
		if ( m_layout )
		{
			m_layout->markDirty();
		}

		m_size = size;
	}

	void ControlsManager::setLayout( LayoutUPtr layout )
	{
		m_layout = castor::move( layout );
	}

	ThemeRPtr ControlsManager::createTheme( castor::String const & name
		, Scene * scene )
	{
		auto [it, res] = m_themes.try_emplace( name );

		if ( res )
		{
			it->second = castor::makeUnique< Theme >( name, scene, *getEngine() );
		}

		return it->second.get();
	}

	ThemeRPtr ControlsManager::getTheme( castor::String const & name )const
	{
		if ( auto it = m_themes.find( name );
			it != m_themes.end() )
		{
			return it->second.get();
		}

		return nullptr;
	}

	ButtonStyleRPtr ControlsManager::getButtonStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ButtonStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getButtonStyle( name );
	}

	ComboBoxStyleRPtr ControlsManager::getComboBoxStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ComboBoxStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getComboBoxStyle( name );
	}

	EditStyleRPtr ControlsManager::getEditStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< EditStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getEditStyle( name );
	}

	ExpandablePanelStyleRPtr ControlsManager::getExpandablePanelStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ExpandablePanelStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getExpandablePanelStyle( name );
	}

	FrameStyleRPtr ControlsManager::getFrameStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< FrameStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getFrameStyle( name );
	}

	ListBoxStyleRPtr ControlsManager::getListBoxStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ListBoxStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getListBoxStyle( name );
	}

	PanelStyleRPtr ControlsManager::getPanelStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< PanelStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getPanelStyle( name );
	}

	ProgressStyleRPtr ControlsManager::getProgressStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ProgressStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getProgressStyle( name );
	}

	ScrollBarStyleRPtr ControlsManager::getScrollBarStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< ScrollBarStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getScrollBarStyle( name );
	}

	SliderStyleRPtr ControlsManager::getSliderStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< SliderStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getSliderStyle( name );
	}

	StaticStyleRPtr ControlsManager::getStaticStyle( castor::String const & name )const
	{
		if ( auto style = ctrlmgr::getThemeStyle< StaticStyle >( name, m_themes ) )
		{
			return style;
		}

		return StylesHolder::getStaticStyle( name );
	}

	void ControlsManager::create( ControlRPtr control )
	{
		addControl( control );
		control->create( *this );
	}

	void ControlsManager::destroy( ControlRPtr control )
	{
		control->destroy();
		removeControl( control->getId() );
	}

	void ControlsManager::destroyControls( Scene const & scene )
	{
		auto it = m_rootControls.begin();

		while ( it != m_rootControls.end() )
		{
			if ( auto control = *it;
				control->hasScene() && &control->getScene() == &scene )
			{
				doDestroyControlsRec( control );
				it = m_rootControls.erase( it );
			}
			else
			{
				++it;
			}
		}

		doMarkDirty();
	}

	ControlRPtr ControlsManager::registerControl( ControlUPtr control )
	{
		return &static_cast< Control & >( *doAddHandler( castor::ptrRefCast< EventHandler >( control ) ) );
	}

	void ControlsManager::unregisterControl( Control const & control )
	{
		doRemoveHandlerNL( control );
	}

	void ControlsManager::addControl( ControlRPtr control )
	{
		{
			ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };

			if ( m_controlsById.find( control->getId() ) != m_controlsById.end() )
			{
				CU_Exception( "A control with ID " + castor::string::toMbString( control->getId() ) + " [" + castor::toUtf8( control->getName() ) + "] already exists in the manager" );
			}

			m_controlsById.insert( std::make_pair( control->getId(), control ) );

			if ( !control->getParent() )
			{
				m_rootControls.push_back( control );
			}
		}
		doMarkDirty();
	}

	void ControlsManager::removeControl( ControlID id )
	{
		{
			ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
			doRemoveControlNL( id );
		}
		doMarkDirty();
	}

	void ControlsManager::doRemoveControlNL( ControlID id, bool rootControl )
	{
		auto it = m_controlsById.find( id );

		if ( it == m_controlsById.end() )
		{
			CU_Exception( "This control does not exist in the manager." );
		}

		auto control = it->second;
		m_controlsById.erase( it );

		if ( rootControl && !control->getParent() )
		{
			auto rootit = std::find( m_rootControls.begin()
				, m_rootControls.end()
				, control );

			if ( rootit != m_rootControls.end() )
			{
				m_rootControls.erase( rootit );
			}
		}
	}

	void ControlsManager::doDestroyControlsRec( ControlRPtr control )
	{
		control->destroy();
		doRemoveControlNL( control->getId(), false );
		m_layout->removeControl( *control, false );

		while ( !control->getChildren().empty() )
		{
			doDestroyControlsRec( *control->getChildren().begin() );
		}

		doRemoveHandlerNL( *control );
	}

	ControlRPtr ControlsManager::getControl( ControlID id )const
	{
		auto controls = doGetControlsById();
		auto it = controls.find( id );

		if ( it == controls.end() )
		{
			CU_Exception( "This control does not exist in the manager" );
		}

		return it->second;
	}

	ControlRPtr ControlsManager::findControl( castor::String const & name, SceneRPtr scene )const
	{
		auto controls = doGetHandlers();
		auto it = std::find_if( controls.begin()
			, controls.end()
			, [&name, scene]( EventHandler const * lookup )
			{
				auto result = lookup->getName() == name;

				if ( result && scene )
				{
					auto control = static_cast< Control const * >( lookup );
					result = ( control->hasScene() && scene == &control->getScene() );
				}

				return result;
			} );
		return it == controls.end()
			? nullptr
			: &static_cast< Control & >( **it );
	}

	bool ControlsManager::setMovedControl( ControlRPtr control
		, MouseEvent const & event )
	{
		if ( m_movedControl
			&& m_movedControl->isMoving()
			&& m_movedControl != control
			&& control != nullptr )
		{
			return false;
		}

		if ( m_movedControl )
		{
			m_movedControl->endMove();
		}

		m_movedControl = control;
		return true;
	}

	bool ControlsManager::setResizedControl( ControlRPtr control
		, MouseEvent const & event )
	{
		if ( m_resizedControl
			&& m_resizedControl->isResizing()
			&& m_resizedControl != control
			&& control != nullptr )
		{
			return false;
		}

		if ( m_resizedControl )
		{
			m_resizedControl->endResize();
		}

		m_resizedControl = control;
		return true;
	}

	castor::Vector< ControlRPtr > ControlsManager::getRootControls()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		return m_rootControls;
	}

	castor::AttributeParsers ControlsManager::createParsers()
	{
		return createGuiParsers();
	}

	castor::StrUInt32Map ControlsManager::createSections()
	{
		return createGuiSections();
	}

	void ControlsManager::connectEvents( ButtonCtrl & control )
	{
		m_onButtonClicks.try_emplace( &control
			, control.connect( ButtonEvent::eClicked
				, [this, &control]()
				{
					onClickAction( control.getName() );
				} ) );
	}

	void ControlsManager::connectEvents( ComboBoxCtrl & control )
	{
		m_onComboSelects.try_emplace( &control
			, control.connect( ComboBoxEvent::eSelected
				, [this, &control]( int index )
				{
					onSelectAction( control.getName(), index );
				} ) );
	}

	void ControlsManager::connectEvents( EditCtrl & control )
	{
		m_onEditUpdates.try_emplace( &control
			, control.connect( EditEvent::eUpdated
				, [this, &control]( castor::U32String const & text )
				{
					onTextAction( control.getName(), text );
				} ) );
	}

	void ControlsManager::connectEvents( ExpandablePanelCtrl & control )
	{
		m_onPanelExpands.try_emplace( &control
			, control.connect( ExpandablePanelEvent::eExpand
				, [this, &control]()
				{
					onExpandAction( control.getName(), true );
				} ) );
		m_onPanelRetracts.try_emplace( &control
			, control.connect( ExpandablePanelEvent::eRetract
				, [this, &control]()
				{
					onExpandAction( control.getName(), false );
				} ) );
	}

	void ControlsManager::connectEvents( ListBoxCtrl & control )
	{
		m_onListSelects.try_emplace( &control
			, control.connect( ListBoxEvent::eSelected
				, [this, &control]( int index )
				{
					onSelectAction( control.getName(), index );
				} ) );
	}

	void ControlsManager::connectEvents( ScrollBarCtrl & control )
	{
		m_onScrollTracks.try_emplace( &control
			, control.connect( ScrollBarEvent::eThumbTrack
				, [this, &control]( int index )
				{
					onSelectAction( control.getName(), index );
				} ) );
		m_onScrollReleases.try_emplace( &control
			, control.connect( ScrollBarEvent::eThumbRelease
				, [this, &control]( int index )
				{
					onSelectAction( control.getName(), index );
				} ) );
	}

	void ControlsManager::connectEvents( SliderCtrl & control )
	{
		m_onSliderTracks.try_emplace( &control
			, control.connect( SliderEvent::eThumbTrack
				, [this, &control]( int index )
				{
					onSelectAction( control.getName(), index );
				} ) );
		m_onSliderReleases.try_emplace( &control
			, control.connect( SliderEvent::eThumbRelease
				, [this, &control]( int index )
				{
					onSelectAction( control.getName(), index );
				} ) );
	}

	void ControlsManager::disconnectEvents( ButtonCtrl const & control )
	{
		ctrlmgr::removeElem( control, m_onButtonClicks );
	}

	void ControlsManager::disconnectEvents( ComboBoxCtrl const & control )
	{
		ctrlmgr::removeElem( control, m_onComboSelects );
	}

	void ControlsManager::disconnectEvents( EditCtrl const & control )
	{
		ctrlmgr::removeElem( control, m_onEditUpdates );
	}

	void ControlsManager::disconnectEvents( ExpandablePanelCtrl const & control )
	{
		ctrlmgr::removeElem( control, m_onPanelExpands );
		ctrlmgr::removeElem( control, m_onPanelRetracts );
	}

	void ControlsManager::disconnectEvents( ListBoxCtrl const & control )
	{
		ctrlmgr::removeElem( control, m_onListSelects );
	}

	void ControlsManager::disconnectEvents( ScrollBarCtrl const & control )
	{
		ctrlmgr::removeElem( control, m_onScrollTracks );
		ctrlmgr::removeElem( control, m_onScrollReleases );
	}

	void ControlsManager::disconnectEvents( SliderCtrl const & control )
	{
		ctrlmgr::removeElem( control, m_onSliderTracks );
		ctrlmgr::removeElem( control, m_onSliderReleases );
	}

	bool ControlsManager::doInitialise()
	{
		return true;
	}

	void ControlsManager::doCleanup()
	{
		for ( auto const & [id, controls] : m_controlsById )
		{
			if ( auto control = controls )
			{
				control->destroy();
			}
		}

		auto lock( castor::makeUniqueLock( m_mutexControlsById ) );

		m_onButtonClicks.clear();
		m_onComboSelects.clear();
		m_onEditUpdates.clear();
		m_onListSelects.clear();
		m_onScrollTracks.clear();
		m_onScrollReleases.clear();
		m_onSliderTracks.clear();
		m_onSliderReleases.clear();
		m_onPanelExpands.clear();
		m_onPanelRetracts.clear();
		m_themes.clear();

		if ( auto event = m_event.exchange( nullptr ) )
		{
			event->skip();
		}

		m_controlsByZIndex.clear();
		m_rootControls.clear();
		m_controlsById.clear();
		m_movedControl = {};
		m_resizedControl = {};
		cleanupStyles();
	}

	EventHandlerRPtr ControlsManager::doGetMouseTargetableHandler( castor::Position const & position )const
	{
		if ( m_movedControl
			&& m_movedControl->isMoving() )
		{
			return m_movedControl;
		}

		if ( m_resizedControl
			&& m_resizedControl->isResizing() )
		{
			return m_resizedControl;
		}

		auto controls = doGetControlsByZIndex();
		EventHandlerRPtr result{};
		auto it = controls.rbegin();

		while ( !result && it != controls.rend() )
		{
			if ( ControlRPtr control = *it;
				control
					&& !control->isBackgroundInvisible()
					&& control->catchesMouseEvents()
					&& control->getAbsolutePosition().x() <= position.x()
					&& control->getAbsolutePosition().x() + int32_t( control->getSize().getWidth() ) > position.x()
					&& control->getAbsolutePosition().y() <= position.y()
					&& control->getAbsolutePosition().y() + int32_t( control->getSize().getHeight() ) > position.y() )
			{
				auto cursor = control->getCursor();

				if ( control->isResizable() )
				{
					auto [isOnN, isOnW, isOnS, isOnE] = control->isInResizeRange( position );

					if ( ( isOnN && isOnW )
						|| ( isOnS && isOnE ) )
					{
						cursor = MouseCursor::eSizeNWSE;
					}
					else if ( ( isOnN && isOnE )
						|| ( isOnS && isOnW ) )
					{
						cursor = MouseCursor::eSizeNESW;
					}
					else if ( isOnN || isOnS )
					{
						cursor = MouseCursor::eSizeNS;
					}
					else if ( isOnW || isOnE )
					{
						cursor = MouseCursor::eSizeWE;
					}
				}

				onCursorAction( cursor );
				result = control;
			}

			++it;
		}

		if ( !result )
		{
			onCursorAction( MouseCursor::eArrow );
		}

		return result;
	}

	void ControlsManager::doUpdate()
	{
		castor::Vector< ControlRPtr > result;
		castor::Vector< ControlRPtr > top;
		auto controls = getRootControls();
		result.reserve( controls.size() );
		top.reserve( controls.size() );
		uint32_t index{};

		for ( auto control : controls )
		{
			if ( control )
			{
				control->updateZIndex( index, result, top );
			}
		}

		for ( auto control : top )
		{
			control->adjustZIndex( index );
		}

		result.insert( result.end()
			, top.begin()
			, top.end() );
		doSetControlsByZIndex( castor::move( result ) );
	}

	void ControlsManager::doSetControlsByZIndex( castor::Vector< ControlRPtr > v )
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsByZIndex ) };
		m_controlsByZIndex = castor::move( v );
	}

	castor::Vector< ControlRPtr > ControlsManager::doGetControlsByZIndex()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsByZIndex ) };
		return m_controlsByZIndex;
	}

	castor::Map< ControlID, ControlRPtr > ControlsManager::doGetControlsById()const
	{
		ctrlmgr::LockType lock{ castor::makeUniqueLock( m_mutexControlsById ) };
		return m_controlsById;
	}

	void ControlsManager::doMarkDirty()
	{
		if ( !m_event )
		{
			m_event = getEngine()->postEvent( makeCpuFunctorEvent( CpuEventType::ePostCpuStep
				, [this]()
				{
					if ( m_event.exchange( nullptr ) )
					{
						doUpdate();
					}
				} ) );
		}
	}

	Engine * getEngine( GuiContext const & context )
	{
		return context.controls->getEngine();
	}
}
