namespace Castor
{
//*************************************************************************************************

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns >::Matrix()
		:	m_pPointer( new T[Rows * Columns] )
		,	m_bOwnCoords( true )
	{
		initialise();
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns >::Matrix( T const & p_value )
		:	m_pPointer( new T[Rows * Columns] )
		,	m_bOwnCoords( true )
	{
		initialise( p_value );
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns >::Matrix( Type const * p_pMatrix )
		:	m_pPointer( new T[Rows * Columns] )
		,	m_bOwnCoords( true )
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
		:	m_pPointer( new T[Rows * Columns] )
		,	m_bOwnCoords( true )
	{
		initialise();
		std::memcpy( m_pPointer, p_matrix.const_ptr(), my_type::size );
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix< T, Rows, Columns >::Matrix( Matrix< T, Rows, Columns > && p_matrix )
		:	m_pPointer( NULL )
		,	m_bOwnCoords( true )
	{
		m_pPointer		= std::move( p_matrix.m_pPointer	);
		m_bOwnCoords	= std::move( p_matrix.m_bOwnCoords	);
		p_matrix.m_pPointer = NULL;
		p_matrix.m_bOwnCoords = true;
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns >::Matrix( Matrix< Type, Rows, Columns > const & p_matrix )
		:	m_pPointer( new T[Rows * Columns] )
		,	m_bOwnCoords( true )
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
			for ( uint32_t i = 0; i < Columns; i++ )
			{
				this->operator[]( i )[p_uiRow] = T( p_row[i] );
			}
		}
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_row( uint32_t p_uiRow, Point<T, Columns> const & p_row )
	{
		if ( Columns >= 1 && p_uiRow < Rows )
		{
			for ( uint32_t i = 0; i < Columns; i++ )
			{
				this->operator[]( i )[p_uiRow] = T( p_row[i] );
			}
		}
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Point<T, Columns> Matrix< T, Rows, Columns >::get_row( uint32_t p_uiRow )const
	{
		CASTOR_ASSERT( p_uiRow < Rows );
		Point< T, Columns > l_ptReturn;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			l_ptReturn[i] = this->operator[]( i )[p_uiRow];
		}

		return l_ptReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::get_row( uint32_t p_uiRow, Point< T, Columns > & p_mResult )const
	{
		CASTOR_ASSERT( p_uiRow < Rows );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			p_mResult[i] = this->operator[]( i )[p_uiRow];
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_column( uint32_t p_uiColumn, T const * p_col )
	{
		if ( Rows >= 1 && p_uiColumn < Columns )
		{
			std::memcpy( this->operator[]( p_uiColumn ), p_col, Rows * sizeof( T ) );
		}
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_column( uint32_t p_uiColumn, Point< T, Rows > const & p_col )
	{
		if ( Rows >= 1 && p_uiColumn < Columns )
		{
			std::memcpy( this->operator[]( p_uiColumn ), p_col.const_ptr(), Rows * sizeof( T ) );
		}
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Point< T, Rows > Matrix< T, Rows, Columns >::get_column( uint32_t p_uiColumn )const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		CASTOR_ASSERT( p_uiColumn < Columns );
		return Point< T, Rows >( this->operator[]( p_uiColumn ) );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Coords< T, Rows > Matrix< T, Rows, Columns >::get_column( uint32_t p_uiColumn )
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		CASTOR_ASSERT( p_uiColumn < Columns );
		return Coords< T, Rows >( this->operator[]( p_uiColumn ) );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::get_column( uint32_t p_uiColumn, Point< T, Rows > & p_mResult )const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		CASTOR_ASSERT( p_uiColumn < Columns );
		p_mResult = Point< T, Rows >( this->operator[]( p_uiColumn ) );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T Matrix< T, Rows, Columns >::value_at( uint32_t p_uiRow, uint32_t p_uiColumn )
	{
		CASTOR_ASSERT( p_uiRow < Rows );
		CASTOR_ASSERT( p_uiColumn < Columns );
		return this->operator[]( p_uiColumn )[p_uiRow];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Columns, Rows > Matrix< T, Rows, Columns >::get_transposed()const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		Matrix< T, Columns, Rows > l_mtxReturn;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_mtxReturn[j][i] = this->operator[]( i )[j];
			}
		}

		return l_mtxReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::get_transposed( Matrix< T, Columns, Rows > & p_mtxResult )const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				p_mtxResult[j][i] = this->operator[]( i )[j];
			}
		}
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > Matrix< T, Rows, Columns >::get_triangle()const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		Matrix< T, Rows, Columns > l_mReturn( *this );
		l_mReturn.set_triangle();
		return l_mReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::set_triangle()
	{
		uint32_t l_uiMinDim = std::min< uint32_t >( Rows, Columns );
		bool l_bContinue = true;
		T l_tCoef;
		uint32_t l_uiMaxIndex;

		for ( uint32_t i = 0; i < l_uiMinDim && l_bContinue; i++ )
		{
			l_uiMaxIndex = 0;
			T * l_pColumn = this->operator[]( i );

			for ( uint32_t j = 1; j < Rows; j++ )
			{
				if ( std::abs( l_pColumn[j] ) > std::abs( l_pColumn[l_uiMaxIndex] ) )
				{
					l_uiMaxIndex = j;
				}
			}

			if ( ! Castor::Policy< T >::is_null( l_pColumn[l_uiMaxIndex] ) )
			{
				for ( uint32_t k = i; k < Columns; k++ )
				{
					std::swap( this->operator[]( k )[i], this->operator[]( k )[l_uiMaxIndex] );
				}

				for ( uint32_t j = i + 1; j < Rows; j++ )
				{
					l_tCoef = l_pColumn[j] / l_pColumn[i];

					for ( uint32_t k = Columns - 1; k >= i && k < Columns; k-- )
					{
						this->operator[]( k )[j] -= l_tCoef * this->operator[]( k )[i];
					}
				}
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( l_bContinue )
		{
			T l_tDefault = T();

			for ( uint32_t i = 0; i < Rows; i++ )
			{
				for ( uint32_t j = 0; j < Columns; j++ )
				{
					if ( Castor::Policy< T >::is_null( this->operator[]( j )[i] ) )
					{
						this->operator[]( j )[i] = l_tDefault;
					}
				}
			}
		}
#if !defined( NDEBUG )
		do_update_debug();
#endif
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
						l_mReturn[l_j++][l_i] = this->operator[]( j )[i];
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
#if !defined( NDEBUG )
		do_update_debug();
#endif
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
	inline void Matrix< T, Rows, Columns >::set_jordan( T p_tLambda )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				if ( i == j )
				{
					this->operator[]( i )[i] = p_tLambda;
				}
				else
				{
					this->operator[]( i )[i] = T();
				}
			}
		}

		this->operator[]( 0 * Columns + 0 ) = p_tLambda;

		for ( uint32_t i = 0; i < Rows - 1; i++ )
		{
			this->operator[]( i + 1 )[i] = Castor::Policy< T >::unit();
		}
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > Matrix< T, Rows, Columns >::get_jordan( T p_tLambda )const
	{
		Matrix< T, Rows, Columns > l_mReturn( *this );
		l_mReturn.set_jordan();
		return l_mReturn;
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
					this->operator[]( j )[i] = T();
				}
			}
		}
#if !defined( NDEBUG )
		do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > Matrix< T, Rows, Columns >::get_identity()const
	{
		Matrix< T, Rows, Columns > l_mReturn( * this );
		l_mReturn.set_identity();
		return l_mReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator =( Matrix< T, Rows, Columns > const & p_matrix )
	{
		std::memcpy( m_pPointer, p_matrix.m_pPointer, my_type::size );
#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator =( Matrix< T, Rows, Columns > && p_matrix )
	{
		if ( this != &p_matrix )
		{
			if ( m_bOwnCoords )
			{
				delete [] m_pPointer;
			}

			m_pPointer		= std::move( p_matrix.m_pPointer	);
			m_bOwnCoords	= std::move( p_matrix.m_bOwnCoords	);
			p_matrix.m_pPointer = NULL;
			p_matrix.m_bOwnCoords = true;
		}
		
#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator =( Matrix< Type, Rows, Columns > const & p_matrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] = p_matrix[j][i];
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator +=( Matrix< Type, Rows, Columns > const & p_matrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] += T( p_matrix[j][i] );
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator -=( Matrix< Type, Rows, Columns > const & p_matrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] -= T( p_matrix[j][i] );
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator =( Type const * p_pMatrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] = T( p_pMatrix[j * Rows + i] );
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator +=( Type const * p_pMatrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] += T( p_pMatrix[j * Rows + i] );
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	template< typename Type >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator -=( Type const * p_pMatrix )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] -= T( p_pMatrix[j * Rows + i] );
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator +=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] += p_tValue;

				if ( Castor::Policy< T >::is_null( this->operator[]( j )[i] ) )
				{
					Castor::Policy< T >::init( this->operator[]( j )[i] );
				}
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator -=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] -= p_tValue;

				if ( Castor::Policy< T >::is_null( this->operator[]( j )[i] ) )
				{
					Castor::Policy< T >::init( this->operator[]( j )[i] );
				}
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator *=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] *= p_tValue;

				if ( Castor::Policy< T >::is_null( this->operator[]( j )[i] ) )
				{
					Castor::Policy< T >::init( this->operator[]( j )[i] );
				}
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix< T, Rows, Columns > & Matrix< T, Rows, Columns >::operator /=( T const & p_tValue )
	{
		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				this->operator[]( j )[i] /= p_tValue;

				if ( Castor::Policy< T >::is_null( this->operator[]( j )[i] ) )
				{
					Castor::Policy< T >::init( this->operator[]( j )[i] );
				}
			}
		}

#if !defined( NDEBUG )
		do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T const * Matrix< T, Rows, Columns >::operator []( uint32_t i )const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		CASTOR_ASSERT( i < Columns );
		return &m_pPointer[i * Rows];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T * Matrix< T, Rows, Columns >::operator []( uint32_t i )
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		CASTOR_ASSERT( i < Columns );
		return &m_pPointer[i * Rows];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T & Matrix< T, Rows, Columns >::operator()( uint32_t p_row, uint32_t p_col )
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		return this->operator[]( p_col )[p_row];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T const & Matrix< T, Rows, Columns >::operator()( uint32_t p_row, uint32_t p_col )const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		return this->operator[]( p_col )[p_row];
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T * Matrix< T, Rows, Columns >::ptr()
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
		return m_pPointer;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline T const * Matrix< T, Rows, Columns >::const_ptr()const
	{
#if !defined( NDEBUG )
		do_update_debug();
#endif
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

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void Matrix< T, Rows, Columns >::do_update_debug()const
	{
		memcpy( m_debugData, m_pPointer, sizeof( m_debugData ) );
	}

//*************************************************************************************************

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline bool operator ==( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< T, Rows, Columns > const & p_mtxB )
	{
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Rows && l_bReturn; i++ )
		{
			for ( uint32_t j = 0; j < Columns && l_bReturn; j++ )
			{
				l_bReturn = Castor::Policy< T >::equals( p_mtxA[j][i], p_mtxB[j][i] );
			}
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline bool operator !=( Matrix< T, Rows, Columns > const & p_mtxA, Matrix< T, Rows, Columns > const & p_mtxB )
	{
		return ! operator ==( p_mtxA, p_mtxB );
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	Matrix <T, Rows, Columns> operator + ( Matrix< T, Rows, Columns > const & p_mtxA, Matrix<U, Rows, Columns> const & p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	Matrix <T, Rows, Columns> operator - ( Matrix< T, Rows, Columns > const & p_mtxA, Matrix<U, Rows, Columns> const & p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U, uint32_t _Columns>
	Matrix<T, Rows, _Columns>	operator * ( Matrix< T, Rows, Columns > const & p_mtxA, Matrix<U, Columns, _Columns> const & p_mtxB )
	{
		Matrix<T, Rows, _Columns> l_mtxReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < _Columns; j++ )
			{
				for ( uint32_t k = 0; k < Columns; k++ )
				{
					l_mtxReturn[j][i] += p_mtxA[k][i] * T( p_mtxB[j][k] );
				}
			}
		}

		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	inline Point <T, Rows> operator * ( Matrix< T, Rows, Columns > const & p_matrix, Point<U, Columns> const & p_ptVector )
	{
		Point< T, Rows > l_ptReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				l_ptReturn[i] += p_matrix[j][i] * T( p_ptVector[j] );
			}
		}

		return l_ptReturn;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	Matrix <T, Rows, Columns> operator + ( Matrix< T, Rows, Columns > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template <typename T, uint32_t Rows, uint32_t Columns, typename U>
	Matrix <T, Rows, Columns> operator - ( Matrix< T, Rows, Columns > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Rows, Columns > l_mtx( p_mtxA );
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator + ( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx += p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator - ( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx -= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator * ( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx *= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	Matrix <T, Rows, Columns> operator / ( Matrix< T, Rows, Columns > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Rows, Columns > l_mtx( p_matrix );
		l_mtx /= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator + ( T const & p_scalar, Matrix< T, Rows, Columns > const & p_matrix )
	{
		return operator +( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator - ( T const & p_scalar, Matrix< T, Rows, Columns > const & p_matrix )
	{
		return operator -( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator * ( T const & p_scalar, Matrix< T, Rows, Columns > const & p_matrix )
	{
		return operator *( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Matrix <T, Rows, Columns> operator - ( Matrix< T, Rows, Columns > const & p_matrix )
	{
		Matrix< T, Rows, Columns > l_mtxReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				l_mtxReturn[j][i] = -p_matrix[j][i];
			}
		}

		return l_mtxReturn;
	}

//*************************************************************************************************
}

template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::String & operator << ( Castor::String & p_strOut, Castor::Matrix< T, Rows, Columns > const & p_matrix )
{
	Castor::StringStream l_streamOut;

	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			l_streamOut << cuT( "\t" ) << p_matrix[j][i];
		}

		l_streamOut << std::endl;
	}

	p_strOut += l_streamOut.str();
	return p_strOut;
}
template< typename T, uint32_t Rows, uint32_t Columns >
inline Castor::String & operator >> ( Castor::String & p_strIn, Castor::Matrix <T, Rows, Columns> & p_matrix )
{
	Castor::StringStream l_streamIn( p_strIn );

	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			l_streamIn >> p_matrix[j][i];
		}

		l_streamIn.ignore();
	}

	p_strIn = l_streamIn.str();
	return p_strIn;
}
template< typename T, uint32_t Rows, uint32_t Columns >
inline std::ostream & operator << ( std::ostream & p_streamOut, Castor::Matrix< T, Rows, Columns > const & p_matrix )
{
	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			p_streamOut << "\t" << p_matrix[j][i];
		}

		p_streamOut << std::endl;
	}

	return p_streamOut;
}
template< typename T, uint32_t Rows, uint32_t Columns >
inline std::istream & operator >> ( std::istream & p_streamIn, Castor::Matrix <T, Rows, Columns> & p_matrix )
{
	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			p_streamIn >> p_matrix[j][i];
		}

		p_streamIn.ignore();
	}

	return p_streamIn;
}
template< typename T, uint32_t Rows, uint32_t Columns >
inline std::wostream & operator << ( std::wostream & p_streamOut, Castor::Matrix< T, Rows, Columns > const & p_matrix )
{
	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			p_streamOut << L"\t" << p_matrix[j][i];
		}

		p_streamOut << std::endl;
	}

	return p_streamOut;
}
template< typename T, uint32_t Rows, uint32_t Columns >
inline std::wistream & operator >> ( std::wistream & p_streamIn, Castor::Matrix <T, Rows, Columns> & p_matrix )
{
	for ( uint32_t i = 0; i < Rows; i++ )
	{
		for ( uint32_t j = 0; j < Columns; j++ )
		{
			p_streamIn >> p_matrix[j][i];
		}

		p_streamIn.ignore();
	}

	return p_streamIn;
}
