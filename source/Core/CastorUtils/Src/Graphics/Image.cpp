#include "Image.hpp"
#include "Rectangle.hpp"

namespace castor
{
	Image::Image( String const & p_name, Size const & p_size, PixelFormat p_format, ByteArray const & p_buffer, PixelFormat p_eBufferFormat )
		: Resource< Image >( p_name )
		, m_buffer( PxBufferBase::create( p_size, p_format, &p_buffer[0], p_eBufferFormat ) )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( String const & p_name, Size const & p_size, PixelFormat p_format, uint8_t const * p_buffer, PixelFormat p_eBufferFormat )
		: Resource< Image >( p_name )
		, m_buffer( PxBufferBase::create( p_size, p_format, p_buffer, p_eBufferFormat ) )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( String const & p_name, PxBufferBase const & p_buffer )
		: Resource< Image >( p_name )
		, m_buffer( p_buffer.clone() )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( String const & p_name, Path const & p_pathFile )
		: Resource< Image >( p_name )
		, m_pathFile( p_pathFile )
	{
		Image::BinaryLoader()( *this, p_pathFile );
	}

	Image::Image( Image const & p_image )
		: Resource< Image >( p_image )
		, m_pathFile( p_image.m_pathFile )
		, m_buffer( p_image.m_buffer->clone() )
	{
		CHECK_INVARIANTS();
	}

	Image::Image( Image && p_image )
		: Resource< Image >( std::move( p_image ) )
		, m_pathFile( std::move( p_image.m_pathFile ) )
		, m_buffer( std::move( p_image.m_buffer ) )
	{
		p_image.m_pathFile.clear();
		p_image.m_buffer.reset();
		CHECK_INVARIANTS();
	}

