#include "CastorUtils/Math/Point.hpp"
#include "CastorUtils/Math/Coords.hpp"
#include "CastorUtils/Design/Templates.hpp"
#include "CastorUtils/Math/Simd.hpp"

#include <cstring>

namespace castor
{
	//*********************************************************************************************

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & lhs, T2 const & rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & lhs, T2 const & rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & lhs, T2 const & rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & lhs, T2 const & rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & lhs, T2 const * rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & lhs, T2 const * rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & lhs, T2 const * rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & lhs, T2 const * rhs )
	{
		Point< std::remove_cv_t< T1 >, C1 > result( lhs );
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
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & lhs, PtType2 const & rhs )
	{
		return add( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & lhs, PtType2 const & rhs )
	{
		return sub( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & lhs, PtType2 const & rhs )
	{
		return mul( lhs, rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< std::remove_cv_t< T1 >, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & lhs, PtType2 const & rhs )
	{
		return div( lhs, rhs.constPtr() );
	}

	//*********************************************************************************************

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & lhs, T2 const & rhs )
	{
		T1 * buffer = lhs.ptr();
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
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
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
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
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
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
		std::for_each( buffer
			, buffer + MinValue< C1, C2 >::value
			, [&]( T1 & value )
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
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
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
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
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
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
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
		std::for_each( rhs
			, rhs + MinValue< C1, C2 >::value
			, [&]( T2 value )
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

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 4, 4 >
	{
		using RawT1 = std::remove_cv_t< T1 >;

		template< typename PtType1 >
		static inline Point< RawT1, 4 > add( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] + rhs
				, lhs[1] + rhs
				, lhs[2] + rhs
				, lhs[3] + rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > sub( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] - rhs
				, lhs[1] - rhs
				, lhs[2] - rhs
				, lhs[3] - rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > mul( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] * rhs
				, lhs[1] * rhs
				, lhs[2] * rhs
				, lhs[3] * rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > div( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 4 >( lhs[0] / rhs
				, lhs[1] / rhs
				, lhs[2] / rhs
				, lhs[3] / rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > add( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] + rhs[0]
				, lhs[1] + rhs[1]
				, lhs[2] + rhs[2]
				, lhs[3] + rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > sub( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] - rhs[0]
				, lhs[1] - rhs[1]
				, lhs[2] - rhs[2]
				, lhs[3] - rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > mul( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] * rhs[0]
				, lhs[1] * rhs[1]
				, lhs[2] * rhs[2]
				, lhs[3] * rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 4 > div( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 4 >( lhs[0] / rhs[0]
				, lhs[1] / rhs[1]
				, lhs[2] / rhs[2]
				, lhs[3] / rhs[3] );
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
		using RawT1 = std::remove_cv_t< T1 >;

		template< typename PtType1 >
		static inline Point< RawT1, 3 > add( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] + rhs
				, lhs[1] + rhs
				, lhs[2] + rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > sub( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] - rhs
				, lhs[1] - rhs
				, lhs[2] - rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > mul( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] * rhs
				, lhs[1] * rhs
				, lhs[2] * rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > div( PtType1 const & lhs, T2 const & rhs )
		{
			return Point< RawT1, 3 >( lhs[0] / rhs
				, lhs[1] / rhs
				, lhs[2] / rhs );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > add( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] + rhs[0]
				, lhs[1] + rhs[1]
				, lhs[2] + rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > sub( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] - rhs[0]
				, lhs[1] - rhs[1]
				, lhs[2] - rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > mul( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] * rhs[0]
				, lhs[1] * rhs[1]
				, lhs[2] * rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< RawT1, 3 > div( PtType1 const & lhs, T2 const * rhs )
		{
			return Point< RawT1, 3 >( lhs[0] / rhs[0]
				, lhs[1] / rhs[1]
				, lhs[2] / rhs[2] );
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
		using RawT1 = std::remove_cv_t< T1 >;

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
