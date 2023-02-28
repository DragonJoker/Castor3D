#include "Castor3D/Overlay/TextOverlay.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/Rectangle.hpp>

CU_ImplementCUSmartPtr( castor3d, TextOverlay )

#if defined( drawText )
#	undef drawText
#endif

namespace castor3d
{
	namespace ovrltxt
	{
		static castor::Font & getFont( TextOverlay const & overlay )
		{
			FontTextureSPtr fontTexture = overlay.getFontTexture();

			if ( !fontTexture )
			{
				CU_Failure( cuT( "The TextOverlay has no FontTexture. Did you set its font?" ) );
				CU_Exception( cuT( "The TextOverlay [" ) + overlay.getOverlayName() + cuT( "] has no FontTexture. Did you set its font?" ) );
			}

			auto pfont = fontTexture->getFont();

			if ( !pfont )
			{
				CU_Failure( cuT( "The TextOverlay has no Font. Did you set its font?" ) );
				CU_Exception( cuT( "The TextOverlay [" ) + overlay.getOverlayName() + cuT( "] has no Font. Did you set its font?" ) );
			}

			return *pfont;
		}
	}

	TextOverlay::TextOverlay()
		: OverlayCategory{ OverlayType::eText }
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

	void TextOverlay::setFont( castor::String const & name )
	{
		// Récupération / Création de la police
		Engine * engine = m_pOverlay->getEngine();
		auto & fontCache = engine->getFontCache();

		if ( auto font = fontCache.find( name ).lock() )
		{
			FontTextureSPtr fontTexture = engine->getOverlayCache().getFontTexture( font->getName() );

			if ( !fontTexture )
			{
				fontTexture = engine->getOverlayCache().createFontTexture( font );
				fontTexture->update( false );
			}

			m_connection.disconnect();
			m_fontTexture = fontTexture;
			m_connection = fontTexture->onChanged.connect( [this]( DoubleBufferedTextureLayout const & )
			{
				m_textChanged = true;
			} );
		}
		else
		{
			CU_Failure( cuT( "Font not found" ) );
			CU_Exception( "Font " + castor::string::stringCast< char >( name ) + "not found" );
		}

		m_textChanged = true;
	}

