#include "CastorGui/CtrlEdit.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Overlay/Overlay.hpp>

#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>
#include <Castor3D/Texture/TextureUnit.hpp>

#include <CastorUtils/Graphics/Font.hpp>

using namespace castor;
using namespace castor3d;

namespace CastorGui
{
	EditCtrl::EditCtrl( String const & p_name
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id )
		: EditCtrl( p_name
			, engine
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
		, Engine & engine
		, ControlRPtr p_parent
		, uint32_t p_id
		, String const & p_caption
		, Position const & p_position
		, Size const & p_size
		, uint32_t p_style
		, bool p_visible )
		: Control( ControlType::eEdit
			, p_name
			, engine
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
		setBackgroundBorders( Rectangle( 1, 1, 1, 1 ) );

		EventHandler::connect( MouseEventType::ePushed, [this]( MouseEvent const & p_event )
		{
			onMouseLButtonDown( p_event );
		} );
		EventHandler::connect( MouseEventType::eReleased, [this]( MouseEvent const & p_event )
		{
			onMouseLButtonUp( p_event );
		} );
		EventHandler::connect( KeyboardEventType::ePushed, [this]( KeyboardEvent const & p_event )
		{
			onKeyDown( p_event );
		} );
		EventHandler::connect( KeyboardEventType::eReleased, [this]( KeyboardEvent const & p_event )
		{
			onKeyUp( p_event );
		} );
		EventHandler::connect( KeyboardEventType::eChar, [this]( KeyboardEvent const & p_event )
		{
			onChar( p_event );
		} );
		EventHandler::connect( HandlerEventType::eActivate, [this]( HandlerEvent const & p_event )
		{
			onActivate( p_event );
		} );
		EventHandler::connect( HandlerEventType::eDeactivate, [this]( HandlerEvent const & p_event )
		{
			onDeactivate( p_event );
		} );

		TextOverlaySPtr text = getEngine().getOverlayCache().add( cuT( "T_CtrlEdit_" ) + string::toString( getId() )
			, OverlayType::eText
			, nullptr
			, getBackground()->getOverlay().shared_from_this() )->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setVAlign( VAlign::eBottom );
		text->setVisible( p_visible );
		m_text = text;

		doUpdateStyle();
	}

	EditCtrl::~EditCtrl()
	{
	}

