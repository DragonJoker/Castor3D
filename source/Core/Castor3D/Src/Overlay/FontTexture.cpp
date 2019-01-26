#include "FontTexture.hpp"

#include "Engine.hpp"

#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	FontTexture::FontTexture( Engine & engine, FontSPtr font )
		: OwnedBy< Engine >( engine )
		, m_font( font )
	{
		uint32_t const maxWidth = font->getMaxWidth();
		uint32_t const maxHeight = font->getMaxHeight();
		uint32_t const count = uint32_t( std::ceil( std::distance( font->begin(), font->end() ) / 16.0 ) );

		SamplerSPtr sampler = getEngine()->getSamplerCache().add( font->getName() );
		sampler->setWrapS( ashes::WrapMode::eClampToEdge );
		sampler->setWrapT( ashes::WrapMode::eClampToEdge );
		sampler->setMinFilter( ashes::Filter::eLinear );
		sampler->setMagFilter( ashes::Filter::eLinear );
		m_sampler = sampler;
		ashes::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = maxWidth * 16;
		image.extent.height = maxHeight * count;
		image.extent.depth = 1u;
		image.format = ashes::Format::eR8_UNORM;
		image.imageType = ashes::TextureType::e2D;
		image.initialLayout = ashes::ImageLayout::eUndefined;
		image.mipLevels = 1u;
		image.samples = ashes::SampleCountFlag::e1;
		image.sharingMode = ashes::SharingMode::eExclusive;
		image.tiling = ashes::ImageTiling::eOptimal;
		image.usage = ashes::ImageUsageFlag::eTransferDst | ashes::ImageUsageFlag::eSampled;
		m_texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
			, image
			, ashes::MemoryPropertyFlag::eDeviceLocal );
		m_texture->getDefaultImage().initialiseSource();
	}

	FontTexture::~FontTexture()
	{
	}

	void FontTexture::initialise()
	{
		m_texture->initialise();
		m_texture->generateMipmaps();
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
			auto & image = m_texture->getDefaultImage();
			image.initialiseSource( PxBufferBase::create( Size( maxWidth * 16, maxHeight * count ), PixelFormat::eR8_UNORM ) );

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
						CU_Ensure( dstLineIndex + size.getWidth() <= bufsize );
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

	Position const & FontTexture::getGlyphPosition( char32_t glyphChar )const
	{
		GlyphPositionMapConstIt it = m_glyphsPositions.find( glyphChar );

		if ( it == m_glyphsPositions.end() )
		{
			CU_Exception( std::string( "No loaded glyph for character " ) + string::stringCast< char >( string::toString( glyphChar ) ) );
		}

		return it->second;
	}
}
