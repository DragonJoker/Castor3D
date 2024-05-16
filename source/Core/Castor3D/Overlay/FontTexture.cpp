#include "Castor3D/Overlay/FontTexture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Shader/Ubos/FontUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/Image.hpp>

CU_ImplementSmartPtr( castor3d, FontTexture )

namespace castor3d
{
	//*********************************************************************************************

	namespace fonttex
	{
		static TextureLayoutUPtr createTexture( Engine const & engine
			, castor::FontResPtr font
			, castor::String suffix )
		{
			if ( !font )
			{
				CU_Exception( "No Font given to FontTexture" );
			}

			auto const maxWidth = font->isSDF()
				? font->getMaxImageWidth()
				: uint32_t( font->getMaxGlyphWidth() );
			auto const maxHeight = font->isSDF()
				? font->getMaxImageHeight()
				: uint32_t( font->getMaxGlyphHeight() );
			auto const format = font->isSDF()
				? VK_FORMAT_R32G32B32A32_SFLOAT
				: VK_FORMAT_R8_UNORM;
			uint32_t const count = castor::divRoundUp( uint32_t( std::distance( font->begin(), font->end() ) ), 16u );

			ashes::ImageCreateInfo image{ 0u
				, VK_IMAGE_TYPE_2D
				, format
				, { maxWidth * 16, maxHeight * count, 1u }
				, 1u
				, 1u
				, VK_SAMPLE_COUNT_1_BIT
				, VK_IMAGE_TILING_OPTIMAL
				, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
			return castor::makeUnique< TextureLayout >( *engine.getRenderSystem()
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, cuT( "FontTexture_" ) + font->getFaceName()
					+ ( font->isSDF() ? castor::String{ cuT( "SDF" ) } : castor::string::toString( font->getHeight() ) )
					+ suffix );
		}
	}

	//*********************************************************************************************

