#include "CastorGui/CtrlListBox.hpp"

#include "CastorGui/ControlsManager.hpp"
#include "CastorGui/CtrlStatic.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	ListBoxCtrl::ListBoxCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: ListBoxCtrl( p_name
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

	ListBoxCtrl::ListBoxCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, StringArray const & p_values
		, int p_selected
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control( ControlType::eListBox
			, p_name
			, engine
			, p_parent
			, p_id
			, p_position
			, p_size
			, p_style
			, p_visible )
		, m_initialValues( p_values )
		, m_values( p_values )
		, m_selected( p_selected )
	{
	}

	void ListBoxCtrl::setTextMaterial( MaterialRPtr p_material )
	{
		m_textMaterial = p_material;
	}

	void ListBoxCtrl::setSelectedItemBackgroundMaterial( MaterialRPtr p_value )
	{
		m_selectedItemBackgroundMaterial = p_value;
		int i = 0;

		for ( auto item : m_items )
		{
			if ( i++ == m_selected )
			{
				item->setBackgroundMaterial( p_value );
			}
		}
	}

	void ListBoxCtrl::setSelectedItemForegroundMaterial( MaterialRPtr p_value )
	{
		m_selectedItemForegroundMaterial = p_value;
		int i = 0;

		for ( auto item : m_items )
		{
			if ( i++ == m_selected )
			{
				item->setForegroundMaterial( p_value );
			}
		}
	}

	void ListBoxCtrl::appendItem( String const & p_value )
	{
		m_values.push_back( p_value );

		if ( getControlsManager() )
		{
			StaticCtrlSPtr item = doCreateItemCtrl( p_value
				, uint32_t( m_values.size() - 1u ) );
			getEngine().postEvent( makeGpuFunctorEvent( EventType::ePreRender
				, [this, item]( RenderDevice const & device
					, QueueData const & queueData )
				{
					getControlsManager()->create( item );
				} ) );

			doUpdateItems();
		}
		else
		{
			m_initialValues.push_back( p_value );
		}
	}

	void ListBoxCtrl::removeItem( int p_value )
	{
		if ( uint32_t( p_value ) < m_values.size() && p_value >= 0 )
		{
			m_values.erase( m_values.begin() + p_value );

			if ( uint32_t( p_value ) < m_items.size() )
			{
				if ( p_value < m_selected )
				{
					setSelected( m_selected - 1 );
				}
				else if ( p_value == m_selected )
				{
					m_selectedItem.reset();
					m_selected = -1;
				}

				auto it = m_items.begin() + p_value;

				if ( getControlsManager() )
				{
					ControlSPtr control = *it;
					getEngine().postEvent( makeGpuFunctorEvent( EventType::ePreRender
						, [this, control]( RenderDevice const & device
							, QueueData const & queueData )
						{
							getControlsManager()->destroy( control );
						} ) );
				}

				m_items.erase( it );
				doUpdateItems();
			}
		}
	}

	void ListBoxCtrl::setItemText( int p_index, String const & p_text )
	{
		auto index = uint32_t( p_index );

		if ( index < m_values.size() && p_index >= 0 )
		{
			m_values[index] = p_text;

			if ( index < m_items.size() )
			{
				StaticCtrlSPtr item = m_items[index];

				if ( item )
				{
					item->setCaption( p_text );
				}
			}
		}
	}

	String ListBoxCtrl::getItemText( int p_index )
	{
		auto index = uint32_t( p_index );
		String result;

		if ( index < m_values.size() )
		{
			result = m_values[index];
		}

		return result;
	}

	void ListBoxCtrl::clear()
	{
		m_values.clear();
		m_items.clear();
		m_selectedItem.reset();
		m_selected = -1;
	}

	void ListBoxCtrl::setSelected( int p_value )
	{
		auto selected = uint32_t( m_selected );

		if ( m_selected >= 0 && selected < m_items.size() )
		{
			StaticCtrlSPtr item = m_items[selected];

			if ( item )
			{
				item->setBackgroundMaterial( getItemBackgroundMaterial() );
				item->setForegroundMaterial( getForegroundMaterial() );
				m_selectedItem.reset();
			}
		}

		m_selected = p_value;

		if ( m_selected >= 0 && selected < m_items.size() )
		{
			StaticCtrlSPtr item = m_items[selected];

			if ( item )
			{
				item->setBackgroundMaterial( getSelectedItemBackgroundMaterial() );
				item->setForegroundMaterial( getSelectedItemForegroundMaterial() );
				m_selectedItem = item;
			}
		}
	}

	void ListBoxCtrl::setFont( castor::String const & p_font )
	{
		m_fontName = p_font;

		for ( auto item : m_items )
		{
			item->setFont( m_fontName );
		}
	}

	void ListBoxCtrl::doUpdateItems()
	{
		Position position;

		for ( auto item : m_items )
		{
			item->setPosition( position );
			item->setSize( Size( getSize().getWidth(), DEFAULT_HEIGHT ) );
			position.y() += DEFAULT_HEIGHT;
		}

		BorderPanelOverlaySPtr background = getBackground();

		if ( background )
		{
			background->setPixelSize( Size( getSize().getWidth(), uint32_t( m_items.size() * DEFAULT_HEIGHT ) ) );
		}
	}

	StaticCtrlSPtr ListBoxCtrl::doCreateItemCtrl( String const & p_value
		, uint32_t itemIndex )
	{
		StaticCtrlSPtr item = std::make_shared< StaticCtrl >( getName() + cuT( "_Item" ) + string::toString( itemIndex )
			, getEngine()
			, this
			, p_value
			, Position()
			, Size( getSize().getWidth(), DEFAULT_HEIGHT ), uint32_t( StaticStyle::eVAlignCenter ) );
		item->setCatchesMouseEvents( true );

		item->connectNC( MouseEventType::eEnter, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			onItemMouseEnter( p_control, p_event );
		} );
		item->connectNC( MouseEventType::eLeave, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			onItemMouseLeave( p_control, p_event );
		} );
		item->connectNC( MouseEventType::eReleased, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			onItemMouseLButtonUp( p_control, p_event );
		} );
		item->connectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			onItemKeyDown( p_control, p_event );
		} );

		if ( m_fontName.empty() )
		{
			item->setFont( getControlsManager()->getDefaultFont().lock()->getName() );
		}
		else
		{
			item->setFont( m_fontName );
		}

		m_items.push_back( item );
		return item;
	}

	void ListBoxCtrl::doCreateItem( String const & p_value
		, uint32_t itemIndex )
	{
		StaticCtrlSPtr item = doCreateItemCtrl( p_value, itemIndex );
		getControlsManager()->create( item );
		item->setBackgroundMaterial( getItemBackgroundMaterial() );
		item->setForegroundMaterial( getForegroundMaterial() );
		item->setTextMaterial( getTextMaterial() );
		item->setVisible( doIsVisible() );
	}

	void ListBoxCtrl::doCreate()
	{
		auto material = getTextMaterial();

		if ( !material )
		{
			m_textMaterial = getForegroundMaterial();
		}

		material = getSelectedItemBackgroundMaterial();

		if ( !material )
		{
			setSelectedItemBackgroundMaterial( getEngine().getMaterialCache().find( cuT( "DarkBlue" ) ).lock().get() );
		}

		material = getSelectedItemForegroundMaterial();

		if ( !material )
		{
			setSelectedItemForegroundMaterial( getEngine().getMaterialCache().find( cuT( "White" ) ).lock().get() );
		}

		material = getHighlightedItemBackgroundMaterial();

		if ( !material )
		{
			RgbColour colour = getMaterialColour( *getBackgroundMaterial()->getPass( 0u ) );
			colour.red() = std::min( 1.0f, float( colour.red() ) / 2.0f );
			colour.green() = std::min( 1.0f, float( colour.green() ) / 2.0f );
			colour.blue() = std::min( 1.0f, float( colour.blue() ) / 2.0f );
			setHighlightedItemBackgroundMaterial( createMaterial( getEngine(), getBackgroundMaterial()->getName() + cuT( "_Highlight" ), colour ) );
		}

		setBackgroundBorders( castor::Rectangle( 1, 1, 1, 1 ) );
		setSize( Size( getSize().getWidth(), uint32_t( m_values.size() * DEFAULT_HEIGHT ) ) );

		EventHandler::connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			onKeyDown( p_event );
		} );
		uint32_t index = 0u;

		for ( auto value : m_initialValues )
		{
			doCreateItem( value, index );
			++index;
		}

		m_initialValues.clear();
		doUpdateItems();
		setSelected( m_selected );
		getControlsManager()->connectEvents( *this );
	}

	void ListBoxCtrl::doDestroy()
	{
		CU_Require( getControlsManager() );
		auto & manager = *getControlsManager();
		manager.disconnectEvents( *this );

		for ( auto item : m_items )
		{
			manager.destroy( item );
		}

		m_items.clear();
		m_selectedItem.reset();
	}

	void ListBoxCtrl::doSetPosition( Position const & p_value )
	{
		doUpdateItems();
	}

	void ListBoxCtrl::doSetSize( Size const & p_value )
	{
		doUpdateItems();
	}

	void ListBoxCtrl::doSetBackgroundMaterial( MaterialRPtr p_material )
	{
		int i = 0;
		RgbColour colour = getMaterialColour( *p_material->getPass( 0u ) );
		setItemBackgroundMaterial( p_material );
		colour.red() = std::min( 1.0f, colour.red() / 2.0f );
		colour.green() = std::min( 1.0f, colour.green() / 2.0f );
		colour.blue() = std::min( 1.0f, colour.blue() / 2.0f );
		setHighlightedItemBackgroundMaterial( createMaterial( getEngine(), getBackgroundMaterial()->getName() + cuT( "_Highlight" ), colour ) );

		setMaterialColour( *p_material->getPass( 0u ), colour );

		for ( auto item : m_items )
		{
			if ( i++ != m_selected )
			{
				item->setBackgroundMaterial( p_material );
			}
		}
	}

	void ListBoxCtrl::doSetForegroundMaterial( MaterialRPtr p_material )
	{
		int i = 0;

		for ( auto item : m_items )
		{
			if ( i++ != m_selected )
			{
				item->setForegroundMaterial( p_material );
			}
		}
	}

	void ListBoxCtrl::doSetVisible( bool p_visible )
	{
		for ( auto item : m_items )
		{
			item->setVisible( p_visible );
		}
	}

	void ListBoxCtrl::onItemMouseEnter( ControlSPtr p_control, MouseEvent const & p_event )
	{
		p_control->setBackgroundMaterial( getHighlightedItemBackgroundMaterial() );
	}

	void ListBoxCtrl::onItemMouseLeave( ControlSPtr p_control, MouseEvent const & p_event )
	{
		if ( m_selectedItem.lock() == p_control )
		{
			p_control->setBackgroundMaterial( getSelectedItemBackgroundMaterial() );
		}
		else
		{
			p_control->setBackgroundMaterial( getItemBackgroundMaterial() );
		}
	}

	void ListBoxCtrl::onItemMouseLButtonUp( ControlSPtr p_control, MouseEvent const & p_event )
	{
		if ( p_event.getButton() == MouseButton::eLeft )
		{
			if ( m_selectedItem.lock() != p_control )
			{
				int index = -1;
				auto it = m_items.begin();
				int i = 0;

				while ( index == -1 && it != m_items.end() )
				{
					if ( *it == p_control )
					{
						index = i;
					}

					++it;
					++i;
				}

				setSelected( index );
				m_signals[size_t( ListBoxEvent::eSelected )]( m_selected );
			}
		}
	}

	void ListBoxCtrl::onKeyDown( KeyboardEvent const & p_event )
	{
		if ( m_selected != -1 )
		{
			bool changed = false;
			int index = m_selected;

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
				index = std::max( 0, std::min( index, int( m_items.size() - 1 ) ) );
				setSelected( index );
				m_signals[size_t( ListBoxEvent::eSelected )]( index );
			}
		}
	}

	void ListBoxCtrl::onItemKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event )
	{
		onKeyDown( p_event );
	}
}
