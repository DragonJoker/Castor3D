#include "Point.hpp"
#include "Coords.hpp"
#include "Templates.hpp"

#include <cstring>
#include <xmmintrin.h>

namespace Castor
{
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_ptA, T2 const & p_scalar )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value += p_scalar;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_ptA, T2 const & p_scalar )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value -= p_scalar;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_ptA, T2 const & p_scalar )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value *= p_scalar;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_ptA, T2 const & p_scalar )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value /= p_scalar;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_ptA, T2 const * p_buffer )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer += p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_ptA, T2 const * p_buffer )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer -= p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_ptA, T2 const * p_buffer )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer *= p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_ptA, T2 const * p_buffer )
	{
		Point< T1, C1 > l_return( p_ptA );
		T1 * l_pBuffer = l_return.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer /= p_pVal;
			l_pBuffer++;
		} );
		return l_return;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::add( PtType1 const & p_ptA, PtType2 const & p_ptB )
	{
		return add( p_ptA, p_ptB.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::sub( PtType1 const & p_ptA, PtType2 const & p_ptB )
	{
		return sub( p_ptA, p_ptB.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::mul( PtType1 const & p_ptA, PtType2 const & p_ptB )
	{
		return mul( p_ptA, p_ptB.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline Point< T1, C1 > PtOperators< T1, T2, C1, C2 >::div( PtType1 const & p_ptA, PtType2 const & p_ptB )
	{
		return div( p_ptA, p_ptB.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_ptA, T2 const & p_scalar )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value += p_scalar;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_ptA, T2 const & p_scalar )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value -= p_scalar;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_ptA, T2 const & p_scalar )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value *= p_scalar;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_ptA, T2 const & p_scalar )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( l_pBuffer, l_pBuffer + min_value< C1, C2 >::value, [&]( T1 & p_value )
		{
			p_value /= p_scalar;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_ptA, T2 const * p_buffer )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer + p_pVal );
			l_pBuffer++;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_ptA, T2 const * p_buffer )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer - p_pVal );
			l_pBuffer++;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_ptA, T2 const * p_buffer )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer * p_pVal );
			l_pBuffer++;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_ptA, T2 const * p_buffer )
	{
		T1 * l_pBuffer = p_ptA.ptr();
		std::for_each( p_buffer, p_buffer + min_value< C1, C2 >::value, [&]( T2 p_pVal )
		{
			*l_pBuffer = T1( *l_pBuffer / p_pVal );
			l_pBuffer++;
		} );
		return p_ptA;
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::add( PtType1 & p_ptA, PtType2 const & p_ptB )
	{
		return add( p_ptA, p_ptB.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::sub( PtType1 & p_ptA, PtType2 const & p_ptB )
	{
		return sub( p_ptA, p_ptB.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::mul( PtType1 & p_ptA, PtType2 const & p_ptB )
	{
		return mul( p_ptA, p_ptB.const_ptr() );
	}

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	template< typename PtType1, typename PtType2 >
	inline PtType1 & PtAssignOperators< T1, T2, C1, C2 >::div( PtType1 & p_ptA, PtType2 const & p_ptB )
	{
		return div( p_ptA, p_ptB.const_ptr() );
	}

	template<>
	struct PtOperators< float, float, 4, 4 >
	{
#if C3D_USE_POINT_SIMD_OPERATIONS

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_scalar = _mm_set_ps1( p_scalar );			// l_scalar = { p_scalar, p_scalar, p_scalar, p_scalar }
			l_value = _mm_add_ps( l_value, l_scalar );			// l_value = p_ptA + p_scalar
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_scalar = _mm_set_ps1( p_scalar );			// l_scalar = { p_scalar, p_scalar, p_scalar, p_scalar }
			l_value = _mm_sub_ps( l_value, l_scalar );			// l_value = p_ptA - p_scalar
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_scalar = _mm_set_ps1( p_scalar );			// l_scalar = { p_scalar, p_scalar, p_scalar, p_scalar }
			l_value = _mm_mul_ps( l_value, l_scalar );			// l_value = p_ptA * p_scalar
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_scalar = _mm_set_ps1( p_scalar );			// l_scalar = { p_scalar, p_scalar, p_scalar, p_scalar }
			l_value = _mm_div_ps( l_value, l_scalar );			// l_value = p_ptA / p_scalar
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_buffer = _mm_load_ps( p_buffer );			// l_scalar = { p_buffer[0], p_buffer[1], p_buffer[2], p_buffer[3] }
			l_value = _mm_add_ps( l_value, l_buffer );			// l_value = p_ptA + p_buffer
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_buffer = _mm_load_ps( p_buffer );			// l_scalar = { p_buffer[0], p_buffer[1], p_buffer[2], p_buffer[3] }
			l_value = _mm_sub_ps( l_value, l_buffer );			// l_value = p_ptA - p_buffer
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_buffer = _mm_load_ps( p_buffer );			// l_scalar = { p_buffer[0], p_buffer[1], p_buffer[2], p_buffer[3] }
			l_value = _mm_mul_ps( l_value, l_buffer );			// l_value = p_ptA * p_buffer
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			__m128 l_value = _mm_load_ps( p_ptA.const_ptr() );	// l_value = p_ptA
			__m128 l_buffer = _mm_load_ps( p_buffer );			// l_buffer = { p_buffer[0], p_buffer[1], p_buffer[2], p_buffer[3] }
			l_value = _mm_div_ps( l_value, l_buffer );			// l_value = p_ptA / p_buffer
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_value );
			return l_result;
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			__m128 l_valueA = _mm_load_ps( p_ptA.const_ptr() );	// l_valueA = p_ptA
			__m128 l_valueB = _mm_load_ps( p_ptB.const_ptr() );	// l_valueB = p_ptB
			l_valueA = _mm_add_ps( l_valueA, l_valueB );		// l_value = p_ptA + p_ptB
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_valueA );
			return l_result;
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			__m128 l_valueA = _mm_load_ps( p_ptA.const_ptr() );	// l_valueA = p_ptA
			__m128 l_valueB = _mm_load_ps( p_ptB.const_ptr() );	// l_valueB = p_ptB
			l_valueA = _mm_sub_ps( l_valueA, l_valueB );		// l_value = p_ptA - p_ptB
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_valueA );
			return l_result;
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			__m128 l_valueA = _mm_load_ps( p_ptA.const_ptr() );	// l_valueA = p_ptA
			__m128 l_valueB = _mm_load_ps( p_ptB.const_ptr() );	// l_valueB = p_ptB
			l_valueA = _mm_mul_ps( l_valueA, l_valueB );		// l_value = p_ptA * p_ptB
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_valueA );
			return l_result;
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			__m128 l_valueA = _mm_load_ps( p_ptA.const_ptr() );	// l_valueA = p_ptA
			__m128 l_valueB = _mm_load_ps( p_ptB.const_ptr() );	// l_valueB = p_ptB
			l_valueA = _mm_div_ps( l_valueA, l_valueB );		// l_value = p_ptA / p_ptB
			Castor::Point< float, 4 > l_result;
			_mm_store_ps( l_result.ptr(), l_valueA );
			return l_result;
		}

#else

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] += p_scalar;
			l_result[1] += p_scalar;
			l_result[2] += p_scalar;
			l_result[3] += p_scalar;
			return l_result;
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] -= p_scalar;
			l_result[1] -= p_scalar;
			l_result[2] -= p_scalar;
			l_result[3] -= p_scalar;
			return l_result;
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] *= p_scalar;
			l_result[1] *= p_scalar;
			l_result[2] *= p_scalar;
			l_result[3] *= p_scalar;
			return l_result;
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_ptA, float const & p_scalar )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] /= p_scalar;
			l_result[1] /= p_scalar;
			l_result[2] /= p_scalar;
			l_result[3] /= p_scalar;
			return l_result;
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] += p_buffer[0];
			l_result[1] += p_buffer[1];
			l_result[2] += p_buffer[2];
			l_result[3] += p_buffer[3];
			return l_result;
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] -= p_buffer[0];
			l_result[1] -= p_buffer[1];
			l_result[2] -= p_buffer[2];
			l_result[3] -= p_buffer[3];
			return l_result;
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] *= p_buffer[0];
			l_result[1] *= p_buffer[1];
			l_result[2] *= p_buffer[2];
			l_result[3] *= p_buffer[3];
			return l_result;
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_ptA, float const * p_buffer )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] /= p_buffer[0];
			l_result[1] /= p_buffer[1];
			l_result[2] /= p_buffer[2];
			l_result[3] /= p_buffer[3];
			return l_result;
		}

		static inline Castor::Point< float, 4 > add( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] += p_ptB[0];
			l_result[1] += p_ptB[1];
			l_result[2] += p_ptB[2];
			l_result[3] += p_ptB[3];
			return l_result;
		}

		static inline Castor::Point< float, 4 > sub( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] -= p_ptB[0];
			l_result[1] -= p_ptB[1];
			l_result[2] -= p_ptB[2];
			l_result[3] -= p_ptB[3];
			return l_result;
		}

		static inline Castor::Point< float, 4 > mul( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] *= p_ptB[0];
			l_result[1] *= p_ptB[1];
			l_result[2] *= p_ptB[2];
			l_result[3] *= p_ptB[3];
			return l_result;
		}

		static inline Castor::Point< float, 4 > div( Castor::Point< float, 4 > const & p_ptA, Castor::Point< float, 4 > const & p_ptB )
		{
			Castor::Point< float, 4 > l_result( p_ptA );
			l_result[0] /= p_ptB[0];
			l_result[1] /= p_ptB[1];
			l_result[2] /= p_ptB[2];
			l_result[3] /= p_ptB[3];
			return l_result;
		}