	void postPreRenderGpuEvent( Engine & engine
		, castor::Function< void( RenderDevice const &, QueueData const & ) > event )
	{
		engine.postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload, castor::move( event ) ) );
	}

	void postQueueRenderCpuEvent( Engine & engine
		, castor::Function< void() > event )
	{
		engine.postEvent( makeCpuFunctorEvent( CpuEventType::ePreCpuStep, castor::move( event ) ) );
	}

	//*********************************************************************************************

	FontTexture::FontTexture( Engine & engine, castor::FontResPtr font )
		: DoubleBufferedTextureLayout{ engine
			, fonttex::createTexture( engine, font, "_0" )
			, fonttex::createTexture( engine, font, "_1" ) }
		, m_font( font )
		, m_ubo{ castor::makeUnique< FontUbo >( *engine.getRenderDevice() ) }
	{
		if ( !m_font )
		{
			CU_Exception( "No Font given to FontTexture" );
		}

		if ( auto sampler = getEngine()->addNewSampler( m_font->getName(), *getEngine() ) )
		{
			sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
			sampler->setMinFilter( VK_FILTER_LINEAR );
			sampler->setMagFilter( VK_FILTER_LINEAR );
			sampler->setMinLod( 0.0f );
			sampler->setMaxLod( 1.0f );

			m_sampler = sampler;
		}
	}

	void FontTexture::initialise( RenderDevice const & device
		, QueueData const & queueData )
	{
		doInitialise( device, queueData );
		updateResource( doGetFront() );
		updateResource( doGetBack() );
		onResourceChanged( *this );
		log::info << "Initialised FontTexture for Font [" << m_font->getName() << "]" << std::endl;
	}

	void FontTexture::cleanup( RenderDevice const & )
	{
		doCleanup();
		log::info << "Cleaned up FontTexture for Font [" << m_font->getName() << "]" << std::endl;
	}

	void FontTexture::upload( UploadData & uploader )
	{
		if ( auto & resource = doGetFront();
			resource.needsUpload )
		{
			resource.resource->upload( uploader );
			resource.needsUpload = false;
		}
	}

	castor::UInt32Array FontTexture::convert( castor::U32String const & text )const
	{
		castor::UInt32Array result;
		result.resize( text.size() );

		if ( auto defaultIt = m_charIndices.find( U'?' );
			defaultIt != m_charIndices.end() )
		{
			auto dst = result.begin();
			for ( auto c : text )
			{
				auto it = m_charIndices.find( c );
				*dst = ( it != m_charIndices.end()
					? it->second
					: defaultIt->second );
				++dst;
			}
		}

		return result;
	}

	castor::String const & FontTexture::getFontName()const
	{
		return getFont()->getName();
	}

	castor::Position const & FontTexture::getGlyphPosition( char32_t glyphChar )const
	{
		auto it = m_glyphsPositions.find( glyphChar );

		if ( it == m_glyphsPositions.end() )
		{
			it = m_glyphsPositions.find( U'?' );
			CU_Require( it != m_glyphsPositions.end() );
			return it->second;
		}

		return it->second;
	}

	void FontTexture::initialiseResource( Resource & resource
		, RenderDevice const & device
		, QueueData const & queueData )
	{
		resource.resource->initialise( device );
	}

	void FontTexture::cleanupResource( Resource & resource )
	{
		resource.resource->cleanup();
	}

	void FontTexture::updateResource( Resource & resource )
	{
		if ( auto font = getFont() )
		{
			auto & glyphPositions = m_glyphsPositions;
			auto const maxWidth = font->isSDF()
				? font->getMaxImageWidth()
				: uint32_t( font->getMaxGlyphWidth() );
			auto const maxHeight = font->isSDF()
				? font->getMaxImageHeight()
				: uint32_t( font->getMaxGlyphHeight() );
			auto const format = font->isSDF()
				? castor::PixelFormat::eR32G32B32A32_SFLOAT
				: castor::PixelFormat::eR8_UNORM;
			uint32_t const count = castor::divRoundUp( uint32_t( std::distance( font->begin(), font->end() ) ), 16u );
			castor::Size size{ maxWidth * 16, maxHeight * count };
			m_ubo->cpuUpdate( size, font->isSDF(), font->getPixelRange() );
			resource.resource->setSource( castor::PxBufferBase::create( castor::Size( maxWidth * 16, maxHeight * count ), format ), true );
			auto & image = resource.resource->getImage();
			auto pixelSize = uint32_t( getBytesPerPixel( format ) );

			auto it = font->begin();
			castor::Size const & sizeImg = size;
			uint32_t const imgLineSize = sizeImg.getWidth();
			uint32_t offY = sizeImg.getHeight() - maxHeight;
			auto buffer = image.getBuffer();
			uint8_t * dstBuffer = buffer.data();

			for ( uint32_t y = 0; y < count && it != font->end(); ++y )
			{
				uint32_t offX = 0;

				for ( uint32_t x = 0; x < 16 && it != font->end(); ++x )
				{
					castor::Glyph const & glyph = *it;
					auto const glyphSize = font->isSDF()
						? castor::Point2ui{ glyph.getBitmapSize()->x, glyph.getBitmapSize()->y }
						: castor::Point2ui{ glyph.getSize() };
					auto srcGlyphBuffer = glyph.getBitmap().data();
					uint32_t dstGlyphIndex = ( imgLineSize * offY ) + offX;
					uint8_t * dstGlyphBuffer = &dstBuffer[dstGlyphIndex * pixelSize];

					for ( uint32_t i = 0; i < glyphSize->y; ++i )
					{
						CU_Ensure( size_t( dstGlyphIndex ) + glyphSize->x * pixelSize <= buffer.size() );
						std::memcpy( dstGlyphBuffer, srcGlyphBuffer, glyphSize->x * pixelSize );
						dstGlyphBuffer += imgLineSize * pixelSize;
						dstGlyphIndex += imgLineSize * pixelSize;
						srcGlyphBuffer += glyphSize->x * pixelSize;
					}

					glyphPositions[glyph.getCharacter()] = castor::Position( int32_t( offX ), int32_t( offY ) );
					offX += maxWidth;
					++it;

					m_charIndices.try_emplace( glyph.getCharacter(), uint32_t( m_charIndices.size() ) ).second;
				}

				offY -= maxHeight;
			}

			resource.needsUpload = true;
		}
	}

	void FontTexture::swapResources()
	{
	}

	//*********************************************************************************************
}
