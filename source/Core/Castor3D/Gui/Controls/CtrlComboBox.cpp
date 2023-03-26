#include "Castor3D/Gui/Controls/CtrlComboBox.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlButton.hpp"
#include "Castor3D/Gui/Controls/CtrlListBox.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, ComboBoxCtrl )

namespace castor3d
{
	ComboBoxCtrl::ComboBoxCtrl( SceneRPtr scene
		, castor::String const & name
		, ComboBoxStyle * style
		, ControlRPtr parent )
		: ComboBoxCtrl{ scene
			, name
			, style
			, parent
			, castor::StringArray{}
			, -1
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	ComboBoxCtrl::ComboBoxCtrl( SceneRPtr scene
		, castor::String const & name
		, ComboBoxStyle * style
		, ControlRPtr parent
		, castor::StringArray const & values
		, int selected
		, castor::Position const & position
		, castor::Size const & size
		, ControlFlagType flags
		, bool visible )
		: Control{ Type
			, scene
			, name
			, style
			, parent
			, position
			, size
			, flags
			, visible }
		, m_values{ values }
		, m_selected{ selected }
	{
		m_expand = std::make_shared< ButtonCtrl >( m_scene
			, cuT( "Expand" )
			, &style->getExpandStyle()
			, this
			, U"+"
			, castor::Position{ int32_t( size->x - size->y ), 0 }
			, castor::Size{ size->y, size->y } );
		m_expand->setVisible( visible );
		m_expandClickedConnection = m_expand->connect( ButtonEvent::eClicked
			, [this]()
			{
				doSwitchExpand();
			} );

		m_choices = std::make_shared< ListBoxCtrl >( m_scene
			, cuT( "Choices" )
			, &style->getElementsStyle()
			, this
			, m_values
			, m_selected
			, castor::Position{ 0, int32_t( size->y ) }
			, castor::Size{ size->x - size->y, ~( 0u ) }
			, uint64_t( ControlFlag::eAlwaysOnTop )
			, false );
		m_choicesSelectedConnection = m_choices->connect( ListBoxEvent::eSelected
			, [this]( int sel )
			{
				onSelected( sel );
			} );

		auto clientSize = getClientSize();
		auto text = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay();
		text->setPixelPosition( getClientOffset() );
		text->setPixelSize( { clientSize->x - clientSize->x, clientSize->y } );
		text->setVAlign( VAlign::eCenter );
		m_text = text;

		setStyle( style );
	}

	void ComboBoxCtrl::appendItem( castor::String const & value )
	{
		m_choices->appendItem( value );
	}

	void ComboBoxCtrl::removeItem( int value )
	{
		m_choices->removeItem( value );
	}

	void ComboBoxCtrl::setItemText( int index
		, castor::String const & text )
	{
		return m_choices->setItemText( index, text );
	}

	void ComboBoxCtrl::clear()
	{
		return m_choices->clear();
	}

	void ComboBoxCtrl::setSelected( int value )
	{
		return m_choices->setSelected( value );
	}

	castor::StringArray const & ComboBoxCtrl::getItems()const
	{
		return m_choices->getItems();
	}

	uint32_t ComboBoxCtrl::getItemCount()const
	{
		return m_choices->getItemCount();
	}

	int ComboBoxCtrl::getSelected()const
	{
		return m_choices->getSelected();
	}

	void ComboBoxCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		m_expand->setStyle( &style.getExpandStyle() );
		m_choices->setStyle( &style.getElementsStyle() );
	}

