namespace Castor
{
	//*************************************************************************************************

	template< TPL_PIXEL_FORMAT FT, TPL_PIXEL_FORMAT  FU > struct PxOperators
	{
		static void Add( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB )
		{
			Pixel< FT > & l_pxB( p_pxB );
			PxOperators< FT, FT >::Add( p_pxA, l_pxB );
		}

		static void Substract( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB )
		{
			Pixel< FT > & l_pxB( p_pxB );
			PxOperators< FT, FT >::Substract( p_pxA, l_pxB );
		}

		static void Multiply( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB )
		{
			Pixel< FT > & l_pxB( p_pxB );
			PxOperators< FT, FT >::Multiply( p_pxA, l_pxB );
		}

		static void Divide( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB )
		{
			Pixel< FT > & l_pxB( p_pxB );
			PxOperators< FT, FT >::Divide( p_pxA, l_pxB );
		}

		static void Assign( Pixel< FT > & p_pxA, Pixel< FU > const & p_pxB )
		{
			Pixel< FT > & l_pxB( p_pxB );
			PxOperators< FT, FT >::Assign( p_pxA, l_pxB );
		}

		static bool Equals( Pixel< FT > const & p_pxA, Pixel< FU > const & p_pxB )
		{
			Pixel< FT > & l_pxB( p_pxB );
			return PxOperators< FT, FT >::Equals( p_pxA, l_pxB );
		}
	};

	template< TPL_PIXEL_FORMAT FT > struct PxOperators< FT, FT >
	{
		static void Add( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB )
		{
			for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
			{
				p_pxA[i] += p_pxB[i];
			}
		}

		static void Substract( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB )
		{
			for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
			{
				p_pxA[i] -= p_pxB[i];
			}
		}
		static void Multiply( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB )
		{
			for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
			{
				p_pxA[i] *= p_pxB[i];
			}
		}

		static void Divide( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB )
		{
			for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
			{
				p_pxA[i] = ( p_pxB[i] == 0 ? p_pxA[i] : p_pxA[i] / p_pxB[i] );
			}
		}

		static void Assign( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB )
		{
			for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
			{
				p_pxA[i] = p_pxB[i];
			}
		}

		static bool Equals( Pixel< FT > & p_pxA, Pixel< FT > const & p_pxB )
		{
			bool l_return = true;

			for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
			{
				l_return = p_pxA[i] == p_pxB[i];
			}

			return l_return;
		}
	};

	//*************************************************************************************************

