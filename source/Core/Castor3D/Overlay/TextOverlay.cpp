#include "Castor3D/Overlay/TextOverlay.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/Image.hpp>

using namespace castor;

#if defined( drawText )
#	undef drawText
#endif

namespace castor3d
{
	TextOverlay::TextWriter::TextWriter( String const & p_tabs, TextOverlay const * p_category )
		: OverlayCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool TextOverlay::TextWriter::operator()( TextOverlay const & p_overlay, TextFile & p_file )
	{
		static String const TexturingModes[]
		{
			cuT( "letter" ),
			cuT( "text" ),
		};

		static String const LineSpacingModes[]
		{
			cuT( "own_height" ),
			cuT( "max_lines_height" ),
			cuT( "max_font_height" ),
		};

		static String const TextWrappingModes[]
		{
			cuT( "none" ),
			cuT( "break" ),
			cuT( "break_words" ),
		};

		static String const VerticalAligns[]
		{
			cuT( "top" ),
			cuT( "center" ),
			cuT( "bottom" ),
		};

		static String const HorizontalAligns[]
		{
			cuT( "left" ),
			cuT( "center" ),
			cuT( "right" ),
		};

		Logger::logInfo( m_tabs + cuT( "Writing TextOverlay " ) + p_overlay.getOverlayName() );
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "text_overlay \"" ) + p_overlay.getOverlayName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		OverlayCategory::TextWriter::checkError( result, "TextOverlay name" );

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tfont \"" ) + p_overlay.getFontTexture()->getFontName() + cuT( "\"\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay font" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttext \"" ) + p_overlay.getCaption() + cuT( "\"\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttext_wrapping " ) + TextWrappingModes[size_t( p_overlay.getTextWrappingMode() )] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text wrapping" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tvertical_align " ) + VerticalAligns[size_t( p_overlay.getVAlign() )] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text vertical align" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\thorizontal_align " ) + HorizontalAligns[size_t( p_overlay.getHAlign() )] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text horizontal align" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttexturing_mode " ) + TexturingModes[size_t( p_overlay.getTexturingMode() )] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text texturing mode" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tline_spacing_mode " ) + LineSpacingModes[size_t( p_overlay.getLineSpacingMode() )] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay line spacing mode" );
		}

		if ( result )
		{
			result = OverlayCategory::TextWriter{ m_tabs }( p_overlay, p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	bool TextOverlay::TextWriter::writeInto( castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	TextOverlay::TextOverlay()
		: OverlayCategory{ OverlayType::eText }
	{
	}

	TextOverlay::~TextOverlay()
	{
	}

	OverlayCategorySPtr TextOverlay::create()
	{
		return std::make_shared< TextOverlay >();
	}

	void TextOverlay::accept( OverlayVisitor & visitor )const
	{
		visitor.visit( *this );
	}

	void TextOverlay::setFont( String const & p_strFont )
	{
		// Récupération / Création de la police
		Engine * engine = m_pOverlay->getEngine();
		auto & fontCache = engine->getFontCache();
		FontSPtr pFont = fontCache.find( p_strFont );

		if ( pFont )
		{
			FontTextureSPtr fontTexture = engine->getOverlayCache().getFontTexture( pFont->getName() );

			if ( !fontTexture )
			{
				fontTexture = engine->getOverlayCache().createFontTexture( pFont );
				fontTexture->update();
			}

			m_connection.disconnect();
			m_fontTexture = fontTexture;
			m_connection = fontTexture->onChanged.connect( [this]( FontTexture const & p_texture )
			{
				m_textChanged = true;
			} );
		}
		else
		{
			CU_Exception( "Font " + string::stringCast< char >( p_strFont ) + "not found" );
		}

		m_textChanged = true;
	}

	void TextOverlay::doUpdate( OverlayRenderer const & renderer )
	{
		m_fontChanged = false;
		FontTextureSPtr fontTexture = getFontTexture();

		if ( !fontTexture )
		{
			setVisible( false );
			CU_Exception( cuT( "The TextOverlay [" ) + getOverlayName() + cuT( "] has no FontTexture. Did you set its font?" ) );
		}

		FontSPtr font = fontTexture->getFont();
		std::vector< char32_t > newCaption;

		for ( string::utf8::iterator it{ m_currentCaption.begin() }; it != m_currentCaption.end(); ++it )
		{
			if ( !font->hasGlyphAt( *it ) )
			{
				newCaption.push_back( *it );
			}
		}

		if ( !newCaption.empty() )
		{
			for ( auto c : newCaption )
			{
				font->loadGlyph( c );
			}

			fontTexture->update();

			getOverlay().getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [this, fontTexture]()
				{
					m_fontChanged = true;
					fontTexture->cleanup();
					fontTexture->initialise();
				} ) );
		}
	}

	void TextOverlay::doUpdateBuffer( Size const & p_size
		, std::function< void( Point2d const & p_size
			, Rectangle const & p_absolute
			, Point4r const & p_fontUV
			, real & p_uvLeft
			, real & p_uvTop
			, real & p_uvRight
			, real & p_uvBottom ) > p_generateUvs )
	{
		FontTextureSPtr fontTexture = getFontTexture();

		if ( fontTexture )
		{
			FontSPtr font = fontTexture->getFont();

			if ( !m_currentCaption.empty() && font )
			{
				if ( m_textChanged )
				{
					Point2d const ovAbsSize = getOverlay().getAbsoluteSize();
					Point2d const size( p_size.getWidth() * ovAbsSize[0], p_size.getHeight() * ovAbsSize[1] );
					m_previousCaption = m_currentCaption;
					m_arrayVtx.clear();
					m_arrayVtx.reserve( m_previousCaption.size() * 6 );

					DisplayableLineArray lines = doPrepareText( p_size, size );
					Size const & texDim = { fontTexture->getTexture()->getDimensions().width, fontTexture->getTexture()->getDimensions().height };

					for ( auto const & line : lines )
					{
						double const height = line.m_height;
						double topCrop = std::max( 0.0, -line.m_position[1] );
						double bottomCrop = std::max( 0.0, line.m_position[1] + height - size[1] );

						if ( topCrop + bottomCrop <= height )
						{
							for ( auto const & c : line.m_characters )
							{
								topCrop = std::max( 0.0, -line.m_position[1] - ( height - c.m_glyph.getBearing().y() ) );
								bottomCrop = std::max( 0.0, line.m_position[1] + ( height - c.m_position[1] ) + c.m_size[1] - size[1] );

								double const leftUncropped = c.m_position[0] + line.m_position[0];
								double const leftCrop = std::max( 0.0, -leftUncropped );
								double const rightCrop = std::max( 0.0, leftUncropped + c.m_size[0] - size[0] );

								if ( leftCrop + rightCrop < c.m_size[0] )
								{
									//
									// Compute Letter's Position.
									//
									double const topUncropped = line.m_position[1] + height - c.m_position[1];
									int32_t const left = std::max( 0, int32_t( leftUncropped + leftCrop ) );
									int32_t const top = std::max( 0, int32_t( topUncropped + topCrop ) );
									int32_t const right = int32_t( std::min( leftUncropped + c.m_size[0] - rightCrop, size[0] ) );
									int32_t const bottom = int32_t( std::min( topUncropped + c.m_size[1] - bottomCrop, size[1] ) );

									//
									// Compute Letter's Font UV.
									//
									double const fontUvTopCrop = topCrop / texDim.getHeight();
									double const fontUvBottomCrop = bottomCrop / texDim.getHeight();
									Position const fontUvPosition = fontTexture->getGlyphPosition( c.m_glyph.getCharacter() );
									double const fontUvLeftCrop = leftCrop / texDim.getHeight();
									double const fontUvRightCrop = rightCrop / texDim.getHeight();
									double const fontUvLeftUncropped = double( fontUvPosition.x() ) / texDim.getWidth();
									double const fontUvTopUncropped = double( fontUvPosition.y() ) / texDim.getHeight();
									real const fontUvLeft = real( fontUvLeftUncropped + fontUvLeftCrop );
									real const fontUvRight = real( fontUvLeftUncropped + ( c.m_size[0] / texDim.getWidth() ) - fontUvRightCrop );
									// The UV is vertically inverted since the image is top-bottom, and the overlay is drawn bottom-up.
									real const fontUvBottom = real( fontUvTopUncropped + fontUvBottomCrop );
									real const fontUvTop = real( fontUvTopUncropped + ( c.m_size[1] / texDim.getHeight() ) - fontUvTopCrop );

									//
									// Compute Letter's Texture UV.
									//
									real texUvLeft{};
									real texUvRight{};
									real texUvBottom{};
									real texUvTop{};
									p_generateUvs( size,
										castor::Rectangle{ left, top, right, bottom },
										Point4r{ fontUvLeft, fontUvTop, fontUvRight, fontUvBottom },
										texUvLeft, texUvTop, texUvRight, texUvBottom );


									//
									// Fill buffer
									//
									TextOverlay::Vertex const vertexTR = { Point2f{ float( right ) / p_size.getWidth(), float( top ) / p_size.getHeight() },    Point2f{ fontUvRight, fontUvTop },    Point2f{ texUvRight, texUvTop } };
									TextOverlay::Vertex const vertexTL = { Point2f{ float( left ) / p_size.getWidth(),  float( top ) / p_size.getHeight() },    Point2f{ fontUvLeft,  fontUvTop },    Point2f{ texUvLeft,  texUvTop } };
									TextOverlay::Vertex const vertexBL = { Point2f{ float( left ) / p_size.getWidth(),  float( bottom ) / p_size.getHeight() }, Point2f{ fontUvLeft,  fontUvBottom }, Point2f{ texUvLeft,  texUvBottom } };
									TextOverlay::Vertex const vertexBR = { Point2f{ float( right ) / p_size.getWidth(), float( bottom ) / p_size.getHeight() }, Point2f{ fontUvRight, fontUvBottom }, Point2f{ texUvRight, texUvBottom } };

									m_arrayVtx.push_back( vertexBL );
									m_arrayVtx.push_back( vertexBR );
									m_arrayVtx.push_back( vertexTL );

									m_arrayVtx.push_back( vertexTR );
									m_arrayVtx.push_back( vertexTL );
									m_arrayVtx.push_back( vertexBR );
								}
							}
						}
					}

					m_textChanged = false;
				}
			}
		}
	}

	void TextOverlay::doUpdateBuffer( Size const & p_size )
	{
		switch ( m_texturingMode )
		{
		case TextTexturingMode::eLetter:
			doUpdateBuffer( p_size
				, [this]( Point2d const & p_size
					, Rectangle const & p_absolute
					, Point4r const & p_fontUV
					, real & p_uvLeft
					, real & p_uvTop
					, real & p_uvRight
					, real & p_uvBottom )
				{
					p_uvLeft = 0.0_r;
					p_uvTop = 0.0_r;
					p_uvRight = 1.0_r;
					p_uvBottom = 1.0_r;
				} );
			break;

		case TextTexturingMode::eText:
			doUpdateBuffer( p_size
				, [this]( Point2d const & p_size
					, Rectangle const & p_absolute
					, Point4r const & p_fontUV
					, real & p_uvLeft
					, real & p_uvTop
					, real & p_uvRight
					, real & p_uvBottom )
			{
				p_uvLeft = real( p_absolute[0] / p_size[0] );
				p_uvTop = real( p_absolute[1] / p_size[1] );
				p_uvRight = real( p_absolute[2] / p_size[0] );
				p_uvBottom = real( p_absolute[3] / p_size[1] );
			} );
			break;
		}
	}

	TextOverlay::DisplayableLineArray TextOverlay::doPrepareText( Size const & p_renderSize, Point2d const & p_size )
	{
		FontTextureSPtr fontTexture = getFontTexture();
		FontSPtr font = fontTexture->getFont();
		StringArray lines = string::split( m_previousCaption, cuT( "\n" ), uint32_t( std::count( m_previousCaption.begin(), m_previousCaption.end(), cuT( '\n' ) ) + 1 ), true );
		DisplayableLineArray result;
		DisplayableLine line;

		for ( auto const & lineText : lines )
		{
			double left = 0;
			double wordWidth = 0;
			std::u32string word;

			for ( string::utf8::const_iterator itLine{ lineText.begin() }; itLine != lineText.end(); ++itLine )
			{
				Glyph const & glyph{ font->getGlyphAt( *itLine ) };
				DisplayableChar character{ Point2d{}, Point2d{ glyph.getAdvance(), glyph.getSize().getHeight() }, glyph };

				if ( glyph.getCharacter() == cuT( ' ' )
						|| glyph.getCharacter() == cuT( '\t' ) )
				{
					// write the word and leave space before next word.
					doPrepareWord( p_renderSize, word, wordWidth, p_size, left, line, result );
					word.clear();
					wordWidth = 0;
					left += character.m_size[0];
				}
				else
				{
					word += glyph.getCharacter();
					wordWidth += character.m_size[0];
				}
			}

			if ( !word.empty() )
			{
				doPrepareWord( p_renderSize, word, wordWidth, p_size, left, line, result );
			}

			line = doFinishLine( p_size, line, left, result );
		}

		doAlignVertically( p_size[1], result );
		return result;
	}

	void TextOverlay::doPrepareWord( Size const & p_renderSize, std::u32string const & p_word, double p_wordWidth, Point2d const & p_size, double & p_left, DisplayableLine & p_line, DisplayableLineArray & p_lines )
	{
		auto fontTexture = getFontTexture();
		auto const & font = *fontTexture->getFont();
		auto ovPosition = getAbsolutePosition( p_renderSize );

		if ( p_left + p_wordWidth > p_size[0] && m_wrappingMode == TextWrappingMode::eBreakWords )
		{
			// The word will overflow the overlay size, so we jump to the next line,
			// and will write the word on this next line.
			p_line = doFinishLine( p_size, p_line, p_left, p_lines );
		}

		for ( auto character : p_word )
		{
			Glyph const & glyph = font[character];
			Point2d charSize( double( glyph.getAdvance() ), double( glyph.getSize().getHeight() ) );

			p_left += glyph.getBearing().x();

			if ( p_left > p_size[0] )
			{
				// The character is completely out of the overlay.
				if ( m_wrappingMode == TextWrappingMode::eNone )
				{
					// No wrapping => ignore the character.
					charSize[0] = 0;
				}
				else if ( m_wrappingMode == TextWrappingMode::eBreak )
				{
					// Break => Jump to the next line.
					p_line = doFinishLine( p_size, p_line, p_left, p_lines );
				}
			}
			else if ( p_left + charSize[0] > p_size[0] )
			{
				// The character is partially out of the overlay.
				if ( m_wrappingMode == TextWrappingMode::eBreak )
				{
					// Break => Jump to the next line.
					p_line = doFinishLine( p_size, p_line, p_left, p_lines );
				}
			}

			if ( charSize[0] > 0 )
			{
				p_line.m_characters.emplace_back( Point2d{ p_left, 0.0 }, charSize, glyph );
			}

			p_left += charSize[0];
		}
	}

	TextOverlay::DisplayableLine TextOverlay::doFinishLine( Point2d const & p_size, DisplayableLine p_line, double & p_left, DisplayableLineArray & p_lines )
	{
		p_line.m_height = 0.0;
		double maxBottomBearing{ 0.0 };

		for ( auto const & character : p_line.m_characters )
		{
			auto bottomBearing = std::max( 0.0, double( character.m_glyph.getSize().getHeight() ) - character.m_glyph.getBearing().y() );
			p_line.m_height = std::max( p_line.m_height, character.m_size[1] + bottomBearing );
			maxBottomBearing = std::max( maxBottomBearing, bottomBearing );
		}

		for ( auto & character : p_line.m_characters )
		{
			character.m_position[1] = std::max( 0.0, maxBottomBearing + character.m_glyph.getBearing().y() );
		}

		p_line.m_width = p_left;
		p_left = 0;
		doAlignHorizontally( p_size[0], p_line, p_lines );
		return DisplayableLine{ Point2d{ 0, p_line.m_position[1] + p_line.m_height }, 0.0, 0.0 };
	}

	void TextOverlay::doAlignHorizontally( double p_width, DisplayableLine p_line, DisplayableLineArray & p_lines )
	{
		// Move letters according to halign
		if ( m_hAlign != HAlign::eLeft )
		{
			double offset = p_width - p_line.m_width;

			if ( m_hAlign == HAlign::eCenter )
			{
				offset /= 2;
			}

			for ( auto & character : p_line.m_characters )
			{
				character.m_position[0] += offset;
			}
		}

		// Remove letters overflowing the maximum width
		auto removed = std::remove_if( p_line.m_characters.begin(), p_line.m_characters.end(), [&p_width]( DisplayableChar const & p_char )
		{
			return p_char.m_position[0] > p_width
				   || p_char.m_position[0] + p_char.m_size[0] < 0;
		} );

		if ( !p_line.m_characters.empty() )
		{
			p_line.m_characters.erase( removed, p_line.m_characters.end() );

			// add the line to the lines array.
			if ( !p_line.m_characters.empty() )
			{
				p_lines.push_back( p_line );
			}
		}
		else
		{
			p_lines.push_back( p_line );
		}
	}

	void TextOverlay::doAlignVertically( double p_height, DisplayableLineArray & p_lines )
	{
		// Compute each line height, according to lines spacing mode
		if ( m_lineSpacingMode != TextLineSpacingMode::eOwnHeight )
		{
			double height{ 0.0 };

			if ( m_lineSpacingMode == TextLineSpacingMode::eMaxLineHeight )
			{
				for ( auto const & line : p_lines )
				{
					height = std::max( line.m_height, height );
				}
			}
			else if ( m_lineSpacingMode == TextLineSpacingMode::eMaxFontHeight )
			{
				height = double( getFontTexture()->getFont()->getMaxHeight() );
			}

			double offset{ 0.0 };

			for ( auto & line : p_lines )
			{
				line.m_position[1] += offset;
				offset += height - line.m_height;
				line.m_height = height;
			}
		}

		double linesHeight{ 0.0 };

		for ( auto const & line : p_lines )
		{
			linesHeight += line.m_height;
		}

		// Move lines according to valign
		if ( m_vAlign != VAlign::eTop )
		{
			double offset = p_height - linesHeight;

			if ( m_vAlign == VAlign::eCenter )
			{
				offset /= 2;
			}

			for ( auto & line : p_lines )
			{
				line.m_position[1] += offset;
			}
		}

		// Remove lines overflowing the maximum height
		auto removed = std::remove_if( p_lines.begin(), p_lines.end(), [&p_height]( DisplayableLine const & p_line )
		{
			return p_line.m_position[1] > p_height
				   || p_line.m_position[1] + p_line.m_height < 0;
		} );

		p_lines.erase( removed, p_lines.end() );
	}
}
