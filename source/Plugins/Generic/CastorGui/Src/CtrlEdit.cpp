#include "CtrlEdit.hpp"

#include "ControlsManager.hpp"

#include <OverlayManager.hpp>

#include <Overlay/BorderPanelOverlay.hpp>
#include <Overlay/PanelOverlay.hpp>
#include <Overlay/TextOverlay.hpp>
#include <Texture/TextureUnit.hpp>

#include <Font.hpp>

using namespace Castor;
using namespace Castor3D;

namespace CastorGui
{
	EditCtrl::EditCtrl( Engine * p_engine, ControlRPtr p_parent, uint32_t p_id )
		: EditCtrl( p_engine, p_parent, p_id, String(), Position(), Size(), 0, true )
	{
	}

	EditCtrl::EditCtrl( Engine * p_engine, ControlRPtr p_parent, uint32_t p_id, String const & p_caption, Position const & p_position, Size const & p_size, uint32_t p_style, bool p_visible )
		: Control( eCONTROL_TYPE_EDIT, p_engine, p_parent, p_id, p_position, p_size, p_style, p_visible )
		, m_caption( p_caption )
		, m_caretIt( p_caption.end() )
		, m_active( false )
		, m_multiLine( false )
	{
		m_caretIt = m_caption.end();
		m_cursor = eMOUSE_CURSOR_TEXT;
		SetBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );

		EventHandler::Connect( eMOUSE_EVENT_BUTTON_PUSHED, [this]( MouseEvent const & p_event )
		{
			OnMouseLButtonDown( p_event );
		} );
		EventHandler::Connect( eMOUSE_EVENT_BUTTON_RELEASED, [this]( MouseEvent const & p_event )
		{
			OnMouseLButtonUp( p_event );
		} );
		EventHandler::Connect( eKEYBOARD_EVENT_KEY_PUSHED, [this]( KeyboardEvent const & p_event )
		{
			OnKeyDown( p_event );
		} );
		EventHandler::Connect( eKEYBOARD_EVENT_KEY_RELEASED, [this]( KeyboardEvent const & p_event )
		{
			OnKeyUp( p_event );
		} );
		EventHandler::Connect( eKEYBOARD_EVENT_CHAR, [this]( KeyboardEvent const & p_event )
		{
			OnChar( p_event );
		} );
		EventHandler::Connect( eHANDLER_EVENT_ACTIVATE, [this]( HandlerEvent const & p_event )
		{
			OnActivate( p_event );
		} );
		EventHandler::Connect( eHANDLER_EVENT_DEACTIVATE, [this]( HandlerEvent const & p_event )
		{
			OnDeactivate( p_event );
		} );

		TextOverlaySPtr l_text = GetEngine()->GetOverlayManager().Create( cuT( "T_CtrlEdit_" ) + string::to_string( GetId() ), eOVERLAY_TYPE_TEXT, GetBackground()->GetOverlay().shared_from_this(), nullptr )->GetTextOverlay();
		l_text->SetPixelSize( GetSize() );
		l_text->SetVAlign( eVALIGN_BOTTOM );
		l_text->SetVisible( DoIsVisible() );
		m_text = l_text;

