#include "TextOverlay.hpp"

#include "Engine.hpp"
#include "OverlayCache.hpp"
#include "SamplerCache.hpp"

#include "FontTexture.hpp"
#include "OverlayRenderer.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Material/Material.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
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

		Logger::LogInfo( m_tabs + cuT( "Writing TextOverlay " ) + p_overlay.GetOverlayName() );
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "text_overlay \"" ) + p_overlay.GetOverlayName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay name" );

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tfont \"" ) + p_overlay.GetFontTexture()->GetFontName() + cuT( "\"\n" ) ) > 0;
			OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay font" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttext \"" ) + p_overlay.GetCaption() + cuT( "\"\n" ) ) > 0;
			OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay text" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttext_wrapping " ) + TextWrappingModes[p_overlay.GetTextWrappingMode()] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay text wrapping" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tvertical_align " ) + VerticalAligns[p_overlay.GetVAlign()] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay text vertical align" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\thorizontal_align " ) + HorizontalAligns[p_overlay.GetHAlign()] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay text horizontal align" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttexturing_mode " ) + TexturingModes[p_overlay.GetTexturingMode()] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay text texturing mode" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tline_spacing_mode " ) + LineSpacingModes[p_overlay.GetLineSpacingMode()] + cuT( "\n" ) ) > 0;
			OverlayCategory::TextWriter::CheckError( l_return, "TextOverlay line spacing mode" );
		}

		if ( l_return )
		{
			l_return = OverlayCategory::TextWriter{ m_tabs }( p_overlay, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	bool TextOverlay::TextWriter::WriteInto( Castor::TextFile & p_file )
	{
		return ( *this )( *m_category, p_file );
	}

	//*************************************************************************************************

	TextOverlay::TextOverlay()
		: OverlayCategory{ eOVERLAY_TYPE_TEXT }
	{
	}

	TextOverlay::~TextOverlay()
	{
	}

	OverlayCategorySPtr TextOverlay::Create()
	{
		return std::make_shared< TextOverlay >();
	}

	void TextOverlay::SetFont( String const & p_strFont )
	{
		// Récupération / Création de la police
		Engine * l_engine = m_pOverlay->GetEngine();
		auto & l_fontCache = l_engine->GetFontCache();
		FontSPtr l_pFont = l_fontCache.Find( p_strFont );

		if ( l_pFont )
		{
			FontTextureSPtr l_fontTexture = l_engine->GetOverlayCache().GetFontTexture( l_pFont->GetName() );

			if ( !l_fontTexture )
			{
				l_fontTexture = l_engine->GetOverlayCache().CreateFontTexture( l_pFont );
				l_fontTexture->Update();
			}

			auto l_texture = m_fontTexture.lock();

			if ( m_connection && l_texture )
			{
				l_texture->Disconnect( m_connection );
			}

			m_fontTexture = l_fontTexture;

			m_connection = l_fontTexture->Connect( [this]( FontTexture const & p_texture )
			{
				m_textChanged = true;
			} );
		}
		else
		{
			CASTOR_EXCEPTION( "Font " + string::string_cast< char >( p_strFont ) + "not found" );
		}

		m_textChanged = true;
	}

	void TextOverlay::DoUpdate()
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();

		if ( !l_fontTexture )
		{
			SetVisible( false );
			CASTOR_EXCEPTION( cuT( "The TextOverlay [" ) + GetOverlayName() + cuT( "] has no FontTexture. Did you set its font?" ) );
		}

		FontSPtr l_font = l_fontTexture->GetFont();
		std::vector< char32_t > l_new;

		for ( string::utf8::iterator l_it = m_currentCaption.begin(); l_it != m_currentCaption.end(); ++l_it )
		{
			if ( !l_font->HasGlyphAt( *l_it ) )
			{
				l_new.push_back( *l_it );
			}
		}

		if ( !l_new.empty() )
		{
			for ( auto l_char : l_new )
			{
				l_font->LoadGlyph( l_char );
			}

			l_fontTexture->Update();

			GetOverlay().GetEngine()->PostEvent( MakeFunctorEvent( EventType::PreRender, [l_fontTexture]()
			{
				l_fontTexture->Cleanup();
				l_fontTexture->Initialise();
			} ) );
		}
	}

	void TextOverlay::DoRender( OverlayRendererSPtr p_renderer )
	{
		p_renderer->DrawText( *this );
	}

	void TextOverlay::DoUpdateBuffer( Size const & p_size, std::function< void( Point2d const & p_size, Rectangle const & p_absolute, Point4r const & p_fontUV,
																				real & p_uvLeft, real & p_uvTop, real & p_uvRight, real & p_uvBottom ) > p_generateUvs )
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();

		if ( l_fontTexture )
		{
			FontSPtr l_font = l_fontTexture->GetFont();

			if ( !m_currentCaption.empty() && l_font )
			{
				if ( m_textChanged )
				{
					Point2d const l_ovAbsSize = GetOverlay().GetAbsoluteSize();
					Point2d const l_size( p_size.width() * l_ovAbsSize[0], p_size.height() * l_ovAbsSize[1] );
					m_previousCaption = m_currentCaption;
					m_arrayVtx.clear();
					m_arrayVtx.reserve( m_previousCaption.size() * 6 );
					Position const l_ovPosition = GetAbsolutePosition( p_size );

					DisplayableLineArray l_lines = DoPrepareText( p_size, l_size );
					Size const & l_texDim = l_fontTexture->GetTexture()->GetImage().GetDimensions();

					for ( auto const & l_line : l_lines )
					{
						double const l_height = l_line.m_height;
						double l_topCrop = std::max( 0.0, -l_line.m_position[1] );
						double l_bottomCrop = std::max( 0.0, l_line.m_position[1] + l_height - l_size[1] );

						if ( l_topCrop + l_bottomCrop <= l_height )
						{
							for ( auto const & l_char : l_line.m_characters )
							{
								l_topCrop = std::max( 0.0, -l_line.m_position[1] - ( l_height - l_char.m_glyph.GetBearing().y() ) );
								l_bottomCrop = std::max( 0.0, l_line.m_position[1] + ( l_height - l_char.m_position[1] ) + l_char.m_size[1] - l_size[1] );

								double const l_leftUncropped = l_char.m_position[0] + l_line.m_position[0];
								double const l_leftCrop = std::max( 0.0, -l_leftUncropped );
								double const l_rightCrop = std::max( 0.0, l_leftUncropped + l_char.m_size[0] - l_size[0] );

								if ( l_leftCrop + l_rightCrop < l_char.m_size[0] )
								{
									//
									// Compute Letter's Position.
									//
									double const l_topUncropped = l_line.m_position[1] + l_height - l_char.m_position[1];
									int32_t const l_left = l_ovPosition.x() + std::max( 0, int32_t( l_leftUncropped + l_leftCrop ) );
									int32_t const l_top = l_ovPosition.y() + std::max( 0, int32_t( l_topUncropped + l_topCrop ) );
									int32_t const l_right = l_ovPosition.x() + int32_t( std::min( l_leftUncropped + l_char.m_size[0] - l_rightCrop, l_size[0] ) );
									int32_t const l_bottom = l_ovPosition.y() + int32_t( std::min( l_topUncropped + l_char.m_size[1] - l_bottomCrop, l_size[1] ) );

									//
									// Compute Letter's Font UV.
									//
									double const l_fontUvTopCrop = l_topCrop / l_texDim.height();
									double const l_fontUvBottomCrop = l_bottomCrop / l_texDim.height();
									Position const l_fontUvPosition = l_fontTexture->GetGlyphPosition( l_char.m_glyph.GetCharacter() );
									double const l_fontUvLeftCrop = l_leftCrop / l_texDim.height();
									double const l_fontUvRightCrop = l_rightCrop / l_texDim.height();
									double const l_fontUvLeftUncropped = double( l_fontUvPosition.x() ) / l_texDim.width();
									double const l_fontUvTopUncropped = double( l_fontUvPosition.y() ) / l_texDim.height();
									real const l_fontUvLeft = real( l_fontUvLeftUncropped + l_fontUvLeftCrop );
									real const l_fontUvRight = real( l_fontUvLeftUncropped + ( l_char.m_size[0] / l_texDim.width() ) - l_fontUvRightCrop );
									// The UV is vertically inverted since the image is top-bottom, and the overlay is drawn bottom-up.
									real const l_fontUvBottom = real( l_fontUvTopUncropped + l_fontUvBottomCrop );
									real const l_fontUvTop = real( l_fontUvTopUncropped + ( l_char.m_size[1] / l_texDim.height() ) - l_fontUvTopCrop );

									//
									// Compute Letter's Texture UV.
									//
									real l_texUvLeft{};
									real l_texUvRight{};
									real l_texUvBottom{};
									real l_texUvTop{};
									p_generateUvs( l_size,
												   Castor::Rectangle{ l_left, l_top, l_right, l_bottom },
												   Point4r{ l_fontUvLeft, l_fontUvTop, l_fontUvRight, l_fontUvBottom },
												   l_texUvLeft, l_texUvTop, l_texUvRight, l_texUvBottom );


									//
									// Fill buffer
									//
									TextOverlay::Vertex const l_vertexTR = { { l_right, l_top },    { l_fontUvRight, l_fontUvTop },    { l_texUvRight, l_texUvTop } };
									TextOverlay::Vertex const l_vertexTL = { { l_left,  l_top },    { l_fontUvLeft,  l_fontUvTop },    { l_texUvLeft,  l_texUvTop } };
									TextOverlay::Vertex const l_vertexBL = { { l_left,  l_bottom }, { l_fontUvLeft,  l_fontUvBottom }, { l_texUvLeft,  l_texUvBottom } };
									TextOverlay::Vertex const l_vertexBR = { { l_right, l_bottom }, { l_fontUvRight, l_fontUvBottom }, { l_texUvRight, l_texUvBottom } };

									m_arrayVtx.push_back( l_vertexBL );
									m_arrayVtx.push_back( l_vertexBR );
									m_arrayVtx.push_back( l_vertexTL );

									m_arrayVtx.push_back( l_vertexTR );
									m_arrayVtx.push_back( l_vertexTL );
									m_arrayVtx.push_back( l_vertexBR );
								}
							}
						}
					}

					m_textChanged = false;
				}
			}
		}
	}

	void TextOverlay::DoUpdateBuffer( Size const & p_size )
	{
		switch ( m_texturingMode )
		{
		case eTEXT_TEXTURING_MODE_LETTER:
			DoUpdateBuffer( p_size, [this]( Point2d const & p_size, Rectangle const & p_absolute, Point4r const & p_fontUV,
											real & p_uvLeft, real & p_uvTop, real & p_uvRight, real & p_uvBottom )
			{
				p_uvLeft = 0.0_r;
				p_uvTop = 0.0_r;
				p_uvRight = 1.0_r;
				p_uvBottom = 1.0_r;
			} );
			break;

		case eTEXT_TEXTURING_MODE_TEXT:
			DoUpdateBuffer( p_size, [this]( Point2d const & p_size, Rectangle const & p_absolute, Point4r const & p_fontUV,
											real & p_uvLeft, real & p_uvTop, real & p_uvRight, real & p_uvBottom )
			{
				p_uvLeft = real( p_absolute[0] / p_size[0] );
				p_uvTop = real( p_absolute[1] / p_size[1] );
				p_uvRight = real( p_absolute[2] / p_size[0] );
				p_uvBottom = real( p_absolute[3] / p_size[1] );
			} );
			break;
		}
	}

	TextOverlay::DisplayableLineArray TextOverlay::DoPrepareText( Size const & p_renderSize, Point2d const & p_size )
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();
		FontSPtr l_font = l_fontTexture->GetFont();
		StringArray l_lines = string::split( m_previousCaption, cuT( "\n" ), uint32_t( std::count( m_previousCaption.begin(), m_previousCaption.end(), cuT( '\n' ) ) + 1 ) );
		DisplayableLineArray l_return;
		DisplayableLine l_line;

		for ( auto const & l_lineText : l_lines )
		{
			double l_left = 0;
			double l_wordWidth = 0;
			std::u32string l_word;

			for ( string::utf8::const_iterator l_itLine = l_lineText.begin(); l_itLine != l_lineText.end(); ++l_itLine )
			{
				Glyph const & l_glyph{ l_font->GetGlyphAt( *l_itLine ) };
				DisplayableChar l_character{ Point2d{}, Point2d{ l_glyph.GetAdvance(), l_glyph.GetSize().height() }, l_glyph };

				if ( l_glyph.GetCharacter() == cuT( ' ' )
						|| l_glyph.GetCharacter() == cuT( '\t' ) )
				{
					// Write the word and leave space before next word.
					DoPrepareWord( p_renderSize, l_word, l_wordWidth, p_size, l_left, l_line, l_return );
					l_word.clear();
					l_wordWidth = 0;
					l_left += l_character.m_size[0];
				}
				else
				{
					l_word += l_glyph.GetCharacter();
					l_wordWidth += l_character.m_size[0];
				}
			}

			if ( !l_word.empty() )
			{
				DoPrepareWord( p_renderSize, l_word, l_wordWidth, p_size, l_left, l_line, l_return );
			}

			l_line = DoFinishLine( p_size, l_line, l_left, l_return );
		}

		DoAlignVertically( p_size[1], l_return );
		return l_return;
	}

	void TextOverlay::DoPrepareWord( Size const & p_renderSize, std::u32string const & p_word, double p_wordWidth, Point2d const & p_size, double & p_left, DisplayableLine & p_line, DisplayableLineArray & p_lines )
	{
		auto l_fontTexture = GetFontTexture();
		auto const & l_font = *l_fontTexture->GetFont();
		auto l_ovPosition = GetAbsolutePosition( p_renderSize );

		if ( p_left + p_wordWidth > p_size[0] && m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK_WORDS )
		{
			// The word will overflow the overlay size, so we jump to the next line,
			// and will write the word on this next line.
			p_line = DoFinishLine( p_size, p_line, p_left, p_lines );
		}

		for ( auto l_character : p_word )
		{
			Glyph const & l_glyph = l_font[l_character];
			Point2d l_charSize( double( l_glyph.GetAdvance() ), double( l_glyph.GetSize().height() ) );

			p_left += l_glyph.GetBearing().x();

			if ( p_left > p_size[0] )
			{
				// The character is completely out of the overlay.
				if ( m_wrappingMode == eTEXT_WRAPPING_MODE_NONE )
				{
					// No wrapping => ignore the character.
					l_charSize[0] = 0;
				}
				else if ( m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK )
				{
					// Break => Jump to the next line.
					p_line = DoFinishLine( p_size, p_line, p_left, p_lines );
				}
			}
			else if ( p_left + l_charSize[0] > p_size[0] )
			{
				// The character is partially out of the overlay.
				if ( m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK )
				{
					// Break => Jump to the next line.
					p_line = DoFinishLine( p_size, p_line, p_left, p_lines );
				}
			}

			if ( l_charSize[0] > 0 )
			{
				p_line.m_characters.push_back( { Point2d{ p_left, 0.0 }, l_charSize, l_glyph } );
			}

			p_left += l_charSize[0];
		}
	}

	TextOverlay::DisplayableLine TextOverlay::DoFinishLine( Point2d const & p_size, DisplayableLine p_line, double & p_left, DisplayableLineArray & p_lines )
	{
		p_line.m_height = 0.0;
		double l_maxBottomBearing{ 0.0 };

		for ( auto const & l_character : p_line.m_characters )
		{
			auto l_bottomBearing = std::max( 0.0, double( l_character.m_glyph.GetSize().height() - l_character.m_glyph.GetBearing().y() ) );
			p_line.m_height = std::max( p_line.m_height, l_character.m_size[1] + l_bottomBearing );
			l_maxBottomBearing = std::max( l_maxBottomBearing, l_bottomBearing );
		}

		for ( auto & l_character : p_line.m_characters )
		{
			l_character.m_position[1] = std::max( 0.0, double( l_maxBottomBearing + l_character.m_glyph.GetBearing().y() ) );
		}

		p_line.m_width = p_left;
		p_left = 0;
		DoAlignHorizontally( p_size[0], p_line, p_lines );
		return DisplayableLine{ Point2d{ 0, p_line.m_position[1] + p_line.m_height }, 0.0, 0.0 };
	}

	void TextOverlay::DoAlignHorizontally( double p_width, DisplayableLine p_line, DisplayableLineArray & p_lines )
	{
		// Move letters according to halign
		if ( m_hAlign != eHALIGN_LEFT )
		{
			double l_offset = p_width - p_line.m_width;

			if ( m_hAlign == eHALIGN_CENTER )
			{
				l_offset /= 2;
			}

			for ( auto & l_character : p_line.m_characters )
			{
				l_character.m_position[0] += l_offset;
			}
		}

		// Remove letters overflowing the maximum width
		auto l_removed = std::remove_if( p_line.m_characters.begin(), p_line.m_characters.end(), [&p_width]( DisplayableChar const & p_char )
		{
			return p_char.m_position[0] > p_width
				   || p_char.m_position[0] + p_char.m_size[0] < 0;
		} );

		p_line.m_characters.erase( l_removed, p_line.m_characters.end() );

		// Add the line to the lines array.
		if ( !p_line.m_characters.empty() )
		{
			p_lines.push_back( p_line );
		}
	}

	void TextOverlay::DoAlignVertically( double p_height, DisplayableLineArray & p_lines )
	{
		// Compute each line height, according to lines spacing mode
		if ( m_lineSpacingMode != eTEXT_LINE_SPACING_MODE_OWN_HEIGHT )
		{
			double l_height{ 0.0 };

			if ( m_lineSpacingMode == eTEXT_LINE_SPACING_MODE_MAX_LINE_HEIGHT )
			{
				for ( auto const & l_line : p_lines )
				{
					l_height = std::max( l_line.m_height, l_height );
				}
			}
			else if ( m_lineSpacingMode == eTEXT_LINE_SPACING_MODE_MAX_FONT_HEIGHT )
			{
				l_height = double( GetFontTexture()->GetFont()->GetMaxHeight() );
			}

			double l_offset{ 0.0 };

			for ( auto & l_line : p_lines )
			{
				l_line.m_position[1] += l_offset;
				l_offset += l_height - l_line.m_height;
				l_line.m_height = l_height;
			}
		}

		double l_linesHeight{ 0.0 };

		for ( auto const & l_line : p_lines )
		{
			l_linesHeight += l_line.m_height;
		}

		// Move lines according to valign
		if ( m_vAlign != eVALIGN_TOP )
		{
			double l_offset = p_height - l_linesHeight;

			if ( m_vAlign == eVALIGN_CENTER )
			{
				l_offset /= 2;
			}

			for ( auto & l_line : p_lines )
			{
				l_line.m_position[1] += l_offset;
			}
		}

		// Remove lines overflowing the maximum height
		auto l_removed = std::remove_if( p_lines.begin(), p_lines.end(), [&p_height]( DisplayableLine const & p_line )
		{
			return p_line.m_position[1] > p_height
				   || p_line.m_position[1] + p_line.m_height < 0;
		} );

		p_lines.erase( l_removed, p_lines.end() );
	}
}