	void EditCtrl::setFont( castor::String const & p_font )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setFont( p_font );
		}
	}

	void EditCtrl::doCreate()
	{
		TextOverlaySPtr text = m_text.lock();
		text->setMaterial( getForegroundMaterial() );

		if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
		{
			text->setFont( getControlsManager()->getDefaultFont()->getName() );
		}

		doUpdateCaption();
		getControlsManager()->connectEvents( *this );
	}

	void EditCtrl::doDestroy()
	{
		getControlsManager()->disconnectEvents( *this );
	}

	void EditCtrl::doSetPosition( Position const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelPosition( Position() );
			text.reset();
		}
	}

	void EditCtrl::doSetSize( Size const & p_value )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setPixelSize( p_value );
			text.reset();
		}
	}

	void EditCtrl::doSetBackgroundMaterial( MaterialSPtr p_material )
	{
	}

	void EditCtrl::doSetForegroundMaterial( MaterialSPtr p_material )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setMaterial( p_material );
			text.reset();
		}
	}

	void EditCtrl::doSetCaption( String const & p_value )
	{
		m_caption = p_value;
		m_caretIt = m_caption.end();
		doUpdateCaption();
	}

	void EditCtrl::doSetVisible( bool p_visible )
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			text->setVisible( p_visible );
			text.reset();
		}
	}

	void EditCtrl::doUpdateStyle()
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			if ( isMultiLine() )
			{
				text->setTextWrappingMode( TextWrappingMode::eBreak );
			}
		}
	}

	String EditCtrl::doGetCaptionWithCaret()const
	{
		String caption( m_caption.begin(), m_caretIt.internal() );
		caption += cuT( '|' );

		if ( m_caretIt != m_caption.end() )
		{
			caption += String( m_caretIt.internal(), m_caption.end() );
		}

		return caption;
	}

	void EditCtrl::onActivate( HandlerEvent const & p_event )
	{
		m_active = true;
		doUpdateCaption();
	}

	void EditCtrl::onDeactivate( HandlerEvent const & p_event )
	{
		m_active = false;
		doUpdateCaption();
	}

	void EditCtrl::onMouseLButtonDown( MouseEvent const & p_event )
	{
	}

	void EditCtrl::onMouseLButtonUp( MouseEvent const & p_event )
	{
	}

	void EditCtrl::onChar( KeyboardEvent const & p_event )
	{
		KeyboardKey code = p_event.getKey();

		if ( code >= KeyboardKey( 0x20 )
			 && code <= KeyboardKey( 0xFF )
			 && code != KeyboardKey::eDelete )
		{
			doAddCharAtCaret( p_event.getChar() );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
		else if ( code == KeyboardKey::eReturn && isMultiLine() )
		{
			doAddCharAtCaret( cuT( "\n" ) );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
	}

	void EditCtrl::onKeyDown( KeyboardEvent const & p_event )
	{
		if ( !p_event.isCtrlDown() && !p_event.isAltDown() )
		{
			KeyboardKey code = p_event.getKey();

			if ( code == KeyboardKey::eBackspace )
			{
				doDeleteCharBeforeCaret();
				m_signals[size_t( EditEvent::eUpdated )]( m_caption );
			}
			else if ( code == KeyboardKey::eDelete )
			{
				doDeleteCharAtCaret();
				m_signals[size_t( EditEvent::eUpdated )]( m_caption );
			}
			else if ( code == KeyboardKey::eLeft && m_caretIt != m_caption.begin() )
			{
				--m_caretIt;
				doUpdateCaption();
			}
			else if ( code == KeyboardKey::eRight && m_caretIt != m_caption.end() )
			{
				++m_caretIt;
				doUpdateCaption();
			}
			else if ( code == KeyboardKey::eHome && m_caretIt != m_caption.begin() )
			{
				m_caretIt = m_caption.begin();
				doUpdateCaption();
			}
			else if ( code == KeyboardKey::eEnd && m_caretIt != m_caption.end() )
			{
				m_caretIt = m_caption.end();
				doUpdateCaption();
			}
		}
	}

	void EditCtrl::onKeyUp( KeyboardEvent const & p_event )
	{
	}

	void EditCtrl::doAddCharAtCaret( String const & p_char )
	{
		size_t diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
		m_caption = String( m_caption.cbegin(), m_caretIt.internal() ) + p_char + String( m_caretIt.internal(), m_caption.cend() );
		m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + diff + 1;
		doUpdateCaption();
	}

	void EditCtrl::doDeleteCharAtCaret()
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
			doUpdateCaption();
		}
	}

	void EditCtrl::doDeleteCharBeforeCaret()
	{
		if ( m_caretIt != m_caption.begin() )
		{
			--m_caretIt;
			size_t diff = std::distance( string::utf8::const_iterator( m_caption.begin() ), m_caretIt );
			String caption( m_caption.cbegin(), m_caretIt.internal() );
			string::utf8::const_iterator it = m_caretIt;

			if ( ++it != m_caption.end() )
			{
				caption += String( it.internal(), m_caption.cend() );
			}

			m_caption = caption;
			m_caretIt = string::utf8::const_iterator( m_caption.begin() ) + diff;
			doUpdateCaption();
		}
	}

	void EditCtrl::doUpdateCaption()
	{
		TextOverlaySPtr text = m_text.lock();

		if ( text )
		{
			if ( m_active )
			{
				text->setCaption( doGetCaptionWithCaret() );
			}
			else
			{
				text->setCaption( m_caption );
			}
		}
	}
}
