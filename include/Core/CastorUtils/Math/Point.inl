#include "CastorUtils/Math/PointOperators.hpp"

#include <cstring>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi-stmt"

namespace castor
{
	//*************************************************************************************************

	namespace details
	{
		template< typename SrcType, typename DstType, uint32_t SrcCount, uint32_t DstCount >
		struct DataCopier
		{
			static uint32_t constexpr MinCount = MinValue< SrcCount, DstCount >::value;

			void operator()( SrcType const * src
				, DstType * dst )
			{
				for ( uint32_t i = 0; i < MinCount; i++ )
				{
					dst[i] = DstType( src[i] );
				}

				if constexpr ( DstCount > MinCount )
				{
					for ( uint32_t i = MinCount; i < DstCount; ++i )
					{
						dst[i] = DstType{};
					}
				}
			}
		};

		template< typename Type, uint32_t SrcCount, uint32_t DstCount >
		struct DataCopier< Type, Type, SrcCount, DstCount >
		{
			static uint32_t constexpr MinCount = MinValue< SrcCount, DstCount >::value;

			void operator()( Type const * src
				, Type * dst )
			{
				std::copy( src, src + MinCount, dst );

				if constexpr ( DstCount > MinCount )
				{
					for ( uint32_t i = MinCount; i < DstCount; ++i )
					{
						dst[i] = Type{};
					}
				}
			}
		};
	}

	//*************************************************************************************************

	template< typename T, uint32_t TCount >
	constexpr Point< T, TCount >::Point()noexcept
		: m_data{}
	{
	}

	template< typename T, uint32_t TCount >
	Point< T, TCount >::Point( T const * rhs )noexcept
		: m_data{}
	{
		if ( !rhs )
		{
			std::memset( m_data.coords.data()
				, 0
				, binary_size );
		}
		else
		{
			std::copy( rhs
				, rhs + TCount
				, m_data.coords.begin() );
		}
	}

	template< typename T, uint32_t TCount >
	constexpr Point< T, TCount >::Point( Point< T, TCount > const & rhs )noexcept
		: m_data{}
	{
		std::copy( rhs.begin()
			, rhs.end()
			, m_data.coords.begin() );
	}

	template< typename T, uint32_t TCount >
	constexpr Point< T, TCount >::Point( Point< T, TCount > && rhs )noexcept
		: m_data{}
	{
		std::copy( rhs.begin()
			, rhs.end()
			, m_data.coords.begin() );
	}

	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	Point< T, TCount >::Point( Point< U, UCount > const & rhs )noexcept
		: m_data{}
	{
		details::DataCopier< U, T, UCount, TCount > copier;
		copier( rhs.constPtr(), ptr() );
	}

	template< typename T, uint32_t TCount >
	template< typename U, uint32_t UCount >
	Point< T, TCount >::Point( Coords< U, UCount > const & rhs )noexcept
		: m_data{}
	{
		details::DataCopier< U, T, UCount, TCount > copier;
		copier( rhs.constPtr(), ptr() );
	}

	template< typename T, uint32_t TCount >
	template< typename U >
	Point< T, TCount >::Point( U const * rhs )noexcept
		: m_data{}
	{
		if ( !rhs )
		{
			std::memset( m_data.coords.data()
				, 0
				, binary_size );
		}
		else
		{
			for ( uint32_t i = 0; i < TCount; i++ )
			{
				m_data.coords[i] = T( rhs[i] );
			}
		}
	}

	template< typename T, uint32_t TCount >
	template< typename ValueA >
	constexpr Point< T, TCount >::Point( ValueA a )noexcept
		: m_data{ std::array< T, TCount >{ T( a ) } }
	{
	}

	template< typename T, uint32_t TCount >
	template< typename ValueA, typename ValueB >
	constexpr Point< T, TCount >::Point( ValueA a, ValueB b )noexcept
		: m_data{ std::array< T, TCount >{ T( a ), T( b ) } }
	{
	}

	template< typename T, uint32_t TCount >
	template< typename ValueA, typename ValueB, typename ValueC >
	constexpr Point< T, TCount >::Point( ValueA a, ValueB b, ValueC c )noexcept
		: m_data{ std::array< T, TCount >{ T( a ), T( b ), T( c ) } }
	{
	}

	template< typename T, uint32_t TCount >
	template< typename ValueA, typename ValueB, typename ValueC, typename ValueD >
	constexpr Point< T, TCount >::Point( ValueA a, ValueB b, ValueC c, ValueD d )noexcept
		: m_data{ std::array< T, TCount >{ T( a ), T( b ), T( c ), T( d ) } }
	{
	}

