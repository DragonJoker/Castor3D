#include "CtrlEdit.hpp"

#include "ControlsManager.hpp"

#include <Engine.hpp>
#include <Overlay/Overlay.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Texture/TextureUnit.hpp>

#include <Graphics/Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	EditCtrl::EditCtrl( String const & p_name
		, Engine & p_engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: EditCtrl( p_name
			, p_engine
			, p_parent
			, p_id
			, String()
			, Position()
			, Size()
			, 0
			, true )
	{
	}

	EditCtrl::EditCtrl( String const & p_name
		, Engine & p_engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, String const & p_caption
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control( ControlType::eEdit
			, p_name
			, p_engine
			, p_parent
			, p_id
			, p_position
			, p_size
			, p_style
			, p_visible )
		, m_caption( p_caption )
		, m_caretIt( p_caption.end() )
		, m_active( false )
		, m_multiLine( false )
	{
		m_caretIt = m_caption.end();
		m_cursor = MouseCursor::eText;
		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );

		EventHandler::Connect( MouseEventType::ePushed, [this]( MouseEvent const & p_event )
		{
			OnMouseLButtonDown( p_event );
		} );
		EventHandler::Connect( MouseEventType::eReleased, [this]( MouseEvent const & p_event )
		{
			OnMouseLButtonUp( p_event );
		} );
		EventHandler::Connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			OnKeyDown( p_event );
		} );
		EventHandler::Connect( KeyboardEventType::eReleased, [this]( KeyboardEvent const & p_event )
		{
			OnKeyUp( p_event );
		} );
		EventHandler::Connect( KeyboardEventType::eChar, [this]( KeyboardEvent const & p_event )
		{
			OnChar( p_event );
		} );
		EventHandler::Connect( HandlerEventType::eActivate, [this]( HandlerEvent const & p_event )
		{
			OnActivate( p_event );
		} );
		EventHandler::Connect( HandlerEventType::eDeactivate, [this]( HandlerEvent const & p_event )
		{
			OnDeactivate( p_event );
		} );

		TextOverlaySPtr text = GetEngine().GetOverlayCache().Add( cuT( "T_CtrlEdit_" ) + string::to_string( GetId() )
			, OverlayType::eText
			, nullptr
			, GetBackground()->GetOverlay().shared_from_this() )->GetTextOverlay();
		text->SetPixelSize( GetSize() );
		text->SetVAlign( VAlign::eBottom );
		text->SetVisible( DoIsVisible() );
		m_text = text;

		DoUpdateStyle();
	}

	EditCtrl::~EditCtrl()
	{
	}

	void EditCtrl::SetFont( Castor::String const & p_font )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetFont( p_font );
		}
	}

	void EditCtrl::DoCreate()
	{
		TextOverlaySPtr text = m_text.lock();
		text->SetMaterial( GetForegroundMaterial() );

		if ( !text->GetFontTexture() || !text->GetFontTexture()->GetFont() )
		{
			text->SetFont( GetControlsManager()->GetDefaultFont()->GetName() );
		}

		DoUpdateCaption();
		GetControlsManager()->ConnectEvents( *this );
	}

	void EditCtrl::DoDestroy()
	{
		GetControlsManager()->DisconnectEvents( *this );
	}

	void EditCtrl::DoSetPosition( Position const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetPixelPosition( Position() );
			text.reset();
		}
	}

	void EditCtrl::DoSetSize( Size const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetPixelSize( p_value );
			text.reset();
		}
	}

	void EditCtrl::DoSetBackgroundMaterial( MaterialSPtr p_material )
	{
	}

	void EditCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetMaterial( p_material );
			text.reset();
		}
	}

	void EditCtrl::DoSetCaption( String const & p_value )
	{
		m_caption = p_value;
		m_caretIt = m_caption.end();
		DoUpdateCaption();
	}

	void EditCtrl::DoSetVisible( bool p_visible )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->SetVisible( p_visible );
			text.reset();
		}
	}

	void EditCtrl::DoUpdateStyle()
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			if ( IsMultiLine() )
			{
				text->SetTextWrappingMode( TextWrappingMode::eBreak );
			}
		}
	}

	String EditCtrl::DoGetCaptionWithCaret()const
	{
		String caption( m_caption.begin(), m_caretIt.internal() );
		caption += cuT( '|' );

		if ( m_caretIt != m_caption.end() )
		{
			caption += String( m_caretIt.internal(), m_caption.end() );
		}

		return caption;
	}

	void EditCtrl::OnActivate( HandlerEvent const & p_event )
	{
		m_active = true;
		DoUpdateCaption();
	}

	void EditCtrl::OnDeactivate( HandlerEvent const & p_event )
	{
		m_active = false;
		DoUpdateCaption();
	}

	void EditCtrl::OnMouseLButtonDown( MouseEvent const & p_event )
	{
	}

	void EditCtrl::OnMouseLButtonUp( MouseEvent const & p_event )
	{
	}

	void EditCtrl::OnChar( KeyboardEvent const & p_event )
	{
		KeyboardKey code = p_event.GetKey();

		if ( code >= KeyboardKey( 0x20 )
			 && code <= KeyboardKey( 0xFF )
			 && code != KeyboardKey::eDelete )
		{
			DoAddCharAtCaret( p_event.GetChar() );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
		else if ( code == KeyboardKey::eReturn && IsMultiLine() )
		{
			DoAddCharAtCaret( cuT( "\n" ) );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
	}

	void EditCtrl::OnKeyDown( KeyboardEvent const & p_event )
	{
		if ( !p_event.IsCtrlDown() && !p_event.IsAltDown() )
		{
			KeyboardKey code = p_event.GetKey();

			if ( code == KeyboardKey::eBackspace )
			{
				DoDeleteCharBeforeCaret();
				m_signals[size_t( EditEvent::eUpdated )]( m_caption );
			}
			else if ( code == KeyboardKey::eDelete )
			{
				DoDeleteCharAtCaret();
				m_signals[size_t( EditEvent::eUpdated )]( m_caption );
			}
			else if ( code == KeyboardKey::eLeft && m_caretIt != m_caption.begin() )
			{
				m_caretIt--;
				DoUpdateCaption();
			}
			else if ( code == KeyboardKey::eRight && m_caretIt != m_caption.end() )
			{
				m_caretIt++;
				DoUpdateCaption();
			}
			else if ( code == KeyboardKey::eHome && m_caretIt != m_caption.begin() )
			{
				m_caretIt = m_caption.begin();
				DoUpdateCaption();
			}
			else if ( code == KeyboardKey::eEnd && m_caretIt != m_caption.end() )
			{
				m_caretIt = m_caption.end();
				DoUpdateCaption();
			}
		}
	}

	void EditCtrl::OnKeyUp( KeyboardEvent const & p_event )
	{
	}

	void EditCtrl::DoAddCharAtCaret( String const & p_char )
	{
		size_t diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
		m_caption = String( m_caption.cbegin(), m_caretIt.internal() ) + p_char + String( m_caretIt.internal(), m_caption.cend() );
		m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + diff + 1;
		DoUpdateCaption();
	}

	void EditCtrl::DoDeleteCharAtCaret()
	{
		if ( m_caretIt != m_caption.end() )
		{
			size_t diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
			String caption( m_caption.cbegin(), m_caretIt.internal() );
			string::utf8::const_iterator it = m_caretIt;

			if ( ++it != m_caption.end() )
			{
				caption += String( it.internal(), m_caption.cend() );
			}

			m_caption = caption;
			m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + diff;
			DoUpdateCaption();
		}
	}

	void EditCtrl::DoDeleteCharBeforeCaret()
	{
		if ( m_caretIt != m_caption.begin() )
		{
			m_caretIt--;
			size_t diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
			String caption( m_caption.cbegin(), m_caretIt.internal() );
			string::utf8::const_iterator it = m_caretIt;

			if ( ++it != m_caption.end() )
			{
				caption += String( it.internal(), m_caption.cend() );
			}

			m_caption = caption;
			m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + diff;
			DoUpdateCaption();
		}
	}

	void EditCtrl::DoUpdateCaption()
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			if ( m_active )
			{
				text->SetCaption( DoGetCaptionWithCaret() );
			}
			else
			{
				text->SetCaption( m_caption );
			}
		}
	}
}
