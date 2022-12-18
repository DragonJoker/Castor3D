#include "CastorUtils/Math/PointOperators.hpp"

#include <cstring>
#include <numeric>

namespace castor
{
	//*************************************************************************************************

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords()
		: m_coords( nullptr )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( T * rhs )
		: m_coords( rhs )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > const & rhs )
		: m_coords( rhs.m_coords )
	{
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Coords< T, Count > && rhs )
		: m_coords( nullptr )
	{
		m_coords = std::move( rhs.m_coords );
		rhs.m_coords = nullptr;
	}

	template< typename T, uint32_t Count >
	Coords< T, Count >::Coords( Point< T, Count > & rhs )
		: m_coords( rhs.ptr() )
	{
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count >::~Coords()
	{
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( Coords< T, Count > const & rhs )
	{
		m_coords = rhs.m_coords;
		return *this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( Coords< T, Count > && rhs )
	{
		if ( this != &rhs )
		{
			m_coords = std::move( rhs.m_coords );
			rhs.m_coords = nullptr;
		}

		return *this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( Point< T, Count > const & rhs )
	{
		memcpy( m_coords, rhs.constPtr(), binary_size );
		return *this;
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator=( T * values )
	{
		m_coords = values;
		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t UCount >
	inline Coords< T, Count > & Coords< T, Count >::operator+=( Coords< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, Count, UCount >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t UCount >
	inline Coords< T, Count > & Coords< T, Count >::operator-=( Coords< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, Count, UCount >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t UCount >
	inline Coords< T, Count > & Coords< T, Count >::operator+=( Point< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, Count, UCount >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U, uint32_t UCount >
	inline Coords< T, Count > & Coords< T, Count >::operator-=( Point< U, UCount > const & rhs )
	{
		return PtAssignOperators< T, U, Count, UCount >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator+=( U const * rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline Coords< T, Count > & Coords< T, Count >::operator-=( U const * rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator+=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator-=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator*=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::mul( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline Coords< T, Count > & Coords< T, Count >::operator/=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::div( *this, rhs );
	}

	template< typename T, uint32_t Count >
	void Coords< T, Count >::swap( Coords< T, Count > & rhs )
	{
		std::swap( m_coords, rhs.m_coords );
	}

	template< typename T, uint32_t Count >
	void Coords< T, Count >::flip()
	{
		for ( uint32_t i = 0; i < Count / 2; i++ )
		{
			std::swap( m_coords[i], m_coords[Count - 1 - i] );
		}
	}

	template< typename T, uint32_t Count >
	inline void Coords< T, Count >::toValues( T * result )const
	{
		if ( m_coords )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				result[i] = m_coords[i];
			}
		}
	}

	template< typename T, uint32_t Count >
	T const & Coords< T, Count >::at( uint32_t index )const
	{
		CU_Require( index < Count );
		return m_coords[index];
	}

	template< typename T, uint32_t Count >
	T & Coords< T, Count >::at( uint32_t index )
	{
		CU_Require( index < Count );
		return m_coords[index];
	}

	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	inline bool operator==( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs )
	{
		bool result = ( Count == UCount );

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			result = lhs[i] == rhs[i];
		}

		return result;
	}

	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	inline bool operator!=( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs )
	{
		return !( lhs == rhs );
	}

	template< typename T, uint32_t Count >
	inline Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & rhs )
	{
		Point< T, Count > result;

		for ( uint32_t i = 0; i < Count; ++i )
		{
			result[i] = -rhs[i];
		}

		return result;
	}

	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, Count > operator+( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, Count, UCount >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, Count, UCount >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, Count > operator*( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, Count, UCount >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U, uint32_t UCount >
	inline Point< typename std::remove_cv< T >::type, Count > operator/( Coords< T, Count > const & lhs, Coords< U, UCount > const & rhs )
	{
		return PtOperators< T, U, Count, UCount >::div( lhs, rhs );
	}

	template< typename T, uint32_t Count, typename U >
	inline Point< typename std::remove_cv< T >::type, Count > operator+( Coords< T, Count > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, Count, Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, Count, Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< typename std::remove_cv< T >::type, Count > operator*( Coords< T, Count > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, Count, Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count, typename U >
	inline Point< typename std::remove_cv< T >::type, Count > operator/( Coords< T, Count > const & lhs, U const * rhs )
	{
		return PtOperators< T, U, Count, Count >::div( lhs, rhs );
	}

	template< typename T, uint32_t Count >
	inline Point< typename std::remove_cv< T >::type, Count > operator+( Coords< T, Count > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, Count, Count >::add( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< typename std::remove_cv< T >::type, Count > operator-( Coords< T, Count > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, Count, Count >::sub( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< typename std::remove_cv< T >::type, Count > operator*( Coords< T, Count > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, Count, Count >::mul( lhs, rhs );
	}
	template< typename T, uint32_t Count >
	inline Point< typename std::remove_cv< T >::type, Count > operator/( Coords< T, Count > const & lhs, T const & rhs )
	{
		return PtOperators< T, T, Count, Count >::div( lhs, rhs );
	}

	//*************************************************************************************************

	namespace point
	{
		namespace details
		{
			template< typename T, uint32_t Count >
			struct Computedot
			{
				static T calc( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
				{
					T result{};

					for ( uint32_t i = 0; i < Count; i++ )
					{
						result += lhs[i] * rhs[i];
					}

					return result;
				}
			};

			template< typename T >
			struct Computedot< T, 2 >
			{
				static T calc( Coords< T, 2 > const & lhs, Coords< T, 2 > const & rhs )
				{
					return ( lhs[0] * rhs[0] ) + ( lhs[1] * rhs[1] );
				}
			};

			template< typename T >
			struct Computedot< T, 3 >
			{
				static T calc( Coords< T, 3 > const & lhs, Coords< T, 3 > const & rhs )
				{
					return ( lhs[0] * rhs[0] ) + ( lhs[1] * rhs[1] ) + ( lhs[2] * rhs[2] );
				}
			};

			template< typename T >
			struct Computedot< T, 4 >
			{
				static T calc( Coords< T, 4 > const & lhs, Coords< T, 4 > const & rhs )
				{
					return ( lhs[0] * rhs[0] ) + ( lhs[1] * rhs[1] ) + ( lhs[2] * rhs[2] ) + ( lhs[3] * rhs[3] );
				}
			};
		}

		//*************************************************************************************************

		template< typename T, uint32_t Count >
		T dot( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
		{
			return details::Computedot< T, Count >::calc( lhs, rhs );
		}

		template< typename T, typename U >
		Point< T, 3 > cross( Coords< T, 3 > const & lhs, Coords< U, 3 > const & rhs )
		{
			return Point< T, 3 >(
				( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
				);
		}

		template< typename T, uint32_t Count >
		double cosTheta( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
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
		inline void negate( Coords< T, Count > & point )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				point[i] = -point[i];
			}
		}

		template< typename T, uint32_t Count >
		void normalise( Coords< T, Count > & point )
		{
			T l = T( length( point ) );

			if ( l != T{} )
			{
				std::transform( point.constPtr()
					, point.constPtr() + Count
					, point.ptr()
					, [l]( T const & value )
					{
						return value / l;
					} );
			}
		}

		template< typename T, uint32_t Count >
		double lengthSquared( Coords< T, Count > const & point )
		{
			return std::accumulate( point.constPtr()
				, point.constPtr() + Count
				, 0.0
				, []( double a, T const & b )
				{
					return a + ( double( b ) * b );
				} );
		}

		template< typename T, uint32_t Count >
		double length( Coords< T, Count > const & point )
		{
			return sqrt( lengthSquared( point ) );
		}

		template< typename T, uint32_t Count >
		inline double lengthManhattan( Coords< T, Count > const & point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += abs( point[i] );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double lengthMinkowski( Coords< T, Count > const & point, double order )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += pow( double( abs( point[i] ) ), order );
			}

			result = pow( result, 1.0 / order );
			return result;
		}

		template< typename T, uint32_t Count >
		double lengthChebychev( Coords< T, Count > const & point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result = std::max( result, double( abs( point[i] ) ) );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double distanceSquared( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
		{
			return lengthSquared( rhs - lhs );
		}

		template< typename T, uint32_t Count >
		double distance( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
		{
			return length( rhs - lhs );
		}

		template< typename T, uint32_t Count >
		inline double distanceManhattan( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
		{
			return lengthManhattan( rhs - lhs );
		}

		template< typename T, uint32_t Count >
		double distanceMinkowski( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs, double order )
		{
			return lengthMinkowski( rhs - lhs, order );
		}

		template< typename T, uint32_t Count >
		double distanceChebychev( Coords< T, Count > const & lhs, Coords< T, Count > const & rhs )
		{
			return lengthChebychev( rhs - lhs );
		}
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	inline String & operator<<( String & out, Coords< T, Count > const & in )
	{
		StringStream stream{ makeStringStream() };
		stream << in;
		out += stream.str();
		return out;
	}

	template< typename T, uint32_t Count >
	inline String & operator>>( String & in, Coords< T, Count > & out )
	{
		StringStream stream( in );
		stream >> out;
		in = stream.str();
		return in;
	}

	template< typename T, uint32_t Count, typename CharType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & out, Coords< T, Count > const & in )
	{
		if constexpr ( Count != 0 )
		{
			out << in[0];

			for ( uint32_t i = 1; i < Count; i++ )
			{
				out << "\t" << in[i];
			}
		}

		return out;
	}

	template< typename T, uint32_t Count, typename CharType >
	inline std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & in, Coords< T, Count > & out )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			in >> out[i];
		}

		return in;
	}

	//*************************************************************************************************
}
