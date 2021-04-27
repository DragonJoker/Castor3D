namespace castor
{
	//*************************************************************************************************

	template< PixelFormat FT, PixelFormat  FU >
	struct PxOperators
	{
		using LhsPixel = Pixel< FT >;
		using RhsPixel = Pixel< FU >;

		static void add( LhsPixel & p_lhs, RhsPixel const & p_rhs )
		{
			LhsPixel rhs( p_rhs );
			PxOperators< FT, FT >::add( p_lhs, rhs );
		}

		static void subtract( LhsPixel & p_lhs, RhsPixel const & p_rhs )
		{
			LhsPixel rhs( p_rhs );
			PxOperators< FT, FT >::subtract( p_lhs, rhs );
		}

		static void multiply( LhsPixel & p_lhs, RhsPixel const & p_rhs )
		{
			LhsPixel rhs( p_rhs );
			PxOperators< FT, FT >::multiply( p_lhs, rhs );
		}

		static void divide( LhsPixel & p_lhs, RhsPixel const & p_rhs )
		{
			LhsPixel rhs( p_rhs );
			PxOperators< FT, FT >::divide( p_lhs, rhs );
		}

		static void assign( LhsPixel & p_lhs, RhsPixel const & p_rhs )
		{
			LhsPixel rhs( p_rhs );
			PxOperators< FT, FT >::assign( p_lhs, rhs );
		}

		static bool equals( LhsPixel const & p_lhs, RhsPixel const & p_rhs )
		{
			LhsPixel rhs( p_rhs );
			return PxOperators< FT, FT >::equals( p_lhs, rhs );
		}
	};

	template< PixelFormat FT >
	struct PxOperators< FT, FT >
	{
		using LhsPixel = Pixel< FT >;

		static void add( LhsPixel & p_lhs, LhsPixel const & p_rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				p_lhs[i] += p_rhs[i];
			}
		}

		static void subtract( LhsPixel & p_lhs, LhsPixel const & p_rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				p_lhs[i] -= p_rhs[i];
			}
		}
		static void multiply( LhsPixel & p_lhs, LhsPixel const & p_rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				p_lhs[i] *= p_rhs[i];
			}
		}

		static void divide( LhsPixel & p_lhs, LhsPixel const & p_rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				p_lhs[i] = ( p_rhs[i] == 0 ? p_lhs[i] : p_lhs[i] / p_rhs[i] );
			}
		}

		static void assign( LhsPixel & p_lhs, LhsPixel const & p_rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				p_lhs[i] = p_rhs[i];
			}
		}

		static bool equals( LhsPixel const & p_lhs, LhsPixel const & p_rhs )
		{
			bool result = true;

			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				result &= p_lhs[i] == p_rhs[i];
			}

			return result;
		}
	};

	//*************************************************************************************************

	template< PixelFormat FT >
	Pixel< FT >::Pixel( bool p_init )
	{
		if ( p_init )
		{
			m_components.reset( new uint8_t[PixelDefinitionsT< FT >::Size] );
			std::memset( m_components.get(), 0, PixelDefinitionsT< FT >::Size );
		}
	}

	template< PixelFormat FT >
	Pixel< FT >::Pixel( uint8_t * p_components )
		:	m_components( p_components, g_dummyDtor )
	{
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT >::Pixel( std::array< uint8_t, PixelDefinitionsT< FU >::Size > const & p_components )
		:	m_components( new uint8_t[PixelDefinitionsT< FT >::Size] )
	{
		uint8_t const * src = &p_components[0];
		uint8_t * dst = m_components.get();
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	template< PixelFormat FT >
	Pixel< FT >::Pixel( Pixel< FT > const & p_pxl )
	{
		if ( p_pxl.constPtr() )
		{
			m_components.reset( new uint8_t[PixelDefinitionsT< FT >::Size] );
			std::memcpy( m_components.get(), p_pxl.constPtr(), PixelDefinitionsT< FT >::Size );
		}
	}

	template< PixelFormat FT >
	Pixel< FT >::Pixel( Pixel && p_pxl )
	{
		m_components = std::move( p_pxl.m_components );
		p_pxl.m_components.reset();
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT >::Pixel( Pixel< FU > const & p_pxl )
	{
		if ( p_pxl.constPtr() )
		{
			m_components.reset( new uint8_t[PixelDefinitionsT< FT >::Size] );
			uint8_t const * src = p_pxl.constPtr();
			uint8_t * dst = m_components.get();
			PixelDefinitionsT< FU >::template convert< FT >( src, dst );
		}
	}

	template< PixelFormat FT >
	Pixel< FT >::~Pixel()
	{
		clear();
	}

	template< PixelFormat FT >
	Pixel< FT > & Pixel< FT >::operator=( Pixel< FT > const & p_pxl )
	{
		if ( m_components.get() )
		{
			if ( p_pxl.constPtr() )
			{
				std::memcpy( m_components.get(), p_pxl.constPtr(), PixelDefinitionsT< FT >::Size );
			}
			else
			{
				clear();
			}
		}
		else if ( p_pxl.constPtr() )
		{
			m_components.reset( new uint8_t[PixelDefinitionsT< FT >::Size] );
			std::memcpy( m_components.get(), p_pxl.constPtr(), PixelDefinitionsT< FT >::Size );
		}

		return * this;
	}

	template< PixelFormat FT >
	Pixel< FT > & Pixel< FT >::operator=( Pixel< FT > && p_pxl )
	{
		if ( this != &p_pxl )
		{
			clear();
			m_components = std::move( p_pxl.m_components );
			p_pxl.m_components.reset();
		}

		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator=( Pixel< FU > const & p_pxl )
	{
		if ( m_components.get() )
		{
			if ( p_pxl.constPtr() )
			{
				uint8_t const * src = p_pxl.constPtr();
				uint8_t * dst = m_components.get();
				PixelDefinitionsT< FU >::template convert< FT >( src, dst );
			}
			else
			{
				clear();
			}
		}
		else if ( p_pxl.constPtr() )
		{
			m_components.reset( new uint8_t[PixelDefinitionsT< FT >::Size] );
			uint8_t const * src = p_pxl.constPtr();
			uint8_t * dst = m_components.get();
			PixelDefinitionsT< FU >::template convert< FT >( src, dst );
		}

		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator+=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::add( *this, p_px );
		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator-=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::subtract( *this, p_px );
		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator/=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::divide( *this, p_px );
		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator*=( Pixel< FU > const & p_px )
	{
		PxOperators< FT, FU >::multiply( *this, p_px );
		return *this;
	}

	template< PixelFormat FT >
	void Pixel< FT >::clear()
	{
		m_components.reset();
	}

	template< PixelFormat FT >
	void Pixel< FT >::link( uint8_t * p_components )
	{
		clear();
		m_components = std::shared_ptr< uint8_t >( p_components, g_dummyDtor );
	}

	template< PixelFormat FT >
	void Pixel< FT >::unlink()
	{
		m_components.reset( new uint8_t[PixelDefinitionsT< FT >::Size] );
		std::memset( m_components.get(), 0, PixelDefinitionsT< FT >::Size );
	}

	template< PixelFormat FT >
	template< typename U >
	void Pixel< FT >::sum( U & p_result )const
	{
		U base = 0;
		p_result = std::accumulate( begin(), end(), base );
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FU > Pixel< FT >::mul( Pixel< FU > const & p_px )const
	{
		Pixel< FU > pxReturn( *this );
		pxReturn *= p_px;
		return pxReturn;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	void Pixel< FT >::set( std::array< uint8_t, PixelDefinitionsT< FU >::Size > const & p_components )
	{
		uint8_t const * src = &p_components[0];
		uint8_t * dst = m_components.get();
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	void Pixel< FT >::set( uint8_t const * p_components )
	{
		uint8_t const * src = &p_components[0];
		uint8_t * dst = m_components.get();
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	void Pixel< FT >::set( Pixel< FU > const & p_px )
	{
		uint8_t const * src = p_px.constPtr();
		uint8_t * dst = m_components.get();
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	template < PixelFormat FT, PixelFormat FU >
	bool operator==( Pixel< FT > const & p_pixel, Pixel< FU > const & p_pxl )
	{
		return PxOperators< FT, FU >::equals( p_pixel, p_pxl );
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator+( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > pxReturn( p_pixel );
		pxReturn += p_px;
		return pxReturn;
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator-( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > pxReturn( p_pixel );
		pxReturn -= p_px;
		return pxReturn;
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator/( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > pxReturn( p_pixel );
		pxReturn /= p_px;
		return pxReturn;
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator*( Pixel< FT > const & p_pixel, Pixel< FU > const & p_px )
	{
		Pixel< FT > pxReturn( p_pixel );
		pxReturn *= p_px;
		return pxReturn;
	}
}
