#include "CastorGui/Controls/CtrlComboBox.hpp"

#include "CastorGui/ControlsManager.hpp"
#include "CastorGui/Controls/CtrlButton.hpp"
#include "CastorGui/Controls/CtrlListBox.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	ComboBoxCtrl::ComboBoxCtrl( String const & name
		, ComboBoxStyle * style
		, ControlRPtr parent
		, uint32_t id )
		: ComboBoxCtrl{ name
			, style
			, parent
			, id
			, castor::StringArray{}
			, -1
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	ComboBoxCtrl::ComboBoxCtrl( String const & name
		, ComboBoxStyle * style
		, ControlRPtr parent
		, uint32_t id
		, castor::StringArray const & values
		, int selected
		, castor::Position const & position
		, castor::Size const & size
		, uint32_t flags
		, bool visible )
		: Control{ Type
			, name
			, style
			, parent
			, id
			, position
			, size
			, flags
			, visible }
		, m_values{ values }
		, m_selected{ selected }
	{
		m_expand = std::make_shared< ButtonCtrl >( name + cuT( "_Expand" )
			, &style->getButtonStyle()
			, this
			, getId() << 12
			, cuT( "+" )
			, castor::Position{ int32_t( size.getWidth() - size.getHeight() ), 0 }
			, castor::Size{ size.getHeight(), size.getHeight() } );
		m_expand->setVisible( visible );
		m_expandClickedConnection = m_expand->connect( ButtonEvent::eClicked
			, [this]()
			{
				doSwitchExpand();
			} );

		m_choices = std::make_shared< ListBoxCtrl >( name + cuT( "_Choices" )
			, &style->getListBoxStyle()
			, this
			, ( getId() << 12 ) + 1
			, m_values
			, m_selected
			, castor::Position{ 0, int32_t( size.getHeight() ) }
			, castor::Size{ size.getWidth() - size.getHeight(), ~( 0u ) }
			, 0
			, false );
		m_choicesSelectedConnection = m_choices->connect( ListBoxEvent::eSelected
			, [this]( int selected )
			{
				onSelected( selected );
			} );

		auto text = getEngine().getOverlayCache().add( cuT( "T_CtrlCombo_" ) + castor::string::toString( getId() )
			, getEngine()
			, OverlayType::eText
			, nullptr
			, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( Size( getSize().getWidth() - getSize().getHeight(), getSize().getHeight() ) );
		text->setVAlign( VAlign::eCenter );
		m_text = text;

		doUpdateStyle();
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
		m_expand->setStyle( &style.getButtonStyle() );
		m_choices->setStyle( &style.getListBoxStyle() );
	}

	void ComboBoxCtrl::doCreate()
	{
		auto & style = getStyle();
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		setBackgroundBorders( castor::Rectangle( 1, 1, 1, 1 ) );

		m_expand->setPosition( castor::Position( int32_t( getSize().getWidth() - getSize().getHeight() ), 0 ) );
		m_expand->setSize( castor::Size( getSize().getHeight(), getSize().getHeight() ) );

		m_choices->setPosition( castor::Position( 0, int32_t( getSize().getHeight() ) ) );
		m_choices->setSize( castor::Size( getSize().getWidth() - getSize().getHeight(), ~( 0u ) ) );

		EventHandler::connect( castor3d::KeyboardEventType::ePushed
			, [this]( castor3d::KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );
		NonClientEventHandler::connectNC( castor3d::KeyboardEventType::ePushed
			, [this]( ControlSPtr control
				, castor3d::KeyboardEvent const & event )
			{
				onNcKeyDown( control, event );
			} );

		auto text = m_text.lock();
		text->setMaterial( style.getButtonStyle().getTextMaterial() );
		text->setPixelSize( castor::Size( getSize().getWidth() - getSize().getHeight(), getSize().getHeight() ) );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont( style.getButtonStyle().getFontName() );
		}

		int sel = getSelected();

		if ( sel >= 0 && uint32_t( sel ) < getItemCount() )
		{
			text->setCaption( getItems()[uint32_t( sel )] );
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

	void ComboBoxCtrl::doSetPosition( castor::Position const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			//l_text->setPixelPosition( p_value );
			text.reset();
		}

		m_expand->setPosition( Position( int32_t( getSize().getWidth() - getSize().getHeight() ), 0 ) );
		m_choices->setPosition( Position( 0, int32_t( getSize().getHeight() ) ) );
	}

	void ComboBoxCtrl::doSetSize( castor::Size const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelSize( p_value );
			text.reset();
		}

		m_expand->setSize( Size( p_value.getHeight(), p_value.getHeight() ) );
		m_choices->setSize( Size( p_value.getWidth() - p_value.getHeight(), ~( 0u ) ) );
		m_expand->setPosition( Position( int32_t( p_value.getWidth() - p_value.getHeight() ), 0 ) );
		m_choices->setPosition( Position( 0, int32_t( p_value.getHeight() ) ) );
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
