#include "Image.hpp"
#include "Rectangle.hpp"

namespace Castor
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

	Image & Image::Fill( Colour const & p_clrColour )
	{
		CHECK_INVARIANTS();
		SetPixel( 0, 0, p_clrColour );
		uint32_t uiBpp = PF::GetBytesPerPixel( GetPixelFormat() );

		for ( uint32_t i = uiBpp; i < m_buffer->size(); i += uiBpp )
		{
			memcpy( &m_buffer->ptr()[i], &m_buffer->const_ptr()[0], uiBpp );
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::SetPixel( uint32_t x, uint32_t y, uint8_t const * p_pPixel, PixelFormat p_format )
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() && p_pPixel );
		uint8_t const * pSrc = p_pPixel;
		uint8_t * pDst = &( *m_buffer->get_at( x, y ) );
		PF::ConvertPixel( p_format, pSrc, GetPixelFormat(), pDst );
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::SetPixel( uint32_t x, uint32_t y, Colour const & p_clrColour )
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() );
		Point4ub ptComponents = bgra_byte( p_clrColour );
		uint8_t const * pSrc = ptComponents.const_ptr();
		uint8_t * pDst = &( *m_buffer->get_at( x, y ) );
		PF::ConvertPixel( PixelFormat::eA8R8G8B8, pSrc, GetPixelFormat(), pDst );
		CHECK_INVARIANTS();
		return * this;
	}

	void Image::GetPixel( uint32_t x, uint32_t y, uint8_t * p_pPixel, PixelFormat p_format )const
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() && p_pPixel );
		uint8_t const * pSrc = &( *m_buffer->get_at( x, y ) );
		uint8_t * pDst = p_pPixel;
		PF::ConvertPixel( GetPixelFormat(), pSrc, p_format, pDst );
		CHECK_INVARIANTS();
	}

	Colour Image::GetPixel( uint32_t x, uint32_t y )const
	{
		CHECK_INVARIANTS();
		REQUIRE( x < GetWidth() && y < GetHeight() );
		Point4ub ptComponents;
		uint8_t const * pSrc = &( *m_buffer->get_at( x, y ) );
		uint8_t * pDst = ptComponents.ptr();
		PF::ConvertPixel( GetPixelFormat(), pSrc, PixelFormat::eA8R8G8B8, pDst );
		CHECK_INVARIANTS();
		return Colour::from_bgra( ptComponents );
	}

	Image & Image::CopyImage( Image const & p_src )
	{
		CHECK_INVARIANTS();

		if ( GetDimensions() == p_src.GetDimensions() )
		{
			if ( GetPixelFormat() == p_src.GetPixelFormat() )
			{
				memcpy( m_buffer->ptr(), p_src.m_buffer->ptr(), m_buffer->size() );
			}
			else
			{
				for ( uint32_t i = 0; i < GetWidth(); ++i )
				{
					for ( uint32_t j = 0; j < GetHeight(); ++j )
					{
						uint8_t const * pSrc = &( *p_src.m_buffer->get_at( i, j ) );
						uint8_t * pDst = &( *m_buffer->get_at( i, j ) );
						PF::ConvertPixel( GetPixelFormat(), pSrc, GetPixelFormat(), pDst );
					}
				}
			}
		}
		else
		{
			Point2d ptSrcStep( static_cast< double >( p_src.GetWidth() ) / GetWidth(), static_cast< double >( p_src.GetHeight() ) / GetHeight() );
			ByteArray pSrcPix( PF::GetBytesPerPixel( p_src.GetPixelFormat() ), 0 );

			for ( uint32_t i = 0; i < GetWidth(); ++i )
			{
				for ( uint32_t j = 0; j < GetHeight(); ++j )
				{
					p_src.GetPixel( static_cast< uint32_t >( i * ptSrcStep[0] ), static_cast< uint32_t >( j * ptSrcStep[1] ), pSrcPix.data(), p_src.GetPixelFormat() );
					SetPixel( i, j, pSrcPix.data(), p_src.GetPixelFormat() );
				}
			}
		}

		CHECK_INVARIANTS();
		return * this;
	}

	Image Image::SubImage( Rectangle const & rcRect )const
	{
		CHECK_INVARIANTS();
		REQUIRE( Rectangle( 0, 0 , GetWidth(), GetHeight() ).intersects( rcRect ) == Intersection::eIn );
		Size ptSize( rcRect.width(), rcRect.height() );
		// Création de la sous-image à remplir
		Image img( m_name + cuT( "_Sub" ) + string::to_string( rcRect[0] ) + cuT( "x" ) + string::to_string( rcRect[1] ) + cuT( ":" ) + string::to_string( ptSize.width() ) + cuT( "x" ) + string::to_string( ptSize.height() ), ptSize, GetPixelFormat() );
		// Calcul de variables temporaires
		uint8_t const * pSrc = &( *m_buffer->get_at( rcRect.left(), rcRect.top() ) );
		uint8_t * pDest = &( *img.m_buffer->get_at( 0, 0 ) );
		uint32_t uiSrcPitch = GetWidth() * PF::GetBytesPerPixel( GetPixelFormat() );
		uint32_t uiDestPitch = img.GetWidth() * PF::GetBytesPerPixel( img.GetPixelFormat() );

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

	Image & Image::Flip()
	{
		CHECK_INVARIANTS();
		m_buffer->flip();
		CHECK_INVARIANTS();
		return * this;
	}

	Image & Image::Mirror()
	{
		CHECK_INVARIANTS();
		m_buffer->mirror();
		CHECK_INVARIANTS();
		return * this;
	}
}
