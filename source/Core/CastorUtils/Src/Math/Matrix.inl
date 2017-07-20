#include "Design/Templates.hpp"

#include "Miscellaneous/Utils.hpp"

namespace Castor
{
//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix( NoInit const & )
		: m_data( MatrixDataAllocator< T, Columns, Rows >::Alloc() )
		, m_ownCoords( true )
	{
		do_update_columns();
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix()
		: Matrix( NoInit() )
	{
		initialise();
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix( T const & p_value )
		: Matrix( NoInit() )
	{
		initialise( p_value );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix( T * p_matrix )
		: m_data( p_matrix )
		, m_ownCoords( false )
	{
		do_update_columns();
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows >::Matrix( Type const * p_matrix )
		: Matrix( NoInit() )
	{
		uint64_t count = Rows * Columns;

		for ( uint64_t i = 0; i < count; i++ )
		{
			this->m_data[i] = T( p_matrix[i] );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows >::Matrix( Matrix< T, Columns, Rows > const & p_matrix )
		: Matrix( NoInit() )
	{
		std::memcpy( this->m_data, p_matrix.const_ptr(), my_type::size );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows >::Matrix( Matrix< T, Columns, Rows > && p_matrix )
		: m_data( p_matrix.m_data )
		, m_ownCoords( p_matrix.m_ownCoords )
	{
		p_matrix.m_data = nullptr;
		p_matrix.m_ownCoords = true;
		do_update_columns();
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows >::Matrix( Matrix< Type, Columns, Rows > const & p_matrix )
		: Matrix( NoInit() )
	{
		uint64_t count = Rows * Columns;

		for ( uint64_t i = 0; i < count; i++ )
		{
			this->m_data[i] = T( p_matrix.m_data[i] );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::~Matrix()
	{
		if ( m_ownCoords )
		{
			MatrixDataAllocator< T, Columns, Rows >::Free( m_data );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::initialise( T const & p_value )
	{
		std::memset( this->m_data, 0, my_type::size );

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
		set_row( p_index, p_row.const_ptr() );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_row( uint32_t p_index, Coords< T, Columns > const & p_row )
	{
		set_row( p_index, p_row.const_ptr() );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Columns > Matrix< T, Columns, Rows >::get_row( uint32_t p_index )const
	{
		Point< T, Columns > result;
		get_row( p_index, result );
		return result;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_row( uint32_t p_index, Point< T, Columns > & p_result )const
	{
		REQUIRE( p_index < Columns );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			p_result[i] = m_columns[i][p_index];
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_column( uint32_t p_index, T const * p_col )
	{
		REQUIRE( p_index < Columns );

		if ( Rows >= 1 && p_index < Columns )
		{
			std::memcpy( m_columns[p_index].ptr(), p_col, Rows * sizeof( T ) );
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_column( uint32_t p_index, Point< T, Rows > const & p_col )
	{
		set_column( p_index, p_col.const_ptr() );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_column( uint32_t p_index, Coords< T const, Rows > const & p_col )
	{
		set_column( p_index, p_col.const_ptr() );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_column( uint32_t p_index, Coords< T, Rows > const & p_col )
	{
		set_column( p_index, p_col.const_ptr() );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Coords< T const, Rows > Matrix< T, Columns, Rows >::get_column( uint32_t p_index )const
	{
		REQUIRE( p_index < Columns );
		return Coords< T const, Rows >( m_columns[p_index].const_ptr() );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Coords< T, Rows > Matrix< T, Columns, Rows >::get_column( uint32_t p_index )
	{
		REQUIRE( p_index < Columns );
		return m_columns[p_index];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_column( uint32_t p_index, Coords< T const, Rows > & p_result )const
	{
		REQUIRE( p_index < Columns );
		p_result = Coords< T const, Rows >( m_columns[p_index] );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_column( uint32_t p_index, Coords< T, Rows > & p_result )
	{
		REQUIRE( p_index < Columns );
		p_result = m_columns[p_index];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T Matrix< T, Columns, Rows >::value_at( uint32_t p_column, uint32_t p_row )
	{
		REQUIRE( p_row < Rows && p_column < Columns );
		return m_columns[p_column][p_row];
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Rows, Columns > Matrix< T, Columns, Rows >::get_transposed()const
	{
		Matrix< T, Rows, Columns > result;
		get_transposed( result );
		return result;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::get_transposed( Matrix< T, Rows, Columns > & p_mtxResult )const
	{
		T const * buffer = const_ptr();

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				p_mtxResult[j][i] = *buffer++;
			}
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > Matrix< T, Columns, Rows >::rec_get_minor( uint32_t x, uint32_t y, uint32_t p_rows, uint32_t p_cols )const
	{
		Matrix< T, Columns, Rows > result;
		result.set_identity();
		uint32_t i = 0, j = 0;

		for ( uint32_t i = 0; i < p_cols; i++ )
		{
			if ( i != x )
			{
				j = 0;

				for ( uint32_t j = 0; j < p_rows; j++ )
				{
					if ( j != y )
					{
						result[i][j++] = m_columns[i][j];
					}
				}

				i++;
			}
		}

		return result;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T Matrix< T, Columns, Rows >::get_trace()const
	{
		T sum = T();
		T * buffer = this->m_data;

		for ( int i = 0; i < min_value< Columns, Rows >::value; i++ )
		{
			sum += *buffer;
			buffer += Rows + 1;
		}

		return sum;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::set_identity()
	{
		std::memset( this->m_data, 0, size );
		T * buffer = this->m_data;

		for ( int i = 0; i < min_value< Columns, Rows >::value; i++ )
		{
			*buffer = T( 1 );
			buffer += Rows + 1;
		}
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > Matrix< T, Columns, Rows >::get_identity()const
	{
		Matrix< T, Columns, Rows > result( * this );
		result.set_identity();
		return result;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< T, Columns, Rows > const & p_matrix )
	{
		std::memcpy( this->m_data, p_matrix.m_data, my_type::size );
		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< T, Columns, Rows > && p_matrix )
	{
		if ( this != &p_matrix )
		{
			if ( m_ownCoords )
			{
				MatrixDataAllocator< T, Columns, Rows >::Free( m_data );
			}

			m_data = p_matrix.m_data;
			m_ownCoords = p_matrix.m_ownCoords;
			p_matrix.m_data = nullptr;
			p_matrix.m_ownCoords = true;
			do_update_columns();
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< Type, Columns, Rows > const & p_matrix )
	{
		Type const * buffer = p_matrix.const_ptr();
		T * mine = this->m_data;

		for ( int i = 0; i < count; i++ )
		{
			*mine++ = T( *buffer++ );
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Type const * p_matrix )
	{
		T * mine = this->m_data;

		for ( uint64_t i = 0; i < count; i++ )
		{
			*mine++ = T( *p_matrix++ );
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator+=( Matrix< Type, Columns, Rows > const & p_matrix )
	{
		Type const * buffer = p_matrix.const_ptr();
		T * mine = this->m_data;

		for ( int i = 0; i < count; i++ )
		{
			*mine++ += T( *buffer++ );
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator-=( Matrix< Type, Columns, Rows > const & p_matrix )
	{
		Type const * buffer = p_matrix.const_ptr();
		T * mine = this->m_data;

		for ( int i = 0; i < count; i++ )
		{
			*mine++ -= T( *buffer++ );
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator+=( T const & p_value )
	{
		T * mine = this->m_data;

		for ( int i = 0; i < count; i++ )
		{
			*mine++ += p_value;
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator-=( T const & p_value )
	{
		T * mine = this->m_data;

		for ( int i = 0; i < count; i++ )
		{
			*mine++ -= p_value;
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator*=( T const & p_value )
	{
		T * mine = this->m_data;

		for ( int i = 0; i < count; i++ )
		{
			*mine++ *= p_value;
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator/=( T const & p_value )
	{
		T * mine = this->m_data;

		for ( int i = 0; i < count; i++ )
		{
			*mine++ /= p_value;
		}

		return *this;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Coords< T const, Rows > Matrix< T, Columns, Rows >::operator[]( uint32_t i )const
	{
		REQUIRE( i < Columns );
		return Coords< T const, Rows >( m_columns[i].const_ptr() );
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Coords< T, Rows > & Matrix< T, Columns, Rows >::operator[]( uint32_t i )
	{
		REQUIRE( i < Columns );
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
		return this->m_data;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T const * Matrix< T, Columns, Rows >::const_ptr()const
	{
		return this->m_data;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::link( T * p_coords )
	{
		if ( m_ownCoords )
		{
			MatrixDataAllocator< T, Columns, Rows >::Free( m_data );
			m_data = nullptr;
		}

		m_data = p_coords;
		m_ownCoords = false;
		do_update_columns();
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::do_update_columns()const
	{
		value_type * buffer = this->m_data;

		for ( uint32_t i = 0; i < Columns; ++i )
		{
			m_columns[i] = col_type( buffer );

#if !defined( NDEBUG )

			for ( uint32_t j = 0; j < Rows; ++j )
			{
				m_debugData[i][j] = buffer++;
			}

#else

			buffer += Rows;

#endif

		}
	}

//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline bool operator==( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< T, Columns, Rows > const & p_mtxB )
	{
		bool result = true;

		for ( uint32_t i = 0; i < Columns && result; i++ )
		{
			for ( uint32_t j = 0; j < Rows && result; j++ )
			{
				result = Castor::Policy< T >::equals( p_mtxA[i][j], p_mtxB[i][j] );
			}
		}

		return result;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline bool operator!=( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< T, Columns, Rows > const & p_mtxB )
	{
		return ! operator==( p_mtxA, p_mtxB );
	}
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, Columns, Rows > const & p_mtxB )
	{
		Matrix< T, Columns, Rows > mtx( p_mtxA );
		mtx += p_mtxB;
		return mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, Columns, Rows > const & p_mtxB )
	{
		Matrix< T, Columns, Rows > mtx( p_mtxA );
		mtx -= p_mtxB;
		return mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows, typename U, uint32_t _Columns >
	Matrix< T, _Columns, Rows > operator*( Matrix< T, Columns, Rows > const & p_mtxA, Matrix< U, _Columns, Columns > const & p_mtxB )
	{
		Matrix< T, _Columns, Rows > result;

		for ( uint32_t i = 0; i < _Columns; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				for ( uint32_t k = 0; k < Rows; k++ )
				{
					result[i][k] += T( p_mtxA[j][k] * p_mtxB[i][j] );
				}
			}
		}

		return result;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	inline Point< T, Rows > operator*( Matrix< T, Columns, Rows > const & p_matrix, Point< U, Columns > const & p_vector )
	{
		Point< T, Rows > result;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				result[j] += T( p_matrix[i][j] * p_vector[i] );
			}
		}

		return result;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	inline Point< T, Columns > operator*( Point< T, Rows > const & p_vector, Matrix< U, Columns, Rows > const & p_matrix )
	{
		Point< T, Columns > result;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				result[i] += T( p_matrix[i][j] * p_vector[j] );
			}
		}

		return result;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	Matrix <T, Columns, Rows> operator+( Matrix< T, Columns, Rows > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Columns, Rows > mtx( p_mtxA );
		mtx += p_mtxB;
		return mtx;
	}
	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & p_mtxA, U const * p_mtxB )
	{
		Matrix< T, Columns, Rows > mtx( p_mtxA );
		mtx -= p_mtxB;
		return mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator+( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > mtx( p_matrix );
		mtx += p_uValue;
		return mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > mtx( p_matrix );
		mtx -= p_uValue;
		return mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator*( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > mtx( p_matrix );
		mtx *= p_uValue;
		return mtx;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator/( Matrix< T, Columns, Rows > const & p_matrix, T const & p_uValue )
	{
		Matrix< T, Columns, Rows > mtx( p_matrix );
		mtx /= p_uValue;
		return mtx;
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
		Matrix< T, Columns, Rows > result;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				result[i][j] = -p_matrix[i][j];
			}
		}

		return result;
	}

//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Castor::String & operator<<( Castor::String & p_strOut, Castor::Matrix< T, Columns, Rows > const & p_matrix )
	{
		Castor::StringStream streamOut;
		streamOut.precision( 10 );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				streamOut.width( 15 );
				streamOut << std::right << p_matrix[i][j];
			}

			streamOut << std::endl;
		}

		p_strOut += streamOut.str();
		return p_strOut;
	}
	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Castor::String & operator>>( Castor::String & p_strIn, Castor::Matrix <T, Columns, Rows> & p_matrix )
	{
		Castor::StringStream streamIn( p_strIn );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				streamIn >> p_matrix[i][j];
			}

			streamIn.ignore();
		}

		p_strIn = streamIn.str();
		return p_strIn;
	}
	template< typename CharT, typename T, uint32_t Columns, uint32_t Rows >
	inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & p_streamOut, Castor::Matrix< T, Columns, Rows > const & p_matrix )
	{
		auto precision = p_streamOut.precision( 10 );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				p_streamOut.width( 15 );
				p_streamOut << std::right << p_matrix[i][j];
			}

			p_streamOut << std::endl;
		}

		p_streamOut.precision( precision );
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
}
