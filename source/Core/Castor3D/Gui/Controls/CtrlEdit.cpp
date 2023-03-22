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
		, m_caption{ castor::string::toU32String( caption ) }
		, m_caretIt{ m_caption.end() }
		, m_active{ false }
	{
		setBorderSize( castor::Point4ui{ 1, 1, 1, 1 } );
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
			? m_scene->addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() ).lock()->getTextOverlay();
		text->setPixelSize( getSize() );
		text->setVAlign( VAlign::eCenter );
		text->setVisible( visible );
		m_text = text;

		auto caret = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Caret" )
				, getEngine()
				, OverlayType::ePanel
				, nullptr
				, &text->getOverlay() ).lock()->getPanelOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Caret" )
				, getEngine()
				, OverlayType::ePanel
				, nullptr
				, &text->getOverlay() ).lock()->getPanelOverlay();
		caret->setVisible( false );
		m_caret = caret;

		setStyle( style );
		doUpdateFlags();
	}

	void EditCtrl::updateCaption( castor::String const & value )
	{
		m_caption = castor::string::toU32String( value );
	}

	void EditCtrl::doUpdateStyle()
	{
		auto & style = getStyle();

		if ( auto text = m_text.lock() )
		{
			text->setMaterial( style.getTextMaterial() );

			if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
			{
				text->setFont( style.getFontName() );
			}
		}

		if ( auto caret = m_caret.lock() )
		{
			caret->setMaterial( style.getTextMaterial() );
		}
	}

	void EditCtrl::doCreate()
	{
		doUpdateStyle();
		doUpdateCaption();
		doUpdateCaretPosition();
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

	void EditCtrl::doSetCaption( castor::U32String const & value )
	{
		m_caption = value;
		m_caretIt = m_caption.end();
		doUpdateCaption();
		doUpdateCaretPosition();
	}

	void EditCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
		}

		if ( auto caret = m_caret.lock() )
		{
			caret->setVisible( m_caretVisible );
		}
	}

	void EditCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text.lock() )
		{
			text->setOrder( index++, 0u );
		}

		if ( auto caret = m_caret.lock() )
		{
			caret->setOrder( index++, 0u );
		}
	}

	void EditCtrl::doUpdateFlags()
	{
		if ( auto text = m_text.lock() )
		{
			if ( isMultiLine() )
			{
				text->setTextWrappingMode( TextWrappingMode::eBreakWords );
				text->setLineSpacingMode( TextLineSpacingMode::eMaxLineHeight );
				text->setVAlign( VAlign::eTop );
			}
		}
	}

	void EditCtrl::onActivate( HandlerEvent const & event )
	{
		m_active = true;
		m_caretVisible = m_active && isVisible();

		if ( auto caret = m_caret.lock() )
		{
			caret->setVisible( m_caretVisible );
		}
	}

	void EditCtrl::onDeactivate( HandlerEvent const & event )
	{
		m_active = false;
		m_caretVisible = false;

		if ( auto caret = m_caret.lock() )
		{
			caret->setVisible( m_caretVisible );
		}
	}

	void EditCtrl::doOnMouseButtonDown( MouseEvent const & event )
	{
	}

	void EditCtrl::doOnMouseButtonUp( MouseEvent const & event )
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
				doUpdateCaretPosition();
			}
			else if ( code == KeyboardKey::eRight && m_caretIt != m_caption.end() )
			{
				++m_caretIt;
				doUpdateCaretPosition();
			}
			else if ( code == KeyboardKey::eHome && m_caretIt != m_caption.begin() )
			{
				m_caretIt = m_caption.begin();
				doUpdateCaretPosition();
			}
			else if ( code == KeyboardKey::eEnd && m_caretIt != m_caption.end() )
			{
				m_caretIt = m_caption.end();
				doUpdateCaretPosition();
			}
		}
	}

	void EditCtrl::onKeyUp( KeyboardEvent const & event )
	{
	}

	void EditCtrl::doAddCharAtCaret( castor::String const & c )
	{
		auto diff = size_t( std::distance( m_caption.cbegin(), m_caretIt ) );
		m_caption = castor::U32String( m_caption.cbegin(), m_caretIt )
			+ castor::string::toU32String( c )
			+ castor::U32String( m_caretIt, m_caption.cend() );
		m_caretIt = std::next( m_caption.cbegin(), ptrdiff_t( diff + 1 ) );
		doUpdateCaption();
		doUpdateCaretPosition();
	}

	void EditCtrl::doDeleteCharAtCaret()
	{
		if ( m_caretIt != m_caption.end() )
		{
			auto diff = size_t( std::distance( m_caption.cbegin(), m_caretIt ) );
			castor::U32String caption( m_caption.cbegin(), m_caretIt );
			auto it = m_caretIt;

			if ( ++it != m_caption.end() )
			{
				caption += castor::U32String( it, m_caption.cend() );
			}

			m_caption = caption;
			m_caretIt = std::next( m_caption.cbegin(), ptrdiff_t( diff ) );
			doUpdateCaption();
			doUpdateCaretPosition();
		}
	}

	void EditCtrl::doDeleteCharBeforeCaret()
	{
		if ( m_caretIt != m_caption.begin() )
		{
			--m_caretIt;
			auto diff = size_t( std::distance( m_caption.cbegin(), m_caretIt ) );
			castor::U32String caption( m_caption.cbegin(), m_caretIt );
			auto it = m_caretIt;

			if ( ++it != m_caption.end() )
			{
				caption += castor::U32String( it, m_caption.cend() );
			}

			m_caption = caption;
			m_caretIt = std::next( m_caption.cbegin(), ptrdiff_t( diff ) );
			doUpdateCaption();
			doUpdateCaretPosition();
		}
	}

	void EditCtrl::doUpdateCaretPosition()
	{
		if ( auto text = m_text.lock() )
		{
			if ( auto caret = m_caret.lock() )
			{
				auto font = text->getFontTexture()->getFont();
				castor::Position position{ 1, 0 };
				castor::Size size{ 1u, font->getHeight() };

				if ( !m_caption.empty() )
				{
					auto index = uint32_t( std::distance( m_caption.cbegin(), m_caretIt ) );
					uint32_t i{};
					auto lineIt = m_metrics.lines.begin();
					auto charIt = lineIt->chars.begin();
					uint32_t left{};
					uint32_t top{};

					while ( i < index )
					{
						if ( charIt == lineIt->chars.end() )
						{
							++lineIt;

							if ( lineIt == m_metrics.lines.end() )
							{
								break;
							}

							charIt = lineIt->chars.begin();
						}

						left = *charIt;
						top = lineIt->top;
						++charIt;
						++i;
					}

					if ( lineIt != m_metrics.lines.end() )
					{
						position.x() = int32_t( left );
						position.y() = int32_t( top );
						size->y = uint32_t( m_metrics.yMax - m_metrics.yMin );
					}
				}

				caret->setPixelPosition( position );
				caret->setPixelSize( size );
			}
		}
	}

	void EditCtrl::doUpdateCaption()
	{
		if ( auto text = m_text.lock() )
		{
			auto fontTexture = text->getFontTexture();
			auto font = fontTexture->getFont();
			std::vector< char32_t > newCaption;

			for ( auto c : m_caption )
			{
				if ( !font->hasGlyphAt( c ) )
				{
					newCaption.push_back( c );
				}
			}

			if ( !newCaption.empty() )
			{
				for ( auto c : newCaption )
				{
					font->loadGlyph( c );
				}

				fontTexture->update( true );
			}

			if ( !m_caption.empty() )
			{
				m_metrics = font->getTextMetrics( m_caption, getSize()->x );
			}
			else
			{
				m_metrics = {};
			}

			text->setCaption( m_caption );
		}
	}
}
