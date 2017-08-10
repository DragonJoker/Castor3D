#include "Point.hpp"
#include "Coords.hpp"
#include "Design/Templates.hpp"
#include "Simd.hpp"

#include <cstring>

namespace castor
{
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value += p_rhs;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value -= p_rhs;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value *= p_rhs;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value /= p_rhs;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer += p_pVal;
			pBuffer++;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer -= p_pVal;
			pBuffer++;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer *= p_pVal;
			pBuffer++;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > result( p_lhs );
		T1 * pBuffer = result.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer /= p_pVal;
			pBuffer++;
		} );
		return result;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return add( p_lhs, p_rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return sub( p_lhs, p_rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return mul( p_lhs, p_rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return div( p_lhs, p_rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value += p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value -= p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value *= p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( pBuffer, pBuffer + MinValue< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value /= p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer = T1( *pBuffer + p_pVal );
			pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer = T1( *pBuffer - p_pVal );
			pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer = T1( *pBuffer * p_pVal );
			pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + MinValue< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*pBuffer = T1( *pBuffer / p_pVal );
			pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return add( p_lhs, p_rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return sub( p_lhs, p_rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return mul( p_lhs, p_rhs.constPtr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return div( p_lhs, p_rhs.constPtr() );
	}

	template<>
	struct PtOperators< float, float, 4, 4 >
	{
#if CASTOR_USE_SSE2

		static inline castor::Point< float, 4 > add( castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs, p_rhs, p_rhs, p_rhs }
			lhs += rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > sub( castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs, p_rhs, p_rhs, p_rhs }
			lhs -= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > mul( castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs, p_rhs, p_rhs, p_rhs }
			lhs *= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > div( castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs, p_rhs, p_rhs, p_rhs }
			lhs /= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > add( castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			lhs += rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > sub( castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			lhs -= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > mul( castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			lhs *= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > div( castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs };				// rhs = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			lhs /= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > add( castor::Point< float, 4 > const & p_lhs, castor::Point< float, 4 > const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs.constPtr() };	// rhs = p_rhs
			lhs += rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > sub( castor::Point< float, 4 > const & p_lhs, castor::Point< float, 4 > const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs.constPtr() };	// rhs = p_rhs
			lhs -= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > mul( castor::Point< float, 4 > const & p_lhs, castor::Point< float, 4 > const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs.constPtr() };	// rhs = p_rhs
			lhs *= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

		static inline castor::Point< float, 4 > div( castor::Point< float, 4 > const & p_lhs, castor::Point< float, 4 > const & p_rhs )
		{
			Float4 lhs{ p_lhs.constPtr() };	// lhs = p_lhs
			Float4 rhs{ p_rhs.constPtr() };	// rhs = p_rhs
			lhs /= rhs;						// lhs = p_lhs + p_rhs
			castor::Point< float, 4 > res;
			lhs.toPtr( res.ptr() );
			return res;
		}

#else

		static inline Point4f add( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] += p_rhs;
			result[1] += p_rhs;
			result[2] += p_rhs;
			result[3] += p_rhs;
			return result;
		}

		static inline Point4f sub( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] -= p_rhs;
			result[1] -= p_rhs;
			result[2] -= p_rhs;
			result[3] -= p_rhs;
			return result;
		}

		static inline Point4f mul( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] *= p_rhs;
			result[1] *= p_rhs;
			result[2] *= p_rhs;
			result[3] *= p_rhs;
			return result;
		}

		static inline Point4f div( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] /= p_rhs;
			result[1] /= p_rhs;
			result[2] /= p_rhs;
			result[3] /= p_rhs;
			return result;
		}

		static inline Point4f add( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f result( p_lhs );
			result[0] += p_rhs[0];
			result[1] += p_rhs[1];
			result[2] += p_rhs[2];
			result[3] += p_rhs[3];
			return result;
		}

		static inline Point4f sub( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f result( p_lhs );
			result[0] -= p_rhs[0];
			result[1] -= p_rhs[1];
			result[2] -= p_rhs[2];
			result[3] -= p_rhs[3];
			return result;
		}

		static inline Point4f mul( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f result( p_lhs );
			result[0] *= p_rhs[0];
			result[1] *= p_rhs[1];
			result[2] *= p_rhs[2];
			result[3] *= p_rhs[3];
			return result;
		}

		static inline Point4f div( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f result( p_lhs );
			result[0] /= p_rhs[0];
			result[1] /= p_rhs[1];
			result[2] /= p_rhs[2];
			result[3] /= p_rhs[3];
			return result;
		}

		static inline Point4f add( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] += p_rhs[0];
			result[1] += p_rhs[1];
			result[2] += p_rhs[2];
			result[3] += p_rhs[3];
			return result;
		}

		static inline Point4f sub( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] -= p_rhs[0];
			result[1] -= p_rhs[1];
			result[2] -= p_rhs[2];
			result[3] -= p_rhs[3];
			return result;
		}

		static inline Point4f mul( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] *= p_rhs[0];
			result[1] *= p_rhs[1];
			result[2] *= p_rhs[2];
			result[3] *= p_rhs[3];
			return result;
		}

		static inline Point4f div( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f result( p_lhs );
			result[0] /= p_rhs[0];
			result[1] /= p_rhs[1];
			result[2] /= p_rhs[2];
			result[3] /= p_rhs[3];
			return result;
		}

#endif

		template< typename PtType1 >
		static inline castor::Point< float, 4 > add( PtType1 const & p_lhs, float const & p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] + p_rhs, p_lhs[1] + p_rhs, p_lhs[2] + p_rhs, p_lhs[3] + p_rhs );
		}

		template< typename PtType1 >
		static inline castor::Point< float, 4 > sub( PtType1 const & p_lhs, float const & p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] - p_rhs, p_lhs[1] - p_rhs, p_lhs[2] - p_rhs, p_lhs[3] - p_rhs );
		}

		template< typename PtType1 >
		static inline castor::Point< float, 4 > mul( PtType1 const & p_lhs, float const & p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] * p_rhs, p_lhs[1] * p_rhs, p_lhs[2] * p_rhs, p_lhs[3] * p_rhs );
		}

		template< typename PtType1 >
		static inline castor::Point< float, 4 > div( PtType1 const & p_lhs, float const & p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] / p_rhs, p_lhs[1] / p_rhs, p_lhs[2] / p_rhs, p_lhs[3] / p_rhs );
		}

		template< typename PtType1 >
		static inline castor::Point< float, 4 > add( PtType1 const & p_lhs, float const * p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] + p_rhs[0], p_lhs[1] + p_rhs[1], p_lhs[2] + p_rhs[2], p_lhs[3] + p_rhs[3] );
		}

		template< typename PtType1 >
		static inline castor::Point< float, 4 > sub( PtType1 const & p_lhs, float const * p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] - p_rhs[0], p_lhs[1] - p_rhs[1], p_lhs[2] - p_rhs[2], p_lhs[3] - p_rhs[3] );
		}

		template< typename PtType1 >
		static inline castor::Point< float, 4 > mul( PtType1 const & p_lhs, float const * p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] * p_rhs[0], p_lhs[1] * p_rhs[1], p_lhs[2] * p_rhs[2], p_lhs[3] * p_rhs[3] );
		}

		template< typename PtType1 >
		static inline castor::Point< float, 4 > div( PtType1 const & p_lhs, float const * p_rhs )
		{
			return castor::Point< float, 4 >( p_lhs[0] / p_rhs[0], p_lhs[1] / p_rhs[1], p_lhs[2] / p_rhs[2], p_lhs[3] / p_rhs[3] );
		}

		template< typename PtType1, typename PtType2 >
		static inline castor::Point< float, 4 > add( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline castor::Point< float, 4 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline castor::Point< float, 4 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline castor::Point< float, 4 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.constPtr() );
		}
	};


	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 4, 4 >
	{
		template< typename PtType1 >
		static inline Point< T1, 4 > add( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] + p_rhs, p_lhs[1] + p_rhs, p_lhs[2] + p_rhs, p_lhs[3] + p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > sub( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] - p_rhs, p_lhs[1] - p_rhs, p_lhs[2] - p_rhs, p_lhs[3] - p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > mul( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] * p_rhs, p_lhs[1] * p_rhs, p_lhs[2] * p_rhs, p_lhs[3] * p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > div( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] / p_rhs, p_lhs[1] / p_rhs, p_lhs[2] / p_rhs, p_lhs[3] / p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > add( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] + p_rhs[0], p_lhs[1] + p_rhs[1], p_lhs[2] + p_rhs[2], p_lhs[3] + p_rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > sub( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] - p_rhs[0], p_lhs[1] - p_rhs[1], p_lhs[2] - p_rhs[2], p_lhs[3] - p_rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > mul( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] * p_rhs[0], p_lhs[1] * p_rhs[1], p_lhs[2] * p_rhs[2], p_lhs[3] * p_rhs[3] );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > div( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 4 >( p_lhs[0] / p_rhs[0], p_lhs[1] / p_rhs[1], p_lhs[2] / p_rhs[2], p_lhs[3] / p_rhs[3] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > add( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.constPtr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 3, 3 >
	{
		template< typename PtType1 >
		static inline Point< T1, 3 > add( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] + p_rhs, p_lhs[1] + p_rhs, p_lhs[2] + p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > sub( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] - p_rhs, p_lhs[1] - p_rhs, p_lhs[2] - p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > mul( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] * p_rhs, p_lhs[1] * p_rhs, p_lhs[2] * p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > div( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] / p_rhs, p_lhs[1] / p_rhs, p_lhs[2] / p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > add( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] + p_rhs[0], p_lhs[1] + p_rhs[1], p_lhs[2] + p_rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > sub( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] - p_rhs[0], p_lhs[1] - p_rhs[1], p_lhs[2] - p_rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > mul( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] * p_rhs[0], p_lhs[1] * p_rhs[1], p_lhs[2] * p_rhs[2] );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > div( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 3 >( p_lhs[0] / p_rhs[0], p_lhs[1] / p_rhs[1], p_lhs[2] / p_rhs[2] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > add( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.constPtr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 2, 2 >
	{
		template< typename PtType1 >
		static inline Point< T1, 2 > add( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] + p_rhs, p_lhs[1] + p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > sub( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] - p_rhs, p_lhs[1] - p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > mul( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] * p_rhs, p_lhs[1] * p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > div( PtType1 const & p_lhs, T2 const & p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] / p_rhs, p_lhs[1] / p_rhs );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > add( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] + p_rhs[0], p_lhs[1] + p_rhs[1] );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > sub( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] - p_rhs[0], p_lhs[1] - p_rhs[1] );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > mul( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] * p_rhs[0], p_lhs[1] * p_rhs[1] );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > div( PtType1 const & p_lhs, T2 const * p_rhs )
		{
			return Point< T1, 2 >( p_lhs[0] / p_rhs[0], p_lhs[1] / p_rhs[1] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > add( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.constPtr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 4, 4 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] += p_rhs;
			p_lhs[1] += p_rhs;
			p_lhs[2] += p_rhs;
			p_lhs[3] += p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] -= p_rhs;
			p_lhs[1] -= p_rhs;
			p_lhs[2] -= p_rhs;
			p_lhs[3] -= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] *= p_rhs;
			p_lhs[1] *= p_rhs;
			p_lhs[2] *= p_rhs;
			p_lhs[3] *= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] /= p_rhs;
			p_lhs[1] /= p_rhs;
			p_lhs[2] /= p_rhs;
			p_lhs[3] /= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] += p_rhs[0];
			p_lhs[1] += p_rhs[1];
			p_lhs[2] += p_rhs[2];
			p_lhs[3] += p_rhs[3];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] -= p_rhs[0];
			p_lhs[1] -= p_rhs[1];
			p_lhs[2] -= p_rhs[2];
			p_lhs[3] -= p_rhs[3];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] *= p_rhs[0];
			p_lhs[1] *= p_rhs[1];
			p_lhs[2] *= p_rhs[2];
			p_lhs[3] *= p_rhs[3];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] /= p_rhs[0];
			p_lhs[1] /= p_rhs[1];
			p_lhs[2] /= p_rhs[2];
			p_lhs[3] /= p_rhs[3];
			return p_lhs;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.constPtr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 3, 3 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] += p_rhs;
			p_lhs[1] += p_rhs;
			p_lhs[2] += p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] -= p_rhs;
			p_lhs[1] -= p_rhs;
			p_lhs[2] -= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] *= p_rhs;
			p_lhs[1] *= p_rhs;
			p_lhs[2] *= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] /= p_rhs;
			p_lhs[1] /= p_rhs;
			p_lhs[2] /= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] += p_rhs[0];
			p_lhs[1] += p_rhs[1];
			p_lhs[2] += p_rhs[2];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] -= p_rhs[0];
			p_lhs[1] -= p_rhs[1];
			p_lhs[2] -= p_rhs[2];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] *= p_rhs[0];
			p_lhs[1] *= p_rhs[1];
			p_lhs[2] *= p_rhs[2];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] /= p_rhs[0];
			p_lhs[1] /= p_rhs[1];
			p_lhs[2] /= p_rhs[2];
			return p_lhs;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.constPtr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 2, 2 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] += p_rhs;
			p_lhs[1] += p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] -= p_rhs;
			p_lhs[1] -= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] *= p_rhs;
			p_lhs[1] *= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_lhs, T2 const & p_rhs )
		{
			p_lhs[0] /= p_rhs;
			p_lhs[1] /= p_rhs;
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] += p_rhs[0];
			p_lhs[1] += p_rhs[1];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] -= p_rhs[0];
			p_lhs[1] -= p_rhs[1];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] *= p_rhs[0];
			p_lhs[1] *= p_rhs[1];
			return p_lhs;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_lhs, T2 const * p_rhs )
		{
			p_lhs[0] /= p_rhs[0];
			p_lhs[1] /= p_rhs[1];
			return p_lhs;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.constPtr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.constPtr() );
		}
	};
}
