#include "Point.hpp"
#include "Coords.hpp"
#include "Design/Templates.hpp"

#include <cstring>
#include <xmmintrin.h>

namespace Castor
{
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value += p_rhs;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value -= p_rhs;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value *= p_rhs;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_lhs, T2 const & p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value /= p_rhs;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer += p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer -= p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer *= p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_lhs, T2 const * p_rhs )
	{
		Point< T1, C1 > l_return( p_lhs );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer /= p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return add( p_lhs, p_rhs.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return sub( p_lhs, p_rhs.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return mul( p_lhs, p_rhs.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_lhs, PtType2 const & p_rhs )
	{
		return div( p_lhs, p_rhs.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value += p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value -= p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value *= p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_lhs, T2 const & p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value /= p_rhs;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer + p_pVal );
			l_pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer - p_pVal );
			l_pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer * p_pVal );
			l_pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_lhs, T2 const * p_rhs )
	{
		T1 * l_pBuffer = p_lhs.ptr();
		std::for_each( p_rhs, p_rhs + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer / p_pVal );
			l_pBuffer++;
		} );
		return p_lhs;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return add( p_lhs, p_rhs.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return sub( p_lhs, p_rhs.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return mul( p_lhs, p_rhs.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_lhs, PtType2 const & p_rhs )
	{
		return div( p_lhs, p_rhs.const_ptr() );
	}

	template<>
	struct PtOperators< float, float, 4, 4 >
	{
		static inline Castor::Point< float, 4 > add_sse2( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_scalar = _mm_set_ps1( p_rhs );				// l_scalar = { p_rhs, p_rhs, p_rhs, p_rhs }
			l_value = _mm_add_ps( l_value, l_scalar );			// l_value = p_lhs + p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > sub_sse2( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_scalar = _mm_set_ps1( p_rhs );				// l_scalar = { p_rhs, p_rhs, p_rhs, p_rhs }
			l_value = _mm_sub_ps( l_value, l_scalar );			// l_value = p_lhs - p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > mul_sse2( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_scalar = _mm_set_ps1( p_rhs );				// l_scalar = { p_rhs, p_rhs, p_rhs, p_rhs }
			l_value = _mm_mul_ps( l_value, l_scalar );			// l_value = p_lhs * p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > div_sse2( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_scalar = _mm_set_ps1( p_rhs );				// l_scalar = { p_rhs, p_rhs, p_rhs, p_rhs }
			l_value = _mm_div_ps( l_value, l_scalar );			// l_value = p_lhs / p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > add_sse2( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_buffer = _mm_load_ps( p_rhs );				// l_scalar = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			l_value = _mm_add_ps( l_value, l_buffer );			// l_value = p_lhs + p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > sub_sse2( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_buffer = _mm_load_ps( p_rhs );				// l_scalar = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			l_value = _mm_sub_ps( l_value, l_buffer );			// l_value = p_lhs - p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > mul_sse2( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_buffer = _mm_load_ps( p_rhs );				// l_scalar = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			l_value = _mm_mul_ps( l_value, l_buffer );			// l_value = p_lhs * p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > div_sse2( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			__m128 l_value = _mm_load_ps( p_lhs.const_ptr() );	// l_value = p_lhs
			__m128 l_buffer = _mm_load_ps( p_rhs );				// l_buffer = { p_rhs[0], p_rhs[1], p_rhs[2], p_rhs[3] }
			l_value = _mm_div_ps( l_value, l_buffer );			// l_value = p_lhs / p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_value );
			return l_res;
		}

		static inline Castor::Point< float, 4 > add_sse2( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			__m128 l_valueA = _mm_load_ps( p_lhs.const_ptr() );	// l_valueA = p_lhs
			__m128 l_valueB = _mm_load_ps( p_rhs.const_ptr() );	// l_valueB = p_rhs
			l_valueA = _mm_add_ps( l_valueA, l_valueB );		// l_valueA = p_lhs + p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_valueA );
			return l_res;
		}

		static inline Castor::Point< float, 4 > sub_sse2( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			__m128 l_valueA = _mm_load_ps( p_lhs.const_ptr() );	// l_valueA = p_lhs
			__m128 l_valueB = _mm_load_ps( p_rhs.const_ptr() );	// l_valueB = p_rhs
			l_valueA = _mm_sub_ps( l_valueA, l_valueB );		// l_valueA = p_lhs - p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_valueA );
			return l_res;
		}

		static inline Castor::Point< float, 4 > mul_sse2( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			__m128 l_valueA = _mm_load_ps( p_lhs.const_ptr() );	// l_valueA = p_lhs
			__m128 l_valueB = _mm_load_ps( p_rhs.const_ptr() );	// l_valueB = p_rhs
			l_valueA = _mm_mul_ps( l_valueA, l_valueB );		// l_valueA = p_lhs * p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_valueA );
			return l_res;
		}

		static inline Castor::Point< float, 4 > div_sse2( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			__m128 l_valueA = _mm_load_ps( p_lhs.const_ptr() );	// l_valueA = p_lhs
			__m128 l_valueB = _mm_load_ps( p_rhs.const_ptr() );	// l_valueB = p_rhs
			l_valueA = _mm_div_ps( l_valueA, l_valueB );		// l_valueA = p_lhs / p_rhs
			Castor::Point< float, 4 > l_res;
			_mm_store_ps( l_res.ptr(), l_valueA );
			return l_res;
		}

		static inline Point4f add_cpp( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] += p_rhs;
			l_result[1] += p_rhs;
			l_result[2] += p_rhs;
			l_result[3] += p_rhs;
			return l_result;
		}

		static inline Point4f sub_cpp( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] -= p_rhs;
			l_result[1] -= p_rhs;
			l_result[2] -= p_rhs;
			l_result[3] -= p_rhs;
			return l_result;
		}

		static inline Point4f mul_cpp( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] *= p_rhs;
			l_result[1] *= p_rhs;
			l_result[2] *= p_rhs;
			l_result[3] *= p_rhs;
			return l_result;
		}

		static inline Point4f div_cpp( Point4f const & p_lhs, float const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] /= p_rhs;
			l_result[1] /= p_rhs;
			l_result[2] /= p_rhs;
			l_result[3] /= p_rhs;
			return l_result;
		}

		static inline Point4f add_cpp( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] += p_rhs[0];
			l_result[1] += p_rhs[1];
			l_result[2] += p_rhs[2];
			l_result[3] += p_rhs[3];
			return l_result;
		}

		static inline Point4f sub_cpp( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] -= p_rhs[0];
			l_result[1] -= p_rhs[1];
			l_result[2] -= p_rhs[2];
			l_result[3] -= p_rhs[3];
			return l_result;
		}

		static inline Point4f mul_cpp( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] *= p_rhs[0];
			l_result[1] *= p_rhs[1];
			l_result[2] *= p_rhs[2];
			l_result[3] *= p_rhs[3];
			return l_result;
		}

		static inline Point4f div_cpp( Point4f const & p_lhs, float const * p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] /= p_rhs[0];
			l_result[1] /= p_rhs[1];
			l_result[2] /= p_rhs[2];
			l_result[3] /= p_rhs[3];
			return l_result;
		}

		static inline Point4f add_cpp( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] += p_rhs[0];
			l_result[1] += p_rhs[1];
			l_result[2] += p_rhs[2];
			l_result[3] += p_rhs[3];
			return l_result;
		}

		static inline Point4f sub_cpp( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] -= p_rhs[0];
			l_result[1] -= p_rhs[1];
			l_result[2] -= p_rhs[2];
			l_result[3] -= p_rhs[3];
			return l_result;
		}

		static inline Point4f mul_cpp( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] *= p_rhs[0];
			l_result[1] *= p_rhs[1];
			l_result[2] *= p_rhs[2];
			l_result[3] *= p_rhs[3];
			return l_result;
		}

		static inline Point4f div_cpp( Point4f const & p_lhs, Point4f const & p_rhs )
		{
			Point4f l_result( p_lhs );
			l_result[0] /= p_rhs[0];
			l_result[1] /= p_rhs[1];
			l_result[2] /= p_rhs[2];
			l_result[3] /= p_rhs[3];
			return l_result;
		}

