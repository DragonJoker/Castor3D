#include "FontTexture.hpp"

#include "Engine.hpp"
#include "SamplerCache.hpp"

#include "Event/Frame/InitialiseEvent.hpp"
#include "Render/RenderSystem.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	FontTexture::FontTexture( Engine & p_engine, FontSPtr p_font )
		: OwnedBy< Engine >( p_engine )
		, m_font( p_font )
	{
		uint32_t const l_maxWidth = p_font->GetMaxWidth();
		uint32_t const l_maxHeight = p_font->GetMaxHeight();
		uint32_t const l_count = uint32_t( std::ceil( std::distance( p_font->begin(), p_font->end() ) / 16.0 ) );

		SamplerSPtr l_sampler = GetEngine()->GetSamplerCache().Add( p_font->GetName() );
		l_sampler->SetWrappingMode( TextureUVW::U, WrapMode::ClampToEdge );
		l_sampler->SetWrappingMode( TextureUVW::V, WrapMode::ClampToEdge );
		l_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
		l_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
		m_sampler = l_sampler;
		m_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::Write, AccessType::Read, PixelFormat::eL8, Size{ l_maxWidth * 16, l_maxHeight * l_count } );
		m_texture->GetImage().InitialiseSource();
	}

	FontTexture::~FontTexture()
	{
	}

	void FontTexture::Initialise()
	{
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
			uint32_t const l_maxWidth = l_font->GetMaxWidth();
			uint32_t const l_maxHeight = l_font->GetMaxHeight();
			uint32_t const l_count = uint32_t( std::ceil( std::distance( l_font->begin(), l_font->end() ) / 16.0 ) );
			Size l_size{ l_maxWidth * 16, l_maxHeight * l_count };
			auto & l_image = m_texture->GetImage();
			l_image.InitialiseSource( PxBufferBase::create( Size( l_maxWidth * 16, l_maxHeight * l_count ), PixelFormat::eL8 ) );

			auto l_it = l_font->begin();
			Size const & l_sizeImg = l_size;
			uint32_t const l_uiTotalWidth = l_sizeImg.width();
			uint32_t l_uiOffY = l_sizeImg.height() - l_maxHeight;
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
					l_uiOffX += l_maxWidth;
					++l_it;
				}

				l_uiOffY -= l_maxHeight;
			}
		}

	}

	void FontTexture::Cleanup()
	{
		if ( m_texture )
		{
			m_texture->Cleanup();
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
