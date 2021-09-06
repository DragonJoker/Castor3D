#include "Castor3D/Overlay/FontTexture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/Image.hpp>

CU_ImplementCUSmartPtr( castor3d, FontTexture )

namespace castor3d
{
	FontTexture::FontTexture( Engine & engine, castor::FontResPtr font )
		: OwnedBy< Engine >( engine )
		, m_font( font )
	{
		uint32_t const maxWidth = font.lock()->getMaxWidth();
		uint32_t const maxHeight = font.lock()->getMaxHeight();
		uint32_t const count = uint32_t( std::ceil( std::distance( font.lock()->begin(), font.lock()->end() ) / 16.0 ) );

		auto sampler = getEngine()->getSamplerCache().add( font.lock()->getName(), *getEngine() );
		sampler.lock()->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
		sampler.lock()->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
		sampler.lock()->setMinFilter( VK_FILTER_LINEAR );
		sampler.lock()->setMagFilter( VK_FILTER_LINEAR );
		m_sampler = sampler;
		ashes::ImageCreateInfo image
		{
			0u,
			VK_IMAGE_TYPE_2D,
			VK_FORMAT_R8_UNORM,
			{ maxWidth * 16, maxHeight * count, 1u },
			1u,
			1u,
			VK_SAMPLE_COUNT_1_BIT,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		};
		m_texture = std::make_shared< TextureLayout >( *getEngine()->getRenderSystem()
			, image
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, cuT( "FontTexture_" ) + font.lock()->getFaceName() );
	}

	FontTexture::~FontTexture()
	{
	}

	void FontTexture::initialise( RenderDevice const & device
		, QueueData const & queueData )
	{
		m_texture->initialise( device, queueData );
		onChanged( *this );
	}

	void FontTexture::update()
	{
		auto font = getFont();

		if ( font )
		{
			uint32_t const maxWidth = font->getMaxWidth();
			uint32_t const maxHeight = font->getMaxHeight();
			uint32_t const count = uint32_t( std::ceil( std::distance( font->begin(), font->end() ) / 16.0 ) );
			castor::Size size{ maxWidth * 16, maxHeight * count };
			m_texture->setSource( castor::PxBufferBase::create( castor::Size( maxWidth * 16, maxHeight * count )
				, castor::PixelFormat::eR8_UNORM ), true );
			auto & image = m_texture->getImage();

			auto it = font->begin();
			castor::Size const & sizeImg = size;
			uint32_t const imgLineSize = sizeImg.getWidth();
			uint32_t offY = sizeImg.getHeight() - maxHeight;
			auto buffer = image.getBuffer();
			uint8_t * dstBuffer = buffer.data();
			size_t const bufsize = buffer.size();

			for ( uint32_t y = 0; y < count && it != font->end(); ++y )
			{
				uint32_t offX = 0;

				for ( uint32_t x = 0; x < 16 && it != font->end(); ++x )
				{
					castor::Glyph const & glyph = *it;
					castor::Size const & glyphSize = glyph.getSize();
					auto srcGlyphBuffer = glyph.getBitmap().data();
					uint32_t dstGlyphIndex = ( imgLineSize * offY ) + offX;
					uint8_t * dstGlyphBuffer = &dstBuffer[dstGlyphIndex];

					for ( uint32_t i = 0; i < glyphSize.getHeight(); ++i )
					{
						CU_Ensure( size_t( dstGlyphIndex ) + glyphSize.getWidth() <= bufsize );
						std::memcpy( dstGlyphBuffer, srcGlyphBuffer, glyphSize.getWidth() );
						dstGlyphBuffer += imgLineSize;
						dstGlyphIndex += imgLineSize;
						srcGlyphBuffer += glyphSize.getWidth();
					}

					m_glyphsPositions[glyph.getCharacter()] = castor::Position( offX, offY );
					offX += maxWidth;
					++it;
				}

				offY -= maxHeight;
			}
		}

	}

	void FontTexture::cleanup( RenderDevice const & device )
	{
		if ( m_texture )
		{
			m_texture->cleanup();
		}
	}

	castor::String const & FontTexture::getFontName()const
	{
		return getFont()->getName();
	}

	castor::Position const & FontTexture::getGlyphPosition( char32_t glyphChar )const
	{
		GlyphPositionMapConstIt it = m_glyphsPositions.find( glyphChar );

		if ( it == m_glyphsPositions.end() )
		{
			CU_Exception( std::string( "No loaded glyph for character " ) + castor::string::stringCast< char >( castor::string::toString( glyphChar ) ) );
		}

		return it->second;
	}
}
