#include "Castor3D/Overlay/TextOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/OverlayRenderer.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>

#if defined( drawText )
#	undef drawText
#endif

namespace castor3d
{
	TextOverlay::TextWriter::TextWriter( castor::String const & p_tabs, TextOverlay const * p_category )
		: OverlayCategory::TextWriter{ p_tabs }
		, m_category{ p_category }
	{
	}

	bool TextOverlay::TextWriter::operator()( TextOverlay const & p_overlay, castor::TextFile & p_file )
	{
		log::info << m_tabs << cuT( "Writing TextOverlay " ) << p_overlay.getOverlayName() << std::endl;
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
			result = p_file.writeText( m_tabs + cuT( "\ttext_wrapping " ) + castor3d::getName( p_overlay.getTextWrappingMode() ) + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text wrapping" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tvertical_align " ) + castor3d::getName( p_overlay.getVAlign() ) + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text vertical align" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\thorizontal_align " ) + castor3d::getName( p_overlay.getHAlign() ) + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text horizontal align" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\ttexturing_mode " ) + castor3d::getName( p_overlay.getTexturingMode() ) + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::checkError( result, "TextOverlay text texturing mode" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tline_spacing_mode " ) + castor3d::getName( p_overlay.getLineSpacingMode() ) + cuT( "\n" ) ) > 0;
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

	void TextOverlay::setFont( castor::String const & p_strFont )
	{
		// Récupération / Création de la police
		Engine * engine = m_pOverlay->getEngine();
		auto & fontCache = engine->getFontCache();
		castor::FontSPtr pFont = fontCache.find( p_strFont );

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
			CU_Exception( "Font " + castor::string::stringCast< char >( p_strFont ) + "not found" );
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

		castor::FontSPtr font = fontTexture->getFont();
		std::vector< char32_t > newCaption;

		for ( castor::string::utf8::iterator it{ m_currentCaption.begin() }; it != m_currentCaption.end(); ++it )
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

	void TextOverlay::doUpdateBuffer( castor::Size const & p_size
		, std::function< void( castor::Point2d const & p_size
			, castor::Rectangle const & p_absolute
			, castor::Point4f const & p_fontUV
			, float & p_uvLeft
			, float & p_uvTop
			, float & p_uvRight
			, float & p_uvBottom ) > p_generateUvs )
	{
		FontTextureSPtr fontTexture = getFontTexture();

		if ( fontTexture )
		{
			castor::FontSPtr font = fontTexture->getFont();

			if ( !m_currentCaption.empty() && font )
			{
				if ( m_textChanged )
				{
					castor::Point2d const ovAbsSize = getOverlay().getAbsoluteSize();
					castor::Point2d const size( p_size.getWidth() * ovAbsSize[0], p_size.getHeight() * ovAbsSize[1] );
					m_previousCaption = m_currentCaption;
					m_arrayVtx.clear();
					m_arrayVtx.reserve( m_previousCaption.size() * 6 );

					DisplayableLineArray lines = doPrepareText( p_size, size );
					castor::Size const & texDim = { fontTexture->getTexture()->getDimensions().width, fontTexture->getTexture()->getDimensions().height };

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
									castor::Position const fontUvPosition = fontTexture->getGlyphPosition( c.m_glyph.getCharacter() );
									double const fontUvLeftCrop = leftCrop / texDim.getHeight();
									double const fontUvRightCrop = rightCrop / texDim.getHeight();
									double const fontUvLeftUncropped = double( fontUvPosition.x() ) / texDim.getWidth();
									double const fontUvTopUncropped = double( fontUvPosition.y() ) / texDim.getHeight();
									float const fontUvLeft = float( fontUvLeftUncropped + fontUvLeftCrop );
									float const fontUvRight = float( fontUvLeftUncropped + ( c.m_size[0] / texDim.getWidth() ) - fontUvRightCrop );
									// The UV is vertically inverted since the image is top-bottom, and the overlay is drawn bottom-up.
									float const fontUvBottom = float( fontUvTopUncropped + fontUvBottomCrop );
									float const fontUvTop = float( fontUvTopUncropped + ( c.m_size[1] / texDim.getHeight() ) - fontUvTopCrop );

									//
									// Compute Letter's Texture UV.
									//
									float texUvLeft{};
									float texUvRight{};
									float texUvBottom{};
									float texUvTop{};
									p_generateUvs( size,
										castor::Rectangle{ left, top, right, bottom },
										castor::Point4f{ fontUvLeft, fontUvTop, fontUvRight, fontUvBottom },
										texUvLeft, texUvTop, texUvRight, texUvBottom );

									//
									// Fill buffer
									//
									TextOverlay::Vertex const vertexTR = { { float( right ) / p_size.getWidth(), float( top ) / p_size.getHeight() },    { fontUvRight, fontUvTop },    { texUvRight, texUvTop } };
									TextOverlay::Vertex const vertexTL = { { float( left ) / p_size.getWidth(),  float( top ) / p_size.getHeight() },    { fontUvLeft,  fontUvTop },    { texUvLeft,  texUvTop } };
									TextOverlay::Vertex const vertexBL = { { float( left ) / p_size.getWidth(),  float( bottom ) / p_size.getHeight() }, { fontUvLeft,  fontUvBottom }, { texUvLeft,  texUvBottom } };
									TextOverlay::Vertex const vertexBR = { { float( right ) / p_size.getWidth(), float( bottom ) / p_size.getHeight() }, { fontUvRight, fontUvBottom }, { texUvRight, texUvBottom } };

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

	void TextOverlay::doUpdateBuffer( castor::Size const & size )
	{
		switch ( m_texturingMode )
		{
		case TextTexturingMode::eLetter:
			doUpdateBuffer( size
				, [this]( castor::Point2d const & size
					, castor::Rectangle const & absolute
					, castor::Point4f const & fontUV
					, float & uvLeft
					, float & uvTop
					, float & uvRight
					, float & uvBottom )
				{
					uvLeft = 0.0f;
					uvTop = 0.0f;
					uvRight = 1.0f;
					uvBottom = 1.0f;
				} );
			break;

		case TextTexturingMode::eText:
			doUpdateBuffer( size
				, [this]( castor::Point2d const & size
					, castor::Rectangle const & absolute
					, castor::Point4f const & fontUV
					, float & uvLeft
					, float & uvTop
					, float & uvRight
					, float & uvBottom )
			{
				uvLeft = float( absolute[0] / size[0] );
				uvTop = float( absolute[1] / size[1] );
				uvRight = float( absolute[2] / size[0] );
				uvBottom = float( absolute[3] / size[1] );
			} );
			break;

		default:
			break;
		}
	}

	TextOverlay::DisplayableLineArray TextOverlay::doPrepareText( castor::Size const & p_renderSize, castor::Point2d const & p_size )
	{
		FontTextureSPtr fontTexture = getFontTexture();
		castor::FontSPtr font = fontTexture->getFont();
		castor::StringArray lines = castor::string::split( m_previousCaption
			, cuT( "\n" )
			, uint32_t( std::count( m_previousCaption.begin(), m_previousCaption.end(), cuT( '\n' ) ) + 1 )
			, true );
		DisplayableLineArray result;
		DisplayableLine line;

		for ( auto const & lineText : lines )
		{
			double left = 0;
			double wordWidth = 0;
			std::u32string word;

			for ( castor::string::utf8::const_iterator itLine{ lineText.begin() }; itLine != lineText.end(); ++itLine )
			{
				castor::Glyph const & glyph{ font->getGlyphAt( *itLine ) };
				DisplayableChar character{ castor::Point2d{}, castor::Point2d{ glyph.getAdvance(), glyph.getSize().getHeight() }, glyph };

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

	void TextOverlay::doPrepareWord( castor::Size const & p_renderSize, std::u32string const & p_word, double p_wordWidth, castor::Point2d const & p_size, double & p_left, DisplayableLine & p_line, DisplayableLineArray & p_lines )
	{
		auto fontTexture = getFontTexture();
		auto const & font = *fontTexture->getFont();

		if ( p_left + p_wordWidth > p_size[0] && m_wrappingMode == TextWrappingMode::eBreakWords )
		{
			// The word will overflow the overlay size, so we jump to the next line,
			// and will write the word on this next line.
			p_line = doFinishLine( p_size, p_line, p_left, p_lines );
		}

		for ( auto character : p_word )
		{
			castor::Glyph const & glyph = font[character];
			castor::Point2d charSize( double( glyph.getAdvance() ), double( glyph.getSize().getHeight() ) );

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
				p_line.m_characters.emplace_back( castor::Point2d{ p_left, 0.0 }, charSize, glyph );
			}

			p_left += charSize[0];
		}
	}

	TextOverlay::DisplayableLine TextOverlay::doFinishLine( castor::Point2d const & p_size, DisplayableLine p_line, double & p_left, DisplayableLineArray & p_lines )
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
		return DisplayableLine{ castor::Point2d{ 0, p_line.m_position[1] + p_line.m_height }, 0.0, 0.0 };
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
