#include "FontTexture.hpp"

#include "Engine.hpp"

#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	FontTexture::FontTexture( Engine & engine, FontSPtr p_font )
		: OwnedBy< Engine >( engine )
		, m_font( p_font )
	{
		uint32_t const maxWidth = p_font->getMaxWidth();
		uint32_t const maxHeight = p_font->getMaxHeight();
		uint32_t const count = uint32_t( std::ceil( std::distance( p_font->begin(), p_font->end() ) / 16.0 ) );

		SamplerSPtr sampler = getEngine()->getSamplerCache().add( p_font->getName() );
		sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToEdge );
		sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToEdge );
		sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		m_sampler = sampler;
		m_texture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions, AccessType::eWrite, AccessType::eRead, PixelFormat::eL8, Size{ maxWidth * 16, maxHeight * count } );
		m_texture->getImage().initialiseSource();
	}

	FontTexture::~FontTexture()
	{
	}

	void FontTexture::initialise()
	{
		m_texture->initialise();
		m_texture->bind( 0 );
		m_texture->generateMipmaps();
		m_texture->unbind( 0 );
		onChanged( *this );
	}

	void FontTexture::update()
	{
		FontSPtr font = getFont();

		if ( font )
		{
			uint32_t const maxWidth = font->getMaxWidth();
			uint32_t const maxHeight = font->getMaxHeight();
			uint32_t const count = uint32_t( std::ceil( std::distance( font->begin(), font->end() ) / 16.0 ) );
			Size size{ maxWidth * 16, maxHeight * count };
			auto & image = m_texture->getImage();
			image.initialiseSource( PxBufferBase::create( Size( maxWidth * 16, maxHeight * count ), PixelFormat::eL8 ) );

			auto it = font->begin();
			Size const & sizeImg = size;
			uint32_t const uiTotalWidth = sizeImg.getWidth();
			uint32_t uiOffY = sizeImg.getHeight() - maxHeight;
			uint8_t * pBuffer = image.getBuffer()->ptr();
			size_t const bufsize = image.getBuffer()->size();

			for ( uint32_t y = 0; y < count && it != font->end(); ++y )
			{
				uint32_t uiOffX = 0;

				for ( uint32_t x = 0; x < 16 && it != font->end(); ++x )
				{
					Glyph const & glyph = *it;
					Size const & size = glyph.getSize();
					ByteArray const & buffer = glyph.getBitmap();
					uint32_t const dstLineIndex = ( uiTotalWidth * uiOffY ) + uiOffX;
					uint8_t * dstLineBuffer = &pBuffer[dstLineIndex];

					for ( uint32_t i = 0; i < size.getHeight(); ++i )
					{
						ENSURE( dstLineIndex + size.getWidth() <= bufsize );
						std::memcpy( dstLineBuffer, &buffer[i * size.getWidth()], size.getWidth() );
						dstLineBuffer += uiTotalWidth;
					}

					m_glyphsPositions[glyph.getCharacter()] = Position( uiOffX, uiOffY );
					uiOffX += maxWidth;
					++it;
				}

				uiOffY -= maxHeight;
			}
		}

	}

	void FontTexture::cleanup()
	{
		if ( m_texture )
		{
			m_texture->cleanup();
		}
	}

	String const & FontTexture::getFontName()const
	{
		return getFont()->getName();
	}

	Position const & FontTexture::getGlyphPosition( char32_t p_char )const
	{
		GlyphPositionMapConstIt it = m_glyphsPositions.find( p_char );

		if ( it == m_glyphsPositions.end() )
		{
			CASTOR_EXCEPTION( std::string( "No loaded glyph for character " ) + string::stringCast< char >( string::toString( p_char ) ) );
		}

		return it->second;
	}
}
