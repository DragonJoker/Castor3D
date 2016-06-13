#include "FontTexture.hpp"

#include "Engine.hpp"
#include "SamplerManager.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

#include <Font.hpp>
#include <Image.hpp>

using namespace Castor;

namespace Castor3D
{
	FontTexture::FontTexture( Engine & p_engine, FontSPtr p_font )
		: OwnedBy< Engine >( p_engine )
		, m_font( p_font )
	{
		SamplerSPtr l_pSampler = GetEngine()->GetSamplerManager().Create( p_font->GetName() );
		l_pSampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToEdge );
		l_pSampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToEdge );
		l_pSampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
		l_pSampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
		m_sampler = l_pSampler;
		m_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_WRITE, eACCESS_TYPE_READ );
	}

	FontTexture::~FontTexture()
	{
	}

	void FontTexture::Initialise()
	{
		m_texture->Create();
		m_texture->Initialise();
		m_texture->Bind( 0 );
		m_texture->GenerateMipmaps();
		m_texture->Unbind( 0 );
		m_signal( *this );
	}

	void FontTexture::Update()
	{
		FontSPtr l_font = GetFont();

		if ( l_font )
		{
			uint32_t const l_uiMaxWidth = l_font->GetMaxWidth();
			uint32_t const l_uiMaxHeight = l_font->GetMaxHeight();
			uint32_t const l_count = uint32_t( std::ceil( std::distance( l_font->begin(), l_font->end() ) / 16.0 ) );
			auto & l_image = m_texture->GetImage();
			l_image.SetSource( PxBufferBase::create( Size( l_uiMaxWidth * 16, l_uiMaxHeight * l_count ), ePIXEL_FORMAT_L8 ) );

			auto l_it = l_font->begin();
			Size const & l_sizeImg = l_image.GetDimensions();
			uint32_t const l_uiTotalWidth = l_sizeImg.width();
			uint32_t l_uiOffY = l_sizeImg.height() - l_uiMaxHeight;
			uint8_t * l_pBuffer = l_image.GetBuffer()->ptr();
			size_t const l_bufsize = l_image.GetBuffer()->size();

			for ( uint32_t y = 0; y < l_count && l_it != l_font->end(); ++y )
			{
				uint32_t l_uiOffX = 0;

				for ( uint32_t x = 0; x < 16 && l_it != l_font->end(); ++x )
				{
					Glyph const & l_glyph = *l_it;
					Size const & l_size = l_glyph.GetSize();
					ByteArray const & l_buffer = l_glyph.GetBitmap();
					uint32_t const l_dstLineIndex = ( l_uiTotalWidth * l_uiOffY ) + l_uiOffX;
					uint8_t * l_dstLineBuffer = &l_pBuffer[l_dstLineIndex];

					for ( uint32_t i = 0; i < l_size.height(); ++i )
					{
						ENSURE( l_dstLineIndex + l_size.width() <= l_bufsize );
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
