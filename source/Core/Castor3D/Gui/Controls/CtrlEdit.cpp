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
	static uint32_t constexpr LineEndWidth = 5u;

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
				, &getBackgroundOverlay() ).lock()->getPanelOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Caret" )
				, getEngine()
				, OverlayType::ePanel
				, nullptr
				, &getBackgroundOverlay() ).lock()->getPanelOverlay();
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

		for ( auto sel : m_selections )
		{
			if ( auto panel = sel.lock() )
			{
				panel->setMaterial( style.getSelectionMaterial() );
			}
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

		if ( !m_selections.empty() )
		{
			for ( auto sel : m_selections )
			{
				if ( auto panel = sel.lock() )
				{
					panel->setOrder( index, 0u );
				}
			}

			++index;
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

		for ( auto sel : m_selections )
		{
			if ( auto panel = sel.lock() )
			{
				panel->setOrder( panel->getLevel() + offset, 0u );
			}
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

		doClearSelection();
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

			if ( it != lineIt->chars.end() )
			{
				m_caret.updateIndex( index, m_caption );
			}
			else
			{
				m_caret.updateIndex( lineIt->firstCharIndex + lineIt->chars.size(), m_caption );
			}
		}

		doClearSelection();
		doUpdateCaretIndices();
		doUpdateCaret();
	}

	void EditCtrl::doOnMouseButtonUp( MouseEvent const & event )
	{
	}

	void EditCtrl::onChar( KeyboardEvent const & event )
	{
		auto code = event.getKey();

		if ( code >= KeyboardKey::eAsciiBegin
			 && code <= KeyboardKey::eAsciiEnd
			 && code != KeyboardKey::eDelete )
		{
			if ( code == KeyboardKey( 'c' ) && event.isCtrlDown() )
			{
				doCopyText();
			}
			else if ( code == KeyboardKey( 'v' ) && event.isCtrlDown() )
			{
				doPasteText();
			}
			else if ( code == KeyboardKey( 'x' ) && event.isCtrlDown() )
			{
				doCutText();
			}
			else
			{
				doAddCharAtCaret( event.getChar() );
			}
		}
		else if ( code == KeyboardKey::eReturn && isMultiLine() )
		{
			doAddCharAtCaret( cuT( "\n" ) );
		}
	}

	void EditCtrl::onKeyDown( KeyboardEvent const & event )
	{
		if ( !event.isAltDown() )
		{
			auto code = event.getKey();

			if ( code == KeyboardKey( 'C' ) && event.isCtrlDown() )
			{
				doCopyText();
			}
			else if ( code == KeyboardKey( 'V' ) && event.isCtrlDown() )
			{
				doPasteText();
			}
			else if ( code == KeyboardKey( 'X' ) && event.isCtrlDown() )
			{
				doCutText();
			}
			else
			{
				switch ( code )
				{
				case KeyboardKey::eBackspace:
					doDeleteCharBeforeCaret( event.isCtrlDown() );
					break;
				case KeyboardKey::eLeft:
					doMoveCaretLeft( event.isShiftDown(), event.isCtrlDown() );
					break;
				case KeyboardKey::eUp:
					doMoveCaretUp( event.isShiftDown(), event.isCtrlDown() );
					break;
				case KeyboardKey::eHome:
					doMoveCaretHome( event.isShiftDown(), event.isCtrlDown() );
					break;
				case KeyboardKey::eDelete:
					doDeleteCharAtCaret( event.isCtrlDown() );
					break;
				case KeyboardKey::eRight:
					doMoveCaretRight( event.isShiftDown(), event.isCtrlDown() );
					break;
				case KeyboardKey::eDown:
					doMoveCaretDown( event.isShiftDown(), event.isCtrlDown() );
					break;
				case KeyboardKey::eEnd:
					doMoveCaretEnd( event.isShiftDown(), event.isCtrlDown() );
					break;
				default:
					break;
				}
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
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
	}

	void EditCtrl::doDeleteCharAtCaret( bool isCtrlDown )
	{
		if ( doDeleteSelection( isCtrlDown )
			|| isCtrlDown
			|| m_caret.captionIt == m_caption.end() )
		{
			return;
		}

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
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
	}

	void EditCtrl::doDeleteCharBeforeCaret( bool isCtrlDown )
	{
		if ( doDeleteSelection( isCtrlDown )
			|| isCtrlDown
			|| m_caret.captionIt == m_caption.begin() )
		{
			return;
		}

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
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
	}

	void EditCtrl::doMoveCaretLeft( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.captionIt == m_caption.begin() )
		{
			return;
		}

		if ( isShiftDown && !m_selecting )
		{
			m_selecting = true;
			m_selection.begin = m_caret;
		}

		if ( isCtrlDown )
		{
			std::u32string_view caption{ m_caption.data(), m_caret.captionIndex - 1u };
			auto rit = caption.find_last_of( U" \n\t" );

			if ( rit == castor::U32String::npos )
			{
				m_caret.updateIndex( 0, m_caption );
			}
			else
			{
				m_caret.updateIndex( rit, m_caption );
			}
		}
		else
		{
			m_caret.decrementIndex();
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
	}

	void EditCtrl::doMoveCaretRight( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.captionIt == m_caption.end() )
		{
			return;
		}

		if ( isShiftDown && !m_selecting )
		{
			m_selecting = true;
			m_selection.begin = m_caret;
		}

		if ( isCtrlDown )
		{
			auto it = m_caption.find_first_of( U" \n\t", m_caret.captionIndex + 1u );

			if ( it == castor::U32String::npos )
			{
				m_caret.updateIndex( m_caption.size(), m_caption );
			}
			else
			{
				m_caret.updateIndex( it + 1u, m_caption );
			}
		}
		else
		{
			m_caret.incrementIndex();
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
	}

	void EditCtrl::doMoveCaretUp( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( isCtrlDown
			|| m_caret.captionIt == m_caption.begin() )
		{
			return;
		}

		if ( isShiftDown && !m_selecting )
		{
			m_selecting = true;
			m_selection.begin = m_caret;
		}

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
		}
		else
		{
			m_caret.updateIndex( 0u, m_caption );
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
	}

	void EditCtrl::doMoveCaretDown( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( isCtrlDown
			|| m_caret.captionIt == m_caption.end() )
		{
			return;
		}

		if ( isShiftDown && !m_selecting )
		{
			m_selecting = true;
			m_selection.begin = m_caret;
		}

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
		}
		else
		{
			m_caret.updateIndex( m_caption.size(), m_caption );
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
	}

	void EditCtrl::doMoveCaretHome( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.captionIt == m_caption.begin() )
		{
			return;
		}

		if ( isShiftDown && !m_selecting )
		{
			m_selecting = true;
			m_selection.begin = m_caret;
		}

		if ( isCtrlDown )
		{
			m_caret.updateIndex( 0u, m_caption );
		}
		else if ( m_caret.lineIndex > 0 )
		{
			m_caret.updateIndex( m_metrics.lines[m_caret.lineIndex].firstCharIndex + 1u
				, m_caption );
		}
		else
		{
			m_caret.updateIndex( m_metrics.lines[m_caret.lineIndex].firstCharIndex
				, m_caption );
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
	}

	void EditCtrl::doMoveCaretEnd( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.captionIt == m_caption.end() )
		{
			return;
		}

		if ( isShiftDown && !m_selecting )
		{
			m_selecting = true;
			m_selection.begin = m_caret;
		}

		if ( isCtrlDown )
		{
			m_caret.updateIndex( m_caption.size(), m_caption );
		}
		else
		{
			auto & line = m_metrics.lines[m_caret.lineIndex];
			m_caret.updateIndex( line.firstCharIndex + line.chars.size()
				, m_caption );
		}

		if ( isShiftDown )
		{
			if ( !m_selecting )
			{
				m_selecting = true;
				m_selection.begin = m_caret;
			}

			m_selection.end = m_caret;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
	}

	void EditCtrl::doUpdate( CaretIndices & indices )
	{
		indices.lineIndex = {};
		indices.charIndex = {};

		if ( !m_caption.empty() )
		{
			auto lineIt = m_metrics.lines.begin();
			auto charIt = lineIt->chars.begin();
			uint32_t index{};

			while ( index < indices.captionIndex
				&& lineIt != m_metrics.lines.end() )
			{
				if ( charIt == lineIt->chars.end() )
				{
					++lineIt;
					++indices.lineIndex;
					charIt = lineIt->chars.begin();
					indices.charIndex = 0u;
				}

				++indices.charIndex;
				++index;
				++charIt;
			}
		}
	}

	void EditCtrl::doUpdateCaretIndices()
	{
		doUpdate( m_caret );

		if ( m_selecting )
		{
			doUpdate( m_selection.begin );
			doUpdate( m_selection.end );
		}
	}

	void EditCtrl::doUpdateCaret()
	{
		if ( auto text = m_text.lock() )
		{
			if ( auto caret = m_caret.overlay.lock() )
			{
				auto font = text->getFontTexture()->getFont();
				castor::Position position{ 0, 0 };
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
						//size->y = std::max( size->y
						//	, uint32_t( m_metrics.yMax - m_metrics.yMin ) );
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
		doClearSelection();
		doUpdateMetrics();
		doUpdateCaretIndices();

		if ( auto text = m_text.lock() )
		{
			text->setCaption( m_caption );
		}
	}

	void EditCtrl::doUpdateSelection()
	{
		if ( !m_selecting )
		{
			doClearSelection();
			return;
		}

		auto [selBegin, selEnd] = doGetNormalisedSelection();
		auto lineDiff = selEnd.lineIndex - selBegin.lineIndex;
		auto & style = getStyle();

		while ( lineDiff >= m_selections.size() )
		{
			auto panel = getEngine().addNewOverlay( getName() + cuT( "/Selection" ) + castor::string::toString( m_selections.size() )
				, getEngine()
				, OverlayType::ePanel
				, nullptr
				, &getBackgroundOverlay() ).lock()->getPanelOverlay();
			panel->setMaterial( style.getSelectionMaterial() );
			panel->setVisible( false );
			m_selections.push_back( panel );
		}

		uint32_t selLineIndex{};

		if ( lineDiff > 0 )
		{
			// Multiline selection.
			// Process first and last lines separately (since they may not be fully selected).
			if ( auto panel = m_selections[selLineIndex++].lock() )
			{
				// First line
				auto & line = m_metrics.lines[selBegin.lineIndex++];
				auto left = selBegin.charIndex == 0u
					? 0
					: int32_t( line.chars[selBegin.charIndex - 1u] );
				panel->setPixelPosition( { left, int32_t( line.top ) } );
				panel->setPixelSize( { LineEndWidth + line.chars.back() - uint32_t( left )
					, uint32_t( m_metrics.yMax - m_metrics.yMin ) } );
				panel->setVisible( true );
			}

			if ( auto panel = m_selections[selLineIndex++].lock() )
			{
				// LastLine
				auto & line = m_metrics.lines[selEnd.lineIndex--];
				auto right = int32_t( line.chars[selEnd.charIndex - 1u] );

				panel->setPixelPosition( { 0, int32_t( line.top ) } );
				panel->setPixelSize( { uint32_t( right )
					, uint32_t( m_metrics.yMax - m_metrics.yMin ) } );
				panel->setVisible( true );
			}

			// Process remaining lines
			while ( selBegin.lineIndex <= selEnd.lineIndex )
			{
				if ( auto panel = m_selections[selLineIndex++].lock() )
				{
					auto & line = m_metrics.lines[selBegin.lineIndex];
					panel->setPixelPosition( { 0, int32_t( line.top ) } );
					panel->setPixelSize( { LineEndWidth + line.chars.back()
						, uint32_t( m_metrics.yMax - m_metrics.yMin ) } );
					panel->setVisible( true );
				}

				++selBegin.lineIndex;
			}
		}
		else if ( auto panel = m_selections[selLineIndex++].lock() )
		{
			// Single line selection.
			auto charDiff = selEnd.charIndex - selBegin.charIndex;

			if ( charDiff > 0 )
			{
				auto & line = m_metrics.lines[selBegin.lineIndex];
				auto left = selBegin.charIndex == 0u
					? 0
					: int32_t( line.chars[selBegin.charIndex - 1u] );
				auto right = int32_t( line.chars[selEnd.charIndex - 1u] );
				panel->setPixelPosition( { left, int32_t( line.top ) } );
				panel->setPixelSize( { uint32_t( right - left )
					, uint32_t( m_metrics.yMax - m_metrics.yMin ) } );
				panel->setVisible( true );
			}
			else
			{
				panel->setVisible( false );
			}
		}

		for ( auto sel : castor::makeArrayView( m_selections.begin() + selLineIndex, m_selections.end() ) )
		{
			if ( auto panel = sel.lock() )
			{
				panel->setVisible( false );
			}
		}
	}

	void EditCtrl::doClearSelection()
	{
		m_selecting = false;
		m_selection.begin = {};
		m_selection.end = {};

		for ( auto sel : m_selections )
		{
			if ( auto panel = sel.lock() )
			{
				panel->setVisible( false );
			}
		}
	}

	EditCtrl::Selection EditCtrl::doGetNormalisedSelection()
	{
		auto selBegin = m_selection.begin;
		auto selEnd = m_selection.end;

		if ( selBegin.lineIndex > selEnd.lineIndex
			|| ( selBegin.lineIndex == selEnd.lineIndex
				&& selBegin.charIndex > selEnd.charIndex ) )
		{
			std::swap( selBegin, selEnd );
		}

		return { selBegin, selEnd };
	}

	bool EditCtrl::doDeleteSelection( bool isCtrlDown )
	{
		if ( !m_selecting
			|| isCtrlDown
			|| ( m_selection.begin.lineIndex == m_selection.end.lineIndex
				&& m_selection.begin.charIndex == m_selection.end.charIndex ) )
		{
			return false;
		}

		auto [selBegin, selEnd] = doGetNormalisedSelection();
		m_caption.erase( m_caption.begin() + selBegin.captionIndex
			, m_caption.begin() + selEnd.captionIndex );
		m_caret.updateIndex( selBegin.captionIndex, m_caption );
		doUpdateCaption();
		doUpdateCaret();
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		return true;
	}

	void EditCtrl::doCopyText()
	{
		if ( !m_selecting
			|| ( m_selection.begin.lineIndex == m_selection.end.lineIndex
				&& m_selection.begin.charIndex == m_selection.end.charIndex ) )
		{
			return;
		}

		auto [selBegin, selEnd] = doGetNormalisedSelection();
		getControlsManager()->onClipboardTextAction( true
			, { m_caption.begin() + selBegin.captionIndex
				, m_caption.begin() + selEnd.captionIndex } );
	}

	void EditCtrl::doCutText()
	{
		if ( !m_selecting
			|| ( m_selection.begin.lineIndex == m_selection.end.lineIndex
				&& m_selection.begin.charIndex == m_selection.end.charIndex ) )
		{
			return;
		}

		doCopyText();
		doDeleteSelection( false );
	}

	void EditCtrl::doPasteText()
	{
		auto clipboard = getControlsManager()->onClipboardTextAction( false, {} );

		if ( clipboard.empty() )
		{
			return;
		}

		if ( !m_selecting
			|| ( m_selection.begin.lineIndex == m_selection.end.lineIndex
				&& m_selection.begin.charIndex == m_selection.end.charIndex ) )
		{
			m_caption.insert( m_caret.captionIt
				, clipboard.begin()
				, clipboard.end() );
			m_caret.updateIndex( m_caret.captionIndex + clipboard.size(), m_caption );
		}
		else
		{
			auto [selBegin, selEnd] = doGetNormalisedSelection();
			m_caption.replace( m_caption.begin() + selBegin.captionIndex
				, m_caption.begin() + selEnd.captionIndex
				, clipboard.begin()
				, clipboard.end() );
			m_caret.updateIndex( selBegin.captionIndex + clipboard.size()
				, m_caption );
		}

		doUpdateCaption();
		doUpdateCaret();
	}
}
