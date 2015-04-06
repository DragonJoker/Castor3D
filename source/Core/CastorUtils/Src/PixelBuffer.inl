namespace Castor
{
	template< TPL_PIXEL_FORMAT FT >
	PxBuffer< FT >::PxBuffer( Size const & p_size, uint8_t const * p_pBuffer, ePIXEL_FORMAT p_eBufferFormat )
		: PxBufferBase( p_size, ePIXEL_FORMAT( FT ), p_pBuffer, p_eBufferFormat )
		, m_column( p_size.width() )
	{
		init( p_pBuffer, p_eBufferFormat );
	}

	template< TPL_PIXEL_FORMAT FT >
	PxBuffer< FT >::PxBuffer( PxBuffer< FT > const & p_pixelBuffer )
		: PxBufferBase( p_pixelBuffer )
		, m_column( p_pixelBuffer.width() )
	{
		init( p_pixelBuffer.const_ptr(), p_pixelBuffer.format() );
	}

	template< TPL_PIXEL_FORMAT FT >
	PxBuffer< FT >::PxBuffer( PxBuffer< FT > && p_pixelBuffer )
		: PxBufferBase( std::move( p_pixelBuffer ) )
		, m_column( std::move( p_pixelBuffer.m_column ) )
	{
		p_pixelBuffer.m_column.clear();
	}

	template< TPL_PIXEL_FORMAT FT >
	PxBuffer< FT >::~PxBuffer()
	{
		clear();
	}

	template< TPL_PIXEL_FORMAT FT >
	PxBuffer< FT > & PxBuffer< FT >::operator =( PxBuffer< FT > const & p_pixelBuffer )
	{
		PxBufferBase::operator=( p_pixelBuffer );
		return * this;
	}

	template< TPL_PIXEL_FORMAT FT >
	PxBuffer< FT > & PxBuffer< FT >::operator =( PxBuffer< FT > && p_pixelBuffer )
	{
		PxBufferBase::operator=( p_pixelBuffer );
		m_column = std::move( p_pixelBuffer.m_column );
		p_pixelBuffer.m_column.clear();
		return * this;
	}

	template< TPL_PIXEL_FORMAT FT >
	void PxBuffer< FT >::swap( PxBuffer< FT > & p_pixelBuffer )
	{
		PxBufferBase::swap( p_pixelBuffer );
	}

	template< TPL_PIXEL_FORMAT FT >
	void PxBuffer< FT >::assign( std::vector< uint8_t > const & p_pBuffer, ePIXEL_FORMAT p_eBufferFormat )
	{
		uint8_t l_uiSize = PF::GetBytesPerPixel( p_eBufferFormat );
		uint32_t l_uiDstMax = count();
		uint32_t l_uiSrcMax = uint32_t( p_pBuffer.size() / l_uiSize );

		if ( p_pBuffer.size() > 0 && l_uiSrcMax == l_uiDstMax )
		{
			PF::ConvertBuffer( p_eBufferFormat, p_pBuffer.data(), uint32_t( p_pBuffer.size() ), format(), m_pBuffer, size() );
		}
	}

	template< TPL_PIXEL_FORMAT FT >
	uint8_t const * PxBuffer< FT >::const_ptr()const
	{
		return m_pBuffer;
	}

	template< TPL_PIXEL_FORMAT FT >
	uint8_t * PxBuffer< FT >::ptr()
	{
		return m_pBuffer;
	}

	template< TPL_PIXEL_FORMAT FT >
	uint32_t PxBuffer< FT >::size()const
	{
		return count() * pixel_definitions< FT >::Size;
	}

	template< TPL_PIXEL_FORMAT FT >
	std::shared_ptr< PxBufferBase > PxBuffer< FT >::clone()const
	{
		return std::make_shared< PxBuffer >( *this );
	}

	template< TPL_PIXEL_FORMAT FT >
	uint8_t * PxBuffer< FT >::get_at( uint32_t x, uint32_t y )
	{
		CASTOR_ASSERT( x < width() && y < height() );
		return &m_pBuffer[( x * height() + y ) * pixel_definitions< FT >::Size];
	}

	template< TPL_PIXEL_FORMAT FT >
	uint8_t const * PxBuffer< FT >::get_at( uint32_t x, uint32_t y )const
	{
		CASTOR_ASSERT( x < width() && y < height() );
		return &m_pBuffer[( x * height() + y ) * pixel_definitions< FT >::Size];
	}

	template< TPL_PIXEL_FORMAT FT >
	void PxBuffer< FT >::flip()
	{
		uint32_t 	l_uiWidth		= width() * pixel_definitions< FT >::Size;
		uint32_t	l_uiHeight		= height();
		uint8_t  *	l_pBufferTop	= &m_pBuffer[0];
		uint8_t  *	l_pBufferBottom	= &m_pBuffer[( l_uiHeight - 1 ) * l_uiWidth - 1];

		for ( uint32_t i = 0; i < l_uiHeight / 2; i++ )
		{
			for ( uint32_t j = 0; j < l_uiWidth; j++ )
			{
				std::swap( l_pBufferTop[j], l_pBufferBottom[j] );
			}

			l_pBufferTop	+= l_uiWidth;
			l_pBufferBottom	-= l_uiWidth;
		}
	}

	template< TPL_PIXEL_FORMAT FT >
	void PxBuffer< FT >::mirror()
	{
		uint32_t 	l_uiWidth		= width() * pixel_definitions< FT >::Size;
		uint32_t	l_uiHeight		= height();
		uint8_t		*	l_pxA, *l_pxB;

		for ( uint32_t i = 0; i < l_uiHeight; i++ )
		{
			l_pxA = &m_pBuffer[i * l_uiWidth];
			l_pxB = &m_pBuffer[( i + 1 ) * l_uiWidth - 1];

			for ( uint32_t j = 0; j < l_uiWidth / 2; j += pixel_definitions< FT >::Size )
			{
				for ( uint32_t k = 0; k < pixel_definitions< FT >::Size; k++ )
				{
					std::swap( l_pxA[k], l_pxB[k] );
				}

				l_pxA += pixel_definitions< FT >::Size;
				l_pxB -= pixel_definitions< FT >::Size;
			}
		}
	}

	template< TPL_PIXEL_FORMAT FT >
	void PxBuffer< FT >::do_init_column( uint32_t p_uiColumn )const
	{
		if ( p_uiColumn < width() )
		{
			uint8_t * l_pBuffer = &m_pBuffer[p_uiColumn * width() * pixel_definitions< FT >::Size];
			m_column = column( height() );

			for ( uint32_t j = 0; j < height(); j++ )
			{
				m_column[j].link( l_pBuffer );
				l_pBuffer += pixel_definitions< FT >::Size;
			}
		}
	}
}
