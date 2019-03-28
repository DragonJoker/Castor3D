#include "CastorGui/CtrlComboBox.hpp"

#include "CastorGui/ControlsManager.hpp"
#include "CastorGui/CtrlButton.hpp"
#include "CastorGui/CtrlListBox.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Overlay/Overlay.hpp>

#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	ComboBoxCtrl::ComboBoxCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: ComboBoxCtrl( p_name
			, engine
			, p_parent
			, p_id
			, StringArray()
			, -1
			, Position()
			, Size()
			, 0
			, true )
	{
	}

	ComboBoxCtrl::ComboBoxCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, StringArray const & p_values
		, int p_selected
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control( ControlType::eComboBox
			, p_name
			, engine
			, p_parent
			, p_id
			, p_position
			, p_size
			, p_style
			, p_visible )
		, m_values( p_values )
		, m_selected( p_selected )
	{
		m_expand = std::make_shared< ButtonCtrl >( p_name + cuT( "_Expand" )
			, engine
			, this
			, getId() << 12
			, cuT( "+" )
			, Position( p_size.getWidth() - p_size.getHeight(), 0 )
			, Size( p_size.getHeight(), p_size.getHeight() ) );
		m_expand->setVisible( p_visible );
		m_expandClickedConnection = m_expand->connect( ButtonEvent::eClicked
			, [this]()
			{
				doSwitchExpand();
			} );

		m_choices = std::make_shared< ListBoxCtrl >( p_name + cuT( "_Choices" )
			, engine
			, this
			, ( getId() << 12 ) + 1
			, m_values
			, m_selected
			, Position( 0, p_size.getHeight() )
			, Size( p_size.getWidth() - p_size.getHeight(), -1 )
			, 0
			, false );
		m_choicesSelectedConnection = m_choices->connect( ListBoxEvent::eSelected
			, [this]( int selected )
			{
				onSelected( selected );
			} );

		TextOverlaySPtr text = getEngine().getOverlayCache().add( cuT( "T_CtrlCombo_" ) + string::toString( getId() )
			, OverlayType::eText
			, nullptr
			, getBackground()->getOverlay().shared_from_this() )->getTextOverlay();
		text->setPixelSize( Size( getSize().getWidth() - getSize().getHeight(), getSize().getHeight() ) );
		text->setVAlign( VAlign::eCenter );
		m_text = text;
	}

	ComboBoxCtrl::~ComboBoxCtrl()
	{
	}

	void ComboBoxCtrl::setSelectedItemBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->setSelectedItemBackgroundMaterial( p_material );
	}

	void ComboBoxCtrl::setSelectedItemForegroundMaterial( MaterialSPtr p_material )
	{
		m_choices->setSelectedItemForegroundMaterial( p_material );
	}

	void ComboBoxCtrl::setHighlightedItemBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->setHighlightedItemBackgroundMaterial( p_material );
	}

	void ComboBoxCtrl::setItemBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->setItemBackgroundMaterial( p_material );
	}

	MaterialSPtr ComboBoxCtrl::getSelectedItemBackgroundMaterial()const
	{
		return m_choices->getSelectedItemBackgroundMaterial();
	}

	MaterialSPtr ComboBoxCtrl::getSelectedItemForegroundMaterial()const
	{
		return m_choices->getSelectedItemForegroundMaterial();
	}

	MaterialSPtr ComboBoxCtrl::getHighlightedItemBackgroundMaterial()const
	{
		return m_choices->getHighlightedItemBackgroundMaterial();
	}

	MaterialSPtr ComboBoxCtrl::getItemBackgroundMaterial()const
	{
		return m_choices->getItemBackgroundMaterial();
	}

	void ComboBoxCtrl::appendItem( String  const & p_value )
	{
		m_choices->appendItem( p_value );
	}

	void ComboBoxCtrl::removeItem( int p_value )
	{
		m_choices->removeItem( p_value );
	}

	void ComboBoxCtrl::setItemText( int p_index, String const & p_text )
	{
		return m_choices->setItemText( p_index, p_text );
	}

	void ComboBoxCtrl::clear()
	{
		return m_choices->clear();
	}

	void ComboBoxCtrl::setSelected( int p_value )
	{
		return m_choices->setSelected( p_value );
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

	void ComboBoxCtrl::setFont( castor::String const & p_font )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setFont( p_font );
		}

		m_choices->setFont( p_font );
	}

	void ComboBoxCtrl::doCreate()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		setBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );

		m_expand->setForegroundMaterial( getForegroundMaterial() );
		m_expand->setPosition( Position( getSize().getWidth() - getSize().getHeight(), 0 ) );
		m_expand->setSize( Size( getSize().getHeight(), getSize().getHeight() ) );

		m_choices->setBackgroundMaterial( getBackgroundMaterial() );
		m_choices->setForegroundMaterial( getForegroundMaterial() );
		m_choices->setPosition( Position( 0, getSize().getHeight() ) );
		m_choices->setSize( Size( getSize().getWidth() - getSize().getHeight(), -1 ) );

		EventHandler::connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			onKeyDown( p_event );
		} );
		NonClientEventHandler::connectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			onNcKeyDown( p_control, p_event );
		} );

		TextOverlaySPtr text = m_text.lock();
		text->setMaterial( getForegroundMaterial() );
		text->setPixelSize( Size( getSize().getWidth() - getSize().getHeight(), getSize().getHeight() ) );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont(manager.getDefaultFont()->getName() );
		}

		int sel = getSelected();

		if ( sel >= 0 && uint32_t( sel ) < getItemCount() )
		{
			text->setCaption( getItems()[sel] );
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

	void ComboBoxCtrl::doSetPosition( Position const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			//l_text->setPixelPosition( p_value );
			text.reset();
		}

		m_expand->setPosition( Position( getSize().getWidth() - getSize().getHeight(), 0 ) );
		m_choices->setPosition( Position( 0, getSize().getHeight() ) );
	}

	void ComboBoxCtrl::doSetSize( Size const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelSize( p_value );
			text.reset();
		}

		m_expand->setSize( Size( p_value.getHeight(), p_value.getHeight() ) );
		m_choices->setSize( Size( p_value.getWidth() - p_value.getHeight(), -1 ) );
		m_expand->setPosition( Position( p_value.getWidth() - p_value.getHeight(), 0 ) );
		m_choices->setPosition( Position( 0, p_value.getHeight() ) );
	}

	void ComboBoxCtrl::doSetBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->setBackgroundMaterial( getBackgroundMaterial() );
	}

	void ComboBoxCtrl::doSetForegroundMaterial( MaterialSPtr p_material )
	{
		m_expand->setForegroundMaterial( getForegroundMaterial() );
		m_choices->setForegroundMaterial( getForegroundMaterial() );
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setMaterial( p_material );
			text.reset();
		}
	}

	bool ComboBoxCtrl::doCatchesMouseEvents()const
	{
		return false;
	}

	void ComboBoxCtrl::onKeyDown( KeyboardEvent const & p_event )
	{
		if ( getSelected() != -1 )
		{
			bool changed = false;
			int index = getSelected();

			if ( p_event.getKey() == KeyboardKey::eUp )
			{
				index--;
				changed = true;
			}
			else if ( p_event.getKey() == KeyboardKey::edown )
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

	void ComboBoxCtrl::onNcKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event )
	{
		onKeyDown( p_event );
	}

	void ComboBoxCtrl::doSetVisible( bool p_visible )
	{
		m_expand->setVisible( p_visible );
		m_choices->hide();
	}

	void ComboBoxCtrl::doSwitchExpand()
	{
		m_choices->setVisible( !m_choices->isVisible() );
	}

	void ComboBoxCtrl::onSelected( int p_selected )
	{
		if ( p_selected >= 0 )
		{
			doSwitchExpand();
			TextOverlaySPtr text = m_text.lock();

			if ( text )
			{
				text->setCaption( m_choices->getItemText( p_selected ) );
			}
		}

		m_signals[size_t( ComboBoxEvent::eSelected )]( p_selected );
	}
}