	template< typename T, uint32_t TCount >
	constexpr Point< T, TCount > & Point< T, TCount >::operator=( Point< T, TCount > const & rhs )noexcept
	{
		std::copy( rhs.begin()
			, rhs.end()
			, m_data.coords.begin() );
		return *this;
	}

	template< typename T, uint32_t TCount >
	constexpr Point< T, TCount > & Point< T, TCount >::operator=( Point< T, TCount > && rhs )noexcept
	{
		std::copy( rhs.begin()
			, rhs.end()
			, m_data.coords.begin() );
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
			std::swap( m_data.coords[i], rhs.m_data.coords[i] );
		}
	}

	template< typename T, uint32_t TCount >
	void Point< T, TCount >::flip()
	{
		for ( uint32_t i = 0; i < TCount / 2; i++ )
		{
			std::swap( m_data.coords[i], m_data.coords[TCount - 1 - i] );
		}
	}

	template< typename T, uint32_t TCount >
	inline void Point< T, TCount >::toValues( T * result )const
	{
		for ( uint32_t i = 0; i < TCount; i++ )
		{
			result[i] = m_data.coords[i];
		}
	}

	template< typename T, uint32_t TCount >
	T const & Point< T, TCount >::at( uint32_t index )const
	{
		CU_Require( index < TCount );
		return m_data.coords[index];
	}

	template< typename T, uint32_t TCount >
	T & Point< T, TCount >::at( uint32_t index )
	{
		CU_Require( index < TCount );
		return m_data.coords[index];
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

	template< typename DstCompT, typename SrcT >
	PointTypeT< SrcT, DstCompT > pointCast( SrcT const & src )
	{
		return PointTypeT< SrcT, DstCompT >{ src };
	}

	//*************************************************************************************************

	namespace point
	{
		template< typename CoordT, uint32_t CountT >
		Point< CoordT, CountT > const & getPoint( Point< CoordT, CountT > const & lhs )
		{
			return lhs;
		}

		template< typename CoordT, uint32_t CountT >
		Point< CoordT, CountT > const & getPoint( LengthT< Point< CoordT, CountT > > const & lhs )
		{
			return lhs.value();
		}

		template< typename CoordT, typename CoordU, uint32_t CountT >
		void setPoint( Point< CoordT, CountT > & lhs
			, Point< CoordU, CountT > const & rhs )
		{
			lhs = Point< CoordT, CountT >{ rhs };
		}

		template< typename CoordT, typename CoordU, uint32_t CountT >
		void setPoint( LengthT< Point< CoordT, CountT > > & lhs
			, Point< CoordU, CountT > const & rhs )
		{
			lhs = Point< CoordT, CountT >{ rhs };
		}

		template< typename CoordT, uint32_t CountT >
		void setCoord( Point< CoordT, CountT > & pt
			, uint32_t i
			, CoordT const & c )
		{
			pt[i] = c;
		}

		template< typename CoordT, uint32_t CountT >
		void setCoord( Coords< CoordT, CountT > & pt
			, uint32_t i
			, CoordT const & c )
		{
			pt[i] = c;
		}

		template< typename CoordT, uint32_t CountT >
		void setCoord( LengthT< Point< CoordT, CountT > > & pt
			, uint32_t i
			, CoordT const & c )
		{
			( *pt )[i] = c;
		}

		template< typename CoordT, uint32_t CountT >
		CoordT const & getCoord( Point< CoordT, CountT > const & pt
			, uint32_t i )
		{
			return pt[i];
		}

		template< typename CoordT, uint32_t CountT >
		CoordT const & getCoord( Coords< CoordT, CountT > const & pt
			, uint32_t i )
		{
			return pt[i];
		}

		template< typename CoordT, uint32_t CountT >
		CoordT const & getCoord( LengthT< Point< CoordT, CountT > > const & pt
			, uint32_t i )
		{
			return pt.value()[i];
		}

		template< typename T, uint32_t TCount >
		static void floor( Point< T, TCount > & point )
		{
			for ( uint32_t i = 0; i < TCount; i++ )
			{
				point[i] = std::floor( point[i] );
			}
		}

		template< typename T, uint32_t TCount >
		static Point< T, TCount > getFloored( Point< T, TCount > const & point )
		{
			Point< T, TCount > result{ point };
			point::floor( result );
			return result;
		}

		template< typename T, uint32_t TCount >
		static void ceil( Point< T, TCount > & point )
		{
			for ( uint32_t i = 0; i < TCount; i++ )
			{
				point[i] = std::ceil( point[i] );
			}
		}

		template< typename T, uint32_t TCount >
		static Point< T, TCount > getCeiled( Point< T, TCount > const & point )
		{
			Point< T, TCount > result{ point };
			point::ceil( result );
			return result;
		}

		template< typename T, uint32_t TCount >
		static void round( Point< T, TCount > & point )
		{
			for ( uint32_t i = 0; i < TCount; i++ )
			{
				point[i] = std::round( point[i] );
			}
		}

		template< typename T, uint32_t TCount >
		static Point< T, TCount > getRounded( Point< T, TCount > const & point )
		{
			Point< T, TCount > result{ point };
			point::round( result );
			return result;
		}

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

		template< Vector3T LhsT, Vector3T RhsT >
		LhsT cross( LhsT const & lhs, RhsT const & rhs )
		{
			LhsT result{ lhs };
			setCoord( result, 0, ( getCoord( lhs, 1 ) * getCoord( rhs, 2 ) ) - ( getCoord( lhs, 2 ) * getCoord( rhs, 1 ) ) );
			setCoord( result, 1, ( getCoord( lhs, 2 ) * getCoord( rhs, 0 ) ) - ( getCoord( lhs, 0 ) * getCoord( rhs, 2 ) ) );
			setCoord( result, 2, ( getCoord( lhs, 0 ) * getCoord( rhs, 1 ) ) - ( getCoord( lhs, 1 ) * getCoord( rhs, 0 ) ) );
			return result;
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

		template< Vector3T LhsT, typename U >
		LhsT cross( LhsT const & lhs, Coords< U, 3 > const & rhs )
		{
			LhsT result{ lhs };
			setCoord( result, 0, ( getCoord( lhs, 1 ) * getCoord( rhs, 2 ) ) - ( getCoord( lhs, 2 ) * getCoord( rhs, 1 ) ) );
			setCoord( result, 1, ( getCoord( lhs, 2 ) * getCoord( rhs, 0 ) ) - ( getCoord( lhs, 0 ) * getCoord( rhs, 2 ) ) );
			setCoord( result, 2, ( getCoord( lhs, 0 ) * getCoord( rhs, 1 ) ) - ( getCoord( lhs, 1 ) * getCoord( rhs, 0 ) ) );
			return result;
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

		template< typename T, Vector3T RhsT >
		RhsT cross( Coords< T, 3 > const & lhs, RhsT const & rhs )
		{
			RhsT result{ lhs };
			setCoord( result, 0, ( getCoord( lhs, 1 ) * getCoord( rhs, 2 ) ) - ( getCoord( lhs, 2 ) * getCoord( rhs, 1 ) ) );
			setCoord( result, 1, ( getCoord( lhs, 2 ) * getCoord( rhs, 0 ) ) - ( getCoord( lhs, 0 ) * getCoord( rhs, 2 ) ) );
			setCoord( result, 2, ( getCoord( lhs, 0 ) * getCoord( rhs, 1 ) ) - ( getCoord( lhs, 1 ) * getCoord( rhs, 0 ) ) );
			return result;
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

	//*************************************************************************************************

	template< typename T, uint32_t TCount >
	inline String & operator<<( String & out, Point< T, TCount > const & in )
	{
		StringStream stream{ makeStringStream() };
		stream << in;
		out += stream.str();
		return out;
	}

	template< typename T, uint32_t TCount >
	inline String & operator>>( String & in, Point< T, TCount > & out )
	{
		StringStream stream( in );
		stream >> out;
		in = stream.str();
		return in;
	}

	template< typename T, uint32_t TCount, typename CharType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & out, Point< T, TCount > const & in )
	{
		if constexpr ( TCount != 0u )
		{
			out << in[0];

			for ( uint32_t i = 1; i < TCount; i++ )
			{
				out << "\t" << in[i];
			}
		}

		return out;
	}
	template< typename T, uint32_t TCount, typename CharType >
	inline std::basic_istream< CharType > & operator>>( std::basic_istream< CharType > & in, Point< T, TCount > & out )
	{
		for ( uint32_t i = 0; i < TCount; i++ )
		{
			in >> out[i];
		}

		return in;
	}

	//*************************************************************************************************
}

#pragma clang diagnostic pop
