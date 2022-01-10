#include "CastorGui/Controls/CtrlEdit.hpp"

#include "CastorGui/ControlsManager.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/OverlayCache.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Overlay/Overlay.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Overlay/PanelOverlay.hpp>
#include <Castor3D/Overlay/TextOverlay.hpp>

#include <CastorUtils/Graphics/Font.hpp>

namespace CastorGui
{
	EditCtrl::EditCtrl( castor::String const & name
		, EditStyleRPtr style
		, ControlRPtr parent
		, uint32_t id )
		: EditCtrl{ name
			, style
			, parent
			, id
			, castor::String{}
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	EditCtrl::EditCtrl( castor::String const & name
		, EditStyleRPtr style
		, ControlRPtr parent
		, uint32_t id
		, castor::String const & caption
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
		, m_caption{ caption }
		, m_caretIt{ m_caption.end() }
		, m_active{ false }
	{
		setBackgroundBorders( castor::Rectangle{ 1, 1, 1, 1 } );
		EventHandler::connect( castor3d::MouseEventType::ePushed
			, [this]( castor3d::MouseEvent const & event )
			{
				onMouseLButtonDown( event );
			} );
		EventHandler::connect( castor3d::MouseEventType::eReleased
			, [this]( castor3d::MouseEvent const & event )
			{
				onMouseLButtonUp( event );
			} );
		EventHandler::connect( castor3d::KeyboardEventType::ePushed
			, [this]( castor3d::KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );
		EventHandler::connect( castor3d::KeyboardEventType::eReleased
			, [this]( castor3d::KeyboardEvent const & event )
			{
				onKeyUp( event );
			} );
		EventHandler::connect( castor3d::KeyboardEventType::eChar
			, [this]( castor3d::KeyboardEvent const & event )
			{
				onChar( event );
			} );
		EventHandler::connect( castor3d::HandlerEventType::eActivate
			, [this]( castor3d::HandlerEvent const & event )
			{
				onActivate( event );
			} );
		EventHandler::connect( castor3d::HandlerEventType::eDeactivate
			, [this]( castor3d::HandlerEvent const & event )
			{
				onDeactivate( event );
			} );

		auto text = getEngine().getOverlayCache().add( cuT( "T_CtrlEdit_" ) + castor::string::toString( getId() )
			, getEngine()
			, castor3d::OverlayType::eText
			, nullptr
			, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setVAlign( castor3d::VAlign::eCenter );
		text->setVisible( visible );
		m_text = text;

		doUpdateStyle();
		doUpdateFlags();
	}

	void EditCtrl::doUpdateStyle()
	{
		auto & style = getStyle();

		if ( auto text = m_text.lock() )
		{
			text->setFont( style.getFontName() );
			text->setMaterial( style.getTextMaterial() );
		}
	}

	void EditCtrl::doCreate()
	{
		if ( auto text = m_text.lock() )
		{
			auto & style = getStyle();
			text->setMaterial( style.getTextMaterial() );

			if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
			{
				text->setFont( style.getFontName() );
			}
		}

		doUpdateCaption();
		getControlsManager()->connectEvents( *this );
	}

	void EditCtrl::doDestroy()
	{
		getControlsManager()->disconnectEvents( *this );
	}

	void EditCtrl::doSetPosition( castor::Position const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelPosition( castor::Position{} );
		}
	}

	void EditCtrl::doSetSize( castor::Size const & value )
	{
		if ( auto text = m_text.lock() )
		{
			text->setPixelSize( value );
		}
	}

	void EditCtrl::doSetCaption( castor::String const & value )
	{
		m_caption = value;
		m_caretIt = m_caption.end();
		doUpdateCaption();
	}

	void EditCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
			text.reset();
		}
	}

	void EditCtrl::doUpdateFlags()
	{
		if ( auto text = m_text.lock() )
		{
			if ( isMultiLine() )
			{
				text->setTextWrappingMode( castor3d::TextWrappingMode::eBreak );
				text->setVAlign( castor3d::VAlign::eTop );
			}
		}
	}

	castor::String EditCtrl::doGetCaptionWithCaret()const
	{
		castor::String caption{ m_caption.begin(), m_caretIt.internal() };
		caption += cuT( '|' );

		if ( m_caretIt != m_caption.end() )
		{
			caption += castor::String( m_caretIt.internal(), m_caption.end() );
		}

		return caption;
	}

	void EditCtrl::onActivate( castor3d::HandlerEvent const & event )
	{
		m_active = true;
		doUpdateCaption();
	}

	void EditCtrl::onDeactivate( castor3d::HandlerEvent const & event )
	{
		m_active = false;
		doUpdateCaption();
	}

	void EditCtrl::onMouseLButtonDown( castor3d::MouseEvent const & event )
	{
	}

	void EditCtrl::onMouseLButtonUp( castor3d::MouseEvent const & event )
	{
	}

	void EditCtrl::onChar( castor3d::KeyboardEvent const & event )
	{
		auto code = event.getKey();

		if ( code >= castor3d::KeyboardKey( 0x20 )
			 && code <= castor3d::KeyboardKey( 0xFF )
			 && code != castor3d::KeyboardKey::eDelete )
		{
			doAddCharAtCaret( event.getChar() );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
		else if ( code == castor3d::KeyboardKey::eReturn && isMultiLine() )
		{
			doAddCharAtCaret( cuT( "\n" ) );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
	}

	void EditCtrl::onKeyDown( castor3d::KeyboardEvent const & event )
	{
		if ( !event.isCtrlDown() && !event.isAltDown() )
		{
			auto code = event.getKey();

			if ( code == castor3d::KeyboardKey::eBackspace )
			{
				doDeleteCharBeforeCaret();
				m_signals[size_t( EditEvent::eUpdated )]( m_caption );
			}
			else if ( code == castor3d::KeyboardKey::eDelete )
			{
				doDeleteCharAtCaret();
				m_signals[size_t( EditEvent::eUpdated )]( m_caption );
			}
			else if ( code == castor3d::KeyboardKey::eLeft && m_caretIt != m_caption.begin() )
			{
				--m_caretIt;
				doUpdateCaption();
			}
			else if ( code == castor3d::KeyboardKey::eRight && m_caretIt != m_caption.end() )
			{
				++m_caretIt;
				doUpdateCaption();
			}
			else if ( code == castor3d::KeyboardKey::eHome && m_caretIt != m_caption.begin() )
			{
				m_caretIt = m_caption.begin();
				doUpdateCaption();
			}
			else if ( code == castor3d::KeyboardKey::eEnd && m_caretIt != m_caption.end() )
			{
				m_caretIt = m_caption.end();
				doUpdateCaption();
			}
		}
	}

	void EditCtrl::onKeyUp( castor3d::KeyboardEvent const & event )
	{
	}

	void EditCtrl::doAddCharAtCaret( castor::String const & p_char )
	{
		auto diff = size_t( std::distance( castor::string::utf8::const_iterator( m_caption.begin() ), m_caretIt ) );
		m_caption = castor::String( m_caption.cbegin()
			, m_caretIt.internal() ) + p_char + castor::String( m_caretIt.internal(), m_caption.cend() );
		m_caretIt = castor::string::utf8::const_iterator( m_caption.begin() ) + diff + 1;
		doUpdateCaption();
	}

	void EditCtrl::doDeleteCharAtCaret()
	{
		if ( m_caretIt != m_caption.end() )
		{
			auto diff = size_t( std::distance( castor::string::utf8::const_iterator( m_caption.begin() ), m_caretIt ) );
			castor::String caption( m_caption.cbegin(), m_caretIt.internal() );
			castor::string::utf8::const_iterator it = m_caretIt;

			if ( ++it != m_caption.end() )
			{
				caption += castor::String( it.internal(), m_caption.cend() );
			}

			m_caption = caption;
			m_caretIt = castor::string::utf8::const_iterator( m_caption.begin() ) + diff;
			doUpdateCaption();
		}
	}

	void EditCtrl::doDeleteCharBeforeCaret()
	{
		if ( m_caretIt != m_caption.begin() )
		{
			--m_caretIt;
			auto diff = size_t( std::distance( castor::string::utf8::const_iterator( m_caption.begin() ), m_caretIt ) );
			castor::String caption( m_caption.cbegin(), m_caretIt.internal() );
			castor::string::utf8::const_iterator it = m_caretIt;

			if ( ++it != m_caption.end() )
			{
				caption += castor::String( it.internal(), m_caption.cend() );
			}

			m_caption = caption;
			m_caretIt = castor::string::utf8::const_iterator( m_caption.begin() ) + diff;
			doUpdateCaption();
		}
	}

	void EditCtrl::doUpdateCaption()
	{
		if ( auto text = m_text.lock() )
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
