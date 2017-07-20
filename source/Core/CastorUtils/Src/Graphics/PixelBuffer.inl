namespace Castor
{
	template< PixelFormat FT >
	PxBuffer< FT >::PxBuffer( Size const & p_size, uint8_t const * p_buffer, PixelFormat p_bufferFormat )
		: PxBufferBase( p_size, PixelFormat( FT ) )
		, m_column( p_size.width() )
	{
		init( p_buffer, p_bufferFormat );
	}

	template< PixelFormat FT >
	PxBuffer< FT >::PxBuffer( PxBuffer< FT > const & p_pixelBuffer )
		: PxBufferBase( p_pixelBuffer )
		, m_column( p_pixelBuffer.width() )
	{
		init( p_pixelBuffer.const_ptr(), p_pixelBuffer.format() );
	}

	template< PixelFormat FT >
	PxBuffer< FT >::PxBuffer( PxBuffer< FT > && p_pixelBuffer )
		: PxBufferBase( std::move( p_pixelBuffer ) )
		, m_column( std::move( p_pixelBuffer.m_column ) )
	{
		p_pixelBuffer.m_column.clear();
	}

	template< PixelFormat FT >
	PxBuffer< FT >::~PxBuffer()
	{
		clear();
	}

	template< PixelFormat FT >
	PxBuffer< FT > & PxBuffer< FT >::operator=( PxBuffer< FT > const & p_pixelBuffer )
	{
		PxBufferBase::operator=( p_pixelBuffer );
		return * this;
	}

	template< PixelFormat FT >
	PxBuffer< FT > & PxBuffer< FT >::operator=( PxBuffer< FT > && p_pixelBuffer )
	{
		PxBufferBase::operator=( p_pixelBuffer );
		m_column = std::move( p_pixelBuffer.m_column );
		p_pixelBuffer.m_column.clear();
		return * this;
	}

	template< PixelFormat FT >
	typename PxBuffer< FT >::column const & PxBuffer< FT >::operator[]( uint32_t p_index )const
	{
		do_init_column( p_index );
		return m_column;
	}

	template< PixelFormat FT >
	typename PxBuffer< FT >::column & PxBuffer< FT >::operator[]( uint32_t p_index )
	{
		do_init_column( p_index );
		return m_column;
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::swap( PxBuffer< FT > & p_pixelBuffer )
	{
		PxBufferBase::swap( p_pixelBuffer );
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::assign( std::vector< uint8_t > const & p_buffer, PixelFormat p_bufferFormat )
	{
		uint8_t newSize = PF::GetBytesPerPixel( p_bufferFormat );
		uint32_t dstMax = count();
		uint32_t srcMax = uint32_t( p_buffer.size() / newSize );

		if ( p_buffer.size() > 0 && srcMax == dstMax )
		{
			PF::ConvertBuffer( p_bufferFormat, p_buffer.data(), uint32_t( p_buffer.size() ), format(), m_buffer.data(), size() );
		}
	}

	template< PixelFormat FT >
	uint8_t const * PxBuffer< FT >::const_ptr()const
	{
		return m_buffer.data();
	}

	template< PixelFormat FT >
	uint8_t * PxBuffer< FT >::ptr()
	{
		return m_buffer.data();
	}

	template< PixelFormat FT >
	uint32_t PxBuffer< FT >::size()const
	{
		return count() * pixel_definitions< FT >::Size;
	}

	template< PixelFormat FT >
	std::shared_ptr< PxBufferBase > PxBuffer< FT >::clone()const
	{
		return std::make_shared< PxBuffer >( *this );
	}

	template< PixelFormat FT >
	typename PxBufferBase::pixel_data PxBuffer< FT >::get_at( uint32_t x, uint32_t y )
	{
		REQUIRE( x < width() && y < height() );
		return m_buffer.begin() + ( do_convert( x, y ) * pixel_definitions< FT >::Size );
	}

	template< PixelFormat FT >
	typename PxBufferBase::const_pixel_data PxBuffer< FT >::get_at( uint32_t x, uint32_t y )const
	{
		REQUIRE( x < width() && y < height() );
		return m_buffer.begin() + ( do_convert( x, y ) * pixel_definitions< FT >::Size );
	}

	template< PixelFormat FT >
	Pixel< FT > PxBuffer< FT >::at( uint32_t x, uint32_t y )
	{
		REQUIRE( x < width() && y < height() );
		return *( begin() + do_convert( x, y ) );
	}

	template< PixelFormat FT >
	Pixel< FT > PxBuffer< FT >::at( uint32_t x, uint32_t y )const
	{
		REQUIRE( x < width() && y < height() );
		return *( begin() + do_convert( x, y ) );
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::mirror()
	{
		uint32_t mwidth = width() * pixel_definitions< FT >::Size;
		uint32_t mheight = height();

		for ( uint32_t i = 0; i < mheight; i++ )
		{
			uint8_t * pxA = &m_buffer[i * mwidth];
			uint8_t * rhs = &m_buffer[( i + 1 ) * mwidth - 1];

			for ( uint32_t j = 0; j < mwidth / 2; j += pixel_definitions< FT >::Size )
			{
				for ( uint32_t k = 0; k < pixel_definitions< FT >::Size; k++ )
				{
					std::swap( pxA[k], rhs[k] );
				}

				pxA += pixel_definitions< FT >::Size;
				rhs -= pixel_definitions< FT >::Size;
			}
		}
	}

	template< PixelFormat FT >
	uint32_t PxBuffer< FT >::do_convert( uint32_t x, uint32_t y )const
	{
		return y * width() + x;
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::do_init_column( uint32_t p_column )const
	{
		if ( p_column < width() )
		{
			uint8_t const * buffer = &m_buffer[p_column * width() * pixel_definitions< FT >::Size];
			m_column = column( height() );

			for ( uint32_t j = 0; j < height(); j++ )
			{
				m_column[j].unlink();
				m_column[j].template set< FT >( buffer );
				buffer += pixel_definitions< FT >::Size;
			}
		}
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::do_init_column( uint32_t p_column )
	{
		if ( p_column < width() )
		{
			uint8_t * buffer = &m_buffer[p_column * width() * pixel_definitions< FT >::Size];
			m_column = column( height() );

			for ( uint32_t j = 0; j < height(); j++ )
			{
				m_column[j].link( buffer );
				buffer += pixel_definitions< FT >::Size;
			}
		}
	}
}
