#include "CtrlComboBox.hpp"

#include "ControlsManager.hpp"
#include "CtrlButton.hpp"
#include "CtrlListBox.hpp"

#include <Engine.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>

#include <Graphics/Font.hpp>

using namespace Castor;
using namespace Castor3D;

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
			, GetId() << 12
			, cuT( "+" )
			, Position( p_size.width() - p_size.height(), 0 )
			, Size( p_size.height(), p_size.height() ) );
		m_expand->SetVisible( DoIsVisible() );
		m_expandClickedConnection = m_expand->Connect( ButtonEvent::eClicked, std::bind( &ComboBoxCtrl::DoSwitchExpand, this ) );

		m_choices = std::make_shared< ListBoxCtrl >( p_name + cuT( "_Choices" )
			, engine
			, this
			, ( GetId() << 12 ) + 1
			, m_values
			, m_selected
			, Position( 0, p_size.height() )
			, Size( p_size.width() - p_size.height(), -1 )
			, 0
			, false );
		m_choicesSelectedConnection = m_choices->Connect( ListBoxEvent::eSelected, std::bind( &ComboBoxCtrl::OnSelected, this, std::placeholders::_1 ) );

		TextOverlaySPtr text = GetEngine().GetOverlayCache().Add( cuT( "T_CtrlCombo_" ) + string::to_string( GetId() )
			, OverlayType::eText
			, nullptr
			, GetBackground()->GetOverlay().shared_from_this() )->GetTextOverlay();
		text->SetPixelSize( Size( GetSize().width() - GetSize().height(), GetSize().height() ) );
		text->SetVAlign( VAlign::eCenter );
		m_text = text;
	}

	ComboBoxCtrl::~ComboBoxCtrl()
	{
	}

	void ComboBoxCtrl::SetSelectedItemBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->SetSelectedItemBackgroundMaterial( p_material );
	}

	void ComboBoxCtrl::SetSelectedItemForegroundMaterial( MaterialSPtr p_material )
	{
		m_choices->SetSelectedItemForegroundMaterial( p_material );
	}

	void ComboBoxCtrl::SetHighlightedItemBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->SetHighlightedItemBackgroundMaterial( p_material );
	}

	void ComboBoxCtrl::SetItemBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->SetItemBackgroundMaterial( p_material );
	}

	MaterialSPtr ComboBoxCtrl::GetSelectedItemBackgroundMaterial()const
	{
		return m_choices->GetSelectedItemBackgroundMaterial();
	}

	MaterialSPtr ComboBoxCtrl::GetSelectedItemForegroundMaterial()const
	{
		return m_choices->GetSelectedItemForegroundMaterial();
	}

	MaterialSPtr ComboBoxCtrl::GetHighlightedItemBackgroundMaterial()const
	{
		return m_choices->GetHighlightedItemBackgroundMaterial();
	}

	MaterialSPtr ComboBoxCtrl::GetItemBackgroundMaterial()const
	{
		return m_choices->GetItemBackgroundMaterial();
	}

	void ComboBoxCtrl::AppendItem( String  const & p_value )
	{
		m_choices->AppendItem( p_value );
	}

	void ComboBoxCtrl::RemoveItem( int p_value )
	{
		m_choices->RemoveItem( p_value );
	}

	void ComboBoxCtrl::SetItemText( int p_index, String const & p_text )
	{
		return m_choices->SetItemText( p_index, p_text );
	}

	void ComboBoxCtrl::Clear()
	{
		return m_choices->Clear();
	}

	void ComboBoxCtrl::SetSelected( int p_value )
	{
		return m_choices->SetSelected( p_value );
	}

	StringArray const & ComboBoxCtrl::GetItems()const
	{
		return m_choices->GetItems();
	}

	uint32_t ComboBoxCtrl::GetItemCount()const
	{
		return m_choices->GetItemCount();
	}

	int ComboBoxCtrl::GetSelected()const
	{
		return m_choices->GetSelected();
	}

	void ComboBoxCtrl::SetFont( Castor::String const & p_font )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetFont( p_font );
		}

		m_choices->SetFont( p_font );
	}

	void ComboBoxCtrl::DoCreate()
	{
		REQUIRE( GetControlsManager() );
		auto & manager = *GetControlsManager();
		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );

		m_expand->SetForegroundMaterial( GetForegroundMaterial() );
		m_expand->SetPosition( Position( GetSize().width() - GetSize().height(), 0 ) );
		m_expand->SetSize( Size( GetSize().height(), GetSize().height() ) );

		m_choices->SetBackgroundMaterial( GetBackgroundMaterial() );
		m_choices->SetForegroundMaterial( GetForegroundMaterial() );
		m_choices->SetPosition( Position( 0, GetSize().height() ) );
		m_choices->SetSize( Size( GetSize().width() - GetSize().height(), -1 ) );

		EventHandler::Connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			OnKeyDown( p_event );
		} );
		NonClientEventHandler::ConnectNC( KeyboardEventType::ePushed, [this]( ControlSPtr p_control, KeyboardEvent const & p_event )
		{
			OnNcKeyDown( p_control, p_event );
		} );

		TextOverlaySPtr text = m_text.lock();
		text->SetMaterial( GetForegroundMaterial() );
		text->SetPixelSize( Size( GetSize().width() - GetSize().height(), GetSize().height() ) );

		if ( !text->GetFontTexture() || !text->GetFontTexture()->GetFont() )
		{
			text->SetFont(manager.GetDefaultFont()->GetName() );
		}

		int sel = GetSelected();

		if ( sel >= 0 && uint32_t( sel ) < GetItemCount() )
		{
			text->SetCaption( GetItems()[sel] );
		}

		manager.Create( m_expand );
		manager.Create( m_choices );
		manager.ConnectEvents( *this );
	}

	void ComboBoxCtrl::DoDestroy()
	{
		REQUIRE( GetControlsManager() );
		auto & manager = *GetControlsManager();
		GetControlsManager()->DisconnectEvents( *this );

		if ( m_expand )
		{
			manager.Destroy( m_expand );
		}

		if ( m_choices )
		{
			manager.Destroy( m_choices );
		}
	}

	void ComboBoxCtrl::DoSetPosition( Position const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			//l_text->SetPixelPosition( p_value );
			text.reset();
		}

		m_expand->SetPosition( Position( GetSize().width() - GetSize().height(), 0 ) );
		m_choices->SetPosition( Position( 0, GetSize().height() ) );
	}

	void ComboBoxCtrl::DoSetSize( Size const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetPixelSize( p_value );
			text.reset();
		}

		m_expand->SetSize( Size( p_value.height(), p_value.height() ) );
		m_choices->SetSize( Size( p_value.width() - p_value.height(), -1 ) );
		m_expand->SetPosition( Position( p_value.width() - p_value.height(), 0 ) );
		m_choices->SetPosition( Position( 0, p_value.height() ) );
	}

	void ComboBoxCtrl::DoSetBackgroundMaterial( MaterialSPtr p_material )
	{
		m_choices->SetBackgroundMaterial( GetBackgroundMaterial() );
	}

	void ComboBoxCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		m_expand->SetForegroundMaterial( GetForegroundMaterial() );
		m_choices->SetForegroundMaterial( GetForegroundMaterial() );
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetMaterial( p_material );
			text.reset();
		}
	}

	bool ComboBoxCtrl::DoCatchesMouseEvents()const
	{
		return false;
	}

	void ComboBoxCtrl::OnKeyDown( KeyboardEvent const & p_event )
	{
		if ( GetSelected() != -1 )
		{
			bool changed = false;
			int index = GetSelected();

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
				index = std::max( 0, std::min( index, int( GetItemCount() - 1 ) ) );
				SetSelected( index );
				OnSelected( index );
			}
		}
	}

	void ComboBoxCtrl::OnNcKeyDown( ControlSPtr p_control, KeyboardEvent const & p_event )
	{
		OnKeyDown( p_event );
	}

	void ComboBoxCtrl::DoSetVisible( bool p_visible )
	{
		m_expand->SetVisible( p_visible );
		m_choices->Hide();
	}

	void ComboBoxCtrl::DoSwitchExpand()
	{
		m_choices->SetVisible( !m_choices->IsVisible() );
	}

	void ComboBoxCtrl::OnSelected( int p_selected )
	{
		if ( p_selected >= 0 )
		{
			DoSwitchExpand();
			TextOverlaySPtr text = m_text.lock();

			if ( text )
			{
				text->SetCaption( m_choices->GetItemText( p_selected ) );
			}
		}

		m_signals[size_t( ComboBoxEvent::eSelected )]( p_selected );
	}
}
