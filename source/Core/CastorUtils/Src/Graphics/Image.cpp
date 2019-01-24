#include "Image.hpp"
#include "Rectangle.hpp"

namespace castor
{
	Image::Image( String const & name
		, Size const & size
		, PixelFormat format
		, ByteArray const & buffer
		, PixelFormat bufferFormat )
		: Resource< Image >( name )
		, m_buffer( PxBufferBase::create( size, format, &buffer[0], bufferFormat ) )
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, Size const & size
		, PixelFormat format
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: Resource< Image >( name )
		, m_buffer( PxBufferBase::create( size, format, buffer, bufferFormat ) )
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, PxBufferBase const & buffer )
		: Resource< Image >( name )
		, m_buffer( buffer.clone() )
	{
		CU_CheckInvariants();
	}

	Image::Image( String const & name
		, Path const & pathFile
		, bool dropAlpha )
		: Resource< Image >( name )
		, m_pathFile( pathFile )
	{
		Image::BinaryLoader()( *this, pathFile, dropAlpha );
	}

	Image::Image( Image const & image )
		: Resource< Image >( image )
		, m_pathFile( image.m_pathFile )
		, m_buffer( image.m_buffer->clone() )
	{
		CU_CheckInvariants();
	}

	Image::Image( Image && image )
		: Resource< Image >( std::move( image ) )
		, m_pathFile( std::move( image.m_pathFile ) )
		, m_buffer( std::move( image.m_buffer ) )
	{
		image.m_pathFile.clear();
		image.m_buffer.reset();
		CU_CheckInvariants();
	}

	Image & Image::operator=( Image const & image )
	{
		Resource< Image >::operator=( image );
		m_pathFile = image.m_pathFile;
		m_buffer = image.m_buffer->clone();
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::operator =( Image && image )
	{
		Resource< Image >::operator=( std::move( image ) );
		m_pathFile = std::move( image.m_pathFile );
		m_buffer = std::move( image.m_buffer );
		image.m_pathFile.clear();
		image.m_buffer.reset();
		CU_CheckInvariants();
		return * this;
	}

	Image::~Image()
	{
	}

	CU_BeginInvariantBlock( Image )
	CU_CheckInvariant( m_buffer->count() > 0 );
	CU_EndInvariantBlock()

	Image & Image::fill( RgbColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );

		for ( uint32_t i = uiBpp; i < m_buffer->size(); i += uiBpp )
		{
			memcpy( &m_buffer->ptr()[i], &m_buffer->constPtr()[0], uiBpp );
		}

		CU_CheckInvariants();
		return * this;
	}

	Image & Image::fill( RgbaColour const & colour )
	{
		CU_CheckInvariants();
		setPixel( 0, 0, colour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );

		for ( uint32_t i = uiBpp; i < m_buffer->size(); i += uiBpp )
		{
			memcpy( &m_buffer->ptr()[i], &m_buffer->constPtr()[0], uiBpp );
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
		CU_Require( x < getWidth() && y < getHeight() );
		Point4ub components = toBGRAByte( colour );
		uint8_t const * src = components.constPtr();
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eA8R8G8B8, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::setPixel( uint32_t x
		, uint32_t y
		, RgbaColour const & colour )
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() );
		Point3ub components = toBGRByte( colour );
		uint8_t const * src = components.constPtr();
		uint8_t * dst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eR8G8B8, src, getPixelFormat(), dst );
		CU_CheckInvariants();
		return * this;
	}

	void Image::getPixel( uint32_t x
		, uint32_t y
		, uint8_t * pixel
		, PixelFormat format )const
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() && pixel );
		uint8_t const * src = &( *m_buffer->getAt( x, y ) );
		uint8_t * dst = pixel;
		PF::convertPixel( getPixelFormat(), src, format, dst );
		CU_CheckInvariants();
	}

	RgbaColour Image::getPixel( uint32_t x
		, uint32_t y )const
	{
		CU_CheckInvariants();
		CU_Require( x < getWidth() && y < getHeight() );
		Point4ub components;
		uint8_t const * src = &( *m_buffer->getAt( x, y ) );
		uint8_t * dst = components.ptr();
		PF::convertPixel( getPixelFormat(), src, PixelFormat::eA8R8G8B8, dst );
		CU_CheckInvariants();
		return RgbaColour::fromBGRA( components );
	}

	Image & Image::copyImage( Image const & toCopy )
	{
		CU_CheckInvariants();

		if ( getDimensions() == toCopy.getDimensions() )
		{
			if ( getPixelFormat() == toCopy.getPixelFormat() )
			{
				memcpy( m_buffer->ptr(), toCopy.m_buffer->ptr(), m_buffer->size() );
			}
			else
			{
				for ( uint32_t i = 0; i < getWidth(); ++i )
				{
					for ( uint32_t j = 0; j < getHeight(); ++j )
					{
						uint8_t const * src = &( *toCopy.m_buffer->getAt( i, j ) );
						uint8_t * dst = &( *m_buffer->getAt( i, j ) );
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
		CU_Require( Rectangle( 0, 0 , getWidth(), getHeight() ).intersects( rect ) == Intersection::eIn );
		Size size( rect.getWidth(), rect.getHeight() );
		// Création de la sous-image à remplir
		Image img( m_name + cuT( "_Sub" ) + string::toString( rect[0] ) + cuT( "x" ) + string::toString( rect[1] ) + cuT( ":" ) + string::toString( size.getWidth() ) + cuT( "x" ) + string::toString( size.getHeight() ), size, getPixelFormat() );
		// Calcul de variables temporaires
		uint8_t const * src = &( *m_buffer->getAt( rect.left(), rect.top() ) );
		uint8_t * dst = &( *img.m_buffer->getAt( 0, 0 ) );
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
		m_buffer->flip();
		CU_CheckInvariants();
		return * this;
	}

	Image & Image::mirror()
	{
		CU_CheckInvariants();
		m_buffer->mirror();
		CU_CheckInvariants();
		return * this;
	}
}
