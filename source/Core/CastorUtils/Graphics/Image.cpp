#include "CastorUtils/Graphics/Image.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/ImageLoader.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Log/Logger.hpp"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

namespace castor
{
	namespace
	{
		PxBufferPtrArray cloneBuffers( PxBufferPtrArray const & src )
		{
			PxBufferPtrArray result;

			for ( auto & buffer : src )
			{
				result.emplace_back( buffer->clone() );
			}

			return result;
		}
	}

	Image::Image( String const & name
		, Size const & size
		, PixelFormat format
		, ByteArray const & buffer
		, PixelFormat bufferFormat )
		: Resource< Image >{ name }
		, m_buffers{ PxBufferBase::create( size, format, &buffer[0], bufferFormat ) }
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, Size const & size
		, PixelFormat format
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: Resource< Image >{ name }
		, m_buffers{ PxBufferBase::create( size, format, buffer, bufferFormat ) }
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, PxBufferBase const & buffer )
		: Resource< Image >{ name }
		, m_buffers{ buffer.clone() }
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, PxBufferPtrArray const & buffers )
		: Resource< Image >{ name }
		, m_buffers{ std::move( buffers ) }
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, Path const & pathFile
		, ImageLoader const & loader )
		: Resource< Image >{ name }
		, m_pathFile{ pathFile }
		, m_buffers{ loader.load( pathFile ) }
	{
	}

	Image::Image( Image const & image )
		: Resource< Image >{ static_cast< Resource< Image > const & >( image ) }
		, m_pathFile{ image.m_pathFile }
		, m_buffers{ cloneBuffers( image.m_buffers ) }
	{
		CU_CheckInvariants();
	}

	Image & Image::operator=( Image const & image )
	{
		Resource< Image >::operator=( image );
		m_pathFile = image.m_pathFile;
		m_buffers = cloneBuffers( image.m_buffers );
		CU_CheckInvariants();
		return * this;
	}

	Image::~Image()
	{
	}

	Image & Image::resample( Size const & size )
	{
		CU_Require( getLevels() == 1u );
		using ResizeFunc = int( *)( const unsigned char * input_pixels, int input_w, int input_h, int input_stride_in_bytes,
			unsigned char *output_pixels, int output_w, int output_h, int output_stride_in_bytes,
			int num_channels, int alpha_channel, int flags,
			stbir_edge edge_wrap_mode, stbir_filter filter, stbir_colorspace space,
			void *alloc_context );

		auto srcBuffer = getPixels();
		int channels = int( PF::getComponentsCount( srcBuffer->getFormat() ) );
		int alpha = STBIR_ALPHA_CHANNEL_NONE;
		stbir_colorspace colorSpace{ STBIR_COLORSPACE_LINEAR };
		stbir_datatype dataType = STBIR_TYPE_UINT8;

		switch ( srcBuffer->getFormat() )
		{
		case PixelFormat::eR8_UNORM:
		case PixelFormat::eR8A8_UNORM:
		case PixelFormat::eA8B8G8R8_UNORM:
		case PixelFormat::eR8G8B8A8_UNORM:
			alpha = 1;
			break;

		case PixelFormat::eR8G8B8_SRGB:
		case PixelFormat::eB8G8R8_SRGB:
			colorSpace = STBIR_COLORSPACE_SRGB;
			break;

		case PixelFormat::eA8B8G8R8_SRGB:
		case PixelFormat::eR8G8B8A8_SRGB:
			colorSpace = STBIR_COLORSPACE_SRGB;
			alpha = 1;
			break;

		case PixelFormat::eR16_SFLOAT:
		case PixelFormat::eR32_SFLOAT:
		case PixelFormat::eR16G16B16_SFLOAT:
		case PixelFormat::eR32G32B32_SFLOAT:
			dataType = STBIR_TYPE_FLOAT;
			break;

		case PixelFormat::eR16A16_SFLOAT:
		case PixelFormat::eR32A32_SFLOAT:
		case PixelFormat::eR16G16B16A16_SFLOAT:
		case PixelFormat::eR32G32B32A32_SFLOAT:
			dataType = STBIR_TYPE_FLOAT;
			alpha = 1;
			break;

		default:
			CU_LoaderError( "Unsupported image format for resize." );
			break;
		}

		auto dstBuffer = PxBufferBase::create( size
			, srcBuffer->getFormat() );
		auto result = stbir_resize( srcBuffer->getConstPtr(), int( srcBuffer->getWidth() ), int( srcBuffer->getHeight() ), 0
			, dstBuffer->getPtr(), int( dstBuffer->getWidth() ), int( dstBuffer->getHeight() ), 0
			, dataType
			, channels, alpha, 0
			, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP
			, STBIR_FILTER_CATMULLROM, STBIR_FILTER_CATMULLROM
			, colorSpace, nullptr );

		if ( !result )
		{
			CU_LoaderError( "Image couldn't be resized" );
		}

		m_buffers.front() = dstBuffer;
		return *this;
	}

	CU_BeginInvariantBlock( Image )
	CU_CheckInvariant( !m_buffers.empty() );
	CU_CheckInvariant( m_buffers.front()->getCount() > 0 );
	CU_EndInvariantBlock()

	Image & Image::fill( RgbColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );

		for ( auto & buffer : m_buffers )
		{
			for ( uint32_t i = uiBpp; i < buffer->getSize(); i += uiBpp )
			{
				memcpy( &buffer->getPtr()[i], &buffer->getConstPtr()[0], uiBpp );
			}
		}

		CU_CheckInvariants();
		return * this;
	}

	Image & Image::fill( RgbaColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );

		for ( auto & buffer : m_buffers )
		{
			for ( uint32_t i = uiBpp; i < buffer->getSize(); i += uiBpp )
			{
				memcpy( &buffer->getPtr()[i], &buffer->getConstPtr()[0], uiBpp );
			}
		}

		CU_CheckInvariants();
		return * this;
	}

	Image & Image::setPixel( uint32_t x
		, uint32_t y
		, uint8_t const * pixel
		, PixelFormat format )
	{
		CU_CheckInvariants();
		CU_Require( getLevels() == 1u );
		CU_Require( x < getWidth() && y < getHeight() && pixel );
		uint8_t const * src = pixel;
		uint8_t * dst = &( *m_buffers.front()->getAt( x, y ) );
		PF::convertPixel( format, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::setPixel( uint32_t x
		, uint32_t y
		, RgbColour const & colour )
	{
		CU_CheckInvariants();
		CU_Require( getLevels() == 1u );
		CU_Require( x < getWidth() && y < getHeight() );
		Point4ub components = toBGRAByte( colour );
		uint8_t const * src = components.constPtr();
		uint8_t * dst = &( *m_buffers.front()->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eR8G8B8A8_UNORM, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::setPixel( uint32_t x
		, uint32_t y
		, RgbaColour const & colour )
	{
		CU_CheckInvariants();
		CU_Require( getLevels() == 1u );
		CU_Require( x < getWidth() && y < getHeight() );
		Point3ub components = toBGRByte( colour );
		uint8_t const * src = components.constPtr();
		uint8_t * dst = &( *m_buffers.front()->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eR8G8B8_UNORM, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	void Image::getPixel( uint32_t x
		, uint32_t y
		, uint8_t * pixel
		, PixelFormat format )const
	{
		CU_CheckInvariants();
		CU_Require( getLevels() == 1u );
		CU_Require( x < getWidth() && y < getHeight() && pixel );
		uint8_t const * src = &( *m_buffers.front()->getAt( x, y ) );
		uint8_t * dst = pixel;
		PF::convertPixel( getPixelFormat(), src, format, dst );
		CU_CheckInvariants();
	}

	RgbaColour Image::getPixel( uint32_t x
		, uint32_t y )const
	{
		CU_CheckInvariants();
		CU_Require( getLevels() == 1u );
		CU_Require( x < getWidth() && y < getHeight() );
		Point4ub components;
		uint8_t const * src = &( *m_buffers.front()->getAt( x, y ) );
		uint8_t * dst = components.ptr();
		PF::convertPixel( getPixelFormat(), src, PixelFormat::eR8G8B8A8_UNORM, dst );
		CU_CheckInvariants();
		return RgbaColour::fromBGRA( components );
	}

	Image & Image::copyImage( Image const & toCopy )
	{
		CU_CheckInvariants();
		CU_Require( getLevels() == 1u );
		auto srcBuffer = toCopy.m_buffers.front();
		auto dstBuffer = m_buffers.front();

		if ( getDimensions() == toCopy.getDimensions() )
		{
			if ( getPixelFormat() == toCopy.getPixelFormat() )
			{
				memcpy( dstBuffer->getPtr(), srcBuffer->getPtr(), dstBuffer->getSize() );
			}
			else
			{
				for ( uint32_t i = 0; i < getWidth(); ++i )
				{
					for ( uint32_t j = 0; j < getHeight(); ++j )
					{
						uint8_t const * src = &( *srcBuffer->getAt( i, j ) );
						uint8_t * dst = &( *dstBuffer->getAt( i, j ) );
						PF::convertPixel( getPixelFormat(), src, getPixelFormat(), dst );
					}
				}
			}
		}
		else
		{
			Point2d srcStep( static_cast< double >( toCopy.getWidth() ) / getWidth(), static_cast< double >( toCopy.getHeight() ) / getHeight() );
			ByteArray srcPix( PF::getBytesPerPixel( toCopy.getPixelFormat() ), 0 );

			for ( uint32_t i = 0; i < getWidth(); ++i )
			{
				for ( uint32_t j = 0; j < getHeight(); ++j )
				{
					toCopy.getPixel( static_cast< uint32_t >( i * srcStep[0] ), static_cast< uint32_t >( j * srcStep[1] ), srcPix.data(), toCopy.getPixelFormat() );
					setPixel( i, j, srcPix.data(), toCopy.getPixelFormat() );
				}
			}
		}

		CU_CheckInvariants();
		return * this;
	}

	Image Image::subImage( Rectangle const & rect )const
	{
		CU_CheckInvariants();
		CU_Require( getLevels() == 1u );
		CU_Require( Rectangle( 0, 0 , getWidth(), getHeight() ).intersects( rect ) == Intersection::eIn );
		Size size( rect.getWidth(), rect.getHeight() );
		// Création de la sous-image à remplir
		Image img( m_name + cuT( "_Sub" ) + string::toString( rect[0] ) + cuT( "x" ) + string::toString( rect[1] ) + cuT( ":" ) + string::toString( size.getWidth() ) + cuT( "x" ) + string::toString( size.getHeight() ), size, getPixelFormat() );
		// Calcul de variables temporaires
		uint8_t const * src = &( *m_buffers.front()->getAt( rect.left(), rect.top() ) );
		uint8_t * dst = &( *img.m_buffers.front()->getAt( 0, 0 ) );
		uint32_t srcPitch = getWidth() * PF::getBytesPerPixel( getPixelFormat() );
		uint32_t dstPitch = img.getWidth() * PF::getBytesPerPixel( img.getPixelFormat() );

		// Copie des pixels de l'image originale dans la sous-image
		for ( int i = rect.left(); i < rect.right(); ++i )
		{
			std::memcpy( dst, src, dstPitch );
			src += srcPitch;
			dst += dstPitch;
		}

		CU_CheckInvariants();
		return img;
	}

	Image & Image::flip()
	{
		CU_CheckInvariants();

		for ( auto & buffer : m_buffers )
		{
			buffer->flip();
		}

		CU_CheckInvariants();
		return * this;
	}

	Image & Image::mirror()
	{
		CU_CheckInvariants();

		for ( auto & buffer : m_buffers )
		{
			buffer->mirror();
		}

		CU_CheckInvariants();
		return * this;
	}
}
