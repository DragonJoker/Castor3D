#include "Castor3D/Gui/Controls/CtrlEdit.hpp"

#include "Castor3D/Gui/ControlsManager.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementCUSmartPtr( castor3d, EditCtrl )

namespace castor3d
{
	EditCtrl::EditCtrl( SceneRPtr scene
		, castor::String const & name
		, EditStyleRPtr style
		, ControlRPtr parent )
		: EditCtrl{ scene
			, name
			, style
			, parent
			, castor::String{}
			, castor::Position{}
			, castor::Size{}
			, 0
			, true }
	{
	}

	EditCtrl::EditCtrl( SceneRPtr scene
		, castor::String const & name
		, EditStyleRPtr style
		, ControlRPtr parent
		, castor::String const & caption
		, castor::Position const & position
		, castor::Size const & size
		, uint64_t flags
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
		, m_caption{ caption }
		, m_caretIt{ m_caption.end() }
		, m_active{ false }
	{
		setBackgroundBorders( castor::Point4ui{ 1, 1, 1, 1 } );
		EventHandler::connect( MouseEventType::ePushed
			, [this]( MouseEvent const & event )
			{
				onMouseLButtonDown( event );
			} );
		EventHandler::connect( MouseEventType::eReleased
			, [this]( MouseEvent const & event )
			{
				onMouseLButtonUp( event );
			} );
		EventHandler::connect( KeyboardEventType::ePushed
			, [this]( KeyboardEvent const & event )
			{
				onKeyDown( event );
			} );
		EventHandler::connect( KeyboardEventType::eReleased
			, [this]( KeyboardEvent const & event )
			{
				onKeyUp( event );
			} );
		EventHandler::connect( KeyboardEventType::eChar
			, [this]( KeyboardEvent const & event )
			{
				onChar( event );
			} );
		EventHandler::connect( HandlerEventType::eActivate
			, [this]( HandlerEvent const & event )
			{
				onActivate( event );
			} );
		EventHandler::connect( HandlerEventType::eDeactivate
			, [this]( HandlerEvent const & event )
			{
				onDeactivate( event );
			} );

		auto text = m_scene
			? m_scene->addNewOverlay( cuT( "T_CtrlEdit_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackground()->getOverlay() ).lock()->getTextOverlay()
			: getEngine().addNewOverlay( cuT( "T_CtrlEdit_[" ) + getName() + cuT( "]" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackground()->getOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setVAlign( VAlign::eCenter );
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
				text->setTextWrappingMode( TextWrappingMode::eBreak );
				text->setVAlign( VAlign::eTop );
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

	void EditCtrl::onActivate( HandlerEvent const & event )
	{
		m_active = true;
		doUpdateCaption();
	}

	void EditCtrl::onDeactivate( HandlerEvent const & event )
	{
		m_active = false;
		doUpdateCaption();
	}

	void EditCtrl::onMouseLButtonDown( MouseEvent const & event )
	{
	}

	void EditCtrl::onMouseLButtonUp( MouseEvent const & event )
	{
	}

	void EditCtrl::onChar( KeyboardEvent const & event )
	{
		auto code = event.getKey();

		if ( code >= KeyboardKey( 0x20 )
			 && code <= KeyboardKey( 0xFF )
			 && code != KeyboardKey::eDelete )
		{
			doAddCharAtCaret( event.getChar() );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
		else if ( code == KeyboardKey::eReturn && isMultiLine() )
		{
			doAddCharAtCaret( cuT( "\n" ) );
			m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		}
	}

	void EditCtrl::onKeyDown( KeyboardEvent const & event )
	{
		if ( !event.isCtrlDown() && !event.isAltDown() )
		{
			auto code = event.getKey();

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

	void EditCtrl::onKeyUp( KeyboardEvent const & event )
	{
	}

	void EditCtrl::doAddCharAtCaret( castor::String const & c )
	{
		auto diff = size_t( std::distance( castor::string::utf8::const_iterator( m_caption.begin() ), m_caretIt ) );
		m_caption = castor::String( m_caption.cbegin()
			, m_caretIt.internal() ) + c + castor::String( m_caretIt.internal(), m_caption.cend() );
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
				text->setCaption( castor::string::toU32String( doGetCaptionWithCaret() ) );
			}
			else
			{
				text->setCaption( castor::string::toU32String( m_caption ) );
			}
		}
	}
}