	void ComboBoxCtrl::doCreate()
	{
		auto & style = getStyle();
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		setBorderSize( castor::Point4ui( 1, 1, 1, 1 ) );

		m_expand->setPosition( castor::Position( int32_t( getSize()->x - getSize()->y ), 0 ) );
		m_expand->setSize( castor::Size( getSize()->y, getSize()->y ) );

		m_choices->setPosition( castor::Position( 0, int32_t( getSize()->y ) ) );
		m_choices->setSize( castor::Size( getSize()->x - getSize()->y, ~( 0u ) ) );

		EventHandler::connect( KeyboardEventType::ePushed
			, [this]( KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );
		NonClientEventHandler::connectNC( KeyboardEventType::ePushed
			, [this]( ControlSPtr control
				, KeyboardEvent const & event )
			{
				onNcKeyDown( control, event );
			} );

		if ( auto text = m_text.lock() )
		{
			auto clientSize = getClientSize();
			auto clientOffset = getClientOffset();
			text->setMaterial( style.getExpandStyle().getTextMaterial() );
			text->setPixelSize( castor::Size( clientSize->x - clientSize->y
				, clientSize->y ) );

			if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
			{
				text->setFont( style.getExpandStyle().getFontName() );
			}

			int sel = getSelected();

			if ( sel >= 0 && uint32_t( sel ) < getItemCount() )
			{
				text->setCaption( castor::string::toU32String( getItems()[uint32_t( sel )] ) );
			}
		}

		manager.create( m_expand );
		manager.create( m_choices );
		manager.connectEvents( *this );
	}

	void ComboBoxCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		getControlsManager()->disconnectEvents( *this );

		if ( m_expand )
		{
			manager.destroy( m_expand );
		}

		if ( m_choices )
		{
			manager.destroy( m_choices );
		}
	}

	void ComboBoxCtrl::doSetPosition( castor::Position const & value )
	{
		if ( auto text = m_text.lock() )
		{
			auto clientOffset = getClientOffset();
			text->setPixelPosition( clientOffset );
		}

		m_expand->setPosition( { int32_t( getSize()->x - getSize()->y ), 0 } );
		m_choices->setPosition( { 0, int32_t( getSize()->y ) } );
	}

	void ComboBoxCtrl::doSetSize( castor::Size const & value )
	{
		if ( auto text = m_text.lock() )
		{
			auto clientSize = getClientSize();
			text->setPixelSize( { clientSize->x - clientSize->x, clientSize->y } );
		}

		m_expand->setSize( castor::Size( value->y, value->y ) );
		m_choices->setSize( castor::Size( value->x - value->y, ~( 0u ) ) );
		m_expand->setPosition( castor::Position( int32_t( value->x - value->y ), 0 ) );
		m_choices->setPosition( castor::Position( 0, int32_t( value->y ) ) );
	}

	void ComboBoxCtrl::doSetBorderSize( castor::Point4ui const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelPosition( getClientOffset() );
			text->setPixelSize( getClientSize() );
		}

		auto & size = getSize();
		m_expand->setPosition( { int32_t( size->x - size->y ), 0 } );
		m_choices->setPosition( { 0, int32_t( size->y ) } );
		m_expand->setSize( { size->y, size->y } );
		m_choices->setSize( { size->x - size->y, ~( 0u ) } );
	}

	bool ComboBoxCtrl::doCatchesMouseEvents()const
	{
		return false;
	}

	void ComboBoxCtrl::onKeyDown( KeyboardEvent const & event )
	{
		if ( getSelected() != -1 )
		{
			bool changed = false;
			int index = getSelected();

			if ( event.getKey() == KeyboardKey::eUp )
			{
				index--;
				changed = true;
			}
			else if ( event.getKey() == KeyboardKey::eDown )
			{
				index++;
				changed = true;
			}

			if ( changed )
			{
				index = std::max( 0, std::min( index, int( getItemCount() - 1 ) ) );
				setSelected( index );
				onSelected( index );
			}
		}
	}

	void ComboBoxCtrl::onNcKeyDown( ControlSPtr control, KeyboardEvent const & event )
	{
		onKeyDown( event );
	}

	void ComboBoxCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
		}

		m_expand->setVisible( visible );
		m_choices->hide();
	}

	void ComboBoxCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text.lock() )
		{
			text->setOrder( index++, 0u );
		}
	}

	void ComboBoxCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( auto text = m_text.lock() )
		{
			text->setOrder( text->getLevel() + offset, 0u );
		}
	}

	void ComboBoxCtrl::doSwitchExpand()
	{
		m_choices->setVisible( !m_choices->isVisible() );
	}

	void ComboBoxCtrl::onSelected( int selected )
	{
		if ( selected >= 0 )
		{
			doSwitchExpand();
			TextOverlaySPtr text = m_text.lock();

			if ( text )
			{
				text->setCaption( castor::string::toU32String( m_choices->getItemText( selected ) ) );
			}
		}

		m_signals[size_t( ComboBoxEvent::eSelected )]( selected );
	}
}
