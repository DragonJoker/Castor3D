#include <cstring>
#include <numeric>

namespace c3d
{
	//*************************************************************************************************

	template< typename T, uint32_t Count >
	PointView< T, Count >::PointView( T * rhs )
		: m_coords( rhs )
	{
	}

	template< typename T, uint32_t Count >
	PointView< T, Count >::PointView( Point< T, Count > & rhs )
		: m_coords( rhs.ptr() )
	{
	}

	template< typename T, uint32_t Count >
	inline PointView< T, Count > & PointView< T, Count >::operator=( Point< T, Count > const & rhs )
	{
		memcpy( m_coords, rhs.constPtr(), binary_size );
		return *this;
	}

	template< typename T, uint32_t Count >
	inline PointView< T, Count > & PointView< T, Count >::operator=( T * values )
	{
		m_coords = values;
		return *this;
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline PointView< T, Count > & PointView< T, Count >::operator+=( PointView< U, Count > const & rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline PointView< T, Count > & PointView< T, Count >::operator-=( PointView< U, Count > const & rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline PointView< T, Count > & PointView< T, Count >::operator+=( Point< U, Count > const & rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline PointView< T, Count > & PointView< T, Count >::operator-=( Point< U, Count > const & rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline PointView< T, Count > & PointView< T, Count >::operator+=( U const * rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	template< typename U >
	inline PointView< T, Count > & PointView< T, Count >::operator-=( U const * rhs )
	{
		return PtAssignOperators< T, U, Count, Count >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline PointView< T, Count > & PointView< T, Count >::operator+=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::add( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline PointView< T, Count > & PointView< T, Count >::operator-=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::sub( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline PointView< T, Count > & PointView< T, Count >::operator*=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::mul( *this, rhs );
	}

	template< typename T, uint32_t Count >
	inline PointView< T, Count > & PointView< T, Count >::operator/=( T const & rhs )
	{
		return PtAssignOperators< T, T, Count, Count >::div( *this, rhs );
	}

	template< typename T, uint32_t Count >
	void PointView< T, Count >::swap( PointView< T, Count > & rhs )noexcept
	{
		c3d::swap( m_coords, rhs.m_coords );
	}

	//*************************************************************************************************

	namespace point
	{
		namespace details
		{
			template< typename T, uint32_t Count >
			struct Computedot
			{
				static T calc( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs )
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
				static T calc( PointView< T, 2 > const & lhs, PointView< T, 2 > const & rhs )
				{
					return ( lhs[0] * rhs[0] ) + ( lhs[1] * rhs[1] );
				}
			};

			template< typename T >
			struct Computedot< T, 3 >
			{
				static T calc( PointView< T, 3 > const & lhs, PointView< T, 3 > const & rhs )
				{
					return ( lhs[0] * rhs[0] ) + ( lhs[1] * rhs[1] ) + ( lhs[2] * rhs[2] );
				}
			};

			template< typename T >
			struct Computedot< T, 4 >
			{
				static T calc( PointView< T, 4 > const & lhs, PointView< T, 4 > const & rhs )
				{
					return ( lhs[0] * rhs[0] ) + ( lhs[1] * rhs[1] ) + ( lhs[2] * rhs[2] ) + ( lhs[3] * rhs[3] );
				}
			};
		}

		//*************************************************************************************************

		template< typename T, uint32_t Count >
		T dot( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs )
		{
			return details::Computedot< T, Count >::calc( lhs, rhs );
		}

		template< typename T, typename U >
		Point< T, 3 > cross( PointView< T, 3 > const & lhs, PointView< U, 3 > const & rhs )
		{
			return Point< T, 3 >(
				( lhs[1] * rhs[2] ) - ( lhs[2] * rhs[1] ),
				( lhs[2] * rhs[0] ) - ( lhs[0] * rhs[2] ),
				( lhs[0] * rhs[1] ) - ( lhs[1] * rhs[0] )
				);
		}

		template< typename T, uint32_t Count >
		double cosTheta( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs )
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
		inline void negate( PointView< T, Count > & point )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				point[i] = -point[i];
			}
		}

		template< typename T, uint32_t Count >
		void normalise( PointView< T, Count > & point )
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
		double lengthSquared( PointView< T, Count > const & point )
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
		double length( PointView< T, Count > const & point )
		{
			return sqrt( lengthSquared( point ) );
		}

		template< typename T, uint32_t Count >
		inline double lengthManhattan( PointView< T, Count > const & point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result += abs( point[i] );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double lengthMinkowski( PointView< T, Count > const & point, double order )
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
		double lengthChebychev( PointView< T, Count > const & point )
		{
			double result = 0.0;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				result = std::max( result, double( abs( point[i] ) ) );
			}

			return result;
		}

		template< typename T, uint32_t Count >
		double distanceSquared( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs )
		{
			return lengthSquared( rhs - lhs );
		}

		template< typename T, uint32_t Count >
		double distance( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs )
		{
			return length( rhs - lhs );
		}

		template< typename T, uint32_t Count >
		inline double distanceManhattan( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs )
		{
			return lengthManhattan( rhs - lhs );
		}

		template< typename T, uint32_t Count >
		double distanceMinkowski( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs, double order )
		{
			return lengthMinkowski( rhs - lhs, order );
		}

		template< typename T, uint32_t Count >
		double distanceChebychev( PointView< T, Count > const & lhs, PointView< T, Count > const & rhs )
		{
			return lengthChebychev( rhs - lhs );
		}
	}

	//*************************************************************************************************
}
