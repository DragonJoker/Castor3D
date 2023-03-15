#include "Castor3D/Gui/Controls/CtrlListBox.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlStatic.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, ListBoxCtrl )

namespace castor3d
{
	static const uint32_t DefaultHeight = 25;

	ListBoxCtrl::ListBoxCtrl( SceneRPtr scene
		, castor::String const & name
		, ListBoxStyle * style
		, ControlRPtr parent )
		: ListBoxCtrl{ scene
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

	ListBoxCtrl::ListBoxCtrl( SceneRPtr scene
		, castor::String const & name
		, ListBoxStyle * style
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
		, m_initialValues{ values }
		, m_values{ values }
		, m_selected{ selected }
	{
		setBackgroundBorderSize( castor::Point4ui{ 1, 1, 1, 1 } );
		doUpdateStyle();
	}

	void ListBoxCtrl::appendItem( castor::String const & value )
	{
		m_values.push_back( value );

		if ( getControlsManager() )
		{
			auto item = doCreateItemCtrl( value
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
			m_initialValues.push_back( value );
		}
	}

	void ListBoxCtrl::removeItem( int value )
	{
		if ( uint32_t( value ) < m_values.size() && value >= 0 )
		{
			m_values.erase( m_values.begin() + value );

			if ( uint32_t( value ) < m_items.size() )
			{
				if ( value < m_selected )
				{
					setSelected( m_selected - 1 );
				}
				else if ( value == m_selected )
				{
					m_selectedItem.reset();
					m_selected = -1;
				}

				auto it = std::next( m_items.begin(), value );

				if ( getControlsManager() )
				{
					auto control = *it;
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

	void ListBoxCtrl::setItemText( int pindex, castor::String const & text )
	{
		auto index = uint32_t( pindex );

		if ( index < m_values.size() && pindex >= 0 )
		{
			m_values[index] = text;

			if ( index < m_items.size() )
			{
				auto item = m_items[index];

				if ( item )
				{
					item->setCaption( text );
				}
			}
		}
	}

	castor::String ListBoxCtrl::getItemText( int pindex )
	{
		auto index = uint32_t( pindex );
		castor::String result;

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

	void ListBoxCtrl::setSelected( int value )
	{
		auto & style = getStyle();
		auto selected = uint32_t( m_selected );

		if ( m_selected >= 0 && selected < m_items.size() )
		{
			auto item = m_items[selected];

			if ( item )
			{
				item->setStyle( &style.getItemStyle() );
				m_selectedItem.reset();
			}
		}

		m_selected = value;

		if ( m_selected >= 0 && selected < m_items.size() )
		{
			auto item = m_items[selected];

			if ( item )
			{
				item->setStyle( &style.getSelectedItemStyle() );
				m_selectedItem = item;
			}
		}
	}

	void ListBoxCtrl::doUpdateStyle()
	{
		auto & style = getStyle();
		int i = 0;

		for ( auto item : m_items )
		{
			if ( i++ == m_selected )
			{
				item->setStyle( &style.getSelectedItemStyle() );
			}
			else
			{
				item->setStyle( &style.getItemStyle() );
			}
		}
	}

	void ListBoxCtrl::doUpdateItems()
	{
		castor::Position position;

		for ( auto item : m_items )
		{
			item->setPosition( position );
			item->setSize( castor::Size( getSize().getWidth(), DefaultHeight ) );
			position.y() += DefaultHeight;
		}

		setBackgroundSize( castor::Size( getSize().getWidth(), uint32_t( m_items.size() * DefaultHeight ) ) );
	}

	StaticCtrlSPtr ListBoxCtrl::doCreateItemCtrl( castor::String const & value
		, uint32_t itemIndex )
	{
		auto & style = getStyle();
		auto item = std::make_shared< StaticCtrl >( m_scene
			, getName() + cuT( "_Item" ) + castor::string::toString( itemIndex )
			, &style.getItemStyle()
			, this
			, value
			, castor::Position{}
			, castor::Size{ getSize().getWidth(), DefaultHeight }
			, uint32_t( StaticFlag::eVAlignCenter ) );
		item->setCatchesMouseEvents( true );

		item->connectNC( MouseEventType::eEnter
			, [this]( ControlSPtr control, MouseEvent const & event )
			{
				onItemMouseEnter( control, event );
			} );
		item->connectNC( MouseEventType::eLeave
			, [this]( ControlSPtr control, MouseEvent const & event )
			{
				onItemMouseLeave( control, event );
			} );
		item->connectNC( MouseEventType::eReleased
			, [this]( ControlSPtr control, MouseEvent const & event )
			{
				onItemMouseLButtonUp( control, event );
			} );
		item->connectNC( KeyboardEventType::ePushed
			, [this]( ControlSPtr control, KeyboardEvent const & event )
			{
				onItemKeyDown( control, event );
			} );
		m_items.push_back( item );
		return item;
	}

	void ListBoxCtrl::doCreateItem( castor::String const & value
		, uint32_t itemIndex )
	{
		auto item = doCreateItemCtrl( value, itemIndex );
		getControlsManager()->create( item );
		item->setVisible( doIsVisible() );
	}

	void ListBoxCtrl::doCreate()
	{
		setBackgroundBorderSize( castor::Point4ui{ 1, 1, 1, 1 } );
		setSize( castor::Size( getSize().getWidth(), uint32_t( m_values.size() * DefaultHeight ) ) );

		EventHandler::connect( KeyboardEventType::ePushed
			, [this]( KeyboardEvent const & event )
			{
				onKeyDown( event );
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

	void ListBoxCtrl::doSetPosition( castor::Position const & value )
	{
		doUpdateItems();
	}

	void ListBoxCtrl::doSetSize( castor::Size const & value )
	{
		doUpdateItems();
	}

	void ListBoxCtrl::doSetVisible( bool visible )
	{
		for ( auto item : m_items )
		{
			item->setVisible( visible );
		}
	}

	void ListBoxCtrl::onItemMouseEnter( ControlSPtr control
		, MouseEvent const & event )
	{
		auto & style = getStyle();
		control->setStyle( &style.getHighlightedItemStyle() );
	}

	void ListBoxCtrl::onItemMouseLeave( ControlSPtr control
		, MouseEvent const & event )
	{
		auto & style = getStyle();

		if ( m_selectedItem.lock() == control )
		{
			control->setStyle( &style.getSelectedItemStyle() );
		}
		else
		{
			control->setStyle( &style.getItemStyle() );
		}
	}

	void ListBoxCtrl::onItemMouseLButtonUp( ControlSPtr control
		, MouseEvent const & event )
	{
		if ( event.getButton() == MouseButton::eLeft )
		{
			if ( m_selectedItem.lock() != control )
			{
				auto it = std::find_if( m_items.begin()
					, m_items.end()
					, [&control]( StaticCtrlSPtr const & lookup )
					{
						return lookup == control;
					} );

				int index = -1;

				if ( it != m_items.end() )
				{
					index = int( std::distance( m_items.begin(), it ) );
				}

				setSelected( index );
				m_signals[size_t( ListBoxEvent::eSelected )]( m_selected );
			}
		}
	}

	void ListBoxCtrl::onKeyDown( KeyboardEvent const & event )
	{
		if ( m_selected != -1 )
		{
			bool changed = false;
			int index = m_selected;

			if ( event.getKey() == KeyboardKey::eUp )
			{
				index--;
				changed = true;
			}
			else if ( event.getKey() == KeyboardKey::edown )
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

	void ListBoxCtrl::onItemKeyDown( ControlSPtr control
		, KeyboardEvent const & event )
	{
		onKeyDown( event );
	}
}
