#include "CastorUtils/Graphics/Image.hpp"

#include "CastorUtils/Data/LoaderException.hpp"
#include "CastorUtils/Data/Path.hpp"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <stb_image_resize2.h>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

CU_ImplementSmartPtr( c3d, Image )

namespace c3d
{
	namespace img
	{
		static stbir_datatype getStbDataType( PixelFormat fmt )
		{
			if ( isFloatingPoint( fmt ) )
				return STBIR_TYPE_FLOAT;

			if ( isInt32( fmt ) || isInt16( fmt ) )
				return STBIR_TYPE_UINT16;

			return ( isSRGBFormat( fmt )
				? STBIR_TYPE_UINT8_SRGB
				: STBIR_TYPE_UINT8 );
		}

		static stbir_pixel_layout getStbPixelLayout( PixelFormat fmt )
		{
			auto components = getComponentsCount( fmt );
			if ( components == 1u )
				return STBIR_1CHANNEL;
			if ( components == 2u )
				return STBIR_2CHANNEL;
			if ( components == 3u )
			{
				if ( isBGRFormat( fmt ) )
					return STBIR_BGR;
				return STBIR_RGB;
			}

			if ( isARGBFormat( fmt ) )
				return STBIR_ARGB;
			if ( isABGRFormat( fmt ) )
				return STBIR_ABGR;
			if ( isBGRAFormat( fmt ) )
				return STBIR_BGRA;
			return STBIR_RGBA;
		}
	}

	Image::Image( String const & name
		, Path const & path
		, Size const & size
		, PixelFormat format
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: Named{ name }
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
		, ImageMemoryLayout layout
		, PxBufferBaseUPtr buffer
		, PxBufferBaseUPtr alphaChannel )
		: Named{ name }
		, m_pathFile{ path }
		, m_buffer{ ( buffer
			? c3d::move( buffer )
			: PxBufferBase::create( layout.dimensions()
				, layout.depthLayers()
				, layout.levels
				, layout.format ) ) }
		, m_alphaChannel{ c3d::move( alphaChannel ) }
		, m_layout{ c3d::move( layout ) }
	{
		CU_CheckInvariants();
	}

	Image::Image( Image const & image )
		: Named{ image.getName() }
		, m_pathFile{ image.m_pathFile }
		, m_buffer{ image.m_buffer->clone() }
		, m_alphaChannel{ ( !image.m_alphaChannel ) ? nullptr : image.m_alphaChannel->clone() }
		, m_layout{ image.m_layout }
	{
		CU_CheckInvariants();
	}

	Image::Image( Image && image )noexcept
		: Named{ image.getName() }
		, m_pathFile{ c3d::move( image.m_pathFile ) }
		, m_buffer{ c3d::move( image.m_buffer ) }
		, m_alphaChannel{ c3d::move( image.m_alphaChannel ) }
		, m_layout{ c3d::move( image.m_layout ) }
	{
		CU_CheckInvariants();
	}

	Image & Image::operator=( Image const & image )
	{
		m_pathFile = image.m_pathFile;
		m_layout = image.m_layout;
		m_buffer = image.m_buffer ? image.m_buffer->clone() : nullptr;
		m_alphaChannel = image.m_alphaChannel ? image.m_alphaChannel->clone() : nullptr;
		return * this;
	}

	Image & Image::operator=( Image && image )noexcept
	{
		m_pathFile = c3d::move( image.m_pathFile );
		m_layout = c3d::move( image.m_layout );
		m_buffer = c3d::move( image.m_buffer );
		m_alphaChannel = c3d::move( image.m_alphaChannel );
		return * this;
	}

	PxBufferBaseUPtr Image::updateLayerLayout( Size const & extent
		, PixelFormat format )
	{
		auto result = ( m_layout.extent->x != extent.getWidth()
				|| m_layout.extent->y != extent.getHeight()
				|| format != m_layout.format )
			? c3d::move( m_buffer )
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

	PxBufferBaseUPtr Image::resample( Size const & size
		, PxBufferBaseUPtr buffer )
	{
		if ( buffer->getDimensions() == size )
		{
			return buffer;
		}

		ImageMemoryLayout layout{ *buffer };
		auto format = buffer->getFormat();
		stbir_pixel_layout pixelLayout{ img::getStbPixelLayout( buffer->getFormat() ) };
		stbir_datatype dataType{ img::getStbDataType( buffer->getFormat() ) };
		auto srcLayerSize = layout.layerSize();
		auto src = buffer->getPtr();
		layout.extent->x = size.getWidth();
		layout.extent->y = size.getHeight();
		layout.format = format;
		auto result = PxBufferBase::create( { layout.extent->x, layout.extent->y }
			, layout.depthLayers()
			, layout.levels
			, layout.format );
		auto dstLayerSize = layout.layerSize();
		auto dst = result->getPtr();

		for ( uint32_t layer = 0u; layer < layout.depthLayers(); ++layer )
		{
			if ( auto resized = stbir_resize( src, int( buffer->getWidth() ), int( buffer->getHeight() ), 0
					, dst, int( result->getWidth() ), int( result->getHeight() ), 0
					, pixelLayout, dataType
					, STBIR_EDGE_CLAMP, STBIR_FILTER_CATMULLROM );
				!resized )
			{
				CU_LoaderError( "Image couldn't be resized" );
			}

			dst += dstLayerSize;
			src += srcLayerSize;
		}

		return result;
	}

	Image & Image::resample( Size const & size )
	{
		m_buffer = resample( size, c3d::move( m_buffer ) );
		if ( m_alphaChannel )
			m_alphaChannel = resample( size, c3d::move( m_alphaChannel ) );
		m_layout.extent->x = m_buffer->getHeight();
		m_layout.extent->y = m_buffer->getWidth();
		m_layout.format = m_buffer->getFormat();
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
		auto uiBpp = uint32_t( getBytesPerPixel( getPixelFormat() ) );
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
		auto uiBpp = uint32_t( getBytesPerPixel( getPixelFormat() ) );
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
		convertPixel( format, src, getPixelFormat(), dst );
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
		convertPixel( PixelFormat::eR8G8B8A8_UNORM, src, getPixelFormat(), dst );
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
		convertPixel( PixelFormat::eR8G8B8_UNORM, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::invertX()
	{
		CU_CheckInvariants();
		m_buffer->invertX();
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::invertY()
	{
		CU_CheckInvariants();
		m_buffer->invertY();
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::invertZ()
	{
		CU_CheckInvariants();
		m_buffer->invertZ();
		CU_CheckInvariants();
		return * this;
	}
}
