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
	//*********************************************************************************************

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

		template< typename TextContainerT >
		static bool isEmpty( TextContainerT const & v )
		{
			return v.charEnd == v.charBegin;
		}
	}

	//*********************************************************************************************

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

	uint32_t TextOverlay::getDisplayCount()const
	{
		auto count = std::count( m_previousCaption.begin(), m_previousCaption.end(), U' ' );
		count += std::count( m_previousCaption.begin(), m_previousCaption.end(), U'\t' );
		count += std::count( m_previousCaption.begin(), m_previousCaption.end(), U'\n' );
		return uint32_t( 6u * ( m_previousCaption.size() - uint32_t( std::max( ptrdiff_t{}, count ) ) ) );
	}

	uint32_t TextOverlay::getCount( bool secondary )const
	{
		return uint32_t( m_buffer.size() );
	}

	void TextOverlay::fillBuffer( castor::Size const & renderSize
		, Vertex * buffer
		, bool secondary )const
	{
		std::copy( m_buffer.begin()
			, m_buffer.end()
			, buffer );
	}

	void TextOverlay::setFont( castor::String const & name )
	{
		// Récupération / Création de la police
		Engine * engine = m_overlay->getEngine();
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

		for ( auto c : m_currentCaption )
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

		if ( !m_currentCaption.empty() )
		{
			m_previousCaption = m_currentCaption;
			m_charsCount = getDisplayCount() / 6u;

			switch ( m_texturingMode )
			{
			case TextTexturingMode::eLetter:
				doFillBuffer( renderer.getSize()
					, []( castor::Point2f const & size
						, castor::Point4i const & extent
						, castor::Point4f & uv )
					{
						uv = { 0.0, 0.0, 1.0, 1.0 };
					} );
				break;

			case TextTexturingMode::eText:
				doFillBuffer( renderer.getSize()
					, []( castor::Point2f const & size
						, castor::Point4i const & extent
						, castor::Point4f & uv )
					{
						uv = { float( double( extent->x ) / double( size->x ) )
							, float( double( extent->y ) / double( size->y ) )
							, float( double( extent->z ) / double( size->x ) )
							, float( double( extent->w ) / double( size->y ) ) };
					} );
				break;

			default:
				break;
			}
		}

		m_textChanged = false;
	}

	void TextOverlay::doFillBuffer( castor::Size const & rndSize
		, UvGenFunc generateUvs )
	{
		if ( !m_textChanged )
		{
			return;
		}

		m_buffer.resize( m_charsCount * 6u );

		if ( m_previousCaption.empty() )
		{
			return;
		}

		auto fontTexture = getFontTexture();
		uint32_t index{};
		auto myBuffer = m_buffer.data();
		castor::Point2f renderSize{ castor::Point2f{ rndSize.getWidth(), rndSize.getHeight() }
			* getRenderRatio( rndSize ) };
		castor::Point2f overlaySize( renderSize * getOverlay().getAbsoluteSize() );

		doPrepareText( overlaySize );
		castor::Point2f texDim{ fontTexture->getTexture()->getDimensions().width
			, fontTexture->getTexture()->getDimensions().height };

		for ( auto character : castor::makeArrayView( m_text.begin(), getDisplayCount() / 6u ) )
		{
			auto & word = m_words.words()[character.word];
			auto & line = m_lines.lines()[word.line];
			castor::Point4i extent{ line.position->x + word.left + character.left + character.bearing->x
				, m_lines.topOffset + line.position->y - character.bearing->y
				, 0.0, 0.0 };
			extent->z = extent->x + int32_t( character.size->x );
			extent->w = extent->y + int32_t( character.size->y );

			// check for underflow/overflow
			if ( extent->x < int32_t( overlaySize->x )
				&& extent->y < int32_t( overlaySize->y )
				&& extent->z > 0
				&& extent->w > 0 )
			{
				//
				// Compute Letter's Font UV.
				//
				castor::Point4f fontUv{ character.uvPosition->x / texDim->x
					, 0.0, 0.0
					, character.uvPosition->y / texDim->y };
				fontUv->z = fontUv->x + ( character.size->x / texDim->x );
				fontUv->y = fontUv->w + ( character.size->y / texDim->y );
				//
				// Compute Letter's Texture UV.
				//
				castor::Point4f texUv{};
				generateUvs( overlaySize, extent, texUv );
				auto relative = castor::Point4f{ extent } / castor::Point4f{ renderSize->x, renderSize->y, renderSize->x, renderSize->y };
				//
				// Fill buffer
				//
				TextOverlay::Vertex const vertexTR{ { relative->z, relative->y }, { texUv->z, texUv->y }, { fontUv->z, fontUv->y } };
				TextOverlay::Vertex const vertexTL{ { relative->x, relative->y }, { texUv->x, texUv->y }, { fontUv->x, fontUv->y } };
				TextOverlay::Vertex const vertexBL{ { relative->x, relative->w }, { texUv->x, texUv->w }, { fontUv->x, fontUv->w } };
				TextOverlay::Vertex const vertexBR{ { relative->z, relative->w }, { texUv->z, texUv->w }, { fontUv->z, fontUv->w } };

				*myBuffer = vertexBL; ++myBuffer; ++index;
				*myBuffer = vertexBR; ++myBuffer; ++index;
				*myBuffer = vertexTL; ++myBuffer; ++index;

				*myBuffer = vertexTR; ++myBuffer; ++index;
				*myBuffer = vertexTL; ++myBuffer; ++index;
				*myBuffer = vertexBR; ++myBuffer; ++index;
			}
		}
	}

	void TextOverlay::doPrepareText( castor::Point2f const & overlaySize )
	{
		m_words.count = 0u;
		m_lines.count = 0u;
		m_lines.maxRange = { 100.0, 0.0 };
		m_lines.topOffset = {};
		m_lines.count = {};

		auto caption = m_previousCaption;
		auto fontTexture = getFontTexture();
		auto & font = ovrltxt::getFont( *this );
		float lineTop{};
		{
			float totalLeft{};
			float wordLeft{};
			float charLeft{};
			uint32_t charIndex{};
			uint32_t wordIndex{};
			uint32_t lineIndex{};
			auto cit = caption.begin();
			auto tit = m_text.begin();

			auto nextWord = [&]()
			{
				auto & word = m_words.getNext();
				word.left = wordLeft;
				word.range = { 100.0, 0.0 };
				word.charBegin = charIndex;
				word.charEnd = word.charBegin;
				return &word;
			};

			auto nextLine = [&]()
			{
				auto & line = m_lines.getNext();
				line.position = { 0.0, lineTop };
				line.range = { 100.0, 0.0 };
				line.wordBegin = wordIndex;
				line.wordEnd = line.wordBegin;
				line.charBegin = charIndex;
				line.charEnd = line.charBegin;
				charLeft = totalLeft - wordLeft;
				totalLeft = charLeft;
				wordLeft = 0.0;
				return &line;
			};

			auto word = nextWord();
			auto line = nextLine();

			auto alignLine = [&]()
			{
				if ( !ovrltxt::isEmpty( *line ) )
				{
					if ( m_lineSpacingMode == TextLineSpacingMode::eMaxFontHeight )
					{
						line->range = castor::Point2f{ ovrltxt::getFont( *this ).getMaxRange() };
					}

					// Move line according to halign
					if ( m_hAlign != HAlign::eLeft )
					{
						auto offset = overlaySize->x - line->width;

						if ( m_hAlign == HAlign::eCenter )
						{
							offset /= 2;
						}

						line->position->x = line->position->x + offset;
					}
				}

				lineTop += line->range->y - line->range->x;
				++lineIndex;
			};

			auto addWord = [&]()
			{
				if ( !ovrltxt::isEmpty( *word ) )
				{
					word->width = charLeft;
					word->line = lineIndex;
					line->range->x = std::min( line->range->x, word->range->x );
					line->range->y = std::max( line->range->y, word->range->y );
					line->width += word->width;
					m_lines.maxRange->x = std::min( m_lines.maxRange->x, line->range->x );
					m_lines.maxRange->y = std::max( m_lines.maxRange->y, line->range->y );
					++line->wordEnd;
					line->charEnd = word->charEnd;
				}
			};

			auto addChar = [&]( castor::Point2f charSize
				, castor::Point2f const & bearing )
			{
				auto xMin = bearing->x;
				auto xMax = xMin + charSize->x;
				auto yMin = -bearing->y;
				auto yMax = yMin + charSize->y;

				if ( m_wrappingMode == TextWrappingMode::eBreakWords
					&& wordLeft > 0.0
					&& ( wordLeft > overlaySize->x
						|| totalLeft + xMax > overlaySize->x ) )
				{
					// The word will overflow the overlay size.
					// So we jump to the next line,
					// and will write the word on this next line.
					alignLine();
					line = nextLine();
					line->charBegin = word->charBegin;
					word->left = wordLeft;
				}
				else if ( m_wrappingMode == TextWrappingMode::eBreak
					&& totalLeft + xMax > overlaySize->x )
				{
					// The char will overflow the overlay size.
					// So we write the current word,
					// jump to the next line,
					// then carry on the word on this next line.
					addWord();
					alignLine();
					wordLeft = totalLeft;
					++wordIndex;
					line = nextLine();
					word = nextWord();
				}

				// Setup char
				auto uvPosition = fontTexture->getGlyphPosition( *cit );
				auto & outChar = *tit;
				outChar.c = *cit;
				outChar.left = charLeft;
				outChar.size = charSize;
				outChar.bearing = bearing;
				outChar.uvPosition = { uvPosition.x(), uvPosition.y() };
				outChar.word = wordIndex;

				// Complete word
				word->range->x = std::min( word->range->x, yMin );
				word->range->y = std::max( word->range->y, yMax );
				++word->charEnd;
			};

			while ( cit != caption.end() )
			{
				if ( *cit == U'\n' )
				{
					addWord();
					alignLine();
					line = nextLine();
					++wordIndex;
					word = nextWord();
				}
				else
				{
					castor::Glyph const & glyph{ font.getGlyphAt( *cit ) };

					if ( *cit == U' ' || *cit == U'\t' )
					{
						// write the word and leave space before next word.
						addWord();
						totalLeft += float( glyph.getAdvance() );
						wordLeft = totalLeft;
						charLeft = 0.0;
						++wordIndex;
						word = nextWord();
					}
					else
					{
						addChar( { glyph.getSize().getWidth(), glyph.getSize().getHeight() }
							, { glyph.getBearing().x(), glyph.getBearing().y() } );
						totalLeft += float( glyph.getAdvance() );
						charLeft += float( glyph.getAdvance() );
						++charIndex;
						++tit;
					}
				}

				++cit;
			}

			addWord();
			alignLine();
		}

		auto lines = m_lines.lines();

		if ( !lines.empty() )
		{
			if ( m_lineSpacingMode == TextLineSpacingMode::eMaxLineHeight )
			{
				// Adjust lines heights to maxHeight
				auto lineRange = m_lines.maxRange;
				auto lineHeight = lineRange->y - lineRange->x;
				lineTop = 0.0;

				for ( auto & line : lines )
				{
					line.position->y = lineTop;
					line.range = lineRange;
					lineTop += lineHeight;
				}
			}

			// Move lines according to valign
			if ( m_vAlign != VAlign::eTop )
			{
				auto offset = overlaySize->y - lineTop;

				if ( m_vAlign == VAlign::eCenter )
				{
					offset /= 2;
				}

				m_lines.topOffset = offset - lines.front().range->x;
			}
		}
	}

	//*********************************************************************************************
}