	template< TPL_PIXEL_FORMAT FT >
	Pixel< FT >::Pixel( bool p_bInit )
	{
		if ( p_bInit )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::Size] );
			std::memset( m_pComponents.get(), 0, pixel_definitions< FT >::Size );
		}
	}

	template< TPL_PIXEL_FORMAT FT >
	Pixel< FT >::Pixel( uint8_t * p_components )
		:	m_pComponents( p_components, g_dummyDtor )
	{
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FT >::Pixel( std::array< uint8_t, pixel_definitions< FU >::Size > const & p_components )
		:	m_pComponents( new uint8_t[pixel_definitions< FT >::Size] )
	{
		uint8_t const * l_pSrc = &p_components[0];
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

	template< TPL_PIXEL_FORMAT FT >
	Pixel< FT >::Pixel( Pixel< FT > const & p_pxl )
	{
		if ( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::Size] );
			std::memcpy( m_pComponents.get(), p_pxl.const_ptr(), pixel_definitions< FT >::Size );
		}
	}

	template< TPL_PIXEL_FORMAT FT >
	Pixel< FT >::Pixel( Pixel && p_pxl )
	{
		m_pComponents = std::move( p_pxl.m_pComponents	);
		p_pxl.m_pComponents.reset();
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FT >::Pixel( Pixel< FU > const & p_pxl )
	{
		if ( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::Size] );
			uint8_t const * l_pSrc = p_pxl.const_ptr();
			uint8_t * l_pDst = m_pComponents.get();
			pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
		}
	}

	template< TPL_PIXEL_FORMAT FT >
	Pixel< FT >::~Pixel()
	{
		clear();
	}

	template< TPL_PIXEL_FORMAT FT >
	Pixel< FT > & Pixel< FT >::operator =( Pixel< FT > const & p_pxl )
	{
		if ( m_pComponents.get() )
		{
			if ( p_pxl.const_ptr() )
			{
				std::memcpy( m_pComponents.get(), p_pxl.const_ptr(), pixel_definitions< FT >::Size );
			}
			else
			{
				clear();
			}
		}
		else if ( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::Size] );
			std::memcpy( m_pComponents.get(), p_pxl.const_ptr(), pixel_definitions< FT >::Size );
		}

		return * this;
	}

	template< TPL_PIXEL_FORMAT FT >
	Pixel< FT > & Pixel< FT >::operator =( Pixel< FT > && p_pxl )
	{
		if ( this != &p_pxl )
		{
			clear();
			m_pComponents = std::move( p_pxl.m_pComponents	);
			p_pxl.m_pComponents.reset();
		}

		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT >::operator =( Pixel< FU > const & p_pxl )
	{
		if ( m_pComponents.get() )
		{
			if ( p_pxl.const_ptr() )
			{
				uint8_t const * l_pSrc = p_pxl.const_ptr();
				uint8_t * l_pDst = m_pComponents.get();
				pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
			}
			else
			{
				clear();
			}
		}
		else if ( p_pxl.const_ptr() )
		{
			m_pComponents.reset( new uint8_t[pixel_definitions< FT >::Size] );
			uint8_t const * l_pSrc = p_pxl.const_ptr();
			uint8_t * l_pDst = m_pComponents.get();
			pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
		}

		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT >::operator +=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::Add( *this, p_px );
		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT >::operator +=( U const & p_t )
	{
		for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
		{
			m_pComponents.get()[i] = T( U( m_pComponents.get()[i] ) + p_t );
		}

		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT >::operator -=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::Substract( *this, p_px );
		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT >::operator -=( U const & p_t )
	{
		for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
		{
			m_pComponents.get()[i] = T( U( m_pComponents.get()[i] ) - p_t );
		}

		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT >::operator /=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::Divide( *this, p_px );
		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT >::operator /=( U const & p_t )
	{
		for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
		{
			m_pComponents.get()[i] = ( p_t == 0 ? m_pComponents.get()[i] : T( U( m_pComponents.get()[i] ) + p_t ) );
		}

		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FT > & Pixel< FT >::operator *=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::Multiply( *this, p_px );
		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< typename U >
	Pixel< FT > & Pixel< FT >::operator *=( U const & p_t )
	{
		for ( uint8_t i = 0; i < pixel_definitions< FT >::Size; i++ )
		{
			m_pComponents.get()[i] = T( U( m_pComponents.get()[i] ) * p_t );
		}

		return *this;
	}

	template< TPL_PIXEL_FORMAT FT >
	void Pixel< FT >::clear()
	{
		m_pComponents.reset();
	}

	template< TPL_PIXEL_FORMAT FT >
	void Pixel< FT >::link( uint8_t * p_pComponents )
	{
		clear();
		m_pComponents = std::shared_ptr< uint8_t >( p_pComponents, g_dummyDtor );
	}

	template< TPL_PIXEL_FORMAT FT >
	void Pixel< FT >::unlink()
	{
		m_pComponents.reset( new uint8_t[pixel_definitions< FT >::Size] );
		std::memset( m_pComponents.get(), 0, pixel_definitions< FT >::Size );
	}

	template< TPL_PIXEL_FORMAT FT >
	template< typename U >
	void Pixel< FT >::sum( U & p_uResult )const
	{
		U l_base = 0;
		p_uResult = std::accumulate( begin(), end(), l_base );
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FU > Pixel< FT >::mul( Pixel< FU > const & p_px )const
	{
		Pixel< FU > l_pxReturn( * this );
		l_pxReturn *= p_px;
		return l_pxReturn;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	Pixel< FU > Pixel< FT >::mul( uint8_t const & p_val )const
	{
		Pixel< FU > l_pxReturn( * this );
		l_pxReturn *= p_val;
		return l_pxReturn;
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	void Pixel< FT >::set( std::array< uint8_t, pixel_definitions< FU >::Size > const & p_components )
	{
		uint8_t const * l_pSrc = &p_components[0];
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	void Pixel< FT >::set( uint8_t const * p_components )
	{
		uint8_t const * l_pSrc = &p_components[0];
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

	template< TPL_PIXEL_FORMAT FT >
	template< TPL_PIXEL_FORMAT FU >
	void Pixel< FT >::set( Pixel< FU > const & p_px )
	{
		uint8_t const * l_pSrc = p_px.const_ptr();
		uint8_t * l_pDst = m_pComponents.get();
		pixel_definitions< FU >::template convert< FT >( l_pSrc, l_pDst );
	}

	template < TPL_PIXEL_FORMAT FT, TPL_PIXEL_FORMAT FU >
	bool operator ==( Pixel< FT > const & p_pixel, Pixel< FU > const & p_pxl )
	{
		return PxOperators< FT, FU >::Equals( p_pixel, p_pxl );
	}

	template < TPL_PIXEL_FORMAT FT, TPL_PIXEL_FORMAT FU >
	Pixel< FT > operator +( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn += p_px;
		return l_pxReturn;
	}

	template < TPL_PIXEL_FORMAT FT, typename U >
	Pixel< FT > operator +( Pixel< FT > const & p_pixel, U const & p_t )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn += p_t;
		return l_pxReturn;
	}

	template < TPL_PIXEL_FORMAT FT, TPL_PIXEL_FORMAT FU >
	Pixel< FT > operator -( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn -= p_px;
		return l_pxReturn;
	}

	template < TPL_PIXEL_FORMAT FT, typename U >
	Pixel< FT > operator -( Pixel< FT > const & p_pixel, U const & p_t )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn -= p_t;
		return l_pxReturn;
	}

	template < TPL_PIXEL_FORMAT FT, TPL_PIXEL_FORMAT FU >
	Pixel< FT > operator /( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn /= p_px;
		return l_pxReturn;
	}

	template < TPL_PIXEL_FORMAT FT, typename U >
	Pixel< FT > operator /( Pixel< FT > const & p_pixel, U const & p_t )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn /= p_t;
		return l_pxReturn;
	}

	template < TPL_PIXEL_FORMAT FT, TPL_PIXEL_FORMAT FU >
	Pixel< FT > operator *( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn *= p_px;
		return l_pxReturn;
	}

	template < TPL_PIXEL_FORMAT FT, typename U >
	Pixel< FT > operator *( Pixel< FT > const & p_pixel, U const & p_t )
	{
		Pixel< FT > l_pxReturn( p_pixel );
		l_pxReturn *= p_t;
		return l_pxReturn;
	}
}
