namespace castor
{
	//*********************************************************************************************

	template< PixelFormat FT, PixelFormat  FU >
	struct PxOperators;

	template< PixelFormat FT >
	struct PxOperators< FT, FT >
	{
		using LhsPixel = Pixel< FT >;

		static void add( LhsPixel & lhs, LhsPixel const & rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				lhs[i] += rhs[i];
			}
		}

		static void subtract( LhsPixel & lhs, LhsPixel const & rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				lhs[i] -= rhs[i];
			}
		}
		static void multiply( LhsPixel & lhs, LhsPixel const & rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				lhs[i] *= rhs[i];
			}
		}

		static void divide( LhsPixel & lhs, LhsPixel const & rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				lhs[i] = ( rhs[i] == 0 ? lhs[i] : lhs[i] / rhs[i] );
			}
		}

		static void assign( LhsPixel & lhs, LhsPixel const & rhs )
		{
			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				lhs[i] = rhs[i];
			}
		}

		static bool equals( LhsPixel const & lhs, LhsPixel const & rhs )
		{
			bool result = true;

			for ( uint8_t i = 0; i < PixelDefinitionsT< FT >::Size; i++ )
			{
				result &= lhs[i] == rhs[i];
			}

			return result;
		}
	};

	template< PixelFormat FT, PixelFormat  FU >
	struct PxOperators
	{
		using LhsPixel = Pixel< FT >;
		using RhsPixel = Pixel< FU >;

		static void add( LhsPixel & lhs, RhsPixel const & rhs )
		{
			LhsPixel lhsTRhs{ rhs };
			PxOperators< FT, FT >::add( lhs, lhsTRhs );
		}

		static void subtract( LhsPixel & lhs, RhsPixel const & rhs )
		{
			LhsPixel lhsTRhs{ rhs };
			PxOperators< FT, FT >::subtract( lhs, lhsTRhs );
		}

		static void multiply( LhsPixel & lhs, RhsPixel const & rhs )
		{
			LhsPixel lhsTRhs{ rhs };
			PxOperators< FT, FT >::multiply( lhs, lhsTRhs );
		}

		static void divide( LhsPixel & lhs, RhsPixel const & rhs )
		{
			LhsPixel lhsTRhs{ rhs };
			PxOperators< FT, FT >::divide( lhs, lhsTRhs );
		}

		static void assign( LhsPixel & lhs, RhsPixel const & rhs )
		{
			LhsPixel lhsTRhs{ rhs };
			PxOperators< FT, FT >::assign( lhs, lhsTRhs );
		}

		static bool equals( LhsPixel const & lhs, RhsPixel const & rhs )
		{
			LhsPixel lhsTRhs{ rhs };
			return PxOperators< FT, FT >::equals( lhs, lhsTRhs );
		}
	};

	//*********************************************************************************************

	template< PixelFormat FT >
	Pixel< FT >::Pixel( bool init )
	{
		if ( init )
		{
			m_components = new uint8_t[PixelDefinitionsT< FT >::Size];
			m_delete = true;
			std::memset( m_components, 0, PixelDefinitionsT< FT >::Size );
		}
	}

	template< PixelFormat FT >
	Pixel< FT >::Pixel( uint8_t * components )
		: m_components{ components }
		, m_delete{ false }
	{
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT >::Pixel( std::array< uint8_t, PixelDefinitionsT< FU >::Size > const & components )
		: m_components{ new uint8_t[PixelDefinitionsT< FT >::Size] }
		, m_delete{ true }
	{
		uint8_t const * src = &components[0];
		uint8_t * dst = m_components;
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	template< PixelFormat FT >
	Pixel< FT >::Pixel( Pixel< FT > const & rhs )
	{
		if ( rhs.constPtr() )
		{
			m_components = new uint8_t[PixelDefinitionsT< FT >::Size];
			m_delete = true;
			std::memcpy( m_components, rhs.constPtr(), PixelDefinitionsT< FT >::Size );
		}
	}

	template< PixelFormat FT >
	Pixel< FT >::Pixel( Pixel && rhs )noexcept
	{
		m_components = std::move( rhs.m_components );
		m_delete = rhs.m_delete;

		rhs.m_components = nullptr;
		rhs.m_delete = false;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT >::Pixel( Pixel< FU > const & rhs )
	{
		if ( rhs.constPtr() )
		{
			m_components = new uint8_t[PixelDefinitionsT< FT >::Size];
			m_delete = true;
			uint8_t const * src = rhs.constPtr();
			uint8_t * dst = m_components;
			PixelDefinitionsT< FU >::template convert< FT >( src, dst );
		}
	}

	template< PixelFormat FT >
	Pixel< FT >::~Pixel()noexcept
	{
		clear();
	}

	template< PixelFormat FT >
	Pixel< FT > & Pixel< FT >::operator=( Pixel< FT > const & rhs )
	{
		clear();

		if ( rhs.constPtr() )
		{
			m_components = new uint8_t[PixelDefinitionsT< FT >::Size];
			m_delete = true;
			std::memcpy( m_components, rhs.constPtr(), PixelDefinitionsT< FT >::Size );
		}

		return * this;
	}

	template< PixelFormat FT >
	Pixel< FT > & Pixel< FT >::operator=( Pixel< FT > && rhs )noexcept
	{
		clear();

		m_components = std::move( rhs.m_components );
		m_delete = rhs.m_delete;

		rhs.m_components = nullptr;
		rhs.m_delete = false;

		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator=( Pixel< FU > const & rhs )
	{
		clear();

		if ( rhs.constPtr() )
		{
			m_components = new uint8_t[PixelDefinitionsT< FT >::Size];
			m_delete = true;
			uint8_t const * src = rhs.constPtr();
			uint8_t * dst = m_components;
			PixelDefinitionsT< FU >::template convert< FT >( src, dst );
		}

		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator+=( Pixel< FU > const & rhs )
	{
		PxOperators< FT, FU >::add( *this, rhs );
		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator-=( Pixel< FU > const & rhs )
	{
		PxOperators< FT, FU >::subtract( *this, rhs );
		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator/=( Pixel< FU > const & rhs )
	{
		PxOperators< FT, FU >::divide( *this, rhs );
		return *this;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FT > & Pixel< FT >::operator*=( Pixel< FU > const & rhs )
	{
		PxOperators< FT, FU >::multiply( *this, rhs );
		return *this;
	}

	template< PixelFormat FT >
	void Pixel< FT >::clear()noexcept
	{
		if ( m_delete )
		{
			delete[] m_components;
		}

		m_components = nullptr;
		m_delete = false;
	}

	template< PixelFormat FT >
	void Pixel< FT >::link( uint8_t * components )noexcept
	{
		clear();

		m_components = components;
		m_delete = false;
	}

	template< PixelFormat FT >
	void Pixel< FT >::unlink()noexcept
	{
		clear();

		m_components = new( std::nothrow ) uint8_t[PixelDefinitionsT< FT >::Size];
		m_delete = true;

		if ( m_components )
		{
			std::memset( m_components, 0, PixelDefinitionsT< FT >::Size );
		}
	}

	template< PixelFormat FT >
	template< typename U >
	void Pixel< FT >::sum( U & result )const
	{
		result = std::accumulate( begin(), end(), U{} );
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	Pixel< FU > Pixel< FT >::mul( Pixel< FU > const & rhs )const
	{
		Pixel< FU > result( *this );
		result *= rhs;
		return result;
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	void Pixel< FT >::set( std::array< uint8_t, PixelDefinitionsT< FU >::Size > const & components )
	{
		uint8_t const * src = &components[0];
		uint8_t * dst = m_components;
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	void Pixel< FT >::set( uint8_t const * components )
	{
		uint8_t const * src = &components[0];
		uint8_t * dst = m_components;
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	template< PixelFormat FT >
	template< PixelFormat FU >
	void Pixel< FT >::set( Pixel< FU > const & px )
	{
		uint8_t const * src = px.constPtr();
		uint8_t * dst = m_components;
		PixelDefinitionsT< FU >::template convert< FT >( src, dst );
	}

	//*********************************************************************************************

	template < PixelFormat FT, PixelFormat FU >
	bool operator==( Pixel< FT > const & lhs, Pixel< FU > const & rhs )
	{
		return PxOperators< FT, FU >::equals( lhs, rhs );
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator+( Pixel< FT > const & lhs, Pixel< FU > const & rhs )
	{
		Pixel< FT > result{ lhs };
		result += rhs;
		return result;
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator-( Pixel< FT > const & lhs, Pixel< FU > const & rhs )
	{
		Pixel< FT > result{ lhs };
		result -= rhs;
		return result;
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator/( Pixel< FT > const & lhs, Pixel< FU > const & rhs )
	{
		Pixel< FT > result{ lhs };
		result /= rhs;
		return result;
	}

	template < PixelFormat FT, PixelFormat FU >
	Pixel< FT > operator*( Pixel< FT > const & lhs, Pixel< FU > const & rhs )
	{
		Pixel< FT > result{ lhs };
		result *= rhs;
		return result;
	}

	//*********************************************************************************************
}
