namespace Castor
{
//*************************************************************************************************

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns >::Matrix()
		: m_pPointer( new T[Rows * Columns] )
		, m_bOwnCoords( true )
	{
		initialise();
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns >::Matrix( T const & p_value )
		: m_pPointer( new T[Rows * Columns] )
		, m_bOwnCoords( true )
	{
		initialise( p_value );
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns >::Matrix( Type const * p_pMatrix )
		: m_pPointer( new T[Rows * Columns] )
		, m_bOwnCoords( true )
	{
		initialise();
		uint64_t l_uiCount = Rows * Columns;

		for ( uint64_t i = 0; i < l_uiCount; i++ )
		{
			m_pPointer[i] = T( p_pMatrix[i] );
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix< T, Rows, Columns >::Matrix( Matrix< T, Rows, Columns > const & p_matrix )
		: m_pPointer( new T[Rows * Columns] )
		, m_bOwnCoords( true )
	{
		initialise();
		std::memcpy( m_pPointer, p_matrix.const_ptr(), my_type::size );
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix< T, Rows, Columns >::Matrix( Matrix< T, Rows, Columns > && p_matrix )
		: m_pPointer( NULL )
		, m_bOwnCoords( true )
	{
		m_pPointer = std::move( p_matrix.m_pPointer );
		m_bOwnCoords = std::move( p_matrix.m_bOwnCoords );
		p_matrix.m_pPointer = NULL;
		p_matrix.m_bOwnCoords = true;
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns >::Matrix( Matrix< Type, Rows, Columns > const & p_matrix )
		: m_pPointer( new T[Rows * Columns] )
		, m_bOwnCoords( true )
	{
		initialise();
		uint64_t l_uiCount = Rows * Columns;

		for ( uint64_t i = 0; i < l_uiCount; i++ )
		{
			m_pPointer[i] = T( p_matrix.m_pPointer[i] );
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns >::~Matrix()
	{
		if ( m_bOwnCoords )
		{
			delete [] m_pPointer;
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::initialise( T const & p_value )
	{
		std::memset( m_pPointer, 0, my_type::size );

		if ( p_value )
		{
			for ( uint32_t i = 0; i < Columns && i < Rows; i++ )
			{
				this->operator[]( i )[i] = T( p_value );
			}
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_row( uint32_t p_uiRow, T const * p_row )
	{
		if ( Columns >= 1 && p_uiRow < Rows )
		{
			std::memcpy( this->operator[]( p_uiRow ), p_row, Columns * sizeof( T ) );
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_row( uint32_t p_uiRow, Point< T, Columns > const & p_row )
	{
		if ( Columns >= 1 && p_uiRow < Rows )
		{
			std::memcpy( this->operator[]( p_uiRow ), p_row.const_ptr(), Columns * sizeof( T ) );
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Point< T, Columns > Matrix< T, Rows, Columns >::get_row( uint32_t p_uiRow )const
	{
		CASTOR_ASSERT( p_uiRow < Rows );
		return Point< T, Rows >( this->operator[]( p_uiRow ) );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Coords< T, Columns > Matrix< T, Rows, Columns >::get_row( uint32_t p_uiRow )
	{
		CASTOR_ASSERT( p_uiColumn < Columns );
		return Coords< T, Columns >( this->operator[]( p_uiRow ) );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::get_row( uint32_t p_uiRow, Point< T, Columns > & p_mResult )const
	{
		CASTOR_ASSERT( p_uiRow < Rows );
		p_mResult = Point< T, Columns >( this->operator[]( p_uiRow ) );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_column( uint32_t p_uiColumn, T const * p_col )
	{
		if ( Rows >= 1 && p_uiColumn < Columns )
		{
			for ( uint32_t i = 0; i < Rows; i++ )
			{
				this->operator[]( i )[p_uiColumn] = T( p_col[i] );
			}
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_column( uint32_t p_uiColumn, Point< T, Rows > const & p_col )
	{
		if ( Rows >= 1 && p_uiColumn < Columns )
		{
			for ( uint32_t i = 0; i < Rows; i++ )
			{
				this->operator[]( i )[p_uiColumn] = T( p_col[i] );
			}
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Point< T, Rows > Matrix< T, Rows, Columns >::get_column( uint32_t p_uiColumn )const
	{
		CASTOR_ASSERT( p_uiColumn < Columns );
		Point< T, Rows > l_ptReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			l_ptReturn[i] = this->operator[]( i )[p_uiColumn];
		}

		return l_ptReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::get_column( uint32_t p_uiColumn, Point< T, Rows > & p_mResult )const
	{
		CASTOR_ASSERT( p_uiColumn < Columns );

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			p_mResult[i] = this->operator[]( i )[p_uiColumn];
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T Matrix< T, Rows, Columns >::value_at( uint32_t p_uiRow, uint32_t p_uiColumn )
	{
		CASTOR_ASSERT( p_uiRow < Rows );
		CASTOR_ASSERT( p_uiColumn < Columns );
		return this->operator[]( p_uiRow )[p_uiColumn];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Columns, Rows > Matrix< T, Rows, Columns >::get_transposed()const
	{
		Matrix< T, Columns, Rows > l_mtxReturn;
		get_transposed( l_mtxReturn );
		return l_mtxReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::get_transposed( Matrix< T, Columns, Rows > & p_mtxResult )const
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				p_mtxResult[j][i] = this->operator[]( i )[j];
			}
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > Matrix< T, Rows, Columns >::rec_get_minor( uint32_t x, uint32_t y, uint32_t p_uiRows, uint32_t p_uiCols )const
	{
		Matrix< T, Rows, Columns > l_mReturn;
		l_mReturn.set_identity();
		uint32_t l_i = 0, l_j = 0;

		for ( uint32_t i = 0; i < p_uiRows; i++ )
		{
			if ( i != x )
			{
				l_j = 0;

				for ( uint32_t j = 0; j < p_uiCols; j++ )
				{
					if ( j != y )
					{
						l_mReturn[l_i][l_j++] = this->operator[]( i )[j];
					}
				}

				l_i++;
			}
		}

		return l_mReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T Matrix< T, Rows, Columns >::get_trace()const
	{
		T l_tSum = T();

		for ( int i = 0; i < Rows; i++ )
		{
			if ( !Castor::Policy< T >::is_null( this->operator[]( i )[i] ) )
			{
				l_tSum += this->operator[]( i )[i];
			}
		}

		return l_tSum;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_identity()
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				if ( i == j )
				{
					this->operator[]( i )[i] = T( 1 );
				}
				else
				{
					this->operator[]( i )[j] = T();
				}
			}
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > Matrix< T, Rows, Columns >::get_identity()const
	{
		Matrix< T, Rows, Columns > l_mReturn( * this );
		l_mReturn.set_identity();
		return l_mReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator=( Matrix< T, Rows, Columns > const & p_matrix )
	{
		std::memcpy( m_pPointer, p_matrix.m_pPointer, my_type::size );
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator=( Matrix< T, Rows, Columns > && p_matrix )
	{
		if ( this != &p_matrix )
		{
			if ( m_bOwnCoords )
			{
				delete [] m_pPointer;
			}

			m_pPointer = std::move( p_matrix.m_pPointer );
			m_bOwnCoords = std::move( p_matrix.m_bOwnCoords );
			p_matrix.m_pPointer = NULL;
			p_matrix.m_bOwnCoords = true;

#if !defined( NDEBUG )
			do_update_debug();
#endif
		}
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator=( Matrix< Type, Rows, Columns > const & p_matrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( i )[j] = p_matrix[i][j];
			}
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator=( Type const * p_pMatrix )
	{
		uint64_t l_uiCount = Rows * Columns;

		for ( uint64_t i = 0; i < l_uiCount; i++ )
		{
			m_pPointer[i] = T( p_pMatrix[i] );
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator+=( Matrix< Type, Rows, Columns > const & p_matrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( i )[j] += p_matrix[i][j];
			}
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator-=( Matrix< Type, Rows, Columns > const & p_matrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( i )[j] -= p_matrix[i][j];
			}
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator+=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( i )[j] += p_tValue;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator-=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( i )[j] -= p_tValue;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator*=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( i )[j] *= p_tValue;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator/=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( i )[j] /= p_tValue;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T const * Matrix< T, Rows, Columns >::operator[]( uint32_t i )const
	{
		CASTOR_ASSERT( i < Rows );
		return &m_pPointer[i * Columns];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T * Matrix< T, Rows, Columns >::operator[]( uint32_t i )
	{
		CASTOR_ASSERT( i < Rows );
		return &m_pPointer[i * Columns];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T & Matrix< T, Rows, Columns >::operator()( uint32_t p_row, uint32_t p_col )
	{
		return this->operator[]( p_row )[p_col];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T const & Matrix< T, Rows, Columns >::operator()( uint32_t p_row, uint32_t p_col )const
	{
		return this->operator[]( p_row )[p_col];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T * Matrix< T, Rows, Columns >::ptr()
	{
		return m_pPointer;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T const * Matrix< T, Rows, Columns >::const_ptr()const
	{
		return m_pPointer;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::link( T * p_pCoords )
	{
		if ( m_bOwnCoords )
		{
			delete [] m_pPointer;
			m_pPointer = NULL;
		}

		m_pPointer = p_pCoords;
		m_bOwnCoords = false;
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}

#if !defined( NDEBUG )

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::do_update_debug()const
	{
		value_type * l_pointer = m_pPointer;

		for ( uint32_t i = 0; i < Rows; ++i )
		{
			for ( uint32_t j = 0; j < Columns; ++j )
			{
				m_debugData[i][j] = l_pointer++;
			}
		}
	}

#endif

//*************************************************************************************************

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline bool operator==( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< T, Rows, Columns > const & p_mtxB )
	{
		bool l_return = true;

		for ( uint32_t i = 0; i < Rows && l_return; i++ )
		{
			for ( uint32_t j = 0; j < Columns && l_return; j++ )
			{
				l_return = Castor::Policy< T >::equals( p_mtxA[j][i], p_mtxB[j][i] );
			}
		}

		return l_return;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline bool operator!=( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< T, Rows, Columns > const & p_mtxB )
	{
		return ! operator==( p_mtxA, p_mtxB );
	}
	template< typename T, uint32_t Rows, uint32_t Columns, typename U >
	Matrix< T, Rows, Columns > operator+( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< U, Rows, Columns > const & p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns, typename U >
	Matrix< T, Rows, Columns > operator-( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< U, Rows, Columns > const & p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns, typename U, uint32_t _Columns >
	Matrix< T, Rows, _Columns > operator*( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< U, Columns, _Columns > const & p_mtxB )
	{
		Matrix< T, Rows, _Columns > l_mtxReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < _Columns; j++ )
			{
				for ( uint32_t k = 0; k < Columns; k++ )
				{
					l_mtxReturn[i][j] += T( p_mtxA[i][k] * p_mtxB[k][j] );
				}
			}
		}

		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	inline Point< T, Rows > operator*( Matrix< T, Rows, Columns > const & p_matrix, Point< U, Columns > const & p_ptVector )
	{
		Point< T, Rows > l_ptReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				l_ptReturn[i] += T( p_matrix[i][j] * p_ptVector[j] );
			}
		}

		return l_ptReturn;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	inline Point< T, Columns > operator*( Point< T, Rows > const & p_ptVector, Matrix< U, Rows, Columns > const & p_matrix )
	{
		Point< T, Columns > l_ptReturn;
		
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			for ( uint32_t i = 0; i < Rows; i++ )
			{
				l_ptReturn[i] += T( p_matrix[i][j] * p_ptVector[i] );
			}
		}

		return l_ptReturn;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	Matrix <T, Rows, Columns> operator+( Matrix< T, Rows, Columns > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	Matrix <T, Rows, Columns> operator-( Matrix< T, Rows, Columns > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator+( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx += p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator-( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx -= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator*( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx *= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator/( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx /= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator+( T const & p_scalar, Matrix< T, Rows, Columns > const & p_matrix )
	{
		return operator+( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator-( T const & p_scalar, Matrix< T, Rows, Columns > const & p_matrix )
	{
		return operator-( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator*( T const & p_scalar, Matrix< T, Rows, Columns > const & p_matrix )
	{
		return operator*( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator-( Matrix< T, Rows, Columns > const & p_matrix )
	{
		Matrix< T, Rows, Columns > l_mtxReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				l_mtxReturn[i][j] = -p_matrix[i][j];
			}
		}

		return l_mtxReturn;
	}

//*************************************************************************************************
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::String & operator<<( Castor::String & p_strOut, Castor::Matrix< T, Rows, Columns > const & p_matrix )
{
	Castor::StringStream l_streamOut;
	l_streamOut.precision( 10 );

	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			p_streamOut.width( 15 );
			p_streamOut << std::right << p_matrix[i][j];
		}

		l_streamOut << std::endl;
	}

	p_strOut += l_streamOut.str();
	return p_strOut;
}
template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::String & operator>>( Castor::String & p_strIn, Castor::Matrix <T, Rows, Columns> & p_matrix )
{
	Castor::StringStream l_streamIn( p_strIn );

	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			l_streamIn >> p_matrix[i][j];
		}

		l_streamIn.ignore();
	}

	p_strIn = l_streamIn.str();
	return p_strIn;
}
template< typename CharT, typename T, uint32_t Rows, uint32_t Columns >
inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & p_streamOut, Castor::Matrix< T, Rows, Columns > const & p_matrix )
{
	auto l_precision = p_streamOut.precision( 10 );

	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			p_streamOut.width( 15 );
			p_streamOut << std::right << p_matrix[i][j];
		}

		p_streamOut << std::endl;
	}

	p_streamOut.precision( l_precision );
	return p_streamOut;
}
template< typename CharT, typename T, uint32_t Rows, uint32_t Columns >
inline std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & p_streamIn, Castor::Matrix< T, Rows, Columns > & p_matrix )
{
	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			p_streamIn >> p_matrix[i][j];
		}

		p_streamIn.ignore();
	}

	return p_streamIn;
}
