#include "CtrlListBox.hpp"

#include "ControlsManager.hpp"
#include "CtrlStatic.hpp"

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Texture/TextureUnit.hpp>

#include <Event/Frame/FunctorEvent.hpp>
#include <Material/Pass.hpp>
#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	ListBoxCtrl::ListBoxCtrl( Engine * p_engine, ControlRPtr p_parent, uint32_t p_id )
		: ListBoxCtrl( p_engine, p_parent, p_id, StringArray(), -1, Position(), Size(), 0, true )
	{
	}

	ListBoxCtrl::ListBoxCtrl( Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, StringArray const & p_values, int p_selected, Position const & p_position, Size const & p_size, uint32_t p_style, bool p_visible )
		: Control( eCONTROL_TYPE_LIST, p_engine, p_parent, p_id, p_position, p_size, p_style, p_visible )
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

		for ( auto l_item : m_items )
		{
			if ( i++ == m_selected )
			{
				l_item->SetBackgroundMaterial( p_value );
			}
		}
	}

	void ListBoxCtrl::SetSelectedItemForegroundMaterial( MaterialSPtr p_value )
	{
		m_selectedItemForegroundMaterial = p_value;
		int i = 0;

		for ( auto l_item : m_items )
		{
			if ( i++ == m_selected )
			{
				l_item->SetForegroundMaterial( p_value );
			}
		}
	}

	void ListBoxCtrl::AppendItem( String const & p_value )
	{
		m_values.push_back( p_value );

		if ( GetControlsManager() )
		{
			StaticCtrlSPtr l_item = DoCreateItemCtrl( p_value );
			GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this, l_item]()
			{
				GetControlsManager()->Create( l_item );
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

				auto l_it = m_items.begin() + p_value;

				if ( GetControlsManager() )
				{
					ControlSPtr l_control = *l_it;
					GetEngine()->PostEvent( MakeFunctorEvent( eEVENT_TYPE_PRE_RENDER, [this, l_control]()
					{
						GetControlsManager()->Destroy( l_control );
					} ) );
				}

				m_items.erase( l_it );
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
				StaticCtrlSPtr l_item = m_items[p_index];

				if ( l_item )
				{
					l_item->SetCaption( p_text );
				}
			}
		}
	}

	String ListBoxCtrl::GetItemText( int p_index )
	{
		String l_return;

		if ( uint32_t( p_index ) < m_values.size() && p_index >= 0 )
		{
			l_return = m_values[p_index];
		}

		return l_return;
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
			StaticCtrlSPtr l_item = m_items[m_selected];

			if ( l_item )
			{
				l_item->SetBackgroundMaterial( GetItemBackgroundMaterial() );
				l_item->SetForegroundMaterial( GetForegroundMaterial() );
				m_selectedItem.reset();
			}
		}

		m_selected = p_value;

		if ( m_selected >= 0 && uint32_t( m_selected ) < m_items.size() )
		{
			StaticCtrlSPtr l_item = m_items[m_selected];

			if ( l_item )
			{
				l_item->SetBackgroundMaterial( GetSelectedItemBackgroundMaterial() );
				l_item->SetForegroundMaterial( GetSelectedItemForegroundMaterial() );
				m_selectedItem = l_item;
			}
		}
	}

	void ListBoxCtrl::SetFont( Castor::String const & p_font )
	{
		m_fontName = p_font;

		for ( auto l_item : m_items )
		{
			l_item->SetFont( m_fontName );
		}
	}

	void ListBoxCtrl::DoUpdateItems()
	{
		Position l_position;

		for ( auto l_item : m_items )
		{
			l_item->SetPosition( l_position );
			l_item->SetSize( Size( GetSize().width(), DEFAULT_HEIGHT ) );
			l_position.y() += DEFAULT_HEIGHT;
		}

		BorderPanelOverlaySPtr l_background = GetBackground();

		if ( l_background )
		{
			l_background->SetPixelSize( Size( GetSize().width(), uint32_t( m_items.size() * DEFAULT_HEIGHT ) ) );
		}
	}

	StaticCtrlSPtr ListBoxCtrl::DoCreateItemCtrl( String const & p_value )
	{
		StaticCtrlSPtr l_item = std::make_shared< StaticCtrl >( GetEngine(), this, p_value, Position(), Size( GetSize().width(), DEFAULT_HEIGHT ), eSTATIC_STYLE_VALIGN_CENTER );
		l_item->SetCatchesMouseEvents( true );

		l_item->ConnectNC( eMOUSE_EVENT_ENTER, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnItemMouseEnter( p_control, p_event );
		} );
		l_item->ConnectNC( eMOUSE_EVENT_LEAVE, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnItemMouseLeave( p_control, p_event );
		} );
		l_item->ConnectNC( eMOUSE_EVENT_BUTTON_RELEASED, [this]( ControlSPtr p_control, MouseEvent const & p_event )
		{
			OnItemMouseLButtonUp( p_control, p_event );
		} );
		l_item->ConnectNC( eKEYBOARD_EVENT_KEY_PUSHED, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			OnItemKeyDown( p_control, p_event );
		} );

		if ( m_fontName.empty() )
		{
			l_item->SetFont( GetControlsManager()->GetDefaultFont()->GetName() );
		}
		else
		{
			l_item->SetFont( m_fontName );
		}

		m_items.push_back( l_item );
		return l_item;
	}

	void ListBoxCtrl::DoCreateItem( String const & p_value )
	{
		StaticCtrlSPtr l_item = DoCreateItemCtrl( p_value );
		GetControlsManager()->Create( l_item );
		l_item->SetBackgroundMaterial( GetItemBackgroundMaterial() );
		l_item->SetForegroundMaterial( GetForegroundMaterial() );
		l_item->SetVisible( DoIsVisible() );
	}

	void ListBoxCtrl::DoCreate()
	{
		MaterialSPtr l_material = GetSelectedItemBackgroundMaterial();

		if ( !l_material )
		{
			SetSelectedItemBackgroundMaterial( GetEngine()->GetMaterialCache().Find( cuT( "DarkBlue" ) ) );
		}

		l_material = GetSelectedItemForegroundMaterial();

		if ( !l_material )
		{
			SetSelectedItemForegroundMaterial( GetEngine()->GetMaterialCache().Find( cuT( "White" ) ) );
		}

		l_material = GetHighlightedItemBackgroundMaterial();

		if ( !l_material )
		{
			Colour l_colour = GetBackgroundMaterial()->GetPass( 0 )->GetAmbient();
			l_colour.red() = std::min( 1.0f, float( l_colour.red() ) / 2.0f );
			l_colour.green() = std::min( 1.0f, float( l_colour.green() ) / 2.0f );
			l_colour.blue() = std::min( 1.0f, float( l_colour.blue() ) / 2.0f );
			l_colour.alpha() = 1.0f;
			SetHighlightedItemBackgroundMaterial( CreateMaterial( GetEngine(), GetBackgroundMaterial()->GetName() + cuT( "_Highlight" ), l_colour ) );
		}

		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );
		SetSize( Size( GetSize().width(), uint32_t( m_values.size() * DEFAULT_HEIGHT ) ) );

		EventHandler::Connect( eKEYBOARD_EVENT_KEY_PUSHED, [this]( KeyboardEvent const & p_event )
		{
			OnKeyDown( p_event );
		} );

		for ( auto l_value : m_initialValues )
		{
			DoCreateItem( l_value );
		}

		m_initialValues.clear();
		DoUpdateItems();
		SetSelected( m_selected );
	}

	void ListBoxCtrl::DoDestroy()
	{
		for ( auto l_item : m_items )
		{
			l_item->Destroy();
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
		Colour l_colour = p_material->GetPass( 0 )->GetAmbient();
		SetItemBackgroundMaterial( p_material );

		if ( GetEngine() )
		{
			Colour l_colour;
			l_colour.red() = std::min( 1.0f, l_colour.red() / 2.0f );
			l_colour.green() = std::min( 1.0f, l_colour.green() / 2.0f );
			l_colour.blue() = std::min( 1.0f, l_colour.blue() / 2.0f );
			l_colour.alpha() = 1.0f;
			SetHighlightedItemBackgroundMaterial( CreateMaterial( GetEngine(), GetBackgroundMaterial()->GetName() + cuT( "_Highlight" ), l_colour ) );
		}

		l_colour.alpha() = 0.0;
		p_material->GetPass( 0 )->SetAmbient( l_colour );

		for ( auto l_item : m_items )
		{
			if ( i++ != m_selected )
			{
				l_item->SetBackgroundMaterial( p_material );
			}
		}
	}

	void ListBoxCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		int i = 0;

		for ( auto l_item : m_items )
		{
			if ( i++ != m_selected )
			{
				l_item->SetForegroundMaterial( p_material );
			}
		}
	}

	void ListBoxCtrl::DoSetVisible( bool p_visible )
	{
		for ( auto l_item : m_items )
		{
			l_item->SetVisible( p_visible );
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
		if ( p_event.GetButton() == eMOUSE_BUTTON_LEFT )
		{
			int l_index = -1;

			if ( m_selectedItem.lock() != p_control )
			{
				auto l_it = m_items.begin();
				int i = 0;

				while ( l_index == -1 && l_it != m_items.end() )
				{
					if ( *l_it == p_control )
					{
						l_index = i;
					}

					++l_it;
					++i;
				}

				SetSelected( l_index );
				m_signals[eLISTBOX_EVENT_SELECTED]( m_selected );
			}
			else
			{
				l_index = m_selected;
			}
		}
	}

	void ListBoxCtrl::OnKeyDown( KeyboardEvent const & p_event )
	{
		if ( m_selected != -1 )
		{
			bool l_changed = false;
			int l_index = m_selected;

			if ( p_event.GetKey() == eKEY_UP )
			{
				l_index--;
				l_changed = true;
			}
			else if ( p_event.GetKey() == eKEY_DOWN )
			{
				l_index++;
				l_changed = true;
			}

			if ( l_changed )
			{
				l_index = std::max( 0, std::min( l_index, int( m_items.size() - 1 ) ) );
				SetSelected( l_index );
				m_signals[eLISTBOX_EVENT_SELECTED]( l_index );
			}
		}
	}

	void ListBoxCtrl::OnItemKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event )
	{
		OnKeyDown( p_event );
	}
}