		DoUpdateStyle();
	}

	EditCtrl::~EditCtrl()
	{
	}

	void EditCtrl::SetCaption( String const & p_value )
	{
		m_caption = p_value;
		m_caretIt = m_caption.end();
		DoUpdateCaption();
	}

	void EditCtrl::SetFont( Castor::String const & p_font )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetFont( p_font );
		}
	}

	void EditCtrl::DoCreate()
	{
		TextOverlaySPtr l_text = m_text.lock();
		l_text->SetMaterial( GetForegroundMaterial() );

		if ( !l_text->GetFontTexture() || !l_text->GetFontTexture()->GetFont() )
		{
			l_text->SetFont( GetControlsManager()->GetDefaultFont()->GetName() );
		}

		DoUpdateCaption();
	}

	void EditCtrl::DoDestroy()
	{
	}

	void EditCtrl::DoSetPosition( Position const & p_value )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetPixelPosition( Position() );
			l_text.reset();
		}
	}

	void EditCtrl::DoSetSize( Size const & p_value )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetPixelSize( p_value );
			l_text.reset();
		}
	}

	void EditCtrl::DoSetBackgroundMaterial( MaterialSPtr p_material )
	{
	}

	void EditCtrl::DoSetForegroundMaterial( MaterialSPtr p_material )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetMaterial( p_material );
			l_text.reset();
		}
	}

	void EditCtrl::DoSetVisible( bool p_visible )
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			l_text->SetVisible( p_visible );
			l_text.reset();
		}
	}

	void EditCtrl::DoUpdateStyle()
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			if ( IsMultiLine() )
			{
				l_text->SetTextWrappingMode( eTEXT_WRAPPING_MODE_BREAK );
			}
		}
	}

	String EditCtrl::DoGetCaptionWithCaret()const
	{
		String l_caption( m_caption.begin(), m_caretIt.internal() );
		l_caption += cuT( '|' );

		if ( m_caretIt != m_caption.end() )
		{
			l_caption += String( m_caretIt.internal(), m_caption.end() );
		}

		return l_caption;
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
		eKEYBOARD_KEY l_code = p_event.GetKey();

		if ( l_code >= 0x20 && l_code <= 255 && l_code != eKEY_DELETE )
		{
			DoAddCharAtCaret( p_event.GetChar() );
			m_signals[eEDIT_EVENT_UPDATED]( m_caption );
		}
		else if ( l_code == eKEY_RETURN && IsMultiLine() )
		{
			DoAddCharAtCaret( cuT( "\n" ) );
			m_signals[eEDIT_EVENT_UPDATED]( m_caption );
		}
	}

	void EditCtrl::OnKeyDown( KeyboardEvent const & p_event )
	{
		if ( !p_event.IsCtrlDown() && !p_event.IsAltDown() )
		{
			eKEYBOARD_KEY l_code = p_event.GetKey();

			if ( l_code == eKEY_BACKSPACE )
			{
				DoDeleteCharBeforeCaret();
				m_signals[eEDIT_EVENT_UPDATED]( m_caption );
			}
			else if ( l_code == eKEY_DELETE )
			{
				DoDeleteCharAtCaret();
				m_signals[eEDIT_EVENT_UPDATED]( m_caption );
			}
			else if ( l_code == eKEY_LEFT && m_caretIt != m_caption.begin() )
			{
				m_caretIt--;
				DoUpdateCaption();
			}
			else if ( l_code == eKEY_RIGHT && m_caretIt != m_caption.end() )
			{
				m_caretIt++;
				DoUpdateCaption();
			}
			else if ( l_code == eKEY_HOME && m_caretIt != m_caption.begin() )
			{
				m_caretIt = m_caption.begin();
				DoUpdateCaption();
			}
			else if ( l_code == eKEY_END && m_caretIt != m_caption.end() )
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
		size_t l_diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
		m_caption = String( m_caption.cbegin(), m_caretIt.internal() ) + p_char + String( m_caretIt.internal(), m_caption.cend() );
		m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + l_diff + 1;
		DoUpdateCaption();
	}

	void EditCtrl::DoDeleteCharAtCaret()
	{
		if ( m_caretIt != m_caption.end() )
		{
			size_t l_diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
			String l_caption( m_caption.cbegin(), m_caretIt.internal() );
			string::utf8::const_iterator l_it = m_caretIt;

			if ( ++l_it != m_caption.end() )
			{
				l_caption += String( l_it.internal(), m_caption.cend() );
			}

			m_caption = l_caption;
			m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + l_diff;
			DoUpdateCaption();
		}
	}

	void EditCtrl::DoDeleteCharBeforeCaret()
	{
		if ( m_caretIt != m_caption.begin() )
		{
			m_caretIt--;
			size_t l_diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
			String l_caption( m_caption.cbegin(), m_caretIt.internal() );
			string::utf8::const_iterator l_it = m_caretIt;

			if ( ++l_it != m_caption.end() )
			{
				l_caption += String( l_it.internal(), m_caption.cend() );
			}

			m_caption = l_caption;
			m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + l_diff;
			DoUpdateCaption();
		}
	}

	void EditCtrl::DoUpdateCaption()
	{
		TextOverlaySPtr l_text = m_text.lock();

		if ( l_text )
		{
			if ( m_active )
			{
				l_text->SetCaption( DoGetCaptionWithCaret() );
			}
			else
			{
				l_text->SetCaption( m_caption );
			}
		}
	}
}
