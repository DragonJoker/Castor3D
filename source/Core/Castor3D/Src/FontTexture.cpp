#include "FontTexture.hpp"

#include "StaticTexture.hpp"
#include "Engine.hpp"
#include "InitialiseEvent.hpp"
#include "RenderSystem.hpp"
#include "SamplerManager.hpp"

#include <Font.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	FontTexture::FontTexture( Engine & p_engine, FontSPtr p_font )
		: OwnedBy< Engine >( p_engine )
		, m_font( p_font )
	{
		// Récupération / Création de la police
		FontManager & l_fontManager = GetOwner()->GetFontManager();
		SamplerSPtr l_pSampler = GetOwner()->GetSamplerManager().Create( p_font->GetName() );
		l_pSampler->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE_CLAMP_TO_EDGE );
		l_pSampler->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE_CLAMP_TO_EDGE );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE_LINEAR );
		l_pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE_LINEAR );
		m_sampler = l_pSampler;
		m_texture = GetOwner()->GetRenderSystem()->CreateStaticTexture();
		m_texture->SetType( eTEXTURE_TYPE_2D );
		m_texture->SetSampler( l_pSampler );
	}

	FontTexture::~FontTexture()
	{
	}

	void FontTexture::Initialise()
	{
		FontSPtr l_font = GetFont();

		if ( l_font )
		{
			uint32_t l_uiMaxWidth = l_font->GetMaxWidth();
			uint32_t l_uiMaxHeight = l_font->GetMaxHeight();
			uint32_t l_count = uint32_t( std::ceil( std::distance( l_font->begin(), l_font->end() ) / 16.0 ) );
			m_texture->SetImage( PxBufferBase::create( Size( l_uiMaxWidth * 16, l_uiMaxHeight * l_count ), ePIXEL_FORMAT_L8 ) );

			Castor::Font::GlyphMap::const_iterator l_it = l_font->begin();
			Size l_sizeImg = m_texture->GetDimensions();
			uint32_t l_uiTotalWidth = l_sizeImg.width();
			uint32_t l_uiOffY = l_sizeImg.height() - l_uiMaxHeight;
			uint8_t * l_pBuffer = m_texture->GetBuffer()->ptr();
			size_t l_bufsize = m_texture->GetBuffer()->size();

			for ( uint32_t y = 0; y < l_count && l_it != l_font->end(); ++y )
			{
				uint32_t l_uiOffX = 0;

				for ( uint32_t x = 0; x < 16 && l_it != l_font->end(); ++x )
				{
					Glyph & l_glyph = *l_it->second;
					Size l_size = l_glyph.GetSize();
					ByteArray l_buffer = l_glyph.GetBitmap();
					uint32_t l_dstLineIndex = ( l_uiTotalWidth * l_uiOffY ) + l_uiOffX;
					uint8_t * l_dstLineBuffer = &l_pBuffer[l_dstLineIndex];

					for ( uint32_t i = 0; i < l_size.height(); ++i )
					{
						CASTOR_ASSERT( l_dstLineIndex + l_size.width() <= l_bufsize );
						std::memcpy( l_dstLineBuffer, &l_buffer[i * l_size.width()], l_size.width() );
						l_dstLineBuffer += l_uiTotalWidth;
					}

					m_glyphsPositions[l_glyph.GetCharacter()] = Position( l_uiOffX, l_uiOffY );
					l_uiOffX += l_uiMaxWidth;
					++l_it;
				}

				l_uiOffY -= l_uiMaxHeight;
			}
		}

		m_texture->Create();
		m_texture->Initialise( 0 );
		m_texture->Bind();
		m_texture->GenerateMipmaps();
		m_texture->Unbind();

#if DEBUG_BUFFERS

		uint8_t * l_buffer = m_texture->Lock( eLOCK_FLAG_READ_ONLY );
		std::memcpy( m_texture->GetBuffer()->ptr(), l_buffer, m_texture->GetBuffer()->size() );
		const Image l_tmp( cuT( "tmp" ), *m_texture->GetBuffer() );
		Image::BinaryLoader()( l_tmp, Engine::GetEngineDirectory() / cuT( "Font_" ) + l_font->GetName() + cuT( ".bmp" ) );

#endif
	}

	void FontTexture::Cleanup()
	{
		if ( m_texture )
		{
			m_texture->Cleanup();
			m_texture->Destroy();
		}
	}

	String const & FontTexture::GetFontName()const
	{
		return GetFont()->GetName();
	}

	Position const & FontTexture::GetGlyphPosition( char32_t p_char )const
	{
		GlyphPositionMapConstIt l_it = m_glyphsPositions.find( p_char );

		if ( l_it == m_glyphsPositions.end() )
		{
			CASTOR_EXCEPTION( std::string( "No loaded glyph for character " ) + string::string_cast< char >( string::to_string( p_char ) ) );
		}

		return l_it->second;
	}
}
