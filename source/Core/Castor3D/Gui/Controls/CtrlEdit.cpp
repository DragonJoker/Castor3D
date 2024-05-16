#include "Castor3D/Gui/Controls/CtrlEdit.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Gui/Controls/CtrlScrollBar.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/Font.hpp>

CU_ImplementSmartPtr( castor3d, EditCtrl )

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
		, ScrollableCtrl{ static_cast< Control & >( *this )
			, style }
		, m_caption{ castor::toUtf8U32String( caption ) }
		, m_onScrollContent{ onScrollContent.connect( [this]( castor::Position const & pos )
			{
				doScrollContent( pos );
			} ) }
	{
		m_caret.indices.updateIndex( m_caption.size(), m_caption );
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
			, [this]( HandlerEvent const & )
			{
				onActivate();
			} );
		EventHandler::connect( HandlerEventType::eDeactivate
			, [this]( HandlerEvent const & )
			{
				onDeactivate();
			} );

		auto text = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Text" )
				, getEngine()
				, OverlayType::eText
				, nullptr
				, &getBackgroundOverlay() )->getTextOverlay();
		text->setPixelPosition( getClientOffset() );
		text->setPixelSize( getClientSize() );
		text->setVAlign( VAlign::eCenter );
		text->setVisible( visible );
		m_text = text;

		auto caret = m_scene
			? m_scene->addNewOverlay( getName() + cuT( "/Caret" )
				, getEngine()
				, OverlayType::ePanel
				, nullptr
				, &text->getOverlay() )->getPanelOverlay()
			: getEngine().addNewOverlay( getName() + cuT( "/Caret" )
				, getEngine()
				, OverlayType::ePanel
				, nullptr
				, &text->getOverlay() )->getPanelOverlay();
		caret->setPixelPosition( getClientOffset() );
		caret->setPixelSize( {} );
		caret->setVisible( false );
		m_caret.overlay = caret;

		setStyle( style );
		doUpdateFlags();
	}

	EditCtrl::~EditCtrl()noexcept
	{
		uint32_t i{};

		for ( auto sel : m_selections )
		{
			if ( sel )
			{
				if ( m_scene )
				{
					m_scene->removeOverlay( getName() + cuT( "/Selection" ) + castor::string::toString( i ), true );
				}
				else
				{
					getEngine().removeOverlay( getName() + cuT( "/Selection" ) + castor::string::toString( i ), true );
				}
			}

			++i;
		}

		if ( m_caret.overlay )
		{
			if ( m_scene )
			{
				m_scene->removeOverlay( getName() + cuT( "/Caret" ), true );
			}
			else
			{
				getEngine().removeOverlay( getName() + cuT( "/Caret" ), true );
			}
		}

		if ( m_text )
		{
			if ( m_scene )
			{
				m_scene->removeOverlay( getName() + cuT( "/Text" ), true );
			}
			else
			{
				getEngine().removeOverlay( getName() + cuT( "/Text" ), true );
			}
		}
	}

	void EditCtrl::updateCaption( castor::String const & value )
	{
		m_caption = castor::toUtf8U32String( value );
	}

	void EditCtrl::doUpdateStyle()
	{
		auto const & style = getStyle();

		if ( auto text = m_text )
		{
			text->setMaterial( style.getTextMaterial() );

			if ( !text->getFontTexture() || !text->getFontTexture()->getFont() )
			{
				text->setFont( style.getFontName() );
			}
		}

		if ( auto caret = m_caret.overlay )
		{
			caret->setMaterial( style.getTextMaterial() );
		}

		for ( auto sel : m_selections )
		{
			if ( auto panel = sel )
			{
				panel->setMaterial( style.getSelectionMaterial() );
			}
		}

		updateScrollBarsStyle();
	}

	void EditCtrl::doCreate()
	{
		doUpdateStyle();
		doUpdateCaption();
		doUpdateCaret();
		createScrollBars();
		getControlsManager()->connectEvents( *this );
	}

	void EditCtrl::doDestroy()
	{
		destroyScrollBars();
		getControlsManager()->disconnectEvents( *this );
	}

	void EditCtrl::doAddChild( ControlRPtr control )
	{
		registerControl( *control );
	}

	void EditCtrl::doRemoveChild( ControlRPtr control )
	{
		unregisterControl( *control );
	}

	castor::Point4ui EditCtrl::doUpdateClientRect( castor::Point4ui const & clientRect )
	{
		return updateScrollableClientRect( clientRect );
	}

	void EditCtrl::doSetPosition( castor::Position const & value )
	{
		updateScrollBars();
	}

	void EditCtrl::doSetSize( castor::Size const & value )
	{
		// Add 1 to account for caret at line end.
		if ( !hasVerticalScrollBar()
			&& !hasHorizontalScrollBar() )
		{
			if ( auto text = m_text )
			{
				text->setPixelSize( { value->x + 1u, value->y } );
			}
		}
		else
		{
			updateTotalSize( { uint32_t( m_metrics.width + 1.0f ), uint32_t( m_metrics.height ) } );
			updateScrollBars();
		}
	}

	void EditCtrl::doSetBorderSize( castor::Point4ui const & value )
	{
		updateScrollBars();
	}

	void EditCtrl::doSetCaption( castor::U32String const & value )
	{
		m_caption = value;
		m_caret.indices.updateIndex( m_caption.size(), m_caption );
		doUpdateCaption();
		doUpdateCaret();
	}

	void EditCtrl::doSetVisible( bool visible )
	{
		if ( auto text = m_text )
		{
			text->setVisible( visible );
		}

		if ( auto caret = m_caret.overlay )
		{
			caret->setVisible( m_caret.visible );
		}

		setScrollBarsVisible( visible );
	}

	void EditCtrl::doUpdateZIndex( uint32_t & index )
	{
		if ( auto text = m_text )
		{
			text->setOrder( index, 0u );
			++index;
		}

		if ( auto caret = m_caret.overlay )
		{
			caret->setOrder( index, 0u );
			++index;
		}

		m_panelsZIndex = index;

		if ( !m_selections.empty() )
		{
			uint32_t i{};

			for ( auto sel : m_selections )
			{
				if ( auto panel = sel )
				{
					panel->setOrder( index, i );
					++i;
				}
			}

			++index;
		}

		updateScrollZIndex( index );
	}

	void EditCtrl::doAdjustZIndex( uint32_t offset )
	{
		if ( auto text = m_text )
		{
			text->setOrder( text->getLevel() + offset, 0u );
		}

		if ( auto caret = m_caret.overlay )
		{
			caret->setOrder( caret->getLevel() + offset, 0u );
		}

		m_panelsZIndex += offset;
		uint32_t i{};

		for ( auto sel : m_selections )
		{
			if ( auto panel = sel )
			{
				panel->setOrder( panel->getLevel() + offset, i );
				++i;
			}
		}

		adjustScrollZIndex( offset );
	}

	void EditCtrl::doUpdateFlags()
	{
		if ( auto text = m_text )
		{
			if ( isMultiLine() )
			{
				if ( castor::checkFlag( getFlags(), ScrollBarFlag::eHorizontal ) )
				{
					text->setTextWrappingMode( TextWrappingMode::eNone );
				}
				else
				{
					text->setTextWrappingMode( TextWrappingMode::eBreakWords );
				}

				text->setLineSpacingMode( TextLineSpacingMode::eMaxLineHeight );
				text->setVAlign( VAlign::eTop );
			}
		}

		checkScrollBarFlags();
	}

	void EditCtrl::onActivate()
	{
		m_active = true;
		m_caret.visible = m_active && isVisible();

		if ( auto caret = m_caret.overlay )
		{
			caret->setVisible( m_caret.visible );
		}

		doClearSelection();
	}

	void EditCtrl::onDeactivate()
	{
		m_active = false;
		m_isMouseSelecting = false;
		m_caret.visible = false;

		if ( auto caret = m_caret.overlay )
		{
			caret->setVisible( m_caret.visible );
		}
	}

	void EditCtrl::doOnMouseButtonDown( MouseEvent const & event )
	{
		doClearSelection();
		doUpdateCaretPosition( event.getPosition(), m_caret.indices );
		m_isMouseSelecting = true;
		doBeginSelection( m_caret.indices );
		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
	}

	void EditCtrl::doOnMouseMove( MouseEvent const & event )
	{
		if ( m_isMouseSelecting )
		{
			doUpdateCaretPosition( event.getPosition(), m_caret.indices );
			m_selection.end = m_caret.indices;
			doUpdateCaretIndices();
			doUpdateCaret();
			doUpdateSelection();
		}
	}

	void EditCtrl::doOnMouseButtonUp( MouseEvent const & event )
	{
		if ( m_isMouseSelecting )
		{
			m_isMouseSelecting = false;
		}
	}

	void EditCtrl::onChar( KeyboardEvent const & event )
	{
		auto code = event.getKey();

		if ( code >= KeyboardKey::eAsciiBegin
			 && code <= KeyboardKey::eAsciiEnd
			 && code != KeyboardKey::eDelete )
		{
			doAddCharAtCaret( event.getChar() );
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

			if ( code == KeyboardKey( 'A' ) && event.isCtrlDown() )
			{
				doSelectAllText();
			}
			else if ( code == KeyboardKey( 'C' ) && event.isCtrlDown() )
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

	void EditCtrl::onKeyUp( KeyboardEvent const & )
	{
	}

	void EditCtrl::doAddCharAtCaret( castor::String const & c )
	{
		auto diff = size_t( std::distance( m_caption.cbegin(), m_caret.indices.captionIt ) );
		m_caption = castor::U32String( m_caption.cbegin(), m_caret.indices.captionIt )
			+ castor::toUtf8U32String( c )
			+ castor::U32String( m_caret.indices.captionIt, m_caption.cend() );
		m_caret.indices.updateIndex( diff + 1, m_caption );
		doUpdateCaption();
		doUpdateCaret();
		doAdjustTextPosition();
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
	}

	void EditCtrl::doDeleteCharAtCaret( bool isCtrlDown )
	{
		if ( doDeleteSelection( isCtrlDown )
			|| isCtrlDown
			|| m_caret.indices.captionIt == m_caption.end() )
		{
			return;
		}

		auto diff = size_t( std::distance( m_caption.cbegin(), m_caret.indices.captionIt ) );
		castor::U32String caption( m_caption.cbegin(), m_caret.indices.captionIt );

		if ( auto it = m_caret.indices.captionIt;
			it != m_caption.end() )
		{
			++it;

			if ( it != m_caption.end() )
			{
				caption += castor::U32String( it, m_caption.cend() );
			}
		}

		m_caption = caption;
		m_caret.indices.updateIndex( diff, m_caption );
		doUpdateCaption();
		doUpdateCaret();
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
	}

	void EditCtrl::doDeleteCharBeforeCaret( bool isCtrlDown )
	{
		if ( doDeleteSelection( isCtrlDown )
			|| isCtrlDown
			|| m_caret.indices.captionIt == m_caption.begin() )
		{
			return;
		}

		m_caret.indices.updateIndex( m_caret.indices.captionIndex - 1u, m_caption );
		auto diff = size_t( std::distance( m_caption.cbegin(), m_caret.indices.captionIt ) );
		castor::U32String caption( m_caption.cbegin(), m_caret.indices.captionIt );

		if ( auto it = m_caret.indices.captionIt;
			it != m_caption.end() )
		{
			++it;

			if ( it != m_caption.end() )
			{
				caption += castor::U32String( it, m_caption.cend() );
			}
		}

		m_caption = caption;
		m_caret.indices.updateIndex( diff, m_caption );
		doUpdateCaption();
		doUpdateCaret();
		doAdjustTextPosition();
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
	}

	void EditCtrl::doMoveCaretLeft( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.indices.captionIt == m_caption.begin() )
		{
			return;
		}

		if ( isShiftDown && !hasSelection() )
		{
			doBeginSelection( m_caret.indices );
		}

		if ( isCtrlDown )
		{
			std::u32string_view caption{ m_caption.data(), m_caret.indices.captionIndex - 1u };
			auto rit = caption.find_last_of( U" \n\t" );

			if ( rit == castor::U32String::npos )
			{
				m_caret.indices.updateIndex( 0, m_caption );
			}
			else
			{
				m_caret.indices.updateIndex( rit, m_caption );
			}
		}
		else
		{
			m_caret.indices.decrementIndex();
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret.indices;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
		doAdjustTextPosition();
	}

	void EditCtrl::doMoveCaretRight( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.indices.captionIt == m_caption.end() )
		{
			return;
		}

		if ( isShiftDown && !hasSelection() )
		{
			doBeginSelection( m_caret.indices );
		}

		if ( isCtrlDown )
		{
			auto it = m_caption.find_first_of( U" \n\t", m_caret.indices.captionIndex + 1u );

			if ( it == castor::U32String::npos )
			{
				m_caret.indices.updateIndex( m_caption.size(), m_caption );
			}
			else
			{
				m_caret.indices.updateIndex( it + 1u, m_caption );
			}
		}
		else
		{
			m_caret.indices.incrementIndex();
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret.indices;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
		doAdjustTextPosition();
	}

	void EditCtrl::doMoveCaretUp( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( isCtrlDown
			|| m_caret.indices.captionIt == m_caption.begin() )
		{
			return;
		}

		if ( isShiftDown && !hasSelection() )
		{
			doBeginSelection( m_caret.indices );
		}

		if ( m_caret.indices.lineIndex > 0u )
		{
			auto const & line = m_metrics.lines[m_caret.indices.lineIndex - 1u];

			if ( line.chars.size() < m_caret.indices.charIndex )
			{
				m_caret.indices.charIndex = uint32_t( line.chars.size() );
			}

			if ( m_caret.indices.lineIndex == 1u )
			{
				--m_caret.indices.charIndex;
			}

			m_caret.indices.updateIndex( line.firstCharIndex + m_caret.indices.charIndex, m_caption );
		}
		else
		{
			m_caret.indices.updateIndex( 0u, m_caption );
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret.indices;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
		doAdjustTextPosition();
	}

	void EditCtrl::doMoveCaretDown( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( isCtrlDown
			|| m_caret.indices.captionIt == m_caption.end() )
		{
			return;
		}

		if ( isShiftDown && !hasSelection() )
		{
			doBeginSelection( m_caret.indices );
		}

		if ( m_caret.indices.lineIndex < m_metrics.lines.size() - 1u )
		{
			auto const & line = m_metrics.lines[m_caret.indices.lineIndex + 1u];

			if ( line.chars.size() < m_caret.indices.charIndex )
			{
				m_caret.indices.charIndex = uint32_t( line.chars.size() );
			}

			if ( m_caret.indices.lineIndex == 0u )
			{
				++m_caret.indices.charIndex;
			}

			m_caret.indices.updateIndex( line.firstCharIndex + m_caret.indices.charIndex, m_caption );
		}
		else
		{
			m_caret.indices.updateIndex( m_caption.size(), m_caption );
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret.indices;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
		doAdjustTextPosition();
	}

	void EditCtrl::doMoveCaretHome( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.indices.captionIt == m_caption.begin() )
		{
			return;
		}

		if ( isShiftDown && !hasSelection() )
		{
			doBeginSelection( m_caret.indices );
		}

		if ( isCtrlDown )
		{
			m_caret.indices.updateIndex( 0u, m_caption );
		}
		else if ( m_caret.indices.lineIndex > 0 )
		{
			m_caret.indices.updateIndex( m_metrics.lines[m_caret.indices.lineIndex].firstCharIndex + 1u
				, m_caption );
		}
		else
		{
			m_caret.indices.updateIndex( m_metrics.lines[m_caret.indices.lineIndex].firstCharIndex
				, m_caption );
		}

		if ( isShiftDown )
		{
			m_selection.end = m_caret.indices;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
		doAdjustTextPosition();
	}

	void EditCtrl::doMoveCaretEnd( bool isShiftDown
		, bool isCtrlDown )
	{
		if ( m_caret.indices.captionIt == m_caption.end() )
		{
			return;
		}

		if ( isShiftDown && !hasSelection() )
		{
			doBeginSelection( m_caret.indices );
		}

		if ( isCtrlDown )
		{
			m_caret.indices.updateIndex( m_caption.size(), m_caption );
		}
		else
		{
			auto const & line = m_metrics.lines[m_caret.indices.lineIndex];
			m_caret.indices.updateIndex( line.firstCharIndex + line.chars.size()
				, m_caption );
		}

		if ( isShiftDown )
		{
			if ( !hasSelection() )
			{
				doBeginSelection( m_caret.indices );
			}

			m_selection.end = m_caret.indices;
		}
		else
		{
			doClearSelection();
		}

		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
		doAdjustTextPosition();
	}

	void EditCtrl::doUpdateCaretPosition( castor::Position const & pos
		, CaretIndices & indices )
	{
		auto text = m_text;

		if ( !text )
		{
			return;
		}

		auto position = pos - ( getAbsolutePosition() + text->getPixelPosition() );
		uint32_t index{};
		auto lineIt = std::find_if( m_metrics.lines.begin()
			, m_metrics.lines.end()
			, [&position, &index]( castor::TextLineMetrics const & lookup )
			{
				auto height = uint32_t( lookup.yMax - lookup.yMin );
				auto result = uint32_t( lookup.top ) + height > uint32_t( position->y );

				if ( !result )
				{
					index += uint32_t( lookup.chars.size() );
				}

				return result;
			} );

		if ( lineIt == m_metrics.lines.end() )
		{
			indices.updateIndex( m_caption.size(), m_caption );
		}
		else
		{
			uint32_t previous{};
			auto it = std::find_if( lineIt->chars.begin()
				, lineIt->chars.end()
				, [&position, &index, &previous]( uint32_t lookup )
				{
					auto diff = lookup - previous;
					auto result = lookup - ( diff / 2u ) > uint32_t( position->x );

					if ( !result )
					{
						++index;
					}

					previous = lookup;
					return result;
				} );

			if ( it != lineIt->chars.end() )
			{
				indices.updateIndex( index, m_caption );
			}
			else
			{
				indices.updateIndex( lineIt->firstCharIndex + lineIt->chars.size(), m_caption );
			}
		}
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
		doUpdate( m_caret.indices );

		if ( hasSelection() )
		{
			doUpdate( m_selection.begin );
			doUpdate( m_selection.end );
		}
	}

	void EditCtrl::doUpdateCaret()
	{
		if ( auto text = m_text )
		{
			if ( auto caret = m_caret.overlay )
			{
				auto font = text->getFontTexture()->getFont();
				castor::Position position{};
				castor::Size size{ 1u, font->getHeight() };

				if ( !m_caption.empty() )
				{
					auto index = uint32_t( std::distance( m_caption.cbegin(), m_caret.indices.captionIt ) );
					auto lineIt = m_metrics.lines.begin();
					auto charIt = lineIt->chars.begin();
					float left{};
					float top{};
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
						position.x() += int32_t( left );
						position.y() += int32_t( top );
					}
				}

				caret->setPixelPosition( position );
				caret->setPixelSize( size );
			}
		}
	}

	void EditCtrl::doUpdateMetrics()
	{
		if ( auto text = m_text )
		{
			auto fontTexture = text->getFontTexture();
			auto font = fontTexture->getFont();
			castor::Vector< char32_t > newCaption;

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
				m_metrics = font->getTextMetrics( m_caption
					, getClientSize()->x
					, false );
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

		if ( auto text = m_text )
		{
			text->setCaption( m_caption );
			auto size = text->getPixelSize();

			if ( size->x != uint32_t( m_metrics.width )
				|| size->y != uint32_t( m_metrics.height ) )
			{
				size = { uint32_t( m_metrics.width ), uint32_t( m_metrics.height ) };
				text->setPixelSize( size );
				updateTotalSize( size );
			}
		}
	}

	void EditCtrl::doBeginSelection( CaretIndices const & begin )
	{
		m_hasSelection = true;
		m_selection.begin = begin;
		m_selection.end = m_selection.begin;
	}

	void EditCtrl::doUpdateSelection()
	{
		if ( !hasSelection() )
		{
			doClearSelection();
			return;
		}

		auto text = m_text;

		if ( !text )
		{
			return;
		}

		auto [selBegin, selEnd] = doGetNormalisedSelection();
		auto lineDiff = selEnd.lineIndex - selBegin.lineIndex;
		auto const & style = getStyle();

		while ( lineDiff >= m_selections.size() )
		{
			auto panel = getEngine().addNewOverlay( getName() + cuT( "/Selection" ) + castor::string::toString( m_selections.size() )
				, getEngine()
				, OverlayType::ePanel
				, nullptr
				, &text->getOverlay() )->getPanelOverlay();
			panel->setPixelPosition( {} );
			panel->setPixelSize( {} );
			panel->setMaterial( style.getSelectionMaterial() );
			panel->setVisible( false );
			panel->setOrder( m_panelsZIndex, uint32_t( m_selections.size() ) );
			m_selections.push_back( panel );
		}

		uint32_t selLineIndex{};

		if ( lineDiff > 0 )
		{
			// Multiline selection.
			// Process first and last lines separately (since they may not be fully selected).
			auto panel = m_selections[selLineIndex];
			++selLineIndex;

			if ( panel )
			{
				// First line
				auto & line = m_metrics.lines[selBegin.lineIndex];
				++selBegin.lineIndex;
				auto left = ( selBegin.charIndex == 0u
					? 0
					: int32_t( line.chars[selBegin.charIndex - 1u] ) );
				panel->setPixelPosition( { left, int32_t( line.top ) } );
				panel->setPixelSize( { LineEndWidth + uint32_t( line.chars.back() ) - uint32_t( left )
					, uint32_t( m_metrics.yMax - m_metrics.yMin ) } );
				panel->setVisible( true );
			}

			panel = m_selections[selLineIndex];
			++selLineIndex;

			if ( panel )
			{
				// LastLine
				auto & line = m_metrics.lines[selEnd.lineIndex];
				--selEnd.lineIndex;
				auto right = int32_t( line.chars[selEnd.charIndex - 1u] );

				panel->setPixelPosition( { 0u, int32_t( line.top ) } );
				panel->setPixelSize( { uint32_t( right )
					, uint32_t( m_metrics.yMax - m_metrics.yMin ) } );
				panel->setVisible( true );
			}

			// Process remaining lines
			while ( selBegin.lineIndex <= selEnd.lineIndex )
			{
				panel = m_selections[selLineIndex];
				++selLineIndex;

				if ( panel )
				{
					auto & line = m_metrics.lines[selBegin.lineIndex];
					panel->setPixelPosition( { 0, int32_t( line.top ) } );
					panel->setPixelSize( { LineEndWidth + uint32_t( line.chars.back() )
						, uint32_t( m_metrics.yMax - m_metrics.yMin ) } );
					panel->setVisible( true );
				}

				++selBegin.lineIndex;
			}
		}
		else
		{
			auto panel = m_selections[selLineIndex];
			++selLineIndex;

			if ( panel )
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
		}

		for ( auto sel : castor::makeArrayView( m_selections.begin() + selLineIndex, m_selections.end() ) )
		{
			if ( auto panel = sel )
			{
				panel->setVisible( false );
			}
		}
	}

	void EditCtrl::doClearSelection()
	{
		m_hasSelection = false;
		m_selection.begin = {};
		m_selection.end = {};

		for ( auto sel : m_selections )
		{
			if ( auto panel = sel )
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
			castor::swap( selBegin, selEnd );
		}

		return { selBegin, selEnd };
	}

	bool EditCtrl::doDeleteSelection( bool isCtrlDown )
	{
		if ( !hasSelection()
			|| isCtrlDown
			|| ( m_selection.begin.lineIndex == m_selection.end.lineIndex
				&& m_selection.begin.charIndex == m_selection.end.charIndex ) )
		{
			return false;
		}

		auto [selBegin, selEnd] = doGetNormalisedSelection();
		m_caption.erase( m_caption.begin() + selBegin.captionIndex
			, m_caption.begin() + selEnd.captionIndex );
		m_caret.indices.updateIndex( selBegin.captionIndex, m_caption );
		doUpdateCaption();
		doUpdateCaret();
		m_signals[size_t( EditEvent::eUpdated )]( m_caption );
		return true;
	}

	void EditCtrl::doSelectAllText()
	{
		m_caret.indices.updateIndex( 0u, m_caption );
		doBeginSelection( m_caret.indices );
		m_caret.indices.updateIndex( m_caption.size(), m_caption );
		m_selection.end = m_caret.indices;
		doUpdateCaretIndices();
		doUpdateCaret();
		doUpdateSelection();
		doAdjustTextPosition();
	}

	void EditCtrl::doCopyText()
	{
		if ( !hasSelection()
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
		if ( !hasSelection()
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

		if ( !hasSelection()
			|| ( m_selection.begin.lineIndex == m_selection.end.lineIndex
				&& m_selection.begin.charIndex == m_selection.end.charIndex ) )
		{
			m_caption.insert( m_caret.indices.captionIt
				, clipboard.begin()
				, clipboard.end() );
			m_caret.indices.updateIndex( m_caret.indices.captionIndex + clipboard.size(), m_caption );
		}
		else
		{
			auto [selBegin, selEnd] = doGetNormalisedSelection();
			m_caption.replace( m_caption.begin() + selBegin.captionIndex
				, m_caption.begin() + selEnd.captionIndex
				, clipboard.begin()
				, clipboard.end() );
			m_caret.indices.updateIndex( selBegin.captionIndex + clipboard.size()
				, m_caption );
		}

		doUpdateCaption();
		doUpdateCaret();
	}

	void EditCtrl::doAdjustTextPosition()
	{
		if ( auto text = m_text )
		{
			if ( auto caret = m_caret.overlay )
			{
				auto clientSize = getClientSize();
				auto clientOffset = getClientOffset();
				auto caretPos = caret->getPixelPosition() + castor::Point2i{};
				auto caretSize = caret->getPixelSize();
				auto position = text->getPixelPosition() - clientOffset;
				auto caretTextPos = caretPos + position;
				bool isLVisible = caretTextPos->x >= 0;
				bool isRVisible = caretTextPos->x + int32_t( caretSize->x ) < int32_t( clientSize->x );
				bool isTVisible = caretTextPos->y >= 0;
				bool isBVisible = caretTextPos->y + int32_t( caretSize->y ) < int32_t( clientSize->y );

				if ( isLVisible && isRVisible && isTVisible && isBVisible )
				{
					// Caret is still fully visible, nothing to do.
					return;
				}

				if ( !isLVisible || !isRVisible )
				{
					if ( caretTextPos->x <= 0 )
					{
						position->x -= caretTextPos->x;
					}
					else if ( int32_t( caretSize->x ) + caretPos->x >= int32_t( clientSize->x ) )
					{
						position->x = int32_t( clientSize->x ) - ( int32_t( caretSize->x ) + caretPos->x );
					}
					else if ( caretPos->x <= 0 )
					{
						position->x = caretPos->x;
					}
					else
					{
						position->x = 0;
					}
				}

				if ( !isTVisible || !isBVisible )
				{
					if ( caretTextPos->y <= 0 )
					{
						position->y -= caretTextPos->y;
					}
					else if ( int32_t( caretSize->y ) + caretPos->y >= int32_t( clientSize->y ) )
					{
						position->y = int32_t( clientSize->y ) - ( int32_t( caretSize->y ) + caretPos->y );
					}
					else if ( caretPos->y <= 0 )
					{
						position->y = caretPos->y;
					}
					else
					{
						position->y = 0;
					}
				}

				position += clientOffset;
				text->setPixelPosition( { position->x, position->y } );
				updateScrollBarsThumb( { position->x, position->y } );
			}
		}
	}

	void EditCtrl::doScrollContent( castor::Position const & position )
	{
		if ( auto text = m_text )
		{
			auto pos = position + getClientOffset();
			text->setPixelPosition( { pos->x, pos->y } );
		}
	}
}