#endif

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > add( PtType1 const & p_ptA, float const & p_scalar )
		{
			return Castor::Point< float, 4 >( p_ptA[0] + p_scalar, p_ptA[1] + p_scalar, p_ptA[2] + p_scalar, p_ptA[3] + p_scalar );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > sub( PtType1 const & p_ptA, float const & p_scalar )
		{
			return Castor::Point< float, 4 >( p_ptA[0] - p_scalar, p_ptA[1] - p_scalar, p_ptA[2] - p_scalar, p_ptA[3] - p_scalar );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > mul( PtType1 const & p_ptA, float const & p_scalar )
		{
			return Castor::Point< float, 4 >( p_ptA[0] * p_scalar, p_ptA[1] * p_scalar, p_ptA[2] * p_scalar, p_ptA[3] * p_scalar );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > div( PtType1 const & p_ptA, float const & p_scalar )
		{
			return Castor::Point< float, 4 >( p_ptA[0] / p_scalar, p_ptA[1] / p_scalar, p_ptA[2] / p_scalar, p_ptA[3] / p_scalar );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > add( PtType1 const & p_ptA, float const * p_buffer )
		{
			return Castor::Point< float, 4 >( p_ptA[0] + p_buffer[0], p_ptA[1] + p_buffer[1], p_ptA[2] + p_buffer[2], p_ptA[3] + p_buffer[3] );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > sub( PtType1 const & p_ptA, float const * p_buffer )
		{
			return Castor::Point< float, 4 >( p_ptA[0] - p_buffer[0], p_ptA[1] - p_buffer[1], p_ptA[2] - p_buffer[2], p_ptA[3] - p_buffer[3] );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > mul( PtType1 const & p_ptA, float const * p_buffer )
		{
			return Castor::Point< float, 4 >( p_ptA[0] * p_buffer[0], p_ptA[1] * p_buffer[1], p_ptA[2] * p_buffer[2], p_ptA[3] * p_buffer[3] );
		}

		template< typename PtType1 >
		static inline Castor::Point< float, 4 > div( PtType1 const & p_ptA, float const * p_buffer )
		{
			return Castor::Point< float, 4 >( p_ptA[0] / p_buffer[0], p_ptA[1] / p_buffer[1], p_ptA[2] / p_buffer[2], p_ptA[3] / p_buffer[3] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > add( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return add( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > sub( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return sub( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > mul( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return mul( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Castor::Point< float, 4 > div( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return div( p_ptA, p_ptB.const_ptr() );
		}
	};


	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 4, 4 >
	{
		template< typename PtType1 >
		static inline Point< T1, 4 > add( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 4 >( p_ptA[0] + p_scalar, p_ptA[1] + p_scalar, p_ptA[2] + p_scalar, p_ptA[3] + p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > sub( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 4 >( p_ptA[0] - p_scalar, p_ptA[1] - p_scalar, p_ptA[2] - p_scalar, p_ptA[3] - p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > mul( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 4 >( p_ptA[0] * p_scalar, p_ptA[1] * p_scalar, p_ptA[2] * p_scalar, p_ptA[3] * p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > div( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 4 >( p_ptA[0] / p_scalar, p_ptA[1] / p_scalar, p_ptA[2] / p_scalar, p_ptA[3] / p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > add( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 4 >( p_ptA[0] + p_buffer[0], p_ptA[1] + p_buffer[1], p_ptA[2] + p_buffer[2], p_ptA[3] + p_buffer[3] );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > sub( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 4 >( p_ptA[0] - p_buffer[0], p_ptA[1] - p_buffer[1], p_ptA[2] - p_buffer[2], p_ptA[3] - p_buffer[3] );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > mul( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 4 >( p_ptA[0] * p_buffer[0], p_ptA[1] * p_buffer[1], p_ptA[2] * p_buffer[2], p_ptA[3] * p_buffer[3] );
		}

		template< typename PtType1 >
		static inline Point< T1, 4 > div( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 4 >( p_ptA[0] / p_buffer[0], p_ptA[1] / p_buffer[1], p_ptA[2] / p_buffer[2], p_ptA[3] / p_buffer[3] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > add( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return add( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > sub( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return sub( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > mul( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return mul( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 4 > div( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return div( p_ptA, p_ptB.const_ptr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 3, 3 >
	{
		template< typename PtType1 >
		static inline Point< T1, 3 > add( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 3 >( p_ptA[0] + p_scalar, p_ptA[1] + p_scalar, p_ptA[2] + p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > sub( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 3 >( p_ptA[0] - p_scalar, p_ptA[1] - p_scalar, p_ptA[2] - p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > mul( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 3 >( p_ptA[0] * p_scalar, p_ptA[1] * p_scalar, p_ptA[2] * p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > div( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 3 >( p_ptA[0] / p_scalar, p_ptA[1] / p_scalar, p_ptA[2] / p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > add( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 3 >( p_ptA[0] + p_buffer[0], p_ptA[1] + p_buffer[1], p_ptA[2] + p_buffer[2] );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > sub( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 3 >( p_ptA[0] - p_buffer[0], p_ptA[1] - p_buffer[1], p_ptA[2] - p_buffer[2] );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > mul( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 3 >( p_ptA[0] * p_buffer[0], p_ptA[1] * p_buffer[1], p_ptA[2] * p_buffer[2] );
		}

		template< typename PtType1 >
		static inline Point< T1, 3 > div( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 3 >( p_ptA[0] / p_buffer[0], p_ptA[1] / p_buffer[1], p_ptA[2] / p_buffer[2] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > add( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return add( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > sub( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return sub( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > mul( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return mul( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 3 > div( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return div( p_ptA, p_ptB.const_ptr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtOperators< T1, T2, 2, 2 >
	{
		template< typename PtType1 >
		static inline Point< T1, 2 > add( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 2 >( p_ptA[0] + p_scalar, p_ptA[1] + p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > sub( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 2 >( p_ptA[0] - p_scalar, p_ptA[1] - p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > mul( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 2 >( p_ptA[0] * p_scalar, p_ptA[1] * p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > div( PtType1 const & p_ptA, T2 const & p_scalar )
		{
			return Point< T1, 2 >( p_ptA[0] / p_scalar, p_ptA[1] / p_scalar );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > add( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 2 >( p_ptA[0] + p_buffer[0], p_ptA[1] + p_buffer[1] );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > sub( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 2 >( p_ptA[0] - p_buffer[0], p_ptA[1] - p_buffer[1] );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > mul( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 2 >( p_ptA[0] * p_buffer[0], p_ptA[1] * p_buffer[1] );
		}

		template< typename PtType1 >
		static inline Point< T1, 2 > div( PtType1 const & p_ptA, T2 const * p_buffer )
		{
			return Point< T1, 2 >( p_ptA[0] / p_buffer[0], p_ptA[1] / p_buffer[1] );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > add( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return add( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > sub( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return sub( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > mul( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return mul( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, 2 > div( PtType1 const & p_ptA, PtType2 const & p_ptB )
		{
			return div( p_ptA, p_ptB.const_ptr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 4, 4 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] += p_scalar;
			p_ptA[1] += p_scalar;
			p_ptA[2] += p_scalar;
			p_ptA[3] += p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] -= p_scalar;
			p_ptA[1] -= p_scalar;
			p_ptA[2] -= p_scalar;
			p_ptA[3] -= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] *= p_scalar;
			p_ptA[1] *= p_scalar;
			p_ptA[2] *= p_scalar;
			p_ptA[3] *= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] /= p_scalar;
			p_ptA[1] /= p_scalar;
			p_ptA[2] /= p_scalar;
			p_ptA[3] /= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] += p_buffer[0];
			p_ptA[1] += p_buffer[1];
			p_ptA[2] += p_buffer[2];
			p_ptA[3] += p_buffer[3];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] -= p_buffer[0];
			p_ptA[1] -= p_buffer[1];
			p_ptA[2] -= p_buffer[2];
			p_ptA[3] -= p_buffer[3];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] *= p_buffer[0];
			p_ptA[1] *= p_buffer[1];
			p_ptA[2] *= p_buffer[2];
			p_ptA[3] *= p_buffer[3];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] /= p_buffer[0];
			p_ptA[1] /= p_buffer[1];
			p_ptA[2] /= p_buffer[2];
			p_ptA[3] /= p_buffer[3];
			return p_ptA;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return add( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return sub( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return mul( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return div( p_ptA, p_ptB.const_ptr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 3, 3 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] += p_scalar;
			p_ptA[1] += p_scalar;
			p_ptA[2] += p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] -= p_scalar;
			p_ptA[1] -= p_scalar;
			p_ptA[2] -= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] *= p_scalar;
			p_ptA[1] *= p_scalar;
			p_ptA[2] *= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] /= p_scalar;
			p_ptA[1] /= p_scalar;
			p_ptA[2] /= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] += p_buffer[0];
			p_ptA[1] += p_buffer[1];
			p_ptA[2] += p_buffer[2];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] -= p_buffer[0];
			p_ptA[1] -= p_buffer[1];
			p_ptA[2] -= p_buffer[2];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] *= p_buffer[0];
			p_ptA[1] *= p_buffer[1];
			p_ptA[2] *= p_buffer[2];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] /= p_buffer[0];
			p_ptA[1] /= p_buffer[1];
			p_ptA[2] /= p_buffer[2];
			return p_ptA;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return add( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return sub( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return mul( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return div( p_ptA, p_ptB.const_ptr() );
		}
	};

	template< typename T1, typename T2 >
	struct PtAssignOperators< T1, T2, 2, 2 >
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] += p_scalar;
			p_ptA[1] += p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] -= p_scalar;
			p_ptA[1] -= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] *= p_scalar;
			p_ptA[1] *= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const & p_scalar )
		{
			p_ptA[0] /= p_scalar;
			p_ptA[1] /= p_scalar;
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] += p_buffer[0];
			p_ptA[1] += p_buffer[1];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] -= p_buffer[0];
			p_ptA[1] -= p_buffer[1];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] *= p_buffer[0];
			p_ptA[1] *= p_buffer[1];
			return p_ptA;
		}

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const * p_buffer )
		{
			p_ptA[0] /= p_buffer[0];
			p_ptA[1] /= p_buffer[1];
			return p_ptA;
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return add( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return sub( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return mul( p_ptA, p_ptB.const_ptr() );
		}

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_ptA, PtType2 const & p_ptB )
		{
			return div( p_ptA, p_ptB.const_ptr() );
		}
	};
}