	Image & Image::operator=( Image const & p_image )
	{
		Resource< Image >::operator=( p_image );
		m_pathFile = p_image.m_pathFile;
		m_buffer = p_image.m_buffer->clone();
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::operator =( Image && p_image )
	{
		Resource< Image >::operator=( std::move( p_image ) );

		if ( this != & p_image )
		{
			m_pathFile = std::move( p_image.m_pathFile );
			m_buffer = std::move( p_image.m_buffer );
			p_image.m_pathFile.clear();
			p_image.m_buffer.reset();
			CHECK_INVARIANTS();
		}

		return * this;
	}

	Image::~Image()
	{
	}

	BEGIN_INVARIANT_BLOCK( Image )
	CHECK_INVARIANT( m_buffer->count() > 0 );
	END_INVARIANT_BLOCK()

	Image & Image::fill( Colour const & p_clrColour )
	{
		CHECK_INVARIANTS();
		setPixel( 0, 0, p_clrColour );
		uint32_t uiBpp = PF::getBytesPerPixel( getPixelFormat() );

		for ( uint32_t i = uiBpp; i < m_buffer->size(); i += uiBpp )
		{
			memcpy( &m_buffer->ptr()[i], &m_buffer->constPtr()[0], uiBpp );
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::setPixel( uint32_t x, uint32_t y, uint8_t const * p_pPixel, PixelFormat p_format )
	{
		CHECK_INVARIANTS();
		REQUIRE( x < getWidth() && y < getHeight() && p_pPixel );
		uint8_t const * pSrc = p_pPixel;
		uint8_t * pDst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( p_format, pSrc, getPixelFormat(), pDst );
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::setPixel( uint32_t x, uint32_t y, Colour const & p_clrColour )
	{
		CHECK_INVARIANTS();
		REQUIRE( x < getWidth() && y < getHeight() );
		Point4ub ptComponents = toBGRAByte( p_clrColour );
		uint8_t const * pSrc = ptComponents.constPtr();
		uint8_t * pDst = &( *m_buffer->getAt( x, y ) );
		PF::convertPixel( PixelFormat::eA8R8G8B8, pSrc, getPixelFormat(), pDst );
		CHECK_INVARIANTS();
		return * this;
	}

	void Image::getPixel( uint32_t x, uint32_t y, uint8_t * p_pPixel, PixelFormat p_format )const
	{
		CHECK_INVARIANTS();
		REQUIRE( x < getWidth() && y < getHeight() && p_pPixel );
		uint8_t const * pSrc = &( *m_buffer->getAt( x, y ) );
		uint8_t * pDst = p_pPixel;
		PF::convertPixel( getPixelFormat(), pSrc, p_format, pDst );
		CHECK_INVARIANTS();
	}

	Colour Image::getPixel( uint32_t x, uint32_t y )const
	{
		CHECK_INVARIANTS();
		REQUIRE( x < getWidth() && y < getHeight() );
		Point4ub ptComponents;
		uint8_t const * pSrc = &( *m_buffer->getAt( x, y ) );
		uint8_t * pDst = ptComponents.ptr();
		PF::convertPixel( getPixelFormat(), pSrc, PixelFormat::eA8R8G8B8, pDst );
		CHECK_INVARIANTS();
		return Colour::fromBGRA( ptComponents );
	}

	Image & Image::copyImage( Image const & p_src )
	{
		CHECK_INVARIANTS();

		if ( getDimensions() == p_src.getDimensions() )
		{
			if ( getPixelFormat() == p_src.getPixelFormat() )
			{
				memcpy( m_buffer->ptr(), p_src.m_buffer->ptr(), m_buffer->size() );
			}
			else
			{
				for ( uint32_t i = 0; i < getWidth(); ++i )
				{
					for ( uint32_t j = 0; j < getHeight(); ++j )
					{
						uint8_t const * pSrc = &( *p_src.m_buffer->getAt( i, j ) );
						uint8_t * pDst = &( *m_buffer->getAt( i, j ) );
						PF::convertPixel( getPixelFormat(), pSrc, getPixelFormat(), pDst );
					}
				}
			}
		}
		else
		{
			Point2d ptSrcStep( static_cast< double >( p_src.getWidth() ) / getWidth(), static_cast< double >( p_src.getHeight() ) / getHeight() );
			ByteArray pSrcPix( PF::getBytesPerPixel( p_src.getPixelFormat() ), 0 );

			for ( uint32_t i = 0; i < getWidth(); ++i )
			{
				for ( uint32_t j = 0; j < getHeight(); ++j )
				{
					p_src.getPixel( static_cast< uint32_t >( i * ptSrcStep[0] ), static_cast< uint32_t >( j * ptSrcStep[1] ), pSrcPix.data(), p_src.getPixelFormat() );
					setPixel( i, j, pSrcPix.data(), p_src.getPixelFormat() );
				}
			}
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Image Image::subImage( Rectangle const & rcRect )const
	{
		CHECK_INVARIANTS();
		REQUIRE( Rectangle( 0, 0 , getWidth(), getHeight() ).intersects( rcRect ) == Intersection::eIn );
		Size ptSize( rcRect.getWidth(), rcRect.getHeight() );
		// Création de la sous-image à remplir
		Image img( m_name + cuT( "_Sub" ) + string::toString( rcRect[0] ) + cuT( "x" ) + string::toString( rcRect[1] ) + cuT( ":" ) + string::toString( ptSize.getWidth() ) + cuT( "x" ) + string::toString( ptSize.getHeight() ), ptSize, getPixelFormat() );
		// Calcul de variables temporaires
		uint8_t const * pSrc = &( *m_buffer->getAt( rcRect.left(), rcRect.top() ) );
		uint8_t * pDest = &( *img.m_buffer->getAt( 0, 0 ) );
		uint32_t uiSrcPitch = getWidth() * PF::getBytesPerPixel( getPixelFormat() );
		uint32_t uiDestPitch = img.getWidth() * PF::getBytesPerPixel( img.getPixelFormat() );

		// Copie des pixels de l'image originale dans la sous-image
		for ( int i = rcRect.left(); i < rcRect.right(); ++i )
		{
			std::memcpy( pDest, pSrc, uiDestPitch );
			pSrc += uiSrcPitch;
			pDest += uiDestPitch;
		}

		CHECK_INVARIANTS();
		return img;
	}

	Image & Image::flip()
	{
		CHECK_INVARIANTS();
		m_buffer->flip();
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::mirror()
	{
		CHECK_INVARIANTS();
		m_buffer->mirror();
		CHECK_INVARIANTS();
		return * this;
	}
}
