#include "Design/Templates.hpp"

#include "Miscellaneous/Utils.hpp"

namespace castor
{
//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix( NoInit const & )
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix()
		: Matrix{ noInit }
	{
		initialise();
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix( T const & value )
		: Matrix{ noInit }
	{
		initialise( value );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows >::Matrix( T * rhs )
	{
		std::copy( rhs, rhs + Columns * Rows, m_data.begin() );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows >::Matrix( Type const * rhs )
		: Matrix{ noInit }
	{
		for ( size_t i = 0; i < count; i++ )
		{
			m_data[i] = T( rhs[i] );
		}
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows >::Matrix( Matrix< T, Columns, Rows > const & rhs )
		: Matrix{ noInit }
	{
		std::copy( rhs.m_data.begin(), rhs.m_data.end(), m_data.begin() );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows >::Matrix( Matrix< T, Columns, Rows > && rhs )
		: m_data{ rhs.m_data }
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows >::Matrix( Matrix< Type, Columns, Rows > const & rhs )
		: Matrix( noInit )
	{
		for ( size_t i = 0; i < count; i++ )
		{
			m_data[i] = T( rhs.m_data[i] );
		}
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix< T, Columns, Rows >::~Matrix()
	{
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::initialise()
	{
		std::memset( m_data.data(), 0, size );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::initialise( T const & value )
	{
		static size_t constexpr minCount = MinValue< Columns, Rows >::value;
		initialise();
		T * buffer = m_data.data();

		for ( int i = 0; i < minCount; i++ )
		{
			*buffer = value;
			buffer += Rows + 1;
		}
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setRow( uint32_t index, T const * row )
	{
		if ( Columns >= 1 && index < Rows )
		{
			for ( uint32_t i = 0; i < Columns; i++ )
			{
				m_columns[i][index] = T( row[i] );
			}
		}
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setRow( uint32_t index, Point< T, Columns > const & row )
	{
		setRow( index, row.constPtr() );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setRow( uint32_t index, Coords< T, Columns > const & row )
	{
		setRow( index, row.constPtr() );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Columns > Matrix< T, Columns, Rows >::getRow( uint32_t index )const
	{
		Point< T, Columns > result;
		getRow( index, result );
		return result;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::getRow( uint32_t index, Point< T, Columns > & result )const
	{
		CU_Require( index < Columns );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			result[i] = m_columns[i][index];
		}
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setColumn( uint32_t index, T const * col )
	{
		CU_Require( index < Columns );

		if ( Rows >= 1 && index < Columns )
		{
			std::memcpy( m_columns[index].ptr(), col, Rows * sizeof( T ) );
		}
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setColumn( uint32_t index, Point< T, Rows > const & col )
	{
		setColumn( index, col.constPtr() );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setColumn( uint32_t index, Coords< T const, Rows > const & col )
	{
		setColumn( index, col.constPtr() );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setColumn( uint32_t index, Coords< T, Rows > const & col )
	{
		setColumn( index, col.constPtr() );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Rows > const & Matrix< T, Columns, Rows >::getColumn( uint32_t index )const
	{
		CU_Require( index < Columns );
		return m_columns[index];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Rows > & Matrix< T, Columns, Rows >::getColumn( uint32_t index )
	{
		CU_Require( index < Columns );
		return m_columns[index];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::getColumn( uint32_t index, Point< T, Rows > & result )const
	{
		CU_Require( index < Columns );
		result = m_columns[index];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::getColumn( uint32_t index, Coords< T, Rows > & result )
	{
		CU_Require( index < Columns );
		result = m_columns[index];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Rows, Columns > Matrix< T, Columns, Rows >::getTransposed()const
	{
		Matrix< T, Rows, Columns > result;
		getTransposed( result );
		return result;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::getTransposed( Matrix< T, Rows, Columns > & result )const
	{
		T const * buffer = constPtr();

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				result[j][i] = *buffer++;
			}
		}
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > Matrix< T, Columns, Rows >::recGetMinor( uint32_t x, uint32_t y, uint32_t rows, uint32_t cols )const
	{
		Matrix< T, Columns, Rows > result;
		result.setIdentity();
		uint32_t i = 0, j = 0;

		for ( uint32_t i = 0; i < cols; i++ )
		{
			if ( i != x )
			{
				j = 0;

				for ( uint32_t j = 0; j < rows; j++ )
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
	inline T Matrix< T, Columns, Rows >::getTrace()const
	{
		T sum = T();
		T * buffer = m_data.data();

		for ( int i = 0; i < MinValue< Columns, Rows >::value; i++ )
		{
			sum += *buffer;
			buffer += Rows + 1;
		}

		return sum;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline void Matrix< T, Columns, Rows >::setIdentity()
	{
		initialise( T{ 1 } );
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > Matrix< T, Columns, Rows >::getIdentity()const
	{
		Matrix< T, Columns, Rows > result( * this );
		result.setIdentity();
		return result;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< T, Columns, Rows > const & rhs )
	{
		std::copy( rhs.m_data.begin(), rhs.m_data.end(), m_data.begin() );
		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< T, Columns, Rows > && rhs )
	{
		if ( this != &rhs )
		{
			m_data = rhs.m_data;
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Matrix< Type, Columns, Rows > const & rhs )
	{
		Type const * buffer = rhs.constPtr();
		T * mine = m_data.data();

		for ( int i = 0; i < count; i++ )
		{
			*mine++ = T( *buffer++ );
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator=( Type const * rhs )
	{
		T * mine = m_data.data();

		for ( size_t i = 0; i < count; i++ )
		{
			*mine++ = T( *rhs++ );
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator+=( Matrix< Type, Columns, Rows > const & rhs )
	{
		Type const * buffer = rhs.constPtr();
		T * mine = m_data.data();

		for ( int i = 0; i < count; i++ )
		{
			*mine++ += T( *buffer++ );
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	template< typename Type >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator-=( Matrix< Type, Columns, Rows > const & rhs )
	{
		Type const * buffer = rhs.constPtr();
		T * mine = m_data.data();

		for ( int i = 0; i < count; i++ )
		{
			*mine++ -= T( *buffer++ );
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator+=( T const & rhs )
	{
		T * mine = m_data.data();

		for ( int i = 0; i < count; i++ )
		{
			*mine++ += rhs;
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator-=( T const & rhs )
	{
		T * mine = m_data.data();

		for ( int i = 0; i < count; i++ )
		{
			*mine++ -= rhs;
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator*=( T const & rhs )
	{
		T * mine = m_data.data();

		for ( int i = 0; i < count; i++ )
		{
			*mine++ *= rhs;
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix< T, Columns, Rows > & Matrix< T, Columns, Rows >::operator/=( T const & rhs )
	{
		T * mine = m_data.data();

		for ( int i = 0; i < count; i++ )
		{
			*mine++ /= rhs;
		}

		return *this;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Rows > const & Matrix< T, Columns, Rows >::operator[]( uint32_t i )const
	{
		CU_Require( i < Columns );
		return m_columns[i];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Point< T, Rows > & Matrix< T, Columns, Rows >::operator[]( uint32_t i )
	{
		CU_Require( i < Columns );
		return m_columns[i];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T & Matrix< T, Columns, Rows >::operator()( uint32_t col, uint32_t row )
	{
		return m_columns[col][row];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T const & Matrix< T, Columns, Rows >::operator()( uint32_t col, uint32_t row )const
	{
		return m_columns[col][row];
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T * Matrix< T, Columns, Rows >::ptr()
	{
		return m_data.data();
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline T const * Matrix< T, Columns, Rows >::constPtr()const
	{
		return m_data.data();
	}

//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline bool operator==( Matrix< T, Columns, Rows > const & lhs, Matrix< T, Columns, Rows > const & rhs )
	{
		bool result = true;

		for ( uint32_t i = 0; i < Columns && result; i++ )
		{
			for ( uint32_t j = 0; j < Rows && result; j++ )
			{
				result = lhs[i][j] == rhs[i][j];
			}
		}

		return result;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline bool operator!=( Matrix< T, Columns, Rows > const & lhs, Matrix< T, Columns, Rows > const & rhs )
	{
		return ! operator==( lhs, rhs );
	}

	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator+( Matrix< T, Columns, Rows > const & lhs, Matrix< U, Columns, Rows > const & rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx += rhs;
		return mtx;
	}

	template< typename T, uint32_t Columns, uint32_t Rows, typename U >
	Matrix< T, Columns, Rows > operator-( Matrix< T, Columns, Rows > const & lhs, Matrix< U, Columns, Rows > const & rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx -= rhs;
		return mtx;
	}

	template< typename T, uint32_t Columns, uint32_t Rows, typename U, uint32_t _Columns >
	Matrix< T, _Columns, Rows > operator*( Matrix< T, Columns, Rows > const & lhs, Matrix< U, _Columns, Columns > const & rhs )
	{
		Matrix< T, _Columns, Rows > result;

		for ( uint32_t i = 0; i < _Columns; i++ )
		{
			for ( uint32_t j = 0; j < Columns; j++ )
			{
				for ( uint32_t k = 0; k < Rows; k++ )
				{
					result[i][k] += T( lhs[j][k] * rhs[i][j] );
				}
			}
		}

		return result;
	}

	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	inline Point< T, Rows > operator*( Matrix< T, Columns, Rows > const & lhs, Point< U, Columns > const & rhs )
	{
		Point< T, Rows > result;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				result[j] += T( lhs[i][j] * rhs[i] );
			}
		}

		return result;
	}

	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	inline Point< T, Columns > operator*( Point< T, Rows > const & lhs, Matrix< U, Columns, Rows > const & rhs )
	{
		return rhs * lhs;
	}

	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	Matrix <T, Columns, Rows> operator+( Matrix< T, Columns, Rows > const & lhs, U const * rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx += rhs;
		return mtx;
	}

	template <typename T, uint32_t Columns, uint32_t Rows, typename U>
	Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & lhs, U const * rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx -= rhs;
		return mtx;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator+( Matrix< T, Columns, Rows > const & lhs, T const & rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx += rhs;
		return mtx;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & lhs, T const & rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx -= rhs;
		return mtx;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator*( Matrix< T, Columns, Rows > const & lhs, T const & rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx *= rhs;
		return mtx;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	Matrix <T, Columns, Rows> operator/( Matrix< T, Columns, Rows > const & lhs, T const & rhs )
	{
		Matrix< T, Columns, Rows > mtx( lhs );
		mtx /= rhs;
		return mtx;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator+( T const & lhs, Matrix< T, Columns, Rows > const & rhs )
	{
		return rhs + lhs;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator-( T const & lhs, Matrix< T, Columns, Rows > const & rhs )
	{
		return rhs - lhs;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator*( T const & lhs, Matrix< T, Columns, Rows > const & rhs )
	{
		return rhs * lhs;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline Matrix <T, Columns, Rows> operator-( Matrix< T, Columns, Rows > const & matrix )
	{
		Matrix< T, Columns, Rows > result;

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				result[i][j] = -matrix[i][j];
			}
		}

		return result;
	}

//*************************************************************************************************

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline castor::String & operator<<( castor::String & text, castor::Matrix< T, Columns, Rows > const & matrix )
	{
		castor::StringStream stream{ castor::makeStringStream() };
		stream.precision( 10 );
		stream << matrix;
		text += stream.str();
		return text;
	}

	template< typename T, uint32_t Columns, uint32_t Rows >
	inline castor::String & operator>>( castor::String & text, castor::Matrix< T, Columns, Rows > & matrix )
	{
		castor::StringStream stream( text );
		stream >> matrix;
		text = stream.str();
		return text;
	}

	template< typename CharT, typename T, uint32_t Columns, uint32_t Rows >
	inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & stream, castor::Matrix< T, Columns, Rows > const & matrix )
	{
		auto precision = stream.precision( 10 );

		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				stream.width( 15 );
				stream << std::right << matrix[i][j];
			}

			stream << std::endl;
		}

		stream.precision( precision );
		return stream;
	}

	template< typename CharT, typename T, uint32_t Columns, uint32_t Rows >
	inline std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & stream, castor::Matrix< T, Columns, Rows > & matrix )
	{
		for ( uint32_t i = 0; i < Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				stream >> matrix[i][j];
			}

			stream.ignore();
		}

		return stream;
	}
}
