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

CU_ImplementSmartPtr( c3d, FontTexture )

namespace c3d
{
	//*********************************************************************************************

	namespace fonttex
	{
		static TextureLayoutUPtr createTexture( Engine const & engine
			, FontResPtr font
			, String const & suffix )
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
				? PixelFormat::eR32G32B32A32_SFLOAT
				: PixelFormat::eR8_UNORM;
			uint32_t const count = divRoundUp( uint32_t( std::distance( font->begin(), font->end() ) ), 16u );

			ImageCreateInfo image{ ImageCreateFlags::eNone
				, ImageType::e2D
				, format
				, { maxWidth * 16, maxHeight * count, 1u }
				, 1u
				, 1u
				, SampleCount::e1
				, ImageTiling::eOptimal
				, ImageUsageFlags::eTransferDst | ImageUsageFlags::eSampled };
			return makeUnique< TextureLayout >( *engine.getRenderSystem()
				, image
				, MemoryPropertyFlags::eDeviceLocal
				, cuT( "FontTexture_" ) + font->getFaceName()
					+ ( font->isSDF() ? String{ cuT( "SDF" ) } : string::toString( font->getHeight() ) )
					+ suffix );
		}
	}

	//*********************************************************************************************

	void postPreRenderGpuEvent( Engine const & engine
		, Function< void( RenderDevice const &, QueueData const & ) > event )
	{
		engine.postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload, c3d::move( event ) ) );
	}

	void postQueueRenderCpuEvent( Engine const & engine
		, Function< void() > event )
	{
		engine.postEvent( makeCpuFunctorEvent( CpuEventType::ePreCpuStep, c3d::move( event ) ) );
	}

	//*********************************************************************************************

	FontTexture::FontTexture( Engine & engine, FontResPtr font )
		: DoubleBufferedTextureLayout{ engine
			, fonttex::createTexture( engine, font, "_0" )
			, fonttex::createTexture( engine, font, "_1" ) }
		, m_font{ font }
		, m_ubo{ makeUnique< FontUbo >( *engine.getRenderDevice() ) }
	{
		if ( !m_font )
		{
			CU_Exception( "No Font given to FontTexture" );
		}

		if ( auto sampler = getEngine()->addNewSampler( m_font->getName(), *getEngine() ) )
		{
			sampler->setWrapS( WrapMode::eClampToEdge );
			sampler->setWrapT( WrapMode::eClampToEdge );
			sampler->setMinFilter( FilterMode::eLinear );
			sampler->setMagFilter( FilterMode::eLinear );
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

	UInt32Array FontTexture::convert( U32String const & text )const
	{
		UInt32Array result;
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

	String const & FontTexture::getFontName()const
	{
		return getFont()->getName();
	}

	Position const & FontTexture::getGlyphPosition( char32_t glyphChar )const
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
				? PixelFormat::eR32G32B32A32_SFLOAT
				: PixelFormat::eR8_UNORM;
			uint32_t const count = divRoundUp( uint32_t( std::distance( font->begin(), font->end() ) ), 16u );
			Size size{ maxWidth * 16, maxHeight * count };
			m_ubo->cpuUpdate( size, font->isSDF(), font->getPixelRange() );
			resource.resource->setSource( PxBufferBase::create( Size( maxWidth * 16, maxHeight * count ), format ), true );
			auto & image = resource.resource->getImage();
			auto pixelSize = uint32_t( getBytesPerPixel( format ) );

			auto it = font->begin();
			Size const & sizeImg = size;
			uint32_t const imgLineSize = sizeImg.getWidth();
			auto buffer = image.getBuffer();
			uint8_t * dstBuffer = buffer.data();

			uint32_t offY = sizeImg.getHeight() - maxHeight;
			uint32_t y = 0;
			while ( y < count && it != font->end() )
			{
				uint32_t offX = 0;
				uint32_t x = 0;
				while ( x < 16 && it != font->end() )
				{
					Glyph const & glyph = *it;
					auto const glyphSize = font->isSDF()
						? Point2ui{ glyph.getBitmapSize()->x, glyph.getBitmapSize()->y }
						: Point2ui{ glyph.getSize() };
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

					glyphPositions[glyph.getCharacter()] = Position( int32_t( offX ), int32_t( offY ) );
					offX += maxWidth;
					++x;
					++it;

					m_charIndices.try_emplace( glyph.getCharacter(), uint32_t( m_charIndices.size() ) );
				}

				offY -= maxHeight;
				++y;
			}

			resource.needsUpload = true;
		}
	}

	void FontTexture::swapResources()
	{
	}

	//*********************************************************************************************
}
