#include "PointOperators.hpp"
#include <cstring>

namespace castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename T, uint32_t TCount, uint32_t Index, typename U, typename ... Values >
		void construct( Point< T, TCount > & result, U current, Values ... );

		template< typename T, uint32_t TCount, uint32_t Index >
		void construct( Point< T, TCount > & result )
		{
		}

		template< typename T, uint32_t TCount, uint32_t Index, typename U >
		void construct( Point< T, TCount > & result, U last )
		{
			if ( Index < TCount )
			{
				result[Index] = T( last );
			}
		}

		template< typename T, uint32_t TCount, uint32_t Index, typename U, typename ... Values >
		void construct( Point< T, TCount > & result, U current, Values ... values )
		{
			if ( Index < TCount )
			{
				result[Index] = T( current );
				construct< T, TCount, Index + 1, Values... >( result, values... );
			}
		}
	}

	//*************************************************************************************************

	template< typename T, uint32_t TCount >
	Point< T, TCount >::TextLoader::TextLoader()
		: castor::TextLoader< Point< T, TCount > >()
	{
	}

	template< typename T, uint32_t TCount >
	bool Point< T, TCount >::TextLoader::operator()( Point< T, TCount > & object, TextFile & file )
	{
		String strWord;

		for ( uint32_t i = 0; i < TCount; ++i )
		{
			if ( file.readLine( strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
			{
				StringStream streamWord( strWord );
				streamWord >> object[i];
			}

			xchar cDump;
			file.readChar( cDump );
		}

		return true;
	}

	//*************************************************************************************************

	template< typename T, uint32_t TCount >
	Point< T, TCount >::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Point< T, TCount > >( tabs )
	{
	}

	template< typename T, uint32_t TCount >
	bool Point< T, TCount >::TextWriter::operator()( Point< T, TCount > const & object, TextFile & file )
	{
		StringStream streamWord{ makeStringStream() };

		for ( uint32_t i = 0; i < TCount; ++i )
		{
			if ( !streamWord.str().empty() )
			{
				streamWord << cuT( " " );
			}

			streamWord << object[i];
		}

		bool result = file.print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), streamWord.str().c_str() ) > 0;
		castor::TextWriter< Point< T, TCount > >::checkError( result, "Point value" );
		return result;
	}

	//*************************************************************************************************

	template< typename T, uint32_t TCount >
	Point< T, TCount >::Point()
		: m_coords{}
	{
	}

	template< typename T, uint32_t TCount >
	Point< T, TCount >::Point( T const * rhs )
	{
		if ( !rhs )
		{
			std::memset( m_coords.data()
				, 0
				, binary_size );
		}
		else
		{
			std::copy( rhs
				, rhs + TCount
				, m_coords.begin() );
		}
	}

	template< typename T, uint32_t TCount >
	Point< T, TCount >::Point( Point< T, TCount > const & rhs )
	{
		std::copy( rhs.begin()
			, rhs.end()
			, m_coords.begin() );
	}

	template< typename T, uint32_t TCount >
	Point< T, TCount >::Point( Point< T, TCount > && rhs )
	{
		std::copy( rhs.begin()
			, rhs.end()
			, m_coords.begin() );
	}

	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	Point< T, TCount >::Point( Point< U, UCount > const & rhs )
	{
		static uint32_t constexpr MinCount = MinValue< TCount, UCount >::value;

		if constexpr( std::is_same< T, U >::value )
		{
			std::copy( rhs.begin()
				, rhs.begin() + MinCount
				, m_coords.begin() );
		}
		else
		{
			for ( uint32_t i = 0; i < MinCount; i++ )
			{
				m_coords[i] = T( rhs[i] );
			}
		}

		if constexpr( TCount > UCount )
		{
			for ( uint32_t i = UCount; i < TCount; ++i )
			{
				m_coords[i] = T{};
			}
		}
	}

	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	Point< T, TCount >::Point( Coords< U, UCount > const & rhs )
	{
		static uint32_t constexpr MinCount = MinValue< TCount, UCount >::value;

		if constexpr( std::is_same< T, U >::value )
		{
			std::copy( rhs.begin()
				, rhs.begin() + MinCount
				, m_coords.begin() );
		}
		else
		{
			for ( uint32_t i = 0; i < MinCount; i++ )
			{
				m_coords[i] = T( rhs[i] );
			}
		}

		if constexpr( TCount > UCount )
		{
			for ( uint32_t i = UCount; i < TCount; ++i )
			{
				m_coords[i] = T{};
			}
		}
	}

	template< typename T, uint32_t TCount >
	template< typename U >
	Point< T, TCount >::Point( U const * rhs )
	{
		if ( !rhs )
		{
			std::memset( m_coords.data()
				, 0
				, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < TCount; i++ )
			{
				m_coords[i] = T( rhs[i] );
			}
		}
	}

	template< typename T, uint32_t TCount >
	template< typename ValueA, typename ValueB, typename ... Values >
	Point< T, TCount >::Point( ValueA a, ValueB b, Values ... values )
		: m_coords{}
	{
		construct< T, TCount, 0, ValueA, ValueB, Values... >( *this, a, b, values... );
	}

	template< typename T, uint32_t TCount >
	inline Point< T, TCount > & Point< T, TCount >::operator=( Point< T, TCount > const & rhs )
	{
		std::copy( rhs.begin()
			, rhs.end()
			, m_coords.begin() );
		return *this;
	}

	template< typename T, uint32_t TCount >
	inline Point< T, TCount > & Point< T, TCount >::operator=( Point< T, TCount > && rhs )
	{
		if ( this != &rhs )
		{
			std::copy( rhs.begin()
				, rhs.end()
				, m_coords.begin() );
		}

		return *this;
	}

	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator+=( Point< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::add( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator-=( Point< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::sub( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator*=( Point< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::mul( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator/=( Point< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::div( *this, rhs );
	}

	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator+=( Coords< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::add( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator-=( Coords< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::sub( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator*=( Coords< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::mul( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	inline Point< T, TCount > & Point< T, TCount >::operator/=( Coords< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, TCount, UCount >::div( *this, rhs );
	}

	template< typename T, uint32_t TCount >
	template< typename U >
	inline Point< T, TCount > & Point< T, TCount >::operator+=( U const * rhs )
	{
		return PtAssignOperators< T, U, TCount, TCount >::add( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U >
	inline Point< T, TCount > & Point< T, TCount >::operator-=( U const * rhs )
	{
		return PtAssignOperators< T, U, TCount, TCount >::sub( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U >
	inline Point< T, TCount > & Point< T, TCount >::operator*=( U const * rhs )
	{
		return PtAssignOperators< T, U, TCount, TCount >::mul( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	template< typename U >
	inline Point< T, TCount > & Point< T, TCount >::operator/=( U const * rhs )
	{
		return PtAssignOperators< T, U, TCount, TCount >::div( *this, rhs );
	}

	template< typename T, uint32_t TCount >
	inline Point< T, TCount > & Point< T, TCount >::operator+=( T const & rhs )
	{
		return PtAssignOperators< T, T, TCount, TCount >::add( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	inline Point< T, TCount > & Point< T, TCount >::operator-=( T const & rhs )
	{
		return PtAssignOperators< T, T, TCount, TCount >::sub( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	inline Point< T, TCount > & Point< T, TCount >::operator*=( T const & rhs )
	{
		return PtAssignOperators< T, T, TCount, TCount >::mul( *this, rhs );
	}
	template< typename T, uint32_t TCount >
	inline Point< T, TCount > & Point< T, TCount >::operator/=( T const & rhs )
	{
		return PtAssignOperators< T, T, TCount, TCount >::div( *this, rhs );
	}

	template< typename T, uint32_t TCount >
	void Point< T, TCount >::swap( Point< T, TCount > & rhs )
	{
		for ( uint32_t i = 0; i < TCount; i++ )
		{
			std::swap( m_coords[i], rhs.m_coords[i] );
		}
	}

	template< typename T, uint32_t TCount >
	void Point< T, TCount >::flip()
	{
		for ( uint32_t i = 0; i < TCount / 2; i++ )
		{
			std::swap( m_coords[i], m_coords[TCount - 1 - i] );
		}
	}

	template< typename T, uint32_t TCount >
	inline void Point< T, TCount >::toValues( T * result )const
	{
		for ( uint32_t i = 0; i < TCount; i++ )
		{
			result[i] = m_coords[i];
		}
	}

	template< typename T, uint32_t TCount >
	T const & Point< T, TCount >::at( uint32_t index )const
	{
		REQUIRE( index < TCount );
		return m_coords[index];
	}

	template< typename T, uint32_t TCount >
	T & Point< T, TCount >::at( uint32_t index )
	{
		REQUIRE( index < TCount );
		return m_coords[index];
	}

	//*************************************************************************************************

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline bool operator==( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		bool result = ( TCount == UCount );

		for ( uint32_t i = 0; i < TCount && result; i++ )
		{
			result = lhs[i] == rhs[i];
		}

		return result;
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline bool operator!=( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return !( lhs == rhs );
	}

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline bool operator==( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		bool result = ( TCount == UCount );

		for ( uint32_t i = 0; i < TCount && result; i++ )
		{
			result = lhs[i] == rhs[i];
		}

		return result;
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline bool operator!=( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return !( lhs == rhs );
	}

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline bool operator==( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs )
	{
		bool result = ( TCount == UCount );

		for ( uint32_t i = 0; i < TCount && result; i++ )
		{
			result = lhs[i] == rhs[i];
		}

		return result;
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline bool operator!=( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs )
	{
		return !( lhs == rhs );
	}

	template< typename T, uint32_t TCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & rhs )
	{
		Point< typename std::remove_cv< T >::type, TCount > result;

		for ( uint32_t i = 0; i < TCount; ++i )
		{
			result[i] = -rhs[i];
		}

		return result;
	}

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::add( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::sub( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::mul( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::div( lhs, rhs );
	}

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator+( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::add( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator-( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::sub( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator*( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::mul( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator/( Coords< T, TCount > const & lhs, Point< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::div( lhs, rhs );
	}

	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::add( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::sub( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::mul( lhs, rhs );
	}
	template< typename T, uint32_t TCount, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, TCount, UCount >::div( lhs, rhs );
	}

	template <typename T, uint32_t TCount, typename U>
	inline Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, TCount, TCount >::add( lhs, rhs );
	}
	template <typename T, uint32_t TCount, typename U>
	inline Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, TCount, TCount >::sub( lhs, rhs );
	}
	template <typename T, uint32_t TCount, typename U>
	inline Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, TCount, TCount >::mul( lhs, rhs );
	}
	template <typename T, uint32_t TCount, typename U>
	inline Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, TCount, TCount >::div( lhs, rhs );
	}

	template< typename T, uint32_t TCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator+( Point< T, TCount > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, TCount, TCount >::add( lhs, rhs );
	}
	template< typename T, uint32_t TCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator-( Point< T, TCount > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, TCount, TCount >::sub( lhs, rhs );
	}
	template< typename T, uint32_t TCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator*( Point< T, TCount > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, TCount, TCount >::mul( lhs, rhs );
	}
	template< typename T, uint32_t TCount >
	inline Point< typename std::remove_cv< T >::type, TCount > operator/( Point< T, TCount > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, TCount, TCount >::div( lhs, rhs );
	}

	//*************************************************************************************************

	namespace point
	{
		template< typename T, uint32_t TCount >
		inline void negate( Point< T, TCount > & point )
		{
			for ( uint32_t i = 0; i < TCount; i++ )
			{
				point[i] = -point[i];
			}
		}

		template< typename T, uint32_t TCount >
		void normalise( Point< T, TCount > & point )
		{
			T tLength = T( length( point ) );

			if ( tLength != T{} )
			{
				point /= tLength;
			}
		}

		template< typename T, uint32_t TCount >
		Point< T, TCount > getNormalised( Point< T, TCount > const & point )
		{
			Point< T, TCount > result( point );
			normalise( result );
			return result;
		}

		template< typename T, typename U, uint32_t TCount >
		T dot( Point< T, TCount > const & lhs, Point< U, TCount > const & rhs )
		{
			T result{};

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += T( lhs[i] * rhs[i] );
			}

			return result;
		}

		template< typename T, typename U >
		Point< T, 3 > cross( Point< T, 3 > const & lhs, Point< U, 3 > const & rhs )
		{
			return Point< T, 3 >(
				( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
				);
		}

		template< typename T, uint32_t TCount >
		double cosTheta( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs )
		{
			double result = double( length( lhs ) * length( rhs ) );

			if ( result != 0 )
			{
				result = dot( lhs, rhs ) / result;
			}
			else
			{
				result = dot( lhs, rhs );
			}

			return result;
		}

		template< typename T, uint32_t TCount >
		double lengthSquared( Point< T, TCount > const & point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += point[i] * point[i];
			}

			return result;
		}

		template< typename T, uint32_t TCount >
		double length( Point< T, TCount > const & point )
		{
			return sqrt( lengthSquared( point ) );
		}

		template< typename T, uint32_t TCount >
		inline double lengthManhattan( Point< T, TCount > const & point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += abs( point[i] );
			}

			return result;
		}

		template< typename T, uint32_t TCount >
		double lengthMinkowski( Point< T, TCount > const & point, double order )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += pow( double( abs( point[i] ) ), order );
			}

			result = pow( result, 1.0 / order );
			return result;
		}

		template< typename T, uint32_t TCount >
		double lengthChebychev( Point< T, TCount > const & point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result = std::max( result, double( abs( point[i] ) ) );
			}

			return result;
		}

		template< typename T, uint32_t TCount >
		double distanceSquared( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs )
		{
			return lengthSquared( rhs - lhs );
		}

		template< typename T, uint32_t TCount >
		double distance( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs )
		{
			return length( rhs - lhs );
		}

		template< typename T, uint32_t TCount >
		inline double distanceManhattan( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs )
		{
			return lengthManhattan( rhs - lhs );
		}

		template< typename T, uint32_t TCount >
		double distanceMinkowski( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs, double order )
		{
			return lengthMinkowski( rhs - lhs, order );
		}

		template< typename T, uint32_t TCount >
		double distanceChebychev( Point< T, TCount > const & lhs, Point< T, TCount > const & rhs )
		{
			return lengthChebychev( rhs - lhs );
		}

		template< typename T, typename U, uint32_t TCount >
		T dot( Point< T, TCount > const & lhs, Coords< U, TCount > const & rhs )
		{
			T result = T();

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += T( lhs[i] * rhs[i] );
			}

			return result;
		}

		template< typename T, typename U >
		Point< T, 3 > cross( Point< T, 3 > const & lhs, Coords< U, 3 > const & rhs )
		{
			return Point< T, 3 >(
				( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
				);
		}

		template< typename T, uint32_t TCount >
		double cosTheta( Point< T, TCount > const & lhs, Coords< T, TCount > const & rhs )
		{
			double result = double( length( lhs ) * length( rhs ) );

			if ( result != 0 )
			{
				result = dot( lhs, rhs ) / result;
			}
			else
			{
				result = dot( lhs, rhs );
			}

			return result;
		}

		template< typename T, typename U, uint32_t TCount >
		T dot( Coords< T, TCount > const & lhs, Point< U, TCount > const & rhs )
		{
			T result{};

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += T( lhs[i] * rhs[i] );
			}

			return result;
		}

		template< typename T, typename U >
		Point< T, 3 > cross( Coords< T, 3 > const & lhs, Point< U, 3 > const & rhs )
		{
			return Point< T, 3 >(
				( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
				);
		}

		template< typename T, uint32_t TCount >
		double cosTheta( Coords< T, TCount > const & lhs, Point< T, TCount > const & rhs )
		{
			double result = double( length( lhs ) * length( rhs ) );

			if ( result != 0 )
			{
				result = dot( lhs, rhs ) / result;
			}
			else
			{
				result = dot( lhs, rhs );
			}

			return result;
		}

		template< typename T, typename U, uint32_t TCount >
		T dot( Point< T, TCount > const & lhs, Coords< U const, TCount > const & rhs )
		{
			T result = T();

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += T( lhs[i] * rhs[i] );
			}

			return result;
		}

		template< typename T, typename U >
		Point< T, 3 > cross( Point< T, 3 > const & lhs, Coords< U const, 3 > const & rhs )
		{
			return Point< T, 3 >(
				( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
				);
		}

		template< typename T, uint32_t TCount >
		double cosTheta( Point< T, TCount > const & lhs, Coords< T const, TCount > const & rhs )
		{
			double result = double( length( lhs ) * length( rhs ) );

			if ( result != 0 )
			{
				result = dot( lhs, rhs ) / result;
			}
			else
			{
				result = dot( lhs, rhs );
			}

			return result;
		}

		template< typename T, typename U, uint32_t TCount >
		T dot( Coords< T const, TCount > const & lhs, Point< U, TCount > const & rhs )
		{
			T result{};

			for ( uint32_t i = 0; i < TCount; i++ )
			{
				result += T( lhs[i] * rhs[i] );
			}

			return result;
		}

		template< typename T, typename U >
		Point< T, 3 > cross( Coords< T const, 3 > const & lhs, Point< U, 3 > const & rhs )
		{
			return Point< T, 3 >(
				( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
				);
		}

		template< typename T, uint32_t TCount >
		double cosTheta( Coords< T const, TCount > const & lhs, Point< T, TCount > const & rhs )
		{
			double result = double( length( lhs ) * length( rhs ) );

			if ( result != 0 )
			{
				result = dot( lhs, rhs ) / result;
			}
			else
			{
				result = dot( lhs, rhs );
			}

			return result;
		}
	}
}

//*************************************************************************************************

template< typename T, uint32_t TCount >
inline castor::String & operator<<( castor::String & out, castor::Point< T, TCount > const & in )
{
	castor::StringStream stream{ makeStringStream() };
	stream << in;
	out += stream.str();
	return out;
}

template< typename T, uint32_t TCount >
inline castor::String & operator>>( castor::String & in, castor::Point< T, TCount > & out )
{
	castor::StringStream stream( in );
	stream >> out;
	in = stream.str();
	return in;
}

template< typename T, uint32_t TCount, typename CharType >
inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & out, castor::Point< T, TCount > const & in )
{
	if ( TCount )
	{
		out << in[0];

		for ( uint32_t i = 0; i < TCount; i++ )
		{
			out << "\t" << in[i];
		}
	}

	return out;
}
template< typename T, uint32_t TCount, typename CharType >
inline std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & in, castor::Point< T, TCount > & out )
{
	for ( uint32_t i = 0; i < TCount; i++ )
	{
		in >> out[i];
	}

	return in;
}

//*************************************************************************************************
