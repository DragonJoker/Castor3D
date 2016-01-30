#include "TextOverlay.hpp"

#include "Engine.hpp"
#include "FontTexture.hpp"
#include "InitialiseEvent.hpp"
#include "Overlay.hpp"
#include "OverlayManager.hpp"
#include "OverlayRenderer.hpp"
#include "RenderSystem.hpp"
#include "Sampler.hpp"
#include "StaticTexture.hpp"

#include <Font.hpp>
#include <Image.hpp>

using namespace Castor;

#if defined( DrawText )
#	undef DrawText
#endif

namespace Castor3D
{
	bool TextOverlay::TextLoader::operator()( TextOverlay const & p_overlay, TextFile & p_file )
	{
		String l_strTabs;
		OverlaySPtr l_pParent = p_overlay.GetOverlay().GetParent();

		while ( l_pParent )
		{
			l_strTabs += cuT( '\t' );
			l_pParent = l_pParent->GetParent();
		}

		bool l_return = p_file.WriteText( l_strTabs + cuT( "text_overlay " ) + p_overlay.GetOverlayName() + cuT( "\n" ) + l_strTabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\tfont " ) + p_overlay.GetFontTexture()->GetFontName() ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\tcaption " ) + p_overlay.GetCaption() ) > 0;
		}

		if ( l_return )
		{
			l_return = Overlay::TextLoader()( p_overlay.GetOverlay(), p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	TextOverlay::BinaryParser::BinaryParser( Path const & p_path )
		:	OverlayCategory::BinaryParser( p_path )
	{
	}

	bool TextOverlay::BinaryParser::Fill( TextOverlay const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetFontTexture()->GetFontName(), eCHUNK_TYPE_OVERLAY_FONT, p_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetCaption(), eCHUNK_TYPE_OVERLAY_CAPTION, p_chunk );
		}

		return l_return;
	}

	bool TextOverlay::BinaryParser::Parse( TextOverlay & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_name;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_OVERLAY_FONT:
			l_return = DoParseChunk( l_name, p_chunk );

			if ( l_return )
			{
				p_obj.SetFont( l_name );
			}

			break;

		case eCHUNK_TYPE_OVERLAY_CAPTION:
			l_return = DoParseChunk( l_name, p_chunk );

			if ( l_return )
			{
				p_obj.SetCaption( l_name );
			}

			break;

		default:
			l_return = false;
			break;
		}

		if ( !l_return )
		{
			p_chunk.EndParse();
		}

		return l_return;
	}

	//*************************************************************************************************

	TextOverlay::TextOverlay()
		: OverlayCategory( eOVERLAY_TYPE_TEXT )
		, m_wrappingMode( eTEXT_WRAPPING_MODE_NONE )
		, m_hAlign( eHALIGN_LEFT )
		, m_vAlign( eVALIGN_CENTER )
		, m_textChanged( true )
	{
	}

	TextOverlay::~TextOverlay()
	{
	}

	OverlayCategorySPtr TextOverlay::Create()
	{
		return std::make_shared< TextOverlay >();
	}

	void TextOverlay::Initialise()
	{
	}

	void TextOverlay::Cleanup()
	{
	}

	void TextOverlay::SetFont( String const & p_strFont )
	{
		// Récupération / Création de la police
		Engine * l_engine = m_pOverlay->GetEngine();
		FontManager & l_fontManager = l_engine->GetFontManager();
		FontSPtr l_pFont = l_fontManager.get( p_strFont );

		if ( l_pFont )
		{
			FontTextureSPtr l_fontTexture = l_engine->GetOverlayManager().GetFontTexture( l_pFont->GetName() );

			if ( !l_fontTexture )
			{
				l_fontTexture = l_engine->GetOverlayManager().CreateFontTexture( l_pFont );
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

		l_engine->PostEvent( MakeInitialiseEvent( *this ) );
		m_textChanged = true;
	}

	void TextOverlay::LoadNewGlyphs()
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();
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
			l_fontTexture->Cleanup();

			for ( auto l_char : l_new )
			{
				l_font->LoadGlyph( l_char );
			}

			l_fontTexture->Initialise();
		}
	}

	void TextOverlay::DoRender( OverlayRendererSPtr p_renderer )
	{
		p_renderer->DrawText( *this );
	}

	void TextOverlay::DoUpdateBuffer( Size const & p_size )
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();

		if ( l_fontTexture )
		{
			FontSPtr l_font = l_fontTexture->GetFont();

			if ( !m_currentCaption.empty() && l_font )
			{
				uint32_t l_maxHeight = l_font->GetMaxHeight();

				if ( m_textChanged )
				{
					Point2d l_ovAbsSize = GetOverlay().GetAbsoluteSize();
					Point2d l_size( p_size.width() * l_ovAbsSize[0], p_size.height() * l_ovAbsSize[1] );
					m_previousCaption = m_currentCaption;
					m_arrayVtx.clear();
					m_arrayVtx.reserve( m_previousCaption.size() * 6 );
					Position l_ovPosition = GetAbsolutePosition( p_size );

					DisplayableLineArray l_lines = DoPrepareText( p_size, l_size );
					Size const & l_texDim = l_fontTexture->GetTexture()->GetDimensions();

					for ( auto l_line : l_lines )
					{
						double l_topCrop = std::max( 0.0, -l_line.m_position[1] );
						double l_bottomCrop = std::max( 0.0, l_line.m_position[1] + l_maxHeight - l_size[1] );

						if ( l_topCrop + l_bottomCrop < l_maxHeight )
						{
							for ( auto l_char : l_line.m_characters )
							{
								l_topCrop = std::max( 0.0, -l_line.m_position[1] - ( l_maxHeight - l_char.m_glyph->GetPosition().y() ) );
								l_bottomCrop = std::max( 0.0, l_line.m_position[1] + l_maxHeight - l_size[1] );
								double l_uvTopCrop = l_topCrop / l_texDim.height();
								double l_uvBottomCrop = l_bottomCrop / l_texDim.height();

								double l_leftUncropped = l_char.m_left + l_line.m_position[0];
								double l_leftCrop = std::max( 0.0, -l_leftUncropped );
								double l_rightCrop = std::max( 0.0, l_leftUncropped + l_char.m_size[0] - l_size[0] );

								if ( l_leftCrop + l_rightCrop < l_char.m_size[0] )
								{
									double l_uvLeftCrop = l_leftCrop / l_texDim.height();
									double l_uvRightCrop = l_rightCrop / l_texDim.height();

									double l_topUncropped = l_line.m_position[1] + l_font->GetMaxHeight() - l_char.m_glyph->GetPosition().y();
									Position l_uvPosition = l_fontTexture->GetGlyphPosition( l_char.m_glyph->GetCharacter() );
									double l_uvLeftUncropped = double( l_uvPosition.x() ) / l_texDim.width();
									double l_uvTopUncropped = double( l_uvPosition.y() ) / l_texDim.height();

									int32_t l_left = l_ovPosition.x() + std::max( 0, int32_t( l_leftUncropped + l_leftCrop ) );
									int32_t l_right = l_ovPosition.x() + int32_t( std::min( l_leftUncropped + l_char.m_size[0] - l_rightCrop, l_size[0] ) );
									int32_t l_top = l_ovPosition.y() + std::max( 0, int32_t( l_topUncropped + l_topCrop ) );
									int32_t l_bottom = l_ovPosition.y() + int32_t( std::min( l_topUncropped + l_char.m_size[1] - l_bottomCrop, l_size[1] ) );

									real l_uvLeft = real( l_uvLeftUncropped + l_uvLeftCrop );
									real l_uvRight = real( l_uvLeftUncropped + ( l_char.m_size[0] / l_texDim.width() ) - l_uvRightCrop );
									// The UV is vertically inverted since the image is top-bottom, and the overlay is drawn bottom-up.
									real l_uvBottom = real( l_uvTopUncropped + l_uvBottomCrop );
									real l_uvTop = real( l_uvTopUncropped + ( l_char.m_size[1] / l_texDim.height() ) - l_uvTopCrop );

									OverlayCategory::Vertex l_vertexTR = { { l_right, l_top },    { l_uvRight, l_uvTop } };
									OverlayCategory::Vertex l_vertexTL = { { l_left,  l_top },    { l_uvLeft,  l_uvTop } };
									OverlayCategory::Vertex l_vertexBL = { { l_left,  l_bottom }, { l_uvLeft,  l_uvBottom } };
									OverlayCategory::Vertex l_vertexBR = { { l_right, l_bottom }, { l_uvRight, l_uvBottom } };

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

	TextOverlay::DisplayableLineArray TextOverlay::DoPrepareText( Size const & p_renderSize, Point2d const & p_size )
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();
		FontSPtr l_font = l_fontTexture->GetFont();
		StringArray l_lines = string::split( m_previousCaption, cuT( "\n" ), std::count( m_previousCaption.begin(), m_previousCaption.end(), cuT( '\n' ) ) + 1 );
		DisplayableLineArray l_return;
		DisplayableLine l_line;

		for ( auto l_lineText : l_lines )
		{
			double l_left = 0;
			double l_wordWidth = 0;
			std::u32string l_word;

			for ( string::utf8::iterator l_itLine = l_lineText.begin(); l_itLine != l_lineText.end(); ++l_itLine )
			{
				Glyph * l_glyph = &l_font->GetGlyphAt( *l_itLine );

				DisplayableChar l_character = { l_glyph, 0.0, Point2d( double( std::max( l_glyph->GetSize().width(), l_glyph->GetAdvance().width() ) ), double( std::max( l_glyph->GetSize().height(), l_glyph->GetAdvance().height() ) ) ) };

				if ( l_glyph->GetCharacter() == cuT( ' ' )
					 || l_glyph->GetCharacter() == cuT( '\t' ) )
				{
					// Write the word and leave space before next word.
					DoPrepareWord( p_renderSize, l_word, l_wordWidth, p_size, l_left, l_line, l_return );
					l_word.clear();
					l_wordWidth = 0;
					l_left += l_character.m_size[0];
				}
				else
				{
					l_word += l_glyph->GetCharacter();
					l_wordWidth += l_character.m_size[0];
				}
			}

			if ( !l_word.empty() )
			{
				DoPrepareWord( p_renderSize, l_word, l_wordWidth, p_size, l_left, l_line, l_return );
			}

			if ( !l_line.m_characters.empty() )
			{
				DoFinishLine( p_size, l_left, l_line, l_return );
			}
			else
			{
				l_line.m_position[1] += l_font->GetMaxHeight();
			}
		}

		DoAlignVertically( p_size[1], l_line.m_position[1] + l_font->GetMaxHeight(), l_return );
		return l_return;
	}

	void TextOverlay::DoPrepareWord( Size const & p_renderSize, std::u32string const & p_word, double p_wordWidth, Point2d const & p_size, double & p_left, DisplayableLine & p_line, DisplayableLineArray & p_lines )
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();
		FontSPtr l_font = l_fontTexture->GetFont();
		Position l_ovPosition = GetAbsolutePosition( p_renderSize );

		if ( p_left + p_wordWidth > p_size[0] && m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK_WORDS )
		{
			// The word will overflow the overlay size, so we jump to the next line,
			// and will write the word on this next line.
			DoFinishLine( p_size, p_left, p_line, p_lines );
		}

		for ( auto l_character : p_word )
		{
			Glyph * l_glyph = &l_font->GetGlyphAt( l_character );
			Point2d l_charSize( double( std::max( l_glyph->GetSize().width(), l_glyph->GetAdvance().width() ) ), double( std::max( l_glyph->GetSize().height(), l_glyph->GetAdvance().height() ) ) );

			p_left += l_glyph->GetPosition().x();

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
					DoFinishLine( p_size, p_left, p_line, p_lines );
				}
			}
			else if ( p_left + l_charSize[0] > p_size[0] )
			{
				// The character is partially out of the overlay.
				if ( m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK )
				{
					// Break => Jump to the next line.
					DoFinishLine( p_size, p_left, p_line, p_lines );
				}
			}

			if ( l_charSize[0] > 0 )
			{
				p_line.m_characters.push_back( { l_glyph, p_left, l_charSize } );
			}

			p_left += l_charSize[0];
		}
	}

	void TextOverlay::DoFinishLine( Point2d const & p_size, double & p_left, DisplayableLine & p_line, DisplayableLineArray & p_lines )
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();
		FontSPtr l_font = l_fontTexture->GetFont();
		uint32_t l_maxHeight = l_font->GetMaxHeight();
		double l_top = p_line.m_position[1] + l_maxHeight;
		p_line.m_width = p_left;
		DoAlignHorizontally( p_size[0], p_line, p_lines );
		p_left = 0;
		p_line.m_position = Point2d( 0, l_top );
	}

	void TextOverlay::DoAlignHorizontally( double p_width, DisplayableLine & p_line, DisplayableLineArray & p_lines )
	{
		if ( m_hAlign != eHALIGN_LEFT )
		{
			double l_offset = p_width - p_line.m_width;

			if ( m_hAlign == eHALIGN_CENTER )
			{
				l_offset /= 2;
			}

			for ( auto && l_character : p_line.m_characters )
			{
				l_character.m_left += l_offset;
			}
		}

		auto l_removed = std::remove_if( p_line.m_characters.begin(), p_line.m_characters.end(), [&p_width]( DisplayableChar const & p_char )
		{
			return p_char.m_left > p_width
				|| p_char.m_left + p_char.m_size[0] < 0;
		} );

		p_line.m_characters.erase( l_removed, p_line.m_characters.end() );

		p_lines.push_back( p_line );
		DisplayableLine l_line;
		std::swap( p_line, l_line );
	}

	void TextOverlay::DoAlignVertically( double p_height, double p_linesHeight, DisplayableLineArray & p_lines )
	{
		if ( m_vAlign != eVALIGN_TOP )
		{
			double l_offset = p_height - p_linesHeight;

			if ( m_vAlign == eVALIGN_CENTER )
			{
				l_offset /= 2;
			}

			for ( auto && l_line : p_lines )
			{
				l_line.m_position[1] += l_offset;
			}
		}

		uint32_t l_maxHeight = GetFontTexture()->GetFont()->GetMaxHeight();

		auto l_removed = std::remove_if( p_lines.begin(), p_lines.end(), [&p_height, &l_maxHeight]( DisplayableLine const & p_line )
		{
			return p_line.m_position[1] > p_height
				|| p_line.m_position[1] + l_maxHeight < 0;
		} );

		p_lines.erase( l_removed, p_lines.end() );
	}
}
