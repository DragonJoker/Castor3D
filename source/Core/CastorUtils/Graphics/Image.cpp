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
	Image::Image( String const & name
		, Path const & path
		, Size const & size
		, PixelFormat format
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: Resource< Image >{ name }
		, m_pathFile{ path }
		, m_buffer{ PxBufferBase::create( size, format, buffer, bufferFormat ) }
		, m_layout{ *m_buffer }
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, Path const & path
		, Size const & size
		, PixelFormat format
		, ByteArray const & buffer
		, PixelFormat bufferFormat )
		: Image{ name, path, size, format, buffer.data(), bufferFormat }
	{
	}

	Image::Image( String const & name
		, Path const & path
		, PxBufferBase const & buffer )
		: Image{ name, path, buffer.getDimensions(), buffer.getFormat(), buffer.getConstPtr(), buffer.getFormat() }
	{
	}

	Image::Image( String const & name
		, Path const & path
		, ImageLayout layout
		, PxBufferBaseSPtr buffer )
		: Resource< Image >{ std::move( name ) }
		, m_pathFile{ path }
		, m_buffer{ ( buffer
			? buffer
			: PxBufferBase::create( layout.dimensions()
				, layout.depthLayers()
				, layout.levels
				, layout.format ) ) }
		, m_layout{ layout }
	{
		CU_CheckInvariants();
	}

	Image::Image( Image const & image )
		: Resource< Image >( image )
		, m_pathFile{ image.m_pathFile }
		, m_buffer{ image.m_buffer->clone() }
		, m_layout{ image.m_layout }
	{
		CU_CheckInvariants();
	}

	Image & Image::operator=( Image const & image )
	{
		Resource< Image >::operator=( image );
		m_pathFile = image.m_pathFile;
		m_layout = image.m_layout;
		m_buffer = image.m_buffer ? image.m_buffer->clone() : nullptr;
		return * this;
	}

	PxBufferBaseSPtr Image::updateLayerLayout( Size const & extent
		, PixelFormat format )
	{
		auto result = ( m_layout.extent->x != extent.getWidth()
			|| m_layout.extent->y != extent.getHeight()
			|| format != m_layout.format )
			? m_buffer
			: nullptr;

		if ( result )
		{
			m_layout.extent->x = extent.getWidth();
			m_layout.extent->y = extent.getHeight();
			m_layout.format = format;
			m_buffer = PxBufferBase::create( { m_layout.extent->x, m_layout.extent->y }
				, m_layout.depthLayers()
				, m_layout.levels
				, m_layout.format );
		}

		return result;
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

		auto srcW = m_layout.extent->x;
		auto srcH = m_layout.extent->y;
		auto srcLayerSize = m_layout.layerSize();
		auto src = srcBuffer->getPtr();
		updateLayerLayout( size, srcBuffer->getFormat() );
		auto dstBuffer = getPixels();
		auto dstW = m_layout.extent->x;
		auto dstH = m_layout.extent->y;
		auto dstLayerSize = m_layout.layerSize();
		auto dst = dstBuffer->getPtr();

		for ( uint32_t layer = 0u; layer < m_layout.depthLayers(); ++layer )
		{
			auto result = stbir_resize( src, int( srcBuffer->getWidth() ), int( srcBuffer->getHeight() ), 0
				, dst, int( dstBuffer->getWidth() ), int( dstBuffer->getHeight() ), 0
				, dataType
				, channels, alpha, 0
				, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP
				, STBIR_FILTER_CATMULLROM, STBIR_FILTER_CATMULLROM
				, colorSpace, nullptr );

			if ( !result )
			{
				CU_LoaderError( "Image couldn't be resized" );
			}

			dst += dstLayerSize;
			src += srcLayerSize;
		}

		CU_CheckInvariants();
		return *this;
	}

	CU_BeginInvariantBlock( Image )
	CU_CheckInvariant( m_buffer );
	CU_CheckInvariant( m_buffer->getCount() > 0 );
	CU_EndInvariantBlock()

	Image & Image::fill( RgbColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );
		auto src = m_buffer->getPtr();
		auto buffer = m_buffer->getPtr() + uiBpp;
		auto end = m_buffer->getPtr() + m_buffer->getSize();

		while ( buffer != end )
		{
			memcpy( buffer, src, uiBpp );
			buffer += uiBpp;
		}

		CU_CheckInvariants();
		return * this;
	}

	Image & Image::fill( RgbaColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );
		auto src = m_buffer->getPtr();
		auto buffer = m_buffer->getPtr() + uiBpp;
		auto end = m_buffer->getPtr() + m_buffer->getSize();

		while ( buffer != end )
		{
			memcpy( buffer, src, uiBpp );
			buffer += uiBpp;
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
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
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
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
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
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eR8G8B8_UNORM, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::flip()
	{
		CU_CheckInvariants();
		m_buffer->flip();
		CU_CheckInvariants();
		return * this;
	}
}
