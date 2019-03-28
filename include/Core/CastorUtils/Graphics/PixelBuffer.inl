namespace castor
{
	template< PixelFormat FT >
	PxBuffer< FT >::PxBuffer( Size const & size
		, uint8_t const * buffer
		, PixelFormat bufferFormat )
		: PxBufferBase( size, PixelFormat( FT ) )
		, m_column( size.getWidth() )
	{
		initialise( buffer, bufferFormat );
	}

	template< PixelFormat FT >
	PxBuffer< FT >::PxBuffer( PxBuffer< FT > const & pixelBuffer )
		: PxBufferBase( pixelBuffer )
		, m_column( pixelBuffer.getWidth() )
	{
		initialise( pixelBuffer.getConstPtr(), pixelBuffer.getFormat() );
	}

	template< PixelFormat FT >
	PxBuffer< FT >::PxBuffer( PxBuffer< FT > && pixelBuffer )
		: PxBufferBase( std::move( pixelBuffer ) )
		, m_column( std::move( pixelBuffer.m_column ) )
	{
		pixelBuffer.m_column.clear();
	}

	template< PixelFormat FT >
	PxBuffer< FT >::~PxBuffer()
	{
		clear();
	}

	template< PixelFormat FT >
	PxBuffer< FT > & PxBuffer< FT >::operator=( PxBuffer< FT > const & pixelBuffer )
	{
		PxBufferBase::operator=( pixelBuffer );
		return * this;
	}

	template< PixelFormat FT >
	PxBuffer< FT > & PxBuffer< FT >::operator=( PxBuffer< FT > && pixelBuffer )
	{
		PxBufferBase::operator=( pixelBuffer );
		m_column = std::move( pixelBuffer.m_column );
		pixelBuffer.m_column.clear();
		return * this;
	}

	template< PixelFormat FT >
	typename PxBuffer< FT >::column const & PxBuffer< FT >::operator[]( uint32_t index )const
	{
		doInitColumn( index );
		return m_column;
	}

	template< PixelFormat FT >
	typename PxBuffer< FT >::column & PxBuffer< FT >::operator[]( uint32_t index )
	{
		doInitColumn( index );
		return m_column;
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::swap( PxBuffer< FT > & pixelBuffer )
	{
		PxBufferBase::swap( pixelBuffer );
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::assign( std::vector< uint8_t > const & buffer
		, PixelFormat bufferFormat )
	{
		uint8_t newSize = PF::getBytesPerPixel( bufferFormat );
		uint32_t dstMax = getCount();
		uint32_t srcMax = uint32_t( buffer.size() / newSize );

		if ( buffer.size() > 0 && srcMax == dstMax )
		{
			PF::convertBuffer( bufferFormat
				, buffer.data()
				, uint32_t( buffer.size() )
				, getFormat()
				, m_buffer.data()
				, getSize() );
		}
	}

	template< PixelFormat FT >
	uint8_t const * PxBuffer< FT >::getConstPtr()const
	{
		return m_buffer.data();
	}

	template< PixelFormat FT >
	uint8_t * PxBuffer< FT >::getPtr()
	{
		return m_buffer.data();
	}

	template< PixelFormat FT >
	uint32_t PxBuffer< FT >::getSize()const
	{
		return PF::isCompressed( getFormat() )
			? ( getCount() * PixelDefinitions< FT >::Size ) / 16u
			: getCount() * PixelDefinitions< FT >::Size;
	}

	template< PixelFormat FT >
	std::shared_ptr< PxBufferBase > PxBuffer< FT >::clone()const
	{
		return std::make_shared< PxBuffer >( *this );
	}

	template< PixelFormat FT >
	typename PxBufferBase::pixel_data PxBuffer< FT >::getAt( uint32_t x, uint32_t y )
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return m_buffer.begin() + ( doConvert( x, y ) * PixelDefinitions< FT >::Size );
	}

	template< PixelFormat FT >
	typename PxBufferBase::const_pixel_data PxBuffer< FT >::getAt( uint32_t x, uint32_t y )const
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return m_buffer.begin() + ( doConvert( x, y ) * PixelDefinitions< FT >::Size );
	}

	template< PixelFormat FT >
	Pixel< FT > PxBuffer< FT >::at( uint32_t x, uint32_t y )
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return *( begin() + doConvert( x, y ) );
	}

	template< PixelFormat FT >
	Pixel< FT > PxBuffer< FT >::at( uint32_t x, uint32_t y )const
	{
		CU_Require( x < getWidth() && y < getHeight() );
		return *( begin() + doConvert( x, y ) );
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::mirror()
	{
		uint32_t mwidth = getWidth() * PixelDefinitions< FT >::Size;
		uint32_t mheight = getHeight();

		for ( uint32_t i = 0; i < mheight; i++ )
		{
			uint8_t * pxA = &m_buffer[i * mwidth];
			uint8_t * rhs = &m_buffer[( i + 1 ) * mwidth - 1];

			for ( uint32_t j = 0; j < mwidth / 2; j += PixelDefinitions< FT >::Size )
			{
				for ( uint32_t k = 0; k < PixelDefinitions< FT >::Size; k++ )
				{
					std::swap( pxA[k], rhs[k] );
				}

				pxA += PixelDefinitions< FT >::Size;
				rhs -= PixelDefinitions< FT >::Size;
			}
		}
	}

	template< PixelFormat FT >
	uint32_t PxBuffer< FT >::doConvert( uint32_t x, uint32_t y )const
	{
		return y * getWidth() + x;
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::doInitColumn( uint32_t col )const
	{
		if ( col < getWidth() )
		{
			uint8_t const * buffer = &m_buffer[col * getWidth() * PixelDefinitions< FT >::Size];
			m_column = column( getHeight() );

			for ( uint32_t j = 0; j < getHeight(); j++ )
			{
				m_column[j].unlink();
				m_column[j].template set< FT >( buffer );
				buffer += PixelDefinitions< FT >::Size;
			}
		}
	}

	template< PixelFormat FT >
	void PxBuffer< FT >::doInitColumn( uint32_t col )
	{
		if ( col < getWidth() )
		{
			uint8_t * buffer = &m_buffer[col * getWidth() * PixelDefinitions< FT >::Size];
			m_column = column( getHeight() );

			for ( uint32_t j = 0; j < getHeight(); j++ )
			{
				m_column[j].link( buffer );
				buffer += PixelDefinitions< FT >::Size;
			}
		}
	}
}
