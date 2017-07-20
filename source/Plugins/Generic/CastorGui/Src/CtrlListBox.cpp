#include "CtrlListBox.hpp"

#include "ControlsManager.hpp"
#include "CtrlStatic.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Texture/TextureUnit.hpp>

#include <Event/Frame/FunctorEvent.hpp>
#include <Material/Pass.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Graphics/Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	ListBoxCtrl::ListBoxCtrl( String const & p_name
		, Engine & p_engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: ListBoxCtrl( p_name
			, p_engine
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
		, Engine & p_engine
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
			, p_engine
			, p_parent
			, p_id
			, p_position
			, p_size
			, p_style
			, p_visible )
		, m_values( p_values )
		, m_initialValues( p_values )
		, m_selected( p_selected )
	{
	}

	ListBoxCtrl::~ListBoxCtrl()
	{
	}

	void ListBoxCtrl::SetSelectedItemBackgroundMaterial( MaterialSPtr p_value )
	{
		m_selectedItemBackgroundMaterial = p_value;
		int i = 0;

		for ( auto item : m_items )
		{
			if ( i++ == m_selected )
			{
				item->SetBackgroundMaterial( p_value );
			}
		}
	}

	void ListBoxCtrl::SetSelectedItemForegroundMaterial( MaterialSPtr p_value )
	{
		m_selectedItemForegroundMaterial = p_value;
		int i = 0;

		for ( auto item : m_items )
		{
			if ( i++ == m_selected )
			{
				item->SetForegroundMaterial( p_value );
			}
		}
	}

	void ListBoxCtrl::AppendItem( String const & p_value )
	{
		m_values.push_back( p_value );

		if ( GetControlsManager() )
		{
			StaticCtrlSPtr item = DoCreateItemCtrl( p_value );
			GetEngine().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, item]()
			{
				GetControlsManager()->Create( item );
			} ) );

			DoUpdateItems();
		}
		else
		{
			m_initialValues.push_back( p_value );
		}
	}

	void ListBoxCtrl::RemoveItem( int p_value )
	{
		if ( uint32_t( p_value ) < m_values.size() && p_value >= 0 )
		{
			m_values.erase( m_values.begin() + p_value );

			if ( uint32_t( p_value ) < m_items.size() )
			{
				if ( p_value < m_selected )
				{
					SetSelected( m_selected - 1 );
				}
				else if ( p_value == m_selected )
				{
					m_selectedItem.reset();
					m_selected = -1;
				}

				auto it = m_items.begin() + p_value;

				if ( GetControlsManager() )
				{
					ControlSPtr control = *it;
					GetEngine().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, control]()
					{
						GetControlsManager()->Destroy( control );
					} ) );
				}

				m_items.erase( it );
				DoUpdateItems();
			}
		}
	}

	void ListBoxCtrl::SetItemText( int p_index, String const & p_text )
	{
		if ( uint32_t( p_index ) < m_values.size() && p_index >= 0 )
		{
			m_values[p_index] = p_text;

			if ( uint32_t( p_index ) < m_items.size() )
			{
				StaticCtrlSPtr item = m_items[p_index];

				if ( item )
				{
					item->SetCaption( p_text );
				}
			}
		}
	}

	String ListBoxCtrl::GetItemText( int p_index )
	{
		String result;

		if ( uint32_t( p_index ) < m_values.size() && p_index >= 0 )
		{
			result = m_values[p_index];
		}

		return result;
	}

	void ListBoxCtrl::Clear()
	{
		m_values.clear();
		m_items.clear();
		m_selectedItem.reset();
		m_selected = -1;
	}

	void ListBoxCtrl::SetSelected( int p_value )
	{
		if ( m_selected >= 0 && uint32_t( m_selected ) < m_items.size() )
		{
			StaticCtrlSPtr item = m_items[m_selected];

			if ( item )
			{
				item->SetBackgroundMaterial( GetItemBackgroundMaterial() );
				item->SetForegroundMaterial( GetForegroundMaterial() );
				m_selectedItem.reset();
			}
		}

		m_selected = p_value;

		if ( m_selected >= 0 && uint32_t( m_selected ) < m_items.size() )
		{
			StaticCtrlSPtr item = m_items[m_selected];

			if ( item )
			{
				item->SetBackgroundMaterial( GetSelectedItemBackgroundMaterial() );
				item->SetForegroundMaterial( GetSelectedItemForegroundMaterial() );
				m_selectedItem = item;
			}
		}
	}

	void ListBoxCtrl::SetFont( Castor::String const & p_font )
	{
		m_fontName = p_font;

		for ( auto item : m_items )
		{
			item->SetFont( m_fontName );
		}
	}

	void ListBoxCtrl::DoUpdateItems()
	{
		Position position;

		for ( auto item : m_items )
		{
			item->SetPosition( position );
			item->SetSize( Size( GetSize().width(), DEFAULT_HEIGHT ) );
			position.y() += DEFAULT_HEIGHT;
		}

		BorderPanelOverlaySPtr background = GetBackground();

		if ( background )
		{
			background->SetPixelSize( Size( GetSize().width(), uint32_t( m_items.size() * DEFAULT_HEIGHT ) ) );
		}
	}

	StaticCtrlSPtr ListBoxCtrl::DoCreateItemCtrl( String const & p_value )
	{
		StaticCtrlSPtr item = std::make_shared< StaticCtrl >( GetName() + cuT( "_Item" ), GetEngine(), this, p_value, Position(), Size( GetSize().width(), DEFAULT_HEIGHT ), uint32_t( StaticStyle::eVAlignCenter ) );
		item->SetCatchesMouseEvents( true );

		item->ConnectNC( MouseEventType::eEnter, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnItemMouseEnter( p_control, p_event );
		} );
		item->ConnectNC( MouseEventType::eLeave, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnItemMouseLeave( p_control, p_event );
		} );
		item->ConnectNC( MouseEventType::eReleased, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnItemMouseLButtonUp( p_control, p_event );
		} );
		item->ConnectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			OnItemKeyDown( p_control, p_event );
		} );

		if ( m_fontName.empty() )
		{
			item->SetFont( GetControlsManager()->GetDefaultFont()->GetName() );
		}
		else
		{
			item->SetFont( m_fontName );
		}

		m_items.push_back( item );
		return item;
	}

	void ListBoxCtrl::DoCreateItem( String const & p_value )
	{
		StaticCtrlSPtr item = DoCreateItemCtrl( p_value );
		GetControlsManager()->Create( item );
		item->SetBackgroundMaterial( GetItemBackgroundMaterial() );
		item->SetForegroundMaterial( GetForegroundMaterial() );
		item->SetVisible( DoIsVisible() );
	}

	void ListBoxCtrl::DoCreate()
	{
		MaterialSPtr material = GetSelectedItemBackgroundMaterial();

		if ( !material )
		{
			SetSelectedItemBackgroundMaterial( GetEngine().GetMaterialCache().Find( cuT( "DarkBlue" ) ) );
		}

		material = GetSelectedItemForegroundMaterial();

		if ( !material )
		{
			SetSelectedItemForegroundMaterial( GetEngine().GetMaterialCache().Find( cuT( "White" ) ) );
		}

		material = GetHighlightedItemBackgroundMaterial();

		if ( !material )
		{
			Colour colour = GetBackgroundMaterial()->GetTypedPass< MaterialType::eLegacy >( 0u )->GetDiffuse();
			colour.red() = std::min( 1.0f, float( colour.red() ) / 2.0f );
			colour.green() = std::min( 1.0f, float( colour.green() ) / 2.0f );
			colour.blue() = std::min( 1.0f, float( colour.blue() ) / 2.0f );
			colour.alpha() = 1.0f;
			SetHighlightedItemBackgroundMaterial( CreateMaterial( GetEngine(), GetBackgroundMaterial()->GetName() + cuT( "_Highlight" ), colour ) );
		}

		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		SetSize( Size( GetSize().width(), uint32_t( m_values.size() * DEFAULT_HEIGHT ) ) );

		EventHandler::Connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			OnKeyDown( p_event );
		} );

		for ( auto value : m_initialValues )
		{
			DoCreateItem( value );
		}

		m_initialValues.clear();
		DoUpdateItems();
		SetSelected( m_selected );
		GetControlsManager()->ConnectEvents( *this );
	}

	void ListBoxCtrl::DoDestroy()
	{
		REQUIRE( GetControlsManager() );
		auto & manager = *GetControlsManager();
		manager.DisconnectEvents( *this );

		for ( auto item : m_items )
		{
			manager.Destroy( item );
		}

		m_items.clear();
		m_selectedItem.reset();
	}

	void ListBoxCtrl::DoSetPosition( Position const & p_value )
	{
		DoUpdateItems();
	}

	void ListBoxCtrl::DoSetSize( Size const & p_value )
	{
		DoUpdateItems();
	}

	void ListBoxCtrl::DoSetBackgroundMaterial( MaterialSPtr p_material )
	{
		int i = 0;
		auto pass = p_material->GetTypedPass< MaterialType::eLegacy >( 0u );
		Colour colour = pass->GetDiffuse();
		SetItemBackgroundMaterial( p_material );
		colour.red() = std::min( 1.0f, colour.red() / 2.0f );
		colour.green() = std::min( 1.0f, colour.green() / 2.0f );
		colour.blue() = std::min( 1.0f, colour.blue() / 2.0f );
		colour.alpha() = 1.0f;
		SetHighlightedItemBackgroundMaterial( CreateMaterial( GetEngine(), GetBackgroundMaterial()->GetName() + cuT( "_Highlight" ), colour ) );

		colour.alpha() = 0.0;
		pass->SetDiffuse( colour );

		for ( auto item : m_items )
		{
			if ( i++ != m_selected )
			{
				item->SetBackgroundMaterial( p_material );
			}
		}
	}

	void ListBoxCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		int i = 0;

		for ( auto item : m_items )
		{
			if ( i++ != m_selected )
			{
				item->SetForegroundMaterial( p_material );
			}
		}
	}

	void ListBoxCtrl::DoSetVisible( bool p_visible )
	{
		for ( auto item : m_items )
		{
			item->SetVisible( p_visible );
		}
	}

	void ListBoxCtrl::OnItemMouseEnter( ControlSPtr p_control, MouseEvent const & p_event )
	{
		p_control->SetBackgroundMaterial( GetHighlightedItemBackgroundMaterial() );
	}

	void ListBoxCtrl::OnItemMouseLeave( ControlSPtr p_control, MouseEvent const & p_event )
	{
		if ( m_selectedItem.lock() == p_control )
		{
			p_control->SetBackgroundMaterial( GetSelectedItemBackgroundMaterial() );
		}
		else
		{
			p_control->SetBackgroundMaterial( GetItemBackgroundMaterial() );
		}
	}

	void ListBoxCtrl::OnItemMouseLButtonUp( ControlSPtr p_control, MouseEvent const & p_event )
	{
		if ( p_event.GetButton() == MouseButton::eLeft )
		{
			int index = -1;

			if ( m_selectedItem.lock() != p_control )
			{
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

				SetSelected( index );
				m_signals[size_t( ListBoxEvent::eSelected )]( m_selected );
			}
			else
			{
				index = m_selected;
			}
		}
	}

	void ListBoxCtrl::OnKeyDown( KeyboardEvent const & p_event )
	{
		if ( m_selected != -1 )
		{
			bool changed = false;
			int index = m_selected;

			if ( p_event.GetKey() == KeyboardKey::eUp )
			{
				index--;
				changed = true;
			}
			else if ( p_event.GetKey() == KeyboardKey::eDown )
			{
				index++;
				changed = true;
			}

			if ( changed )
			{
				index = std::max( 0, std::min( index, int( m_items.size() - 1 ) ) );
				SetSelected( index );
				m_signals[size_t( ListBoxEvent::eSelected )]( index );
			}
		}
	}

	void ListBoxCtrl::OnItemKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event )
	{
		OnKeyDown( p_event );
	}
}
