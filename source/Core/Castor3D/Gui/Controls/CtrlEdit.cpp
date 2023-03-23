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
		, m_active{ false }
	{
		m_caret.updateIndex( m_caption.size(), m_caption );
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
		m_caret.overlay = caret;

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

		if ( auto caret = m_caret.overlay.lock() )
		{
			caret->setMaterial( style.getTextMaterial() );
		}
	}

	void EditCtrl::doCreate()
	{
		doUpdateStyle();
		doUpdateCaption();
		doUpdateCaret();
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
		m_caret.updateIndex( m_caption.size(), m_caption );
		doUpdateCaption();
		doUpdateCaret();
	}

	void EditCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text.lock() )
		{
			text->setVisible( visible );
		}

		if ( auto caret = m_caret.overlay.lock() )
		{
			caret->setVisible( m_caret.visible );
		}
	}

	void EditCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text.lock() )
		{
			text->setOrder( index++, 0u );
		}

		if ( auto caret = m_caret.overlay.lock() )
		{
			caret->setOrder( index++, 0u );
		}
	}

	void EditCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( auto text = m_text.lock() )
		{
			text->setOrder( text->getLevel() + offset, 0u );
		}

		if ( auto caret = m_caret.overlay.lock() )
		{
			caret->setOrder( caret->getLevel() + offset, 0u );
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
		m_caret.visible = m_active && isVisible();

		if ( auto caret = m_caret.overlay.lock() )
		{
			caret->setVisible( m_caret.visible );
		}
	}

	void EditCtrl::onDeactivate( HandlerEvent const & event )
	{
		m_active = false;
		m_caret.visible = false;

		if ( auto caret = m_caret.overlay.lock() )
		{
			caret->setVisible( m_caret.visible );
		}
	}

	void EditCtrl::doOnMouseButtonDown( MouseEvent const & event )
	{
		auto position = event.getPosition() - getAbsolutePosition();
		uint32_t index{};
		auto lineIt = std::find_if( m_metrics.lines.begin()
			, m_metrics.lines.end()
			, [&position, &index]( castor::TextLineMetrics const & lookup )
			{
				auto height = uint32_t( lookup.yMax - lookup.yMin );
				auto result = lookup.top + height > uint32_t( position->y );

				if ( !result )
				{
					index += uint32_t( lookup.chars.size() );
				}

				return result;
			} );

		if ( lineIt == m_metrics.lines.end() )
		{
			m_caret.updateIndex( m_caption.size(), m_caption );
		}
		else
		{
			auto it = std::find_if( lineIt->chars.begin()
				, lineIt->chars.end()
				, [&position, &index]( uint32_t lookup )
				{
					auto result = lookup > uint32_t( position->x );

					if ( !result )
					{
						++index;
					}

					return result;
				} );
			m_caret.updateIndex( index, m_caption );
		}

		doUpdateCaretIndices();
		doUpdateCaret();
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
			else if ( code == KeyboardKey::eLeft && m_caret.captionIt != m_caption.begin() )
			{
				--m_caret.captionIt;
				--m_caret.captionIndex;
				doUpdateCaretIndices();
				doUpdateCaret();
			}
			else if ( code == KeyboardKey::eRight && m_caret.captionIt != m_caption.end() )
			{
				++m_caret.captionIt;
				++m_caret.captionIndex;
				doUpdateCaretIndices();
				doUpdateCaret();
			}
			else if ( code == KeyboardKey::eUp )
			{
				doMoveCaretUp();
				doUpdateCaret();
			}
			else if ( code == KeyboardKey::eDown )
			{
				doMoveCaretDown();
				doUpdateCaret();
			}
			else if ( code == KeyboardKey::eHome
				&& m_caret.captionIt != m_caption.begin() )
			{
				if ( event.isCtrlDown() )
				{
					m_caret.updateIndex( 0u, m_caption );
				}
				else if ( m_caret.lineIndex > 0)
				{
					m_caret.updateIndex( m_metrics.lines[m_caret.lineIndex].firstCharIndex + 1u
						, m_caption );
				}
				else
				{
					m_caret.updateIndex( m_metrics.lines[m_caret.lineIndex].firstCharIndex
						, m_caption );
				}

				doUpdateCaretIndices();
				doUpdateCaret();
			}
			else if ( code == KeyboardKey::eEnd && m_caret.captionIt != m_caption.end() )
			{
				if ( event.isCtrlDown() )
				{
					m_caret.updateIndex( m_caption.size(), m_caption );
				}
				else
				{
					auto & line = m_metrics.lines[m_caret.lineIndex];
					m_caret.updateIndex( line.firstCharIndex + line.chars.size()
						, m_caption );
				}

				doUpdateCaretIndices();
				doUpdateCaret();
			}
		}
	}

	void EditCtrl::onKeyUp( KeyboardEvent const & event )
	{
	}

	void EditCtrl::doAddCharAtCaret( castor::String const & c )
	{
		auto diff = size_t( std::distance( m_caption.cbegin(), m_caret.captionIt ) );
		m_caption = castor::U32String( m_caption.cbegin(), m_caret.captionIt )
			+ castor::string::toU32String( c )
			+ castor::U32String( m_caret.captionIt, m_caption.cend() );
		m_caret.updateIndex( diff + 1, m_caption );
		doUpdateCaption();
		doUpdateCaret();
	}

	void EditCtrl::doDeleteCharAtCaret()
	{
		if ( m_caret.captionIt != m_caption.end() )
		{
			auto diff = size_t( std::distance( m_caption.cbegin(), m_caret.captionIt ) );
			castor::U32String caption( m_caption.cbegin(), m_caret.captionIt );
			auto it = m_caret.captionIt;

			if ( ++it != m_caption.end() )
			{
				caption += castor::U32String( it, m_caption.cend() );
			}

			m_caption = caption;
			m_caret.updateIndex( diff, m_caption );
			doUpdateCaption();
			doUpdateCaret();
		}
	}

	void EditCtrl::doDeleteCharBeforeCaret()
	{
		if ( m_caret.captionIt != m_caption.begin() )
		{
			m_caret.updateIndex( m_caret.captionIndex - 1u, m_caption );
			auto diff = size_t( std::distance( m_caption.cbegin(), m_caret.captionIt ) );
			castor::U32String caption( m_caption.cbegin(), m_caret.captionIt );
			auto it = m_caret.captionIt;

			if ( ++it != m_caption.end() )
			{
				caption += castor::U32String( it, m_caption.cend() );
			}

			m_caption = caption;
			m_caret.updateIndex( diff, m_caption );
			doUpdateCaption();
			doUpdateCaret();
		}
	}

	void EditCtrl::doMoveCaretUp()
	{
		if ( m_caret.lineIndex > 0u )
		{
			auto & line = m_metrics.lines[m_caret.lineIndex - 1u];

			if ( line.chars.size() < m_caret.charIndex )
			{
				m_caret.charIndex = uint32_t( line.chars.size() );
			}

			if ( m_caret.lineIndex == 1u )
			{
				--m_caret.charIndex;
			}

			m_caret.updateIndex( line.firstCharIndex + m_caret.charIndex, m_caption );
			doUpdateCaretIndices();
		}
		else
		{
			m_caret.updateIndex( 0u, m_caption );
		}
	}

	void EditCtrl::doMoveCaretDown()
	{
		if ( m_caret.lineIndex < m_metrics.lines.size() - 1u )
		{
			auto & line = m_metrics.lines[m_caret.lineIndex + 1u];

			if ( line.chars.size() < m_caret.charIndex )
			{
				m_caret.charIndex = uint32_t( line.chars.size() );
			}

			if ( m_caret.lineIndex == 0u )
			{
				++m_caret.charIndex;
			}

			m_caret.updateIndex( line.firstCharIndex + m_caret.charIndex, m_caption );
			doUpdateCaretIndices();
		}
		else
		{
			m_caret.updateIndex( m_caption.size(), m_caption );
		}
	}

	void EditCtrl::doUpdateCaretIndices()
	{
		m_caret.lineIndex = {};
		m_caret.charIndex = {};

		if ( !m_caption.empty() )
		{
			auto lineIt = m_metrics.lines.begin();
			auto charIt = lineIt->chars.begin();
			uint32_t index{};

			while ( index < m_caret.captionIndex
				&& lineIt != m_metrics.lines.end() )
			{
				if ( charIt == lineIt->chars.end() )
				{
					++lineIt;
					++m_caret.lineIndex;
					charIt = lineIt->chars.begin();
					m_caret.charIndex = 0u;
				}

				++m_caret.charIndex;
				++index;
				++charIt;
			}
		}
	}

	void EditCtrl::doUpdateCaret()
	{
		if ( auto text = m_text.lock() )
		{
			if ( auto caret = m_caret.overlay.lock() )
			{
				auto font = text->getFontTexture()->getFont();
				castor::Position position{ 1, 0 };
				castor::Size size{ 1u, font->getHeight() };

				if ( !m_caption.empty() )
				{
					auto index = uint32_t( std::distance( m_caption.cbegin(), m_caret.captionIt ) );
					auto lineIt = m_metrics.lines.begin();
					auto charIt = lineIt->chars.begin();
					uint32_t left{};
					uint32_t top{};
					uint32_t i{};

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

	void EditCtrl::doUpdateMetrics()
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
				m_metrics = font->getTextMetrics( m_caption, getSize()->x, isMultiLine() );
			}
			else
			{
				m_metrics = {};
			}
		}
	}

	void EditCtrl::doUpdateCaption()
	{
		doUpdateMetrics();
		doUpdateCaretIndices();

		if ( auto text = m_text.lock() )
		{
			text->setCaption( m_caption );
		}
	}
}
