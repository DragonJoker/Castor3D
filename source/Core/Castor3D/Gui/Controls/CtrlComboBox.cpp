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

CU_ImplementSmartPtr( c3d, ComboBoxCtrl )

namespace c3d
{
	ComboBoxCtrl::ComboBoxCtrl( SceneRPtr scene
		, String const & name
		, ComboBoxStyle * style
		, ControlRPtr parent )
		: ComboBoxCtrl{ scene
			, name
			, style
			, parent
			, StringArray{}
			, -1
			, Position{}
			, Size{}
			, 0
			, true }
	{
	}

	ComboBoxCtrl::ComboBoxCtrl( SceneRPtr scene
		, String const & name
		, ComboBoxStyle * style
		, ControlRPtr parent
		, StringArray const & values
		, int selected
		, Position const & position
		, Size const & size
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
		m_expand = getEngine().getControlsManager()->registerControlT( makeUnique< ButtonCtrl >( m_scene
			, cuT( "Expand" )
			, &style->getExpandStyle()
			, this
			, U"+"
			, Position{ int32_t( size->x - size->y ), 0 }
			, Size{ size->y, size->y } ) );
		m_expand->setVisible( visible );
		m_expandClickedConnection = m_expand->connect( ButtonEvent::eClicked
			, [this]()
			{
				doSwitchExpand();
			} );

		m_choices = getEngine().getControlsManager()->registerControlT( makeUnique< ListBoxCtrl >( m_scene
			, cuT( "Choices" )
			, &style->getElementsStyle()
			, this
			, m_values
			, m_selected
			, Position{ 0, int32_t( size->y ) }
			, Size{ size->x - size->y, ~0u }
			, uint64_t( ControlFlag::eAlwaysOnTop )
			, false ) );
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
				, &getBackgroundOverlay() )->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, &getBackgroundOverlay() )->getTextOverlay();
		text->setPixelPosition( getClientOffset() );
		text->setPixelSize( { getSize()->x - clientSize->x, clientSize->y } );
		text->setVAlign( VAlign::eCenter );
		m_text = text;

		setStyle( style );
	}

	ComboBoxCtrl::~ComboBoxCtrl()noexcept
	{
		if ( m_text )
		{
			if ( m_scene )
			{
				m_scene->removeOverlay( getName() + cuT( "/Text" ), true );
			}
			else
			{
				getEngine().removeOverlay( getName() + cuT( "/Text" ), true );
			}
		}

		auto & manager = *getEngine().getControlsManager();
		manager.unregisterControl( *m_choices );
		manager.unregisterControl( *m_expand );
	}

	void ComboBoxCtrl::appendItem( String const & value )
	{
		m_choices->appendItem( value );
	}

	void ComboBoxCtrl::removeItem( int value )
	{
		m_choices->removeItem( value );
	}

	void ComboBoxCtrl::setItemText( int index
		, String const & text )
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

	StringArray const & ComboBoxCtrl::getItems()const
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
		setBorderSize( Point4ui( 1, 1, 1, 1 ) );

		m_expand->setPosition( Position( int32_t( getSize()->x - getSize()->y ), 0 ) );
		m_expand->setSize( Size( getSize()->y, getSize()->y ) );

		m_choices->setPosition( Position( 0, int32_t( getSize()->y ) ) );
		m_choices->setSize( Size( getSize()->x - getSize()->y, ~0u ) );

		EventHandler::connect( KeyboardEventType::ePushed
			, [this]( KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );
		NonClientEventHandler::connectNC( KeyboardEventType::ePushed
			, [this]( ControlRPtr
				, KeyboardEvent const & event )
			{
				onNcKeyDown( event );
			} );

		if ( auto text = m_text )
		{
			auto clientSize = getClientSize();
			text->setMaterial( style.getExpandStyle().getTextMaterial() );
			text->setPixelSize( Size( clientSize->x - clientSize->y
				, clientSize->y ) );

			if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
			{
				text->setFont( style.getExpandStyle().getFontName() );
			}

			int sel = getSelected();

			if ( sel >= 0 && uint32_t( sel ) < getItemCount() )
			{
				text->setCaption( toUtf8U32String( getItems()[uint32_t( sel )] ) );
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

	void ComboBoxCtrl::doSetPosition( Position const & value )
	{
		if ( auto text = m_text )
		{
			auto clientOffset = getClientOffset();
			text->setPixelPosition( clientOffset );
		}

		m_expand->setPosition( { int32_t( getSize()->x - getSize()->y ), 0 } );
		m_choices->setPosition( { 0, int32_t( getSize()->y ) } );
	}

	void ComboBoxCtrl::doSetSize( Size const & value )
	{
		if ( auto text = m_text )
		{
			auto clientSize = getClientSize();
			text->setPixelSize( { getSize()->x - clientSize->x, clientSize->y } );
		}

		m_expand->setSize( Size( value->y, value->y ) );
		m_choices->setSize( Size( value->x - value->y, ~0u ) );
		m_expand->setPosition( Position( int32_t( value->x - value->y ), 0 ) );
		m_choices->setPosition( Position( 0, int32_t( value->y ) ) );
	}

	void ComboBoxCtrl::doSetBorderSize( Point4ui const & value )
	{
		if ( auto text = m_text )
		{
			text->setPixelPosition( getClientOffset() );
			text->setPixelSize( getClientSize() );
		}

		auto & size = getSize();
		m_expand->setPosition( { int32_t( size->x - size->y ), 0 } );
		m_choices->setPosition( { 0, int32_t( size->y ) } );
		m_expand->setSize( { size->y, size->y } );
		m_choices->setSize( { size->x - size->y, ~0u } );
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

	void ComboBoxCtrl::onNcKeyDown( KeyboardEvent const & event )
	{
		onKeyDown( event );
	}

	void ComboBoxCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text )
		{
			text->setVisible( visible );
		}

		m_expand->setVisible( visible );
		m_choices->hide();
	}

	void ComboBoxCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text )
		{
			text->setOrder( index, 0u );
			++index;
		}
	}

	void ComboBoxCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( auto text = m_text )
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

			if ( auto text = m_text )
			{
				text->setCaption( toUtf8U32String( m_choices->getItemText( selected ) ) );
			}
		}

		m_signals[size_t( ComboBoxEvent::eSelected )]( selected );
	}
}
