#include "TextOverlay.hpp"
#include "InitialiseEvent.hpp"
#include "Overlay.hpp"
#include "OverlayRenderer.hpp"
#include "Engine.hpp"
#include "Sampler.hpp"
#include "DynamicTexture.hpp"

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

		bool l_bReturn = p_file.WriteText( l_strTabs + cuT( "text_overlay " ) + p_overlay.GetOverlayName() + cuT( "\n" ) + l_strTabs + cuT( "{\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tfont " ) + p_overlay.GetFontName() ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tcaption " ) + p_overlay.GetCaption() ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = Overlay::TextLoader()( p_overlay.GetOverlay(), p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	TextOverlay::BinaryParser::BinaryParser( Path const & p_path )
		:	OverlayCategory::BinaryParser( p_path )
	{
	}

	bool TextOverlay::BinaryParser::Fill( TextOverlay const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetFontName(), eCHUNK_TYPE_OVERLAY_FONT, p_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetCaption(), eCHUNK_TYPE_OVERLAY_CAPTION, p_chunk );
		}

		return l_bReturn;
	}

	bool TextOverlay::BinaryParser::Parse( TextOverlay & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;

		switch ( p_chunk.GetChunkType() )
		{
		case eCHUNK_TYPE_OVERLAY_FONT:
			l_bReturn = DoParseChunk( l_name, p_chunk );

			if ( l_bReturn )
			{
				p_obj.SetFont( l_name );
			}

			break;

		case eCHUNK_TYPE_OVERLAY_CAPTION:
			l_bReturn = DoParseChunk( l_name, p_chunk );

			if ( l_bReturn )
			{
				p_obj.SetCaption( l_name );
			}

			break;

		default:
			l_bReturn = false;
			break;
		}

		if ( !l_bReturn )
		{
			p_chunk.EndParse();
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	TextOverlay::TextOverlay()
		: OverlayCategory( eOVERLAY_TYPE_TEXT )
		, m_wrappingMode( eTEXT_WRAPPING_MODE_NONE )
	{
	}

	TextOverlay::~TextOverlay()
	{
		m_pTexture.reset();
		m_wpFont.reset();
		m_wpSampler.reset();
	}

	OverlayCategorySPtr TextOverlay::Create()
	{
		return std::make_shared< TextOverlay >();
	}

	bool TextOverlay::Initialise()
	{
		FontSPtr l_pFont = GetFont();

		if ( l_pFont )
		{
			uint32_t l_uiMaxWidth = l_pFont->GetMaxWidth();
			uint32_t l_uiMaxHeight = l_pFont->GetMaxHeight();
			uint32_t l_uiCount = uint32_t( std::ceil( std::distance( l_pFont->Begin(), l_pFont->End() ) / 16.0 ) );
			m_pTexture->SetImage( Size( l_uiMaxWidth * 16, l_uiMaxHeight * l_uiCount ), ePIXEL_FORMAT_L8 );

			Castor::Font::GlyphMap::const_iterator l_it = l_pFont->Begin();
			Size l_sizeImg = m_pTexture->GetDimensions();
			uint32_t l_uiTotalWidth = l_sizeImg.width();
			uint32_t l_uiTotalHeight = l_sizeImg.height();
			uint32_t l_uiOffY = l_uiTotalHeight - l_uiMaxHeight;
			uint8_t * l_pBuffer = m_pTexture->GetBuffer()->ptr();
			size_t l_bufsize = m_pTexture->GetBuffer()->size();

			for ( uint32_t y = 0; y < l_uiCount && l_it != l_pFont->End(); ++y )
			{
				uint32_t l_uiOffX = 0;

				for ( uint32_t x = 0; x < l_uiCount && l_it != l_pFont->End(); ++x )
				{
					Glyph * l_glyph = l_it->second;
					Size l_size = l_glyph->GetSize();
					ByteArray l_buffer = l_glyph->GetBitmap();

					for ( uint32_t i = 0; i < l_size.height(); ++i )
					{
						CASTOR_ASSERT( ( l_uiTotalWidth * ( l_uiOffY + i ) ) + l_uiOffX + l_size.width() <= l_bufsize );
						std::memcpy( &l_pBuffer[( l_uiTotalWidth * ( l_uiOffY + i ) ) + l_uiOffX], &l_buffer[i * l_size.width()], l_size.width() );
					}

					m_glyphsPositions[l_glyph->GetCharacter()] = Position( l_uiOffX, l_uiOffY );
					l_uiOffX += l_uiMaxWidth;
					++l_it;
				}

				l_uiOffY -= l_uiMaxHeight;
			}

//			Castor::Image l_img( cuT( "" ), *m_pTexture->GetBuffer() );
//			Castor::Image::BinaryLoader()( const_cast< Image const & >( l_img ), l_pFont->GetName() + cuT( ".bmp" ) );
		}

		m_pTexture->Create();
		m_pTexture->Initialise( 0 );

		return true;
	}

	void TextOverlay::Cleanup()
	{
		if ( m_pTexture )
		{
			m_pTexture->Cleanup();
			m_pTexture->Destroy();
		}
	}

	void TextOverlay::SetFont( String const & p_strFont )
	{
		// Récupération / Création de la police
		Engine * l_engine = m_pOverlay->GetEngine();
		FontCollection & l_fontCollection = l_engine->GetFontManager();
		FontSPtr l_pFont = l_fontCollection.find( p_strFont );

		if ( l_pFont )
		{
			m_wpFont = l_pFont;
			SamplerSPtr l_pSampler = l_engine->CreateSampler( p_strFont );
			l_pSampler->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_EDGE );
			l_pSampler->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_EDGE );
			l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
			l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
			m_wpSampler = l_pSampler;
			m_pTexture = l_engine->GetRenderSystem()->CreateDynamicTexture();
			m_pTexture->SetDimension( eTEXTURE_DIMENSION_2D );
			m_pTexture->SetSampler( l_pSampler );
		}
		else
		{
			CASTOR_EXCEPTION( "Font " + str_utils::to_str( p_strFont ) + "not found" );
		}

		m_pOverlay->GetEngine()->PostEvent( std::make_shared< InitialiseEvent< TextOverlay > >( *this ) );
		m_strFontName = p_strFont;
		m_changed = true;
	}

	void TextOverlay::SetMaterial( MaterialSPtr p_pMaterial )
	{
		OverlayCategory::SetMaterial( p_pMaterial );
		m_changed = true;
	}

	String const & TextOverlay::GetFontName()const
	{
		return GetFont()->GetName();
	}

	Position const & TextOverlay::GetGlyphPosition( xchar p_char )const
	{
		GlyphPositionMapConstIt l_it = m_glyphsPositions.find( p_char );

		if ( l_it == m_glyphsPositions.end() )
		{
			CASTOR_EXCEPTION( std::string( "No loaded glyph for character " ) + str_utils::to_str( str_utils::to_string( p_char ) ) );
		}

		return l_it->second;
	}

	void TextOverlay::DoRender( OverlayRendererSPtr p_renderer )
	{
		p_renderer->DrawText( *this );
	}

	void TextOverlay::DoUpdate( OverlayRendererSPtr p_renderer )
	{
		FontSPtr l_pFont = GetFont();

		if ( !m_strCaption.empty() && l_pFont )
		{
			if ( m_previousCaption != m_strCaption )
			{
				Point2d l_ovAbsSize = GetOverlay().GetAbsoluteSize();
				Point2d l_ptSize( p_renderer->GetSize().width() * l_ovAbsSize[0], p_renderer->GetSize().height() * l_ovAbsSize[1] );
				Size l_screenSize = Size( uint32_t( l_ptSize[0] ), uint32_t( l_ptSize[1] ) );
				m_previousCaption = m_strCaption;
				int l_zIndex = 1000 - GetOverlay().GetZIndex();
				m_arrayVtx.clear();
				m_arrayVtx.reserve( m_previousCaption.size() * 6 );
				Point2d l_ptPosition;

				StringArray l_lines = str_utils::split( m_previousCaption, cuT( "\n" ), std::count( m_previousCaption.begin(), m_previousCaption.end(), cuT( '\n' ) ) + 1 );

				for ( StringArrayConstIt l_itLines = l_lines.begin(); l_itLines != l_lines.end() && l_ptPosition[1] < l_ptSize[1]; ++l_itLines )
				{
					String const & l_line = *l_itLines;
					double l_wordWidth = 0;
					String l_word;

					for ( String::const_iterator l_itLine = l_line.begin(); l_itLine != l_line.end() && l_ptPosition[1] < l_ptSize[1]; ++l_itLine )
					{
						xchar const & l_character = *l_itLine;
						Glyph const & l_glyph = l_pFont->GetGlyphAt( l_character );
						Point2d l_charSize( double( std::max( l_glyph.GetSize().width(), l_glyph.GetAdvance().width() ) ), double( std::max( l_glyph.GetSize().height(), l_glyph.GetAdvance().height() ) ) );

						if ( l_character == cuT( '\r' ) )
						{
							DoWriteWord( p_renderer, l_word, l_wordWidth, l_ptSize, l_ptPosition );
							l_ptPosition[0] = 0;
							l_wordWidth = 0;
						}
						else if ( l_character == cuT( ' ' ) )
						{
							DoWriteWord( p_renderer, l_word, l_wordWidth, l_ptSize, l_ptPosition );
							l_word.clear();
							l_wordWidth = 0;
							l_ptPosition[0] += l_charSize[0];
						}
						else if ( l_character == cuT( '\t' ) )
						{
							DoWriteWord( p_renderer, l_word, l_wordWidth, l_ptSize, l_ptPosition );
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

					if ( !l_word.empty() )
					{
						DoWriteWord( p_renderer, l_word, l_wordWidth, l_ptSize, l_ptPosition );
					}

					l_ptPosition[0] = 0;
					l_ptPosition[1] += l_pFont->GetMaxHeight();
				}

				// TODO : Check for glyphs that need to be loaded and added to the texture
			}
		}
	}

	void TextOverlay::DoWriteWord( OverlayRendererSPtr p_renderer, String const & p_word, double p_wordWidth, Point2d const & p_size, Point2d & p_position )
	{
		FontSPtr l_pFont = GetFont();
		int l_zIndex = 1000 - GetOverlay().GetZIndex();
		Size const & l_texDim = m_pTexture->GetDimensions();
		Position l_ovPosition = GetAbsolutePosition( p_renderer->GetSize() );

		if ( p_position[0] + p_wordWidth > p_size[0] && m_wrappingMode == eTEXT_WRAPPING_MODE_BREAK_WORDS )
		{
			p_position[0] = 0;
			p_position[1] += l_pFont->GetMaxHeight();
		}

		for ( String::const_iterator l_it = p_word.begin(); l_it != p_word.end() && p_position[1] < p_size[1]; ++l_it )
		{
			xchar const & l_character = *l_it;
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
					p_position[1] += l_pFont->GetMaxHeight();
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
					p_position[1] += l_pFont->GetMaxHeight();
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
				Position l_uvPosition = GetGlyphPosition( l_character );
				double l_uvX = double( l_uvPosition.x() ) / l_texDim.width();
				double l_uvY = double( l_uvPosition.y() ) / l_texDim.height();
				double l_uvStepX = l_charSize[0] / l_texDim.height();
				double l_uvStepY = l_charSize[1] / l_texDim.height();
				double l_uvCrop = l_charCrop / l_texDim.height();

				OverlayCategory::Vertex l_vertexTR = { { int32_t( l_ovPosition.x() + ( l_position[0] + l_width ) ), int32_t( l_ovPosition.y() + ( l_position[1] ) ),            l_zIndex }, { real( l_uvX + l_uvStepX ), real( l_uvY + l_uvStepY ) } };
				OverlayCategory::Vertex l_vertexTL = { { int32_t( l_ovPosition.x() + ( l_position[0] ) ),           int32_t( l_ovPosition.y() + ( l_position[1] ) ),            l_zIndex }, { real( l_uvX ),             real( l_uvY + l_uvStepY ) } };
				OverlayCategory::Vertex l_vertexBL = { { int32_t( l_ovPosition.x() + ( l_position[0] ) ),           int32_t( l_ovPosition.y() + ( l_position[1] + l_height ) ), l_zIndex }, { real( l_uvX ),             real( l_uvY + l_uvCrop ) } };
				OverlayCategory::Vertex l_vertexBR = { { int32_t( l_ovPosition.x() + ( l_position[0] + l_width ) ), int32_t( l_ovPosition.y() + ( l_position[1] + l_height ) ), l_zIndex }, { real( l_uvX + l_uvStepX ), real( l_uvY + l_uvCrop ) } };

				m_arrayVtx.push_back( l_vertexBL );
				m_arrayVtx.push_back( l_vertexBR );
				m_arrayVtx.push_back( l_vertexTL );

				m_arrayVtx.push_back( l_vertexTR );
				m_arrayVtx.push_back( l_vertexTL );
				m_arrayVtx.push_back( l_vertexBR );
			}

			p_position[0] += l_charSize[0];
		}
	}
}
