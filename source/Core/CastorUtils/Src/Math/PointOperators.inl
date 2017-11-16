#include "Point.hpp"
#include "Coords.hpp"
#include "Design/Templates.hpp"
#include "Simd.hpp"

#include <cstring>

namespace castor
{
	//*********************************************************************************************

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & lhs, T2 const & rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
			{
				value += rhs;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & lhs, T2 const & rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
			{
				value -= rhs;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & lhs, T2 const & rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
			{
				value *= rhs;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & lhs, T2 const & rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
			{
				value /= rhs;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & lhs, T2 const * rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
			{
				*buffer += value;
				buffer++;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & lhs, T2 const * rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
			{
				*buffer -= value;
				buffer++;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & lhs, T2 const * rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
			{
				*buffer *= value;
				buffer++;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & lhs, T2 const * rhs )
	{
		Point< typename std::remove_cv< T1 >::type, C1 > result( lhs );
		auto * buffer = result.ptr();
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
			{
				*buffer /= value;
				buffer++;
			} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & lhs, PtType2 const & rhs )
	{
		return add( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & lhs, PtType2 const & rhs )
	{
		return sub( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & lhs, PtType2 const & rhs )
	{
		return mul( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< typename std::remove_cv< T1 >::type, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & lhs, PtType2 const & rhs )
	{
		return div( lhs, rhs.constPtr() );
	}

	//*********************************************************************************************

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & lhs, T2 const & rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( buffer, buffer + MinValue< C1, C2 >::value, [&]( T1 & value )
		{
			value += rhs;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & lhs, T2 const & rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( buffer, buffer + MinValue< C1, C2 >::value, [&]( T1 & value )
		{
			value -= rhs;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & lhs, T2 const & rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( buffer, buffer + MinValue< C1, C2 >::value, [&]( T1 & value )
		{
			value *= rhs;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & lhs, T2 const & rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( buffer, buffer + MinValue< C1, C2 >::value, [&]( T1 & value )
		{
			value /= rhs;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & lhs, T2 const * rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( rhs, rhs + MinValue< C1, C2 >::value, [&]( T2 value )
		{
			*buffer = T1( *buffer + value );
			buffer++;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & lhs, T2 const * rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( rhs, rhs + MinValue< C1, C2 >::value, [&]( T2 value )
		{
			*buffer = T1( *buffer - value );
			buffer++;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & lhs, T2 const * rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( rhs, rhs + MinValue< C1, C2 >::value, [&]( T2 value )
		{
			*buffer = T1( *buffer * value );
			buffer++;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & lhs, T2 const * rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( rhs, rhs + MinValue< C1, C2 >::value, [&]( T2 value )
		{
			*buffer = T1( *buffer / value );
			buffer++;
		} );
		return lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & lhs, PtType2 const & rhs )
	{
		return add( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & lhs, PtType2 const & rhs )
	{
		return sub( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & lhs, PtType2 const & rhs )
	{
		return mul( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & lhs, PtType2 const & rhs )
	{
		return div( lhs, rhs.constPtr() );
	}

	//*********************************************************************************************

	template<>
	struct PtOperators< float, float, 4, 4 >
	{
#if CASTOR_USE_SSE2

		static inline Point< float, 4 > add( Point< float, 4 > const & lhs, float const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs, rhs, rhs, rhs }
			lhsSimd += rhsSimd;					// lhsSimd = lhs + rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > sub( Point< float, 4 > const & lhs, float const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs, rhs, rhs, rhs }
			lhsSimd -= rhsSimd;					// lhsSimd = lhs - rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > mul( Point< float, 4 > const & lhs, float const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs, rhs, rhs, rhs }
			lhsSimd *= rhsSimd;					// lhsSimd = lhs * rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > div( Point< float, 4 > const & lhs, float const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs, rhs, rhs, rhs }
			lhsSimd /= rhsSimd;					// lhsSimd = lhs / rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > add( Point< float, 4 > const & lhs, float const * rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs[0], rhs[1], rhs[2], rhs[3] }
			lhsSimd += rhsSimd;					// lhsSimd = lhs + rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > sub( Point< float, 4 > const & lhs, float const * rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs[0], rhs[1], rhs[2], rhs[3] }
			lhsSimd -= rhsSimd;					// lhsSimd = lhs - rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > mul( Point< float, 4 > const & lhs, float const * rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs[0], rhs[1], rhs[2], rhs[3] }
			lhsSimd *= rhsSimd;					// lhsSimd = lhs * rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > div( Point< float, 4 > const & lhs, float const * rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs };				// rhsSimd = { rhs[0], rhs[1], rhs[2], rhs[3] }
			lhsSimd /= rhsSimd;					// lhsSimd = lhs / rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > add( Point< float, 4 > const & lhs, Point< float, 4 > const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs.constPtr() };	// rhsSimd = rhs
			lhsSimd += rhsSimd;					// lhsSimd = lhs + rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > sub( Point< float, 4 > const & lhs, Point< float, 4 > const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs.constPtr() };	// rhsSimd = rhs
			lhsSimd -= rhsSimd;					// lhsSimd = lhs - rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > mul( Point< float, 4 > const & lhs, Point< float, 4 > const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs.constPtr() };	// rhsSimd = rhs
			lhsSimd *= rhsSimd;					// lhsSimd = lhs * rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

		static inline Point< float, 4 > div( Point< float, 4 > const & lhs, Point< float, 4 > const & rhs )
		{
			Float4 lhsSimd{ lhs.constPtr() };	// lhsSimd = lhs
			Float4 rhsSimd{ rhs.constPtr() };	// rhsSimd = rhs
			lhsSimd /= rhsSimd;					// lhsSimd = lhs / rhs
			Point< float, 4 > res;
			lhsSimd.toPtr( res.ptr() );
			return res;
		}

#else

		static inline Point4f add( Point4f const & lhs, float const & rhs )
		{
			Point4f result( lhs );
			result[0] += rhs;
			result[1] += rhs;
			result[2] += rhs;
			result[3] += rhs;
			return result;
		}

		static inline Point4f sub( Point4f const & lhs, float const & rhs )
		{
			Point4f result( lhs );
			result[0] -= rhs;
			result[1] -= rhs;
			result[2] -= rhs;
			result[3] -= rhs;
			return result;
		}

		static inline Point4f mul( Point4f const & lhs, float const & rhs )
		{
			Point4f result( lhs );
			result[0] *= rhs;
			result[1] *= rhs;
			result[2] *= rhs;
			result[3] *= rhs;
			return result;
		}

		static inline Point4f div( Point4f const & lhs, float const & rhs )
		{
			Point4f result( lhs );
			result[0] /= rhs;
			result[1] /= rhs;
			result[2] /= rhs;
			result[3] /= rhs;
			return result;
		}

		static inline Point4f add( Point4f const & lhs, float const * rhs )
		{
			Point4f result( lhs );
			result[0] += rhs[0];
			result[1] += rhs[1];
			result[2] += rhs[2];
			result[3] += rhs[3];
			return result;
		}

		static inline Point4f sub( Point4f const & lhs, float const * rhs )
		{
			Point4f result( lhs );
			result[0] -= rhs[0];
			result[1] -= rhs[1];
			result[2] -= rhs[2];
			result[3] -= rhs[3];
			return result;
		}

		static inline Point4f mul( Point4f const & lhs, float const * rhs )
		{
			Point4f result( lhs );
			result[0] *= rhs[0];
			result[1] *= rhs[1];
			result[2] *= rhs[2];
			result[3] *= rhs[3];
			return result;
		}

		static inline Point4f div( Point4f const & lhs, float const * rhs )
		{
			Point4f result( lhs );
			result[0] /= rhs[0];
			result[1] /= rhs[1];
			result[2] /= rhs[2];
			result[3] /= rhs[3];
			return result;
		}

		static inline Point4f add( Point4f const & lhs, Point4f const & rhs )
		{
			Point4f result( lhs );
			result[0] += rhs[0];
			result[1] += rhs[1];
			result[2] += rhs[2];
			result[3] += rhs[3];
			return result;
		}

		static inline Point4f sub( Point4f const & lhs, Point4f const & rhs )
		{
			Point4f result( lhs );
			result[0] -= rhs[0];
			result[1] -= rhs[1];
			result[2] -= rhs[2];
			result[3] -= rhs[3];
			return result;
		}

		static inline Point4f mul( Point4f const & lhs, Point4f const & rhs )
		{
			Point4f result( lhs );
			result[0] *= rhs[0];
			result[1] *= rhs[1];
			result[2] *= rhs[2];
			result[3] *= rhs[3];
			return result;
		}

		static inline Point4f div( Point4f const & lhs, Point4f const & rhs )
		{
			Point4f result( lhs );
			result[0] /= rhs[0];
			result[1] /= rhs[1];
			result[2] /= rhs[2];
			result[3] /= rhs[3];
			return result;
		}

#endif

		template< typename PtType1 >
		static inline Point< float, 4 > add( PtType1 const & lhs, float const & rhs )
		{
			return Point< float, 4 >( lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs, lhs[3] + rhs );
		}

		template< typename PtType1 >
		static inline Point< float, 4 > sub( PtType1 const & lhs, float const & rhs )
		{
			return Point< float, 4 >( lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs, lhs[3] - rhs );
		}

		template< typename PtType1 >
		static inline Point< float, 4 > mul( PtType1 const & lhs, float const & rhs )
		{
			return Point< float, 4 >( lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs );
		}

		template< typename PtType1 >
		static inline Point< float, 4 > div( PtType1 const & lhs, float const & rhs )
		{
			return Point< float, 4 >( lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs );
		}

		template< typename PtType1 >
		static inline Point< float, 4 > add( PtType1 const & lhs, float const * rhs )
		{
			return Point< float, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< float, 4 > sub( PtType1 const & lhs, float const * rhs )
		{
			return Point< float, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< float, 4 > mul( PtType1 const & lhs, float const * rhs )
		{
			return Point< float, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< float, 4 > div( PtType1 const & lhs, float const * rhs )
		{
			return Point< float, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< float, 4 > add( PtType1 const & lhs, PtType2 const & rhs )
		{
			return add( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< float, 4 > sub( PtType1 const & lhs, PtType2 const & rhs )
		{
			return sub( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< float, 4 > mul( PtType1 const & lhs, PtType2 const & rhs )
		{
			return mul( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< float, 4 > div( PtType1 const & lhs, PtType2 const & rhs )
		{
			return div( lhs, rhs.constPtr() );
		}
	};

	//*********************************************************************************************

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 4, 4 >
	{
		using RawT1 = typename std::remove_cv< T1 >::type;

		template< typename PtType1 >
		static inline Point< RawT1, 4 > add( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs, lhs[3] + rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > sub( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs, lhs[3] - rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > mul( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs, lhs[3] * rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > div( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs, lhs[3] / rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > add( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > sub( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > mul( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > div( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 4 > add( PtType1 const & lhs, PtType2 const & rhs )
		{
			return add( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 4 > sub( PtType1 const & lhs, PtType2 const & rhs )
		{
			return sub( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 4 > mul( PtType1 const & lhs, PtType2 const & rhs )
		{
			return mul( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 4 > div( PtType1 const & lhs, PtType2 const & rhs )
		{
			return div( lhs, rhs.constPtr() );
		}
	};

	//*********************************************************************************************

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 3, 3 >
	{
		using RawT1 = typename std::remove_cv< T1 >::type;

		template< typename PtType1 >
		static inline Point< RawT1, 3 > add( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] + rhs, lhs[1] + rhs, lhs[2] + rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > sub( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] - rhs, lhs[1] - rhs, lhs[2] - rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > mul( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] * rhs, lhs[1] * rhs, lhs[2] * rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > div( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] / rhs, lhs[1] / rhs, lhs[2] / rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > add( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > sub( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > mul( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > div( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 3 > add( PtType1 const & lhs, PtType2 const & rhs )
		{
			return add( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 3 > sub( PtType1 const & lhs, PtType2 const & rhs )
		{
			return sub( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 3 > mul( PtType1 const & lhs, PtType2 const & rhs )
		{
			return mul( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 3 > div( PtType1 const & lhs, PtType2 const & rhs )
		{
			return div( lhs, rhs.constPtr() );
		}
	};

	//*********************************************************************************************

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 2, 2 >
	{
		using RawT1 = typename std::remove_cv< T1 >::type;

		template< typename PtType1 >
		static inline Point< RawT1, 2 > add( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 2 >( lhs[0] + rhs, lhs[1] + rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 2 > sub( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 2 >( lhs[0] - rhs, lhs[1] - rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 2 > mul( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 2 >( lhs[0] * rhs, lhs[1] * rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 2 > div( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 2 >( lhs[0] / rhs, lhs[1] / rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 2 > add( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 2 >( lhs[0] + rhs[0], lhs[1] + rhs[1] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 2 > sub( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 2 >( lhs[0] - rhs[0], lhs[1] - rhs[1] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 2 > mul( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 2 >( lhs[0] * rhs[0], lhs[1] * rhs[1] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 2 > div( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 2 >( lhs[0] / rhs[0], lhs[1] / rhs[1] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 2 > add( PtType1 const & lhs, PtType2 const & rhs )
		{
			return add( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 2 > sub( PtType1 const & lhs, PtType2 const & rhs )
		{
			return sub( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 2 > mul( PtType1 const & lhs, PtType2 const & rhs )
		{
			return mul( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< RawT1, 2 > div( PtType1 const & lhs, PtType2 const & rhs )
		{
			return div( lhs, rhs.constPtr() );
		}
	};

	//*********************************************************************************************

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 4, 4 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] += rhs;
			lhs[1] += rhs;
			lhs[2] += rhs;
			lhs[3] += rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] -= rhs;
			lhs[1] -= rhs;
			lhs[2] -= rhs;
			lhs[3] -= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] *= rhs;
			lhs[1] *= rhs;
			lhs[2] *= rhs;
			lhs[3] *= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] /= rhs;
			lhs[1] /= rhs;
			lhs[2] /= rhs;
			lhs[3] /= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] += rhs[0];
			lhs[1] += rhs[1];
			lhs[2] += rhs[2];
			lhs[3] += rhs[3];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] -= rhs[0];
			lhs[1] -= rhs[1];
			lhs[2] -= rhs[2];
			lhs[3] -= rhs[3];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] *= rhs[0];
			lhs[1] *= rhs[1];
			lhs[2] *= rhs[2];
			lhs[3] *= rhs[3];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] /= rhs[0];
			lhs[1] /= rhs[1];
			lhs[2] /= rhs[2];
			lhs[3] /= rhs[3];
			return lhs;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & lhs, PtType2 const & rhs )
		{
			return add( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & lhs, PtType2 const & rhs )
		{
			return sub( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & lhs, PtType2 const & rhs )
		{
			return mul( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & lhs, PtType2 const & rhs )
		{
			return div( lhs, rhs.constPtr() );
		}
	};

	//*********************************************************************************************

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 3, 3 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] += rhs;
			lhs[1] += rhs;
			lhs[2] += rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] -= rhs;
			lhs[1] -= rhs;
			lhs[2] -= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] *= rhs;
			lhs[1] *= rhs;
			lhs[2] *= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] /= rhs;
			lhs[1] /= rhs;
			lhs[2] /= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] += rhs[0];
			lhs[1] += rhs[1];
			lhs[2] += rhs[2];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] -= rhs[0];
			lhs[1] -= rhs[1];
			lhs[2] -= rhs[2];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] *= rhs[0];
			lhs[1] *= rhs[1];
			lhs[2] *= rhs[2];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] /= rhs[0];
			lhs[1] /= rhs[1];
			lhs[2] /= rhs[2];
			return lhs;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & lhs, PtType2 const & rhs )
		{
			return add( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & lhs, PtType2 const & rhs )
		{
			return sub( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & lhs, PtType2 const & rhs )
		{
			return mul( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & lhs, PtType2 const & rhs )
		{
			return div( lhs, rhs.constPtr() );
		}
	};

	//*********************************************************************************************

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 2, 2 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] += rhs;
			lhs[1] += rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] -= rhs;
			lhs[1] -= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] *= rhs;
			lhs[1] *= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const & rhs )
		{
			lhs[0] /= rhs;
			lhs[1] /= rhs;
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] += rhs[0];
			lhs[1] += rhs[1];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] -= rhs[0];
			lhs[1] -= rhs[1];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] *= rhs[0];
			lhs[1] *= rhs[1];
			return lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const * rhs )
		{
			lhs[0] /= rhs[0];
			lhs[1] /= rhs[1];
			return lhs;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & lhs, PtType2 const & rhs )
		{
			return add( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & lhs, PtType2 const & rhs )
		{
			return sub( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & lhs, PtType2 const & rhs )
		{
			return mul( lhs, rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & lhs, PtType2 const & rhs )
		{
			return div( lhs, rhs.constPtr() );
		}
	};

	//*********************************************************************************************
}
