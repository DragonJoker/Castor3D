#include "TextOverlay.hpp"

#include "Engine.hpp"
#include "FontTexture.hpp"
#include "InitialiseEvent.hpp"
#include "Overlay.hpp"
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
		Engine * l_engine = m_pOverlay->GetOwner();
		FontManager & l_fontManager = l_engine->GetFontManager();
		FontSPtr l_pFont = l_fontManager.get_font( p_strFont );

		if ( l_pFont )
		{
			FontTextureSPtr l_fontTexture = l_engine->GetOverlayManager().GetFontTexture( l_pFont->GetName() );

			if ( !l_fontTexture )
			{
				l_fontTexture = l_engine->GetOverlayManager().CreateFontTexture( l_pFont );
			}

			m_fontTexture = l_fontTexture;
		}
		else
		{
			CASTOR_EXCEPTION( "Font " + string::string_cast< char >( p_strFont ) + "not found" );
		}

		l_engine->PostEvent( MakeInitialiseEvent( *this ) );
		m_textChanged = true;
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
			FontSPtr l_pFont = l_fontTexture->GetFont();

			if ( !m_strCaption.empty() && l_pFont )
			{
				if ( m_textChanged )
				{
					Point2d l_ovAbsSize = GetOverlay().GetAbsoluteSize();
					Point2d l_ptSize( p_size.width() * l_ovAbsSize[0], p_size.height() * l_ovAbsSize[1] );
					m_previousCaption = m_strCaption;
					m_arrayVtx.clear();
					m_arrayVtx.reserve( m_previousCaption.size() * 6 );
					Point2d l_ptPosition;

					StringArray l_lines = string::split( m_previousCaption, cuT( "\n" ), std::count( m_previousCaption.begin(), m_previousCaption.end(), cuT( '\n' ) ) + 1 );
					double l_lineWidth = 0;
					std::vector< OverlayCategory::VertexArray > l_linesVtx;
					OverlayCategory::VertexArray l_lineVtx;

					for ( StringArrayConstIt l_itLines = l_lines.begin(); l_itLines != l_lines.end() && l_ptPosition[1] < l_ptSize[1]; ++l_itLines )
					{
						String const & l_line = *l_itLines;
						double l_wordWidth = 0;
						std::u32string l_word;

						for ( string::utf8::iterator l_itLine = l_line.begin(); l_itLine != l_line.end() && l_ptPosition[1] < l_ptSize[1]; ++l_itLine )
						{
							char32_t const & l_character = *l_itLine;

							if ( l_pFont->HasGlyphAt( l_character ) )
							{
								Glyph const & l_glyph = l_pFont->GetGlyphAt( l_character );
								Point2d l_charSize( double( std::max( l_glyph.GetSize().width(), l_glyph.GetAdvance().width() ) ), double( std::max( l_glyph.GetSize().height(), l_glyph.GetAdvance().height() ) ) );

								if ( l_character == cuT( '\r' ) )
								{
									DoWriteWord( p_size, l_word, l_wordWidth, l_ptSize, l_ptPosition, l_lineWidth, l_lineVtx, l_linesVtx );
									l_ptPosition[0] = 0;
									l_wordWidth = 0;
								}
								else if ( l_character == cuT( ' ' ) )
								{
									DoWriteWord( p_size, l_word, l_wordWidth, l_ptSize, l_ptPosition, l_lineWidth, l_lineVtx, l_linesVtx );
									l_word.clear();
									l_wordWidth = 0;
									l_ptPosition[0] += l_charSize[0];
								}
								else if ( l_character == cuT( '\t' ) )
								{
									DoWriteWord( p_size, l_word, l_wordWidth, l_ptSize, l_ptPosition, l_lineWidth, l_lineVtx, l_linesVtx );
									l_word.clear();
									l_wordWidth = 0;
									l_ptPosition[0] += l_charSize[0];
								}
								else
								{
									l_word += l_character;
									l_wordWidth += l_charSize[0];
								}
							}
							else
							{
								Glyph const & l_glyph = l_pFont->GetGlyphAt( cuT( '_' ) );
								Point2d l_charSize( double( std::max( l_glyph.GetSize().width(), l_glyph.GetAdvance().width() ) ), double( std::max( l_glyph.GetSize().height(), l_glyph.GetAdvance().height() ) ) );
								l_word += cuT( '_' );
								l_wordWidth += l_charSize[0];
							}
						}

						if ( !l_word.empty() )
						{
							DoWriteWord( p_size, l_word, l_wordWidth, l_ptSize, l_ptPosition, l_lineWidth, l_lineVtx, l_linesVtx );

							if ( !l_lineVtx.empty() )
							{
								DoAlignHorizontally( l_ptSize[0], l_lineWidth, l_lineVtx, l_linesVtx );
							}
						}

						l_linesVtx.push_back( l_lineVtx );
						l_ptPosition[0] = 0;
						l_ptPosition[1] += l_pFont->GetMaxHeight();
					}

					DoAlignVertically( l_ptSize[1], l_ptPosition[1], l_linesVtx );

					for ( auto && l_lineVtx : l_linesVtx )
					{
						m_arrayVtx.insert( m_arrayVtx.end(), l_lineVtx.begin(), l_lineVtx.end() );
					}

					// TODO : Check for glyphs that need to be loaded and added to the texture
					m_textChanged = false;
				}
			}
		}
	}

	void TextOverlay::DoWriteWord( Size const & p_renderSize, std::u32string const & p_word, double p_wordWidth, Point2d const & p_size, Point2d & p_position, double & p_lineWidth, OverlayCategory::VertexArray & p_lineVtx, std::vector< OverlayCategory::VertexArray > & p_linesVtx )
	{
		FontTextureSPtr l_fontTexture = GetFontTexture();
		FontSPtr l_pFont = l_fontTexture->GetFont();
		Size const & l_texDim = l_fontTexture->GetTexture()->GetDimensions();
		Position l_ovPosition = GetAbsolutePosition( p_renderSize );
		uint32_t l_maxHeight = l_pFont->GetMaxHeight();

		if ( p_position[0] + p_wordWidth > p_size[0] && m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK_WORDS )
		{
			p_position[0] = 0;
			p_position[1] += l_maxHeight;
		}

		for ( auto l_it = p_word.begin(); l_it != p_word.end() && p_position[1] < p_size[1]; ++l_it )
		{
			char32_t const & l_character = *l_it;
			double l_charCrop = 0;
			Glyph const & l_glyph = l_pFont->GetGlyphAt( l_character );
			Point2d l_charSize( double( std::max( l_glyph.GetSize().width(), l_glyph.GetAdvance().width() ) ), double( std::max( l_glyph.GetSize().height(), l_glyph.GetAdvance().height() ) ) );

			p_position[0] += l_glyph.GetPosition().x();

			if ( p_position[0] > p_size[0] )
			{
				if ( m_wrappingMode == eTEXT_WRAPPING_MODE_NONE )
				{
					l_charSize[0] = 0;
				}
				else if ( m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK )
				{
					p_position[0] = 0;
					p_position[1] += l_maxHeight;
					DoAlignHorizontally( p_size[0], p_lineWidth, p_lineVtx, p_linesVtx );
				}
			}
			else if ( p_position[0] + l_charSize[0] > p_size[0] )
			{
				if ( m_wrappingMode == eTEXT_WRAPPING_MODE_NONE )
				{
					l_charSize[0] = p_size[0] - p_position[0];
				}
				else if ( m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK )
				{
					p_position[0] = 0;
					p_position[1] += l_maxHeight;
					DoAlignHorizontally( p_size[0], p_lineWidth, p_lineVtx, p_linesVtx );
				}
			}

			if ( p_position[1] > p_size[1] )
			{
				l_charSize[1] = 0;
			}
			else if ( p_position[1] + l_pFont->GetMaxHeight() > p_size[1] )
			{
				l_charCrop = p_position[1] + l_pFont->GetMaxHeight() - p_size[1];
			}

			if ( l_charSize[1] > 0 && l_charCrop < l_charSize[1] && l_charSize[0] > 0 )
			{
				Point2d l_position( p_position[0], ( p_position[1] + l_pFont->GetMaxHeight() - ( l_glyph.GetPosition().y() ) ) );
				double l_width = l_charSize[0];
				double l_height = ( l_charSize[1] - l_charCrop );
				Position l_uvPosition = l_fontTexture->GetGlyphPosition( l_character );
				double l_uvX = double( l_uvPosition.x() ) / l_texDim.width();
				double l_uvY = double( l_uvPosition.y() ) / l_texDim.height();
				double l_uvStepX = l_charSize[0] / l_texDim.width();
				double l_uvStepY = l_charSize[1] / l_texDim.height();
				double l_uvCrop = l_charCrop / l_texDim.height();
				p_lineWidth += l_width;

				OverlayCategory::Vertex l_vertexTR = { { int32_t( l_ovPosition.x() + ( l_position[0] + l_width ) ), int32_t( l_ovPosition.y() + ( l_position[1] ) ),           }, { real( l_uvX + l_uvStepX ), real( l_uvY + l_uvStepY ) } };
				OverlayCategory::Vertex l_vertexTL = { { int32_t( l_ovPosition.x() + ( l_position[0] ) ),           int32_t( l_ovPosition.y() + ( l_position[1] ) ),           }, { real( l_uvX ),             real( l_uvY + l_uvStepY ) } };
				OverlayCategory::Vertex l_vertexBL = { { int32_t( l_ovPosition.x() + ( l_position[0] ) ),           int32_t( l_ovPosition.y() + ( l_position[1] + l_height ) ) }, { real( l_uvX ),             real( l_uvY + l_uvCrop ) } };
				OverlayCategory::Vertex l_vertexBR = { { int32_t( l_ovPosition.x() + ( l_position[0] + l_width ) ), int32_t( l_ovPosition.y() + ( l_position[1] + l_height ) ) }, { real( l_uvX + l_uvStepX ), real( l_uvY + l_uvCrop ) } };

				p_lineVtx.push_back( l_vertexBL );
				p_lineVtx.push_back( l_vertexBR );
				p_lineVtx.push_back( l_vertexTL );

				p_lineVtx.push_back( l_vertexTR );
				p_lineVtx.push_back( l_vertexTL );
				p_lineVtx.push_back( l_vertexBR );
			}

			p_position[0] += l_charSize[0];
		}
	}

	void TextOverlay::DoAlignHorizontally( double p_width, double & p_lineWidth, OverlayCategory::VertexArray & p_lineVtx, std::vector< OverlayCategory::VertexArray > & p_linesVtx )
	{
		if ( m_hAlign != eHALIGN_LEFT )
		{
			double l_offset = p_width - p_lineWidth;

			if ( m_hAlign == eHALIGN_CENTER )
			{
				l_offset /= 2;
			}

			for ( auto && l_vertex : p_lineVtx )
			{
				int32_t * l_position = l_vertex.coords;
				l_position[0] += int32_t( l_offset );
			}
		}

		p_linesVtx.push_back( p_lineVtx );
		p_lineVtx.clear();
		p_lineWidth = 0;
	}

	void TextOverlay::DoAlignVertically( double p_height, double p_linesHeight, std::vector< OverlayCategory::VertexArray > & p_linesVtx )
	{
		if ( m_vAlign != eVALIGN_TOP )
		{
			double l_offset = 0;

			if ( m_vAlign == eVALIGN_CENTER )
			{
				l_offset = ( p_height - p_linesHeight ) / 2;
			}
			else
			{
				l_offset = p_height - p_linesHeight;
			}

			for ( auto && l_lineVtx : p_linesVtx )
			{
				for ( auto && l_vertex : l_lineVtx )
				{
					int32_t * l_position = l_vertex.coords;
					l_position[1] += int32_t( l_offset );
				}
			}
		}
	}
}