#if CASTOR_USE_SSE2

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return add_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return sub_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return mul_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return div_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return add_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return sub_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return mul_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return div_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return add_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return sub_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return mul_sse2( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return div_sse2( p_lhs, p_rhs );
		}

#else

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return add_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return sub_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return mul_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_lhs, float const & p_rhs )
		{
			return div_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return add_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return sub_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return mul_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_lhs, float const * p_rhs )
		{
			return div_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return add_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return sub_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return mul_cpp( p_lhs, p_rhs );
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_lhs, Castor::Point< float, 4 > const & p_rhs )
		{
			return div_cpp( p_lhs, p_rhs );
		}

#endif

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > add( PtType1 const & p_lhs, float const & p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] + p_rhs, p_lhs[1] + p_rhs, p_lhs[2] + p_rhs, p_lhs[3] + p_rhs );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > sub( PtType1 const & p_lhs, float const & p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] - p_rhs, p_lhs[1] - p_rhs, p_lhs[2] - p_rhs, p_lhs[3] - p_rhs );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > mul( PtType1 const & p_lhs, float const & p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] * p_rhs, p_lhs[1] * p_rhs, p_lhs[2] * p_rhs, p_lhs[3] * p_rhs );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > div( PtType1 const & p_lhs, float const & p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] / p_rhs, p_lhs[1] / p_rhs, p_lhs[2] / p_rhs, p_lhs[3] / p_rhs );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > add( PtType1 const & p_lhs, float const * p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] + p_rhs[0], p_lhs[1] + p_rhs[1], p_lhs[2] + p_rhs[2], p_lhs[3] + p_rhs[3] );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > sub( PtType1 const & p_lhs, float const * p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] - p_rhs[0], p_lhs[1] - p_rhs[1], p_lhs[2] - p_rhs[2], p_lhs[3] - p_rhs[3] );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > mul( PtType1 const & p_lhs, float const * p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] * p_rhs[0], p_lhs[1] * p_rhs[1], p_lhs[2] * p_rhs[2], p_lhs[3] * p_rhs[3] );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > div( PtType1 const & p_lhs, float const * p_rhs )
		{
			return Castor::Point< float, 4 >( p_lhs[0] / p_rhs[0], p_lhs[1] / p_rhs[1], p_lhs[2] / p_rhs[2], p_lhs[3] / p_rhs[3] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > add( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return add( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.const_ptr() );
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
			return add( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.const_ptr() );
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
			return add( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.const_ptr() );
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
			return add( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > sub( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > mul( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > div( PtType1 const & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.const_ptr() );
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
			return add( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.const_ptr() );
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
			return add( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.const_ptr() );
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
			return add( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return sub( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return mul( p_lhs, p_rhs.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_lhs, PtType2 const & p_rhs )
		{
			return div( p_lhs, p_rhs.const_ptr() );
		}
	};
}
