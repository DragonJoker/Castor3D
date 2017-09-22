#include "PointOperators.hpp"
#include <cstring>

namespace castor
{
	//*************************************************************************************************

	namespace
	{
		template< typename T, uint32_t Count, uint32_t Index, typename U, typename ... Values >
		void construct( Point< T, Count > & p_result, U p_current, Values ... );

		template< typename T, uint32_t Count, uint32_t Index >
		void construct( Point< T, Count > & p_result )
		{
		}

		template< typename T, uint32_t Count, uint32_t Index, typename U >
		void construct( Point< T, Count > & p_result, U p_last )
		{
			if ( Index < Count )
			{
				p_result[Index] = T( p_last );
			}
		}

		template< typename T, uint32_t Count, uint32_t Index, typename U, typename ... Values >
		void construct( Point< T, Count > & p_result, U p_current, Values ... p_values )
		{
			if ( Index < Count )
			{
				p_result[Index] = T( p_current );
				construct < T, Count, Index + 1, Values... > ( p_result, p_values... );
			}
		}
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::TextLoader::TextLoader()
		: castor::TextLoader< Point< T, Count > >()
	{
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::TextLoader::operator()( Point< T, Count > & p_object, TextFile & p_file )
	{
		String strWord;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( p_file.readLine( strWord, 1024, cuT( " \r\n;\t" ) ) > 0 )
			{
				StringStream streamWord( strWord );
				streamWord >> p_object[i];
			}

			xchar cDump;
			p_file.readChar( cDump );
		}

		return true;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Point< T, Count > >( p_tabs )
	{
	}

	template< typename T, uint32_t Count >
	bool Point< T, Count >::TextWriter::operator()( Point< T, Count > const & p_object, TextFile & p_file )
	{
		StringStream streamWord;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			if ( !streamWord.str().empty() )
			{
				streamWord << cuT( " " );
			}

			streamWord << p_object[i];
		}

		bool result = p_file.print( 1024, cuT( "%s%s" ), this->m_tabs.c_str(), streamWord.str().c_str() ) > 0;
		castor::TextWriter< Point< T, Count > >::checkError( result, "Point value" );
		return result;
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Point< T, Count >::Point()
	{
		std::memset( this->m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( T const * p_pValues )
	{
		if ( !p_pValues )
		{
			std::memset( this->m_coords, 0, binary_size );
		}
		else
		{
			std::memcpy( this->m_coords, p_pValues, binary_size );
		}
	}

	template< typename T, uint32_t Count >
	template< typename U >
	Point< T, Count >::Point( U const * p_pValues )
	{
		if ( !p_pValues )
		{
			std::memset( this->m_coords, 0, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				this->m_coords[i] = T( p_pValues[i] );
			}
		}
	}

	template< typename T, uint32_t Count >
	template< typename ValueA, typename ValueB, typename ... Values >
	Point< T, Count >::Point( ValueA p_valueA, ValueB p_valueB, Values ... p_values )
	{
		std::memset( this->m_coords, 0, binary_size );
		construct< T, Count, 0, ValueA, ValueB, Values... >( *this, p_valueA, p_valueB, p_values... );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	Point< T, Count >::Point( Point< U, Count > const & p_pt )
	{
		if ( std::is_same< T, U >::value )
		{
			std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				this->m_coords[i] = static_cast< T >( p_pt.m_coords[i] );
			}
		}
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > const & p_pt )
	{
		std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Point< T, Count > && p_pt )
	{
		std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
		std::memset( p_pt.m_coords, 0, binary_size );
	}

	template< typename T, uint32_t Count >
	Point< T, Count >::Point( Coords< T, Count > const & p_pt )
	{
		std::memcpy( this->m_coords, p_pt.constPtr(), binary_size );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count >::~Point()
	{
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator=( Point< T, Count > const & p_pt )
	{
		std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
		return *this;
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator=( Point< T, Count > && p_pt )
	{
		if ( this != &p_pt )
		{
			std::memcpy( this->m_coords, p_pt.m_coords, binary_size );
			std::memset( p_pt.m_coords, 0, binary_size );
		}

		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator+=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::add( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator-=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::sub( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator*=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::mul( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator/=( Point< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::div( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator+=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::add( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator-=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::sub( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator*=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::mul( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t _Count >
	inline Point< T, Count > & Point< T, Count >::operator/=( Coords< U, _Count > const & p_pt )
	{
		return PtAssignOperators< T, U, Count, _Count >::div( *this, p_pt );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator+=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::add( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator-=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::sub( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator*=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::mul( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Point< T, Count > & Point< T, Count >::operator/=( U const * p_coords )
	{
		return PtAssignOperators< T, U, Count, Count >::div( *this, p_coords );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator*=( T const & p_coord )
	{
		return PtAssignOperators< T, T, Count, Count >::mul( *this, p_coord );
	}

	template< typename T, uint32_t Count >
	inline Point< T, Count > & Point< T, Count >::operator/=( T const & p_coord )
	{
		return PtAssignOperators< T, T, Count, Count >::div( *this, p_coord );
	}

	template< typename T, uint32_t Count >
	void Point< T, Count >::swap( Point< T, Count > & p_pt )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			std::swap( this->m_coords[i], p_pt.m_coords[i] );
		}
	}

	template< typename T, uint32_t Count >
	void Point< T, Count >::flip()
	{
		for ( uint32_t i = 0; i < Count / 2; i++ )
		{
			std::swap( this->m_coords[i], this->m_coords[Count - 1 - i] );
		}
	}

	template< typename T, uint32_t Count >
	inline void Point< T, Count >::toValues( T * p_pResult )const
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_pResult[i] = this->m_coords[i];
		}
	}

	template< typename T, uint32_t Count >
	T const & Point< T, Count >::at( uint32_t p_pos )const
	{
		REQUIRE( p_pos < Count );
		return this->m_coords[p_pos];
	}

	template< typename T, uint32_t Count >
	T & Point< T, Count >::at( uint32_t p_pos )
	{
		REQUIRE( p_pos < Count );
		return this->m_coords[p_pos];
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator==( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = lhs[i] == rhs[i];
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator!=( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return !( lhs == rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator+( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator-( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator*( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator/( Point< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::div( lhs, rhs );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator+( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::add( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator-( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::sub( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator*( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coords );
	}
	template <typename T, uint32_t Count, typename U>
	inline Point< T, Count > operator/( Point< T, Count > const & p_pt, U const * p_coords )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coords );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< T, Count > operator*( Point< T, Count > const & p_pt, U const & p_coord )
	{
		return PtOperators< T, U, Count, Count >::mul( p_pt, p_coord );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< T, Count > operator/( Point< T, Count > const & p_pt, U const & p_coord )
	{
		return PtOperators< T, U, Count, Count >::div( p_pt, p_coord );
	}

	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator==( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = lhs[i] == rhs[i];
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator!=( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return !( lhs == rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator+( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator-( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator*( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator/( Coords< T, Count > const & lhs, Point< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::div( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator==( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		bool result = ( Count == _Count );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = lhs[i] == rhs[i];
		}

		return result;
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline bool operator!=( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return !( lhs == rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator+( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator-( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator*( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t _Count >
	inline Point< T, Count > operator/( Point< T, Count > const & lhs, Coords< U, _Count > const & rhs )
	{
		return PtOperators< T, U, Count, _Count >::div( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< T, Count > operator-( Point< T, Count > const & p_pt )
	{
		Point< T, Count > result;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			result[i] = -p_pt[i];
		}

		return result;
	}

	//*************************************************************************************************

	namespace point
	{
		template< typename T, uint32_t Count >
		inline void negate( Point< T, Count > & p_point )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				p_point[i] = -p_point[i];
			}
		}

		template< typename T, uint32_t Count >
		void normalise( Point< T, Count > & p_point )
		{
			T tLength = T( length( p_point ) );

			if ( tLength != T{} )
			{
				p_point /= tLength;
			}
		}

		template< typename T, uint32_t Count >
		Point< T, Count > getNormalised( Point< T, Count > const & p_point )
		{
			Point< T, Count > result( p_point );
			normalise( result );
			return result;
		}

		template< typename T, typename U, uint32_t Count >
		T dot( Point< T, Count > const & lhs, Point< U, Count > const & rhs )
		{
			T result{};

			for ( uint32_t i = 0; i < Count; i++ )
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

		template< typename T, uint32_t Count >
		double cosTheta( Point< T, Count > const & lhs, Point< T, Count > const & rhs )
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

		template< typename T, uint32_t Count >
		double lengthSquared( Point< T, Count > const & p_point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += p_point[i] * p_point[i];
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double length( Point< T, Count > const & p_point )
		{
			return sqrt( lengthSquared( p_point ) );
		}

		template< typename T, uint32_t Count >
		inline double lengthManhattan( Point< T, Count > const & p_point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += abs( p_point[i] );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double lengthMinkowski( Point< T, Count > const & p_point, double p_order )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += pow( double( abs( p_point[i] ) ), p_order );
			}

			result = pow( result, 1.0 / p_order );
			return result;
		}

		template< typename T, uint32_t Count >
		double lengthChebychev( Point< T, Count > const & p_point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result = std::max( result, double( abs( p_point[i] ) ) );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double distanceSquared( Point< T, Count > const & p_lhs, Point< T, Count > const & p_rhs )
		{
			return lengthSquared( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		double distance( Point< T, Count > const & p_lhs, Point< T, Count > const & p_rhs )
		{
			return length( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		inline double distanceManhattan( Point< T, Count > const & p_lhs, Point< T, Count > const & p_rhs )
		{
			return lengthManhattan( p_rhs - p_lhs );
		}

		template< typename T, uint32_t Count >
		double distanceMinkowski( Point< T, Count > const & p_lhs, Point< T, Count > const & p_rhs, double p_order )
		{
			return lengthMinkowski( p_rhs - p_lhs, p_order );
		}

		template< typename T, uint32_t Count >
		double distanceChebychev( Point< T, Count > const & p_lhs, Point< T, Count > const & p_rhs )
		{
			return lengthChebychev( p_rhs - p_lhs );
		}

		template< typename T, typename U, uint32_t Count >
		T dot( Point< T, Count > const & lhs, Coords< U, Count > const & rhs )
		{
			T result = T();

			for ( uint32_t i = 0; i < Count; i++ )
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

		template< typename T, uint32_t Count >
		double cosTheta( Point< T, Count > const & lhs, Coords< T, Count > const & rhs )
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

		template< typename T, typename U, uint32_t Count >
		T dot( Coords< T, Count > const & lhs, Point< U, Count > const & rhs )
		{
			T result{};

			for ( uint32_t i = 0; i < Count; i++ )
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

		template< typename T, uint32_t Count >
		double cosTheta( Coords< T, Count > const & lhs, Point< T, Count > const & rhs )
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

		template< typename T, typename U, uint32_t Count >
		T dot( Point< T, Count > const & lhs, Coords< U const, Count > const & rhs )
		{
			T result = T();

			for ( uint32_t i = 0; i < Count; i++ )
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

		template< typename T, uint32_t Count >
		double cosTheta( Point< T, Count > const & lhs, Coords< T const, Count > const & rhs )
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

		template< typename T, typename U, uint32_t Count >
		T dot( Coords< T const, Count > const & lhs, Point< U, Count > const & rhs )
		{
			T result{};

			for ( uint32_t i = 0; i < Count; i++ )
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

		template< typename T, uint32_t Count >
		double cosTheta( Coords< T const, Count > const & lhs, Point< T, Count > const & rhs )
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

template< typename T, uint32_t Count >
inline castor::String & operator<<( castor::String & p_strOut, castor::Point< T, Count > const & p_pt )
{
	castor::StringStream streamOut;

	if ( Count )
	{
		streamOut << p_pt[0];

		for ( uint32_t i = 0; i < Count; i++ )
		{
			streamOut << cuT( "\t" ) << p_pt[i];
		}
	}

	p_strOut += streamOut.str();
	return p_strOut;
}

template< typename T, uint32_t Count >
inline castor::String & operator>>( castor::String & p_strIn, castor::Point< T, Count > & p_pt )
{
	castor::StringStream streamIn( p_strIn );

	for ( uint32_t i = 0; i < Count; i++ )
	{
		streamIn >> p_pt[i];
	}

	p_strIn = streamIn.str();
	return p_strIn;
}

template< typename T, uint32_t Count, typename CharType >
inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & p_streamOut, castor::Point< T, Count > const & p_pt )
{
	if ( Count )
	{
		p_streamOut << p_pt[0];

		for ( uint32_t i = 0; i < Count; i++ )
		{
			p_streamOut << "\t" << p_pt[i];
		}
	}

	return p_streamOut;
}
template< typename T, uint32_t Count, typename CharType >
inline std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & p_streamIn, castor::Point< T, Count > & p_pt )
{
	for ( uint32_t i = 0; i < Count; i++ )
	{
		p_streamIn >> p_pt[i];
	}

	return p_streamIn;
}

//*************************************************************************************************
