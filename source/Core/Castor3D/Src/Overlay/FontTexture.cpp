#include "FontTexture.hpp"

#include "Engine.hpp"

#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

using namespace Castor;

namespace Castor3D
{
	FontTexture::FontTexture( Engine & engine, FontSPtr p_font )
		: OwnedBy< Engine >( engine )
		, m_font( p_font )
	{
		uint32_t const maxWidth = p_font->GetMaxWidth();
		uint32_t const maxHeight = p_font->GetMaxHeight();
		uint32_t const count = uint32_t( std::ceil( std::distance( p_font->begin(), p_font->end() ) / 16.0 ) );

		SamplerSPtr sampler = GetEngine()->GetSamplerCache().Add( p_font->GetName() );
		sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		sampler->SetWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler = sampler;
		m_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eWrite, AccessType::eRead, PixelFormat::eL8, Size{ maxWidth * 16, maxHeight * count } );
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
		onChanged( *this );
	}

	void FontTexture::Update()
	{
		FontSPtr font = GetFont();

		if ( font )
		{
			uint32_t const maxWidth = font->GetMaxWidth();
			uint32_t const maxHeight = font->GetMaxHeight();
			uint32_t const count = uint32_t( std::ceil( std::distance( font->begin(), font->end() ) / 16.0 ) );
			Size size{ maxWidth * 16, maxHeight * count };
			auto & image = m_texture->GetImage();
			image.InitialiseSource( PxBufferBase::create( Size( maxWidth * 16, maxHeight * count ), PixelFormat::eL8 ) );

			auto it = font->begin();
			Size const & sizeImg = size;
			uint32_t const uiTotalWidth = sizeImg.width();
			uint32_t uiOffY = sizeImg.height() - maxHeight;
			uint8_t * pBuffer = image.GetBuffer()->ptr();
			size_t const bufsize = image.GetBuffer()->size();

			for ( uint32_t y = 0; y < count && it != font->end(); ++y )
			{
				uint32_t uiOffX = 0;

				for ( uint32_t x = 0; x < 16 && it != font->end(); ++x )
				{
					Glyph const & glyph = *it;
					Size const & size = glyph.GetSize();
					ByteArray const & buffer = glyph.GetBitmap();
					uint32_t const dstLineIndex = ( uiTotalWidth * uiOffY ) + uiOffX;
					uint8_t * dstLineBuffer = &pBuffer[dstLineIndex];

					for ( uint32_t i = 0; i < size.height(); ++i )
					{
						ENSURE( dstLineIndex + size.width() <= bufsize );
						std::memcpy( dstLineBuffer, &buffer[i * size.width()], size.width() );
						dstLineBuffer += uiTotalWidth;
					}

					m_glyphsPositions[glyph.GetCharacter()] = Position( uiOffX, uiOffY );
					uiOffX += maxWidth;
					++it;
				}

				uiOffY -= maxHeight;
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
		GlyphPositionMapConstIt it = m_glyphsPositions.find( p_char );

		if ( it == m_glyphsPositions.end() )
		{
			CASTOR_EXCEPTION( std::string( "No loaded glyph for character " ) + string::string_cast< char >( string::to_string( p_char ) ) );
		}

		return it->second;
	}
}