	void TextOverlay::doUpdate( OverlayRenderer const & renderer )
	{
		FontTextureSPtr fontTexture = getFontTexture();

		if ( !fontTexture )
		{
			CU_Failure( cuT( "The TextOverlay has no FontTexture. Did you set its font?" ) );
			CU_Exception( cuT( "The TextOverlay [" ) + getOverlayName() + cuT( "] has no FontTexture. Did you set its font?" ) );
		}

		auto font = fontTexture->getFont();

		if ( !font )
		{
			setVisible( false );
			CU_Failure( cuT( "The TextOverlay has no Font. Did you set its font?" ) );
			CU_Exception( cuT( "The TextOverlay [" ) + getOverlayName() + cuT( "] has no Font. Did you set its font?" ) );
		}

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

			fontTexture->update( true );
		}
	}

	void TextOverlay::doUpdateBuffer( castor::Size const & insize
		, std::function< void( castor::Point2d const &
			, castor::Rectangle const &
			, castor::Point4f const &
			, float &
			, float &
			, float &
			, float & ) > generateUvs )
	{
		FontTextureSPtr fontTexture = getFontTexture();

		if ( fontTexture )
		{
			auto font = fontTexture->getFont();

			if ( !m_currentCaption.empty() && font )
			{
				if ( m_textChanged )
				{
					castor::Point2d const ovAbsSize = getOverlay().getAbsoluteSize();
					castor::Point2d const size( insize.getWidth() * ovAbsSize[0], insize.getHeight() * ovAbsSize[1] );
					m_previousCaption = m_currentCaption;
					m_arrayVtx.clear();
					m_arrayVtx.reserve( m_previousCaption.size() * 6 );

					DisplayableLineArray lines = doPrepareText( insize, size );
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
									generateUvs( size,
										castor::Rectangle{ left, top, right, bottom },
										castor::Point4f{ fontUvLeft, fontUvTop, fontUvRight, fontUvBottom },
										texUvLeft, texUvTop, texUvRight, texUvBottom );

									auto w( float( insize.getWidth() ) );
									auto h( float( insize.getHeight() ) );
									//
									// Fill buffer
									//
									TextOverlay::Vertex const vertexTR = { { float( right ) / w, float( top ) / h },    { texUvRight, texUvTop },    { fontUvRight, fontUvTop } };
									TextOverlay::Vertex const vertexTL = { { float( left ) / w,  float( top ) / h },    { texUvLeft,  texUvTop },    { fontUvLeft,  fontUvTop } };
									TextOverlay::Vertex const vertexBL = { { float( left ) / w,  float( bottom ) / h }, { texUvLeft,  texUvBottom }, { fontUvLeft,  fontUvBottom } };
									TextOverlay::Vertex const vertexBR = { { float( right ) / w, float( bottom ) / h }, { texUvRight, texUvBottom }, { fontUvRight, fontUvBottom } };

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
				, []( castor::Point2d const &
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
				, []( castor::Point2d const & sz
					, castor::Rectangle const & absolute
					, castor::Point4f const & fontUV
					, float & uvLeft
					, float & uvTop
					, float & uvRight
					, float & uvBottom )
			{
				uvLeft = float( absolute[0] / sz[0] );
				uvTop = float( absolute[1] / sz[1] );
				uvRight = float( absolute[2] / sz[0] );
				uvBottom = float( absolute[3] / sz[1] );
			} );
			break;

		default:
			break;
		}
	}

	TextOverlay::DisplayableLineArray TextOverlay::doPrepareText( castor::Size const & renderSize
		, castor::Point2d const & insize )
	{
		auto & font = ovrltxt::getFont( *this );
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
				castor::Glyph const & glyph{ font.getGlyphAt( *itLine ) };
				DisplayableChar character{ castor::Point2d{}, castor::Point2d{ glyph.getAdvance(), glyph.getSize().getHeight() }, glyph };

				if ( glyph.getCharacter() == cuT( ' ' )
						|| glyph.getCharacter() == cuT( '\t' ) )
				{
					// write the word and leave space before next word.
					doPrepareWord( renderSize, word, wordWidth, insize, left, line, result );
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
				doPrepareWord( renderSize, word, wordWidth, insize, left, line, result );
			}

			line = doFinishLine( insize, line, left, result );
		}

		doAlignVertically( insize[1], result );
		return result;
	}

	void TextOverlay::doPrepareWord( castor::Size const & renderSize
		, std::u32string const & word
		, double wordWidth
		, castor::Point2d const & insize
		, double & left
		, DisplayableLine & line
		, DisplayableLineArray & lines )
	{
		auto & font = ovrltxt::getFont( *this );

		if ( left + wordWidth > insize[0] && m_wrappingMode == TextWrappingMode::eBreakWords )
		{
			// The word will overflow the overlay size, so we jump to the next line,
			// and will write the word on this next line.
			line = doFinishLine( insize, line, left, lines );
		}

		for ( auto character : word )
		{
			castor::Glyph const & glyph = font[character];
			castor::Point2d charSize( double( glyph.getAdvance() ), double( glyph.getSize().getHeight() ) );

			left += glyph.getBearing().x();

			if ( left > insize[0] )
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
					line = doFinishLine( insize, line, left, lines );
				}
			}
			else if ( left + charSize[0] > insize[0] )
			{
				// The character is partially out of the overlay.
				if ( m_wrappingMode == TextWrappingMode::eBreak )
				{
					// Break => Jump to the next line.
					line = doFinishLine( insize, line, left, lines );
				}
			}

			if ( charSize[0] > 0 )
			{
				line.m_characters.emplace_back( castor::Point2d{ left, 0.0 }, charSize, glyph );
			}

			left += charSize[0];
		}
	}

	TextOverlay::DisplayableLine TextOverlay::doFinishLine( castor::Point2d const & insize
		, DisplayableLine line
		, double & left
		, DisplayableLineArray & lines )
	{
		line.m_height = 0.0;
		double maxBottomBearing{ 0.0 };

		for ( auto const & character : line.m_characters )
		{
			auto bottomBearing = std::max( 0.0, double( character.m_glyph.getSize().getHeight() ) - character.m_glyph.getBearing().y() );
			line.m_height = std::max( line.m_height, character.m_size[1] + bottomBearing );
			maxBottomBearing = std::max( maxBottomBearing, bottomBearing );
		}

		for ( auto & character : line.m_characters )
		{
			character.m_position[1] = std::max( 0.0, maxBottomBearing + character.m_glyph.getBearing().y() );
		}

		line.m_width = left;
		left = 0;
		doAlignHorizontally( insize[0], line, lines );
		return DisplayableLine{ castor::Point2d{ 0, line.m_position[1] + line.m_height }, 0.0, 0.0 };
	}

	void TextOverlay::doAlignHorizontally( double width
		, DisplayableLine line
		, DisplayableLineArray & lines )
	{
		// Move letters according to halign
		if ( m_hAlign != HAlign::eLeft )
		{
			double offset = width - line.m_width;

			if ( m_hAlign == HAlign::eCenter )
			{
				offset /= 2;
			}

			for ( auto & character : line.m_characters )
			{
				character.m_position[0] += offset;
			}
		}

		// Remove letters overflowing the maximum width
		auto removed = std::remove_if( line.m_characters.begin()
			, line.m_characters.end()
			, [&width]( DisplayableChar const & lookup )
			{
				return lookup.m_position[0] > width
					   || lookup.m_position[0] + lookup.m_size[0] < 0;
			} );

		if ( !line.m_characters.empty() )
		{
			line.m_characters.erase( removed, line.m_characters.end() );

			// add the line to the lines array.
			if ( !line.m_characters.empty() )
			{
				lines.push_back( line );
			}
		}
		else
		{
			lines.push_back( line );
		}
	}

	void TextOverlay::doAlignVertically( double height
		, DisplayableLineArray & lines )
	{
		// Compute each line height, according to lines spacing mode
		if ( m_lineSpacingMode != TextLineSpacingMode::eOwnHeight )
		{
			double lineHeight{ 0.0 };

			if ( m_lineSpacingMode == TextLineSpacingMode::eMaxLineHeight )
			{
				for ( auto const & line : lines )
				{
					lineHeight = std::max( line.m_height, lineHeight );
				}
			}
			else if ( m_lineSpacingMode == TextLineSpacingMode::eMaxFontHeight )
			{
				lineHeight = double( ovrltxt::getFont( *this ).getMaxHeight() );
			}

			double offset{ 0.0 };

			for ( auto & line : lines )
			{
				line.m_position[1] += offset;
				offset += lineHeight - line.m_height;
				line.m_height = lineHeight;
			}
		}

		double linesHeight{ 0.0 };

		for ( auto const & line : lines )
		{
			linesHeight += line.m_height;
		}

		// Move lines according to valign
		if ( m_vAlign != VAlign::eTop )
		{
			double offset = height - linesHeight;

			if ( m_vAlign == VAlign::eCenter )
			{
				offset /= 2;
			}

			for ( auto & line : lines )
			{
				line.m_position[1] += offset;
			}
		}

		// Remove lines overflowing the maximum height
		auto removed = std::remove_if( lines.begin()
			, lines.end()
			, [&height]( DisplayableLine const & line )
			{
				return line.m_position[1] > height
					|| line.m_position[1] + line.m_height < 0;
			} );

		lines.erase( removed, lines.end() );
	}
}
