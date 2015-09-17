﻿#include "Templates.hpp"

namespace Castor
{
//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix()
		: m_data( new T[Rows * Columns] )
		, m_ownCoords( true )
	{
		do_update_columns();
		initialise();
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix( T const & p_value )
		: m_data( new T[Rows * Columns] )
		, m_ownCoords( true )
	{
		do_update_columns();
		initialise( p_value );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows >::Matrix( Type const * p_matrix )
		: m_data( new T[Rows * Columns] )
		, m_ownCoords( true )
	{
		do_update_columns();
		initialise();
		uint64_t l_count = Rows * Columns;

		for ( uint64_t i = 0; i < l_count; i++ )
		{
			m_data[i] = T( p_matrix[i] );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows >::Matrix( Matrix< T, Columns, Rows > const & p_matrix )
		: m_data( new T[Rows * Columns] )
		, m_ownCoords( true )
	{
		do_update_columns();
		initialise();
		std::memcpy( m_data, p_matrix.const_ptr(), my_type::size );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows >::Matrix( Matrix< T, Columns, Rows > && p_matrix )
		: m_data( NULL )
		, m_ownCoords( true )
	{
		m_data = std::move( p_matrix.m_data );
		m_ownCoords = std::move( p_matrix.m_ownCoords );
		p_matrix.m_data = NULL;
		p_matrix.m_ownCoords = true;
		do_update_columns();
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows >::Matrix( Matrix< Type, Columns, Rows > const & p_matrix )
		: m_data( new T[Rows * Columns] )
		, m_ownCoords( true )
	{
		do_update_columns();
		initialise();
		uint64_t l_count = Rows * Columns;

		for ( uint64_t i = 0; i < l_count; i++ )
		{
			m_data[i] = T( p_matrix.m_data[i] );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::~Matrix()
	{
		if ( m_ownCoords )
		{
			delete [] m_data;
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::initialise( T const & p_value )
	{
		std::memset( m_data, 0, my_type::size );

		if ( p_value )
		{
			for ( uint32_t i = 0; i < min_value< Columns, Rows >::value; i++ )
			{
				m_columns[i][i] = T( p_value );
			}
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_row( uint32_t p_index, T const * p_row )
	{
		if ( Columns >= 1 && p_index < Rows )
		{
			for ( uint32_t i = 0; i < Columns; i++ )
			{
				m_columns[i][p_index] = T( p_row[i] );
			}
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_row( uint32_t p_index, Point< T, Columns > const & p_row )
	{
		if ( Columns >= 1 && p_index < Rows )
		{
			for ( uint32_t i = 0; i < Columns; i++ )
			{
				m_columns[i][p_index] = T( p_row[i] );
			}
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_row( uint32_t p_index, Coords< T, Columns > const & p_row )
	{
		if ( Columns >= 1 && p_index < Rows )
		{
			for ( uint32_t i = 0; i < Columns; i++ )
			{
				m_columns[i][p_index] = T( p_row[i] );
			}
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Columns > Matrix< T, Columns, Rows >::get_row( uint32_t p_index )const
	{
		CASTOR_ASSERT( p_index < Rows );
		Point< T, Columns > l_ptReturn;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			l_ptReturn[i] = m_columns[i][p_index];
		}

		return l_ptReturn;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_row( uint32_t p_index, Point< T, Columns > & p_result )const
	{
		CASTOR_ASSERT( p_index < Rows );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			p_result[i] = m_columns[i][p_index];
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_column( uint32_t p_index, T const * p_col )
	{
		if ( Rows >= 1 && p_index < Columns )
		{
			std::memcpy( m_columns[p_index].ptr(), p_col, Rows * sizeof( T ) );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_column( uint32_t p_index, Point< T, Rows > const & p_col )
	{
		if ( Rows >= 1 && p_index < Columns )
		{
			std::memcpy( m_columns[p_index].ptr(), p_col.const_ptr(), Rows * sizeof( T ) );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_column( uint32_t p_index, Coords< T, Rows > const & p_col )
	{
		if ( Rows >= 1 && p_index < Columns )
		{
			std::memcpy( m_columns[p_index].ptr(), p_col.const_ptr(), Rows * sizeof( T ) );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Rows > Matrix< T, Columns, Rows >::get_column( uint32_t p_index )const
	{
		CASTOR_ASSERT( p_index < Columns );
		return Point< T, Rows >( m_columns[p_index] );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Coords< T, Rows > Matrix< T, Columns, Rows >::get_column( uint32_t p_index )
	{
		CASTOR_ASSERT( p_index < Columns );
		return m_columns[p_index];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_column( uint32_t p_index, Point< T, Rows > & p_result )const
	{
		CASTOR_ASSERT( p_index < Columns );
		p_result = Point< T, Rows >( m_columns[p_index] );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_column( uint32_t p_index, Coords< T, Rows > & p_result )
	{
		CASTOR_ASSERT( p_index < Columns );
		p_result = m_columns[p_index];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T Matrix< T, Columns, Rows >::value_at( uint32_t p_row, uint32_t p_column )
	{
		CASTOR_ASSERT( p_row < Rows );
		CASTOR_ASSERT( p_column < Columns );
		return m_columns[p_column][p_row];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Rows, Columns > Matrix< T, Columns, Rows >::get_transposed()const
	{
		Matrix< T, Rows, Columns > l_return;
		get_transposed( l_return );
		return l_return;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_transposed( Matrix< T, Rows, Columns > & p_mtxResult )const
	{
		T const * l_buffer = const_ptr();

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				p_mtxResult[j][i] = *l_buffer++;
			}
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > Matrix< T, Columns, Rows >::rec_get_minor( uint32_t x, uint32_t y, uint32_t p_rows, uint32_t p_cols )const
	{
		Matrix< T, Columns, Rows > l_return;
		l_return.set_identity();
		uint32_t l_i = 0, l_j = 0;

		for ( uint32_t i = 0; i < p_cols; i++ )
		{
			if ( i != x )
			{
				l_j = 0;

				for ( uint32_t j = 0; j < p_rows; j++ )
				{
					if ( j != y )
					{
						l_return[l_i][l_j++] = m_columns[i][j];
					}
				}

				l_i++;
			}
		}

		return l_return;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T Matrix< T, Columns, Rows >::get_trace()const
	{
		T l_sum = T();

		for ( int i = 0; i < Columns; i++ )
		{
			if ( !Castor::Policy< T >::is_null( m_columns[i][i] ) )
			{
				l_sum += m_columns[i][i];
			}
		}

		return l_sum;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_identity()
	{
		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				if ( i == j )
				{
					m_columns[i][i] = T( 1 );
				}
				else
				{
					m_columns[i][j] = T();
				}
			}
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > Matrix< T, Columns, Rows >::get_identity()const
	{
		Matrix< T, Columns, Rows > l_return( * this );
		l_return.set_identity();
		return l_return;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< T, Columns, Rows > const & p_matrix )
	{
		std::memcpy( m_data, p_matrix.m_data, my_type::size );
		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< T, Columns, Rows > && p_matrix )
	{
		if ( this != &p_matrix )
		{
			if ( m_ownCoords )
			{
				delete [] m_data;
			}

			m_data = std::move( p_matrix.m_data );
			m_ownCoords = std::move( p_matrix.m_ownCoords );
			p_matrix.m_data = NULL;
			p_matrix.m_ownCoords = true;
			do_update_columns();
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< Type, Columns, Rows > const & p_matrix )
	{
		Type const * l_buffer = p_matrix.const_ptr();

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				m_columns[i][j] = T( *l_buffer++ );
			}
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Type const * p_matrix )
	{
		uint64_t l_count = Rows * Columns;

		for ( uint64_t i = 0; i < l_count; i++ )
		{
			m_data[i] = T( *p_matrix++ );
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator+=( Matrix< Type, Columns, Rows > const & p_matrix )
	{
		Type const * l_buffer = p_matrix.const_ptr();

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				m_columns[i][j] += *l_buffer++;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator-=( Matrix< Type, Columns, Rows > const & p_matrix )
	{
		Type const * l_buffer = p_matrix.const_ptr();

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				m_columns[i][j] -= *l_buffer++;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator+=( T const & p_value )
	{
		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				m_columns[i][j] += p_value;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator-=( T const & p_value )
	{
		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				m_columns[i][j] -= p_value;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator*=( T const & p_value )
	{
		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				m_columns[i][j] *= p_value;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator/=( T const & p_value )
	{
		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				m_columns[i][j] /= p_value;
			}
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Coords< T, Rows > const & Matrix< T, Columns, Rows >::operator[]( uint32_t i )const
	{
		CASTOR_ASSERT( i < Columns );
		return m_columns[i];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Coords< T, Rows > & Matrix< T, Columns, Rows >::operator[]( uint32_t i )
	{
		CASTOR_ASSERT( i < Columns );
		return m_columns[i];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T & Matrix< T, Columns, Rows >::operator()( uint32_t p_col, uint32_t p_row )
	{
		return m_columns[p_col][p_row];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T const & Matrix< T, Columns, Rows >::operator()( uint32_t p_col, uint32_t p_row )const
	{
		return m_columns[p_col][p_row];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T * Matrix< T, Columns, Rows >::ptr()
	{
		return m_data;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T const * Matrix< T, Columns, Rows >::const_ptr()const
	{
		return m_data;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::link( T * p_coords )
	{
		if ( m_ownCoords )
		{
			delete [] m_data;
			m_data = NULL;
		}

		m_data = p_coords;
		m_ownCoords = false;
		do_update_columns();
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::do_update_columns()const
	{
		value_type * l_buffer = m_data;

		for ( uint32_t i = 0; i < Columns; ++i )
		{
			m_columns[i] = col_type( l_buffer );

#if !defined( NDEBUG )

			for ( uint32_t j = 0; j < Rows; ++j )
			{
				m_debugData[i][j] = l_buffer++;
			}

#else

			l_buffer += Rows;

#endif

		}
	}

//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline bool operator==( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< T, Columns, Rows > const & p_mtxB )
	{
		bool l_return = true;

		for ( uint32_t i = 0; i < Columns && l_return; i++ )
		{
			for ( uint32_t j = 0; j < Rows && l_return; j++ )
			{
				l_return = Castor::Policy< T >::equals( p_mtxA[i][j], p_mtxB[i][j] );
			}
		}

		return l_return;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline bool operator!=( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< T, Columns, Rows > const & p_mtxB )
	{
		return ! operator==( p_mtxA, p_mtxB );
	}
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, Columns, Rows > const & p_mtxB )
	{
		Matrix< T, Columns, Rows > l_mtx( p_mtxA );
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, Columns, Rows > const & p_mtxB )
	{
		Matrix< T, Columns, Rows > l_mtx( p_mtxA );
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows, typename U, uint32_t _Columns >
	Matrix< T, _Columns, Rows > operator*( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, _Columns, Columns > const & p_mtxB )
	{
		Matrix< T, _Columns, Rows > l_return;

		for ( uint32_t i = 0; i < _Columns; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				for ( uint32_t k = 0; k < Rows; k++ )
				{
					l_return[i][k] += T( p_mtxA[j][k] * p_mtxB[i][j] );
				}
			}
		}

		return l_return;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	inline Point< T, Rows > operator*( Matrix< T, Columns, Rows > const & p_matrix, Point< U, Columns > const & p_vector )
	{
		Point< T, Rows > l_ptReturn;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_ptReturn[j] += T( p_matrix[i][j] * p_vector[i] );
			}
		}

		return l_ptReturn;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	inline Point< T, Columns > operator*( Point< T, Rows > const & p_vector, Matrix< U, Columns, Rows > const & p_matrix )
	{
		Point< T, Columns > l_ptReturn;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_ptReturn[i] += T( p_matrix[i][j] * p_vector[j] );
			}
		}

		return l_ptReturn;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	Matrix <T, Columns, Rows> operator+( Matrix< T, Columns, Rows > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Columns, Rows > l_mtx( p_mtxA );
		l_mtx += p_mtxB;
		return l_mtx;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Columns, Rows > l_mtx( p_mtxA );
		l_mtx -= p_mtxB;
		return l_mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator+( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > l_mtx( p_matrix );
		l_mtx += p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > l_mtx( p_matrix );
		l_mtx -= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator*( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > l_mtx( p_matrix );
		l_mtx *= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator/( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > l_mtx( p_matrix );
		l_mtx /= p_uValue;
		return l_mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator+( T const & p_scalar, Matrix< T, Columns, Rows > const & p_matrix )
	{
		return operator+( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator-( T const & p_scalar, Matrix< T, Columns, Rows > const & p_matrix )
	{
		return operator-( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator*( T const & p_scalar, Matrix< T, Columns, Rows > const & p_matrix )
	{
		return operator*( p_matrix, p_scalar );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & p_matrix )
	{
		Matrix< T, Columns, Rows > l_return;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_return[i][j] = -p_matrix[i][j];
			}
		}

		return l_return;
	}

//*************************************************************************************************
}

template< typename T, uint32_t Columns, uint32_t Rows >
inline Castor::String & operator<<( Castor::String & p_strOut, Castor::Matrix< T, Columns, Rows > const & p_matrix )
{
	Castor::StringStream l_streamOut;
	l_streamOut.precision( 10 );

	for ( uint32_t i = 0; i < Columns; i++ )
	{
		for ( uint32_t j = 0; j < Rows; j++ )
		{
			p_streamOut.width( 15 );
			p_streamOut << std::right << p_matrix[i][j];
		}

		l_streamOut << std::endl;
	}

	p_strOut += l_streamOut.str();
	return p_strOut;
}
template< typename T, uint32_t Columns, uint32_t Rows >
inline Castor::String & operator>>( Castor::String & p_strIn, Castor::Matrix <T, Columns, Rows> & p_matrix )
{
	Castor::StringStream l_streamIn( p_strIn );

	for ( uint32_t i = 0; i < Columns; i++ )
	{
		for ( uint32_t j = 0; j < Rows; j++ )
		{
			l_streamIn >> p_matrix[i][j];
		}

		l_streamIn.ignore();
	}

	p_strIn = l_streamIn.str();
	return p_strIn;
}
template< typename CharT, typename T, uint32_t Columns, uint32_t Rows >
inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & p_streamOut, Castor::Matrix< T, Columns, Rows > const & p_matrix )
{
	auto l_precision = p_streamOut.precision( 10 );

	for ( uint32_t i = 0; i < Columns; i++ )
	{
		for ( uint32_t j = 0; j < Rows; j++ )
		{
			p_streamOut.width( 15 );
			p_streamOut << std::right << p_matrix[i][j];
		}

		p_streamOut << std::endl;
	}

	p_streamOut.precision( l_precision );
	return p_streamOut;
}
template< typename CharT, typename T, uint32_t Columns, uint32_t Rows >
inline std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & p_streamIn, Castor::Matrix< T, Columns, Rows > & p_matrix )
{
	for ( uint32_t i = 0; i < Columns; i++ )
	{
		for ( uint32_t j = 0; j < Rows; j++ )
		{
			p_streamIn >> p_matrix[i][j];
		}

		p_streamIn.ignore();
	}

	return p_streamIn;
}
