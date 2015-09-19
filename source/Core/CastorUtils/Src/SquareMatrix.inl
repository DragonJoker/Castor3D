namespace
{
	template< typename Type, uint32_t Count > struct SqrMtxInverter;

	template< typename Type >
	struct SqrMtxInverter< Type, 4 >
	{
		static inline Castor::SquareMatrix< Type, 4 > inverse( Castor::SquareMatrix< Type, 4 > const & p_mtx )
		{
			Type l_tS00 = p_mtx[0][0];
			Type l_tS01 = p_mtx[0][1];
			Type l_tS02 = p_mtx[0][2];
			Type l_tS03 = p_mtx[0][3];
			Type l_tS10 = p_mtx[1][0];
			Type l_tS11 = p_mtx[1][1];
			Type l_tS12 = p_mtx[1][2];
			Type l_tS13 = p_mtx[1][3];
			Type l_tS20 = p_mtx[2][0];
			Type l_tS21 = p_mtx[2][1];
			Type l_tS22 = p_mtx[2][2];
			Type l_tS23 = p_mtx[2][3];
			Type l_tS30 = p_mtx[3][0];
			Type l_tS31 = p_mtx[3][1];
			Type l_tS32 = p_mtx[3][2];
			Type l_tS33 = p_mtx[3][3];
			Type l_tCoef00 = l_tS22 * l_tS33 - l_tS32 * l_tS23;
			Type l_tCoef02 = l_tS12 * l_tS33 - l_tS32 * l_tS13;
			Type l_tCoef03 = l_tS12 * l_tS23 - l_tS22 * l_tS13;
			Type l_tCoef04 = l_tS21 * l_tS33 - l_tS31 * l_tS23;
			Type l_tCoef06 = l_tS11 * l_tS33 - l_tS31 * l_tS13;
			Type l_tCoef07 = l_tS11 * l_tS23 - l_tS21 * l_tS13;
			Type l_tCoef08 = l_tS21 * l_tS32 - l_tS31 * l_tS22;
			Type l_tCoef10 = l_tS11 * l_tS32 - l_tS31 * l_tS12;
			Type l_tCoef11 = l_tS11 * l_tS22 - l_tS21 * l_tS12;
			Type l_tCoef12 = l_tS20 * l_tS33 - l_tS30 * l_tS23;
			Type l_tCoef14 = l_tS10 * l_tS33 - l_tS30 * l_tS13;
			Type l_tCoef15 = l_tS10 * l_tS23 - l_tS20 * l_tS13;
			Type l_tCoef16 = l_tS20 * l_tS32 - l_tS30 * l_tS22;
			Type l_tCoef18 = l_tS10 * l_tS32 - l_tS30 * l_tS12;
			Type l_tCoef19 = l_tS10 * l_tS22 - l_tS20 * l_tS12;
			Type l_tCoef20 = l_tS20 * l_tS31 - l_tS30 * l_tS21;
			Type l_tCoef22 = l_tS10 * l_tS31 - l_tS30 * l_tS11;
			Type l_tCoef23 = l_tS10 * l_tS21 - l_tS20 * l_tS11;
			Castor::SquareMatrix< Type, 4 > l_mtxInverse = Castor::SquareMatrix< Type, 4 >( Castor::mtx_noinit() );
			l_mtxInverse[0][0] = +1 * ( l_tS11 * l_tCoef00 - l_tS12 * l_tCoef04 + l_tS13 * l_tCoef08 );
			l_mtxInverse[0][1] = -1 * ( l_tS01 * l_tCoef00 - l_tS02 * l_tCoef04 + l_tS03 * l_tCoef08 );
			l_mtxInverse[0][2] = +1 * ( l_tS01 * l_tCoef02 - l_tS02 * l_tCoef06 + l_tS03 * l_tCoef10 );
			l_mtxInverse[0][3] = -1 * ( l_tS01 * l_tCoef03 - l_tS02 * l_tCoef07 + l_tS03 * l_tCoef11 );
			l_mtxInverse[1][0] = -1 * ( l_tS10 * l_tCoef00 - l_tS12 * l_tCoef12 + l_tS13 * l_tCoef16 );
			l_mtxInverse[1][1] = +1 * ( l_tS00 * l_tCoef00 - l_tS02 * l_tCoef12 + l_tS03 * l_tCoef16 );
			l_mtxInverse[1][2] = -1 * ( l_tS00 * l_tCoef02 - l_tS02 * l_tCoef14 + l_tS03 * l_tCoef18 );
			l_mtxInverse[1][3] = +1 * ( l_tS00 * l_tCoef03 - l_tS02 * l_tCoef15 + l_tS03 * l_tCoef19 );
			l_mtxInverse[2][0] = +1 * ( l_tS10 * l_tCoef04 - l_tS11 * l_tCoef12 + l_tS13 * l_tCoef20 );
			l_mtxInverse[2][1] = -1 * ( l_tS00 * l_tCoef04 - l_tS01 * l_tCoef12 + l_tS03 * l_tCoef20 );
			l_mtxInverse[2][2] = +1 * ( l_tS00 * l_tCoef06 - l_tS01 * l_tCoef14 + l_tS03 * l_tCoef22 );
			l_mtxInverse[2][3] = -1 * ( l_tS00 * l_tCoef07 - l_tS01 * l_tCoef15 + l_tS03 * l_tCoef23 );
			l_mtxInverse[3][0] = -1 * ( l_tS10 * l_tCoef08 - l_tS11 * l_tCoef16 + l_tS12 * l_tCoef20 );
			l_mtxInverse[3][1] = +1 * ( l_tS00 * l_tCoef08 - l_tS01 * l_tCoef16 + l_tS02 * l_tCoef20 );
			l_mtxInverse[3][2] = -1 * ( l_tS00 * l_tCoef10 - l_tS01 * l_tCoef18 + l_tS02 * l_tCoef22 );
			l_mtxInverse[3][3] = +1 * ( l_tS00 * l_tCoef11 - l_tS01 * l_tCoef19 + l_tS02 * l_tCoef23 );
			Castor::Point< Type, 4 > l_ptRow0( l_mtxInverse[0][0], l_mtxInverse[1][0], l_mtxInverse[2][0], l_mtxInverse[3][0] );
			Type l_tDeterminant = Castor::point::dot( p_mtx[0], l_ptRow0 );
			l_mtxInverse /= l_tDeterminant;
			return l_mtxInverse;
		}
	};

	template< typename Type >
	struct SqrMtxInverter< Type, 3 >
	{
		static inline Castor::SquareMatrix< Type, 3 > inverse( Castor::SquareMatrix< Type, 3 > const & p_mtx )
		{
			Type l_tS00 = p_mtx[0][0];
			Type l_tS01 = p_mtx[0][1];
			Type l_tS02 = p_mtx[0][2];
			Type l_tS10 = p_mtx[1][0];
			Type l_tS11 = p_mtx[1][1];
			Type l_tS12 = p_mtx[1][2];
			Type l_tS20 = p_mtx[2][0];
			Type l_tS21 = p_mtx[2][1];
			Type l_tS22 = p_mtx[2][2];
			Castor::SquareMatrix< Type, 3 > l_mtxInverse = Castor::SquareMatrix< Type, 3 >( Castor::mtx_noinit() );
			l_mtxInverse[0][0] = l_tS11 * l_tS22 - l_tS21 * l_tS12;
			l_mtxInverse[0][1] = l_tS12 * l_tS20 - l_tS22 * l_tS10;
			l_mtxInverse[0][2] = l_tS10 * l_tS21 - l_tS20 * l_tS11;
			l_mtxInverse[1][0] = l_tS02 * l_tS21 - l_tS01 * l_tS22;
			l_mtxInverse[1][1] = l_tS00 * l_tS22 - l_tS02 * l_tS20;
			l_mtxInverse[1][2] = l_tS01 * l_tS20 - l_tS00 * l_tS21;
			l_mtxInverse[2][0] = l_tS12 * l_tS01 - l_tS11 * l_tS02;
			l_mtxInverse[2][1] = l_tS10 * l_tS02 - l_tS12 * l_tS00;
			l_mtxInverse[2][2] = l_tS11 * l_tS00 - l_tS10 * l_tS01;
			Type l_tDeterminant = l_tS00 * ( l_tS11 * l_tS22 - l_tS21 * l_tS12 )
								  - l_tS10 * ( l_tS01 * l_tS22 - l_tS21 * l_tS02 )
								  + l_tS20 * ( l_tS01 * l_tS12 - l_tS11 * l_tS02 );
			l_mtxInverse /= l_tDeterminant;
			return l_mtxInverse;
		}
	};

	template< typename Type >
	struct SqrMtxInverter< Type, 2 >
	{
		static inline Castor::SquareMatrix< Type, 2 > inverse( Castor::SquareMatrix< Type, 2 > const & p_mtx )
		{
			Type l_tDeterminant = p_mtx[0][0] * p_mtx[1][1] - p_mtx[1][0] * p_mtx[0][1];
			Castor::SquareMatrix< Type, 2 > l_mtxInverse = Castor::SquareMatrix< Type, 2 >( Castor::mtx_noinit() );
			l_mtxInverse[1][1] = + p_mtx[1][1] / l_tDeterminant;
			l_mtxInverse[0][1] = - p_mtx[0][1] / l_tDeterminant;
			l_mtxInverse[1][0] = - p_mtx[1][0] / l_tDeterminant;
			l_mtxInverse[0][0] = + p_mtx[0][0] / l_tDeterminant;
			return l_mtxInverse;
		}
	};

	template< typename Type, uint32_t Count > struct SqrMtxOperators;

	template<>
	struct SqrMtxOperators< float, 4 >
	{
		static const uint32_t Size = sizeof( float ) * 4;

		static inline void mul( Castor::SquareMatrix< float, 4 > & p_mtxA, Castor::SquareMatrix< float, 4 > const & p_mtxB )
		{
			Castor::SquareMatrix< float, 4 > l_result = Castor::SquareMatrix< float, 4 >( Castor::mtx_noinit() );
			__m128 l_colA;
			__m128 l_colB;
			__m128 l_colR;

			for (int i = 0; i < 4; ++i )
			{
				l_colA = _mm_load_ps( p_mtxA[0].const_ptr() );					// l_colA = p_mtxA[0]
				l_colB = _mm_set_ps1( p_mtxB[i][0] );							// l_colB = { p_mtxB[i][0], p_mtxB[i][0], p_mtxB[i][0], p_mtxB[i][0] }
				l_colR = _mm_mul_ps( l_colA, l_colB );							// l_colR = l_colA * l_colB
				l_colA = _mm_load_ps( p_mtxA[1].const_ptr() );					// l_colA = p_mtxA[0]
				l_colB = _mm_set_ps1( p_mtxB[i][1] );							// l_colB = { p_mtxB[i][1], p_mtxB[i][1], p_mtxB[i][1], p_mtxB[i][1] }
				l_colR = _mm_add_ps( l_colR, _mm_mul_ps( l_colA, l_colB ) );	// l_colR += l_colA * l_colB
				l_colA = _mm_load_ps( p_mtxA[2].const_ptr() );					// l_colA = p_mtxA[0]
				l_colB = _mm_set_ps1( p_mtxB[i][2] );							// l_colB = { p_mtxB[i][2], p_mtxB[i][2], p_mtxB[i][2], p_mtxB[i][2] }
				l_colR = _mm_add_ps( l_colR, _mm_mul_ps( l_colA, l_colB ) );	// l_colR += l_colA * l_colB
				l_colA = _mm_load_ps( p_mtxA[3].const_ptr() );					// l_colA = p_mtxA[0]
				l_colB = _mm_set_ps1( p_mtxB[i][3] );							// l_colB = { p_mtxB[i][3], p_mtxB[i][3], p_mtxB[i][3], p_mtxB[i][3] }
				l_colR = _mm_add_ps( l_colR, _mm_mul_ps( l_colA, l_colB ) );	// l_colR += l_colA * l_colB
				_mm_store_ps( l_result[i].ptr(), l_colR );
			}

			p_mtxA = l_result;
		}
	};

	template< typename Type >
	struct SqrMtxOperators< Type, 4 >
	{
		static const uint32_t Size = sizeof( Type ) * 4;

		static inline void mul( Castor::SquareMatrix< Type, 4 > & p_mtxA, Castor::SquareMatrix< Type, 4 > const & p_mtxB )
		{
			typedef typename Castor::SquareMatrix< Type, 4 >::row_type row_type;
			Castor::Point< Type, 4 > l_ptSrcA0( p_mtxA[0][0], p_mtxA[0][1], p_mtxA[0][2], p_mtxA[0][3] );
			Castor::Point< Type, 4 > l_ptSrcA1( p_mtxA[1][0], p_mtxA[1][1], p_mtxA[1][2], p_mtxA[1][3] );
			Castor::Point< Type, 4 > l_ptSrcA2( p_mtxA[2][0], p_mtxA[2][1], p_mtxA[2][2], p_mtxA[2][3] );
			Castor::Point< Type, 4 > l_ptSrcA3( p_mtxA[3][0], p_mtxA[3][1], p_mtxA[3][2], p_mtxA[3][3] );
			std::memcpy( p_mtxA[0].ptr(), ( l_ptSrcA0 * p_mtxB[0][0] + l_ptSrcA1 * p_mtxB[0][1] + l_ptSrcA2 * p_mtxB[0][2] + l_ptSrcA3 * p_mtxB[0][3] ).const_ptr(), Size );
			std::memcpy( p_mtxA[1].ptr(), ( l_ptSrcA0 * p_mtxB[1][0] + l_ptSrcA1 * p_mtxB[1][1] + l_ptSrcA2 * p_mtxB[1][2] + l_ptSrcA3 * p_mtxB[1][3] ).const_ptr(), Size );
			std::memcpy( p_mtxA[2].ptr(), ( l_ptSrcA0 * p_mtxB[2][0] + l_ptSrcA1 * p_mtxB[2][1] + l_ptSrcA2 * p_mtxB[2][2] + l_ptSrcA3 * p_mtxB[2][3] ).const_ptr(), Size );
			std::memcpy( p_mtxA[3].ptr(), ( l_ptSrcA0 * p_mtxB[3][0] + l_ptSrcA1 * p_mtxB[3][1] + l_ptSrcA2 * p_mtxB[3][2] + l_ptSrcA3 * p_mtxB[3][3] ).const_ptr(), Size );
		}
	};

	template< typename Type >
	struct SqrMtxOperators< Type, 3 >
	{
		static const uint32_t Size = sizeof( Type ) * 3;

		static inline void mul( Castor::SquareMatrix< Type, 3 > & p_mtxA, Castor::SquareMatrix< Type, 3 > const & p_mtxB )
		{
			typedef typename Castor::SquareMatrix< Type, 3 >::row_type row_type;
			Castor::Point< Type, 3 > l_ptSrcA0( p_mtxA[0][0], p_mtxA[0][1], p_mtxA[0][2] );
			Castor::Point< Type, 3 > l_ptSrcA1( p_mtxA[1][0], p_mtxA[1][1], p_mtxA[1][2] );
			Castor::Point< Type, 3 > l_ptSrcA2( p_mtxA[2][0], p_mtxA[2][1], p_mtxA[2][2] );
			row_type( p_mtxA[0] ) = ( l_ptSrcA0 * p_mtxB[0][0] + l_ptSrcA1 * p_mtxB[0][1] + l_ptSrcA2 * p_mtxB[0][2] );
			row_type( p_mtxA[1] ) = ( l_ptSrcA0 * p_mtxB[1][0] + l_ptSrcA1 * p_mtxB[1][1] + l_ptSrcA2 * p_mtxB[1][2] );
			row_type( p_mtxA[2] ) = ( l_ptSrcA0 * p_mtxB[2][0] + l_ptSrcA1 * p_mtxB[2][1] + l_ptSrcA2 * p_mtxB[2][2] );
		}
	};

	template< typename Type >
	struct SqrMtxOperators< Type, 2 >
	{
		static const uint32_t Size = sizeof( Type ) * 2;

		static inline void mul( Castor::SquareMatrix< Type, 2 > const & p_mtxA, Castor::SquareMatrix< Type, 2 > const & p_mtxB )
		{
			typedef typename Castor::SquareMatrix< Type, 2 >::row_type row_type;
			Castor::Point< Type, 2 > l_ptSrcA0( p_mtxA[0][0], p_mtxA[0][1] );
			Castor::Point< Type, 2 > l_ptSrcA1( p_mtxA[1][0], p_mtxA[1][1] );
			row_type( p_mtxA[0] ) = ( l_ptSrcA0 * p_mtxB[0][0] + l_ptSrcA1 * p_mtxB[0][1] );
			row_type( p_mtxA[1] ) = ( l_ptSrcA0 * p_mtxB[1][0] + l_ptSrcA1 * p_mtxB[1][1] );
		}
	};

	template< typename TypeA, typename TypeB, uint32_t Count >
	struct MtxMultiplicator
	{
		static inline Castor::Point< TypeB, Count > mul( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, Count > const & p_vertex )
		{
			Castor::Point< TypeB, Count > l_return;
			l_return[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[0][1] * p_vertex[1] ) + TypeB( p_matrix[0][2] * p_vertex[2] ) + TypeB( p_matrix[0][3] * p_vertex[3] );
			l_return[1]  = TypeB( p_matrix[1][0] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] ) + TypeB( p_matrix[1][2] * p_vertex[2] ) + TypeB( p_matrix[1][3] * p_vertex[3] );
			l_return[2]  = TypeB( p_matrix[2][0] * p_vertex[0] ) + TypeB( p_matrix[2][1] * p_vertex[1] ) + TypeB( p_matrix[2][2] * p_vertex[2] ) + TypeB( p_matrix[2][3] * p_vertex[3] );
			l_return[3]  = TypeB( p_matrix[3][0] * p_vertex[0] ) + TypeB( p_matrix[3][1] * p_vertex[1] ) + TypeB( p_matrix[3][2] * p_vertex[2] ) + TypeB( p_matrix[3][3] * p_vertex[3] );
			return l_return;
		}
	};

	template< typename TypeA, typename TypeB >
	struct MtxMultiplicator< TypeA, TypeB, 0 >
	{
		static inline Castor::Point< TypeB, 0 > mul( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 0 > const & p_vertex )
		{
			return Castor::Point< TypeB, 0 >();
		}
	};

	template< typename TypeA, typename TypeB >
	struct MtxMultiplicator< TypeA, TypeB, 1 >
	{
		static inline Castor::Point< TypeB, 1 > mul( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 1 > const & p_vertex )
		{
			Castor::Point< TypeB, 1 > l_return;
			l_return[0]  = TypeB( p_matrix[0][0] * p_vertex[0] );
			return l_return;
		}
	};

	template< typename TypeA, typename TypeB >
	struct MtxMultiplicator< TypeA, TypeB, 2 >
	{
		static inline Castor::Point< TypeB, 2 > mul( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 2 > const & p_vertex )
		{
			Castor::Point< TypeB, 2 > l_return;
			l_return[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[0][1] * p_vertex[1] );
			l_return[1]  = TypeB( p_matrix[1][0] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] );
			return l_return;
		}
	};

	template< typename TypeA, typename TypeB >
	struct MtxMultiplicator< TypeA, TypeB, 3 >
	{
		static inline Castor::Point< TypeB, 3 > mul( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 3 > const & p_vertex )
		{
			Castor::Point< TypeB, 3 > l_return;
			l_return[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[0][1] * p_vertex[1] ) + TypeB( p_matrix[0][2] * p_vertex[2] );
			l_return[1]  = TypeB( p_matrix[1][0] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] ) + TypeB( p_matrix[1][2] * p_vertex[2] );
			l_return[2]  = TypeB( p_matrix[2][0] * p_vertex[0] ) + TypeB( p_matrix[2][1] * p_vertex[1] ) + TypeB( p_matrix[2][2] * p_vertex[2] );
			return l_return;
		}
	};

	template< typename T, uint32_t Count > struct CoFactorComputer;

	template< typename T >
	struct CoFactorComputer< T, 1 >
	{
		static inline T get( Castor::SquareMatrix< T, 1 > const & CU_PARAM_UNUSED( p_matrix ), uint32_t CU_PARAM_UNUSED( p_column ), uint32_t CU_PARAM_UNUSED( p_row ) )
		{
			return Castor::Policy< T >::unit();
		}
	};

	template< typename T, uint32_t Count >
	struct CoFactorComputer
	{
		static inline T get( Castor::SquareMatrix< T, Count > const & p_matrix, uint32_t p_column, uint32_t p_row )
		{
			T l_tReturn = T();
			Castor::SquareMatrix < T, Count - 1 > l_mtxTmp = p_matrix.get_minor( p_column, p_row );

			if ( ( p_row + p_column ) % 2 == 0 )
			{
				l_tReturn = l_mtxTmp.get_determinant();
			}
			else
			{
				l_tReturn = -l_mtxTmp.get_determinant();
			}

			return l_tReturn;
		}
	};
}

namespace Castor
{

//*************************************************************************************************

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix()
		: my_matrix_type()
	{
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( mtx_noinit const & p_noinit )
		: my_matrix_type( p_noinit )
	{
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( T const & p_value )
		: my_matrix_type( p_value )
	{
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< T, Count > const & p_matrix )
		: my_matrix_type( p_matrix )
	{
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< T, Count > && p_matrix )
		: my_matrix_type( std::move( p_matrix ) )
	{
	}
	template< typename T, uint32_t Count >
	template< uint32_t _Rows >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< T, _Rows > const & p_matrix )
		: my_matrix_type()
	{
		uint32_t l_count = std::min( Count, _Rows );

		for ( uint32_t i = 0; i < l_count; i++ )
		{
			for ( uint32_t j = 0; j < l_count; j++ )
			{
				this->operator[]( i )[j] = p_matrix[i][j];
			}
		}

		for ( int i = l_count; i < Count; i++ )
		{
			this->operator[]( i )[i] = Castor::Policy< T >::unit();
		}
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< Type, Count > const & p_matrix )
		: my_matrix_type( p_matrix )
	{
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count >::SquareMatrix( Matrix< Type, Count, Count > const & p_matrix )
		: my_matrix_type( p_matrix )
	{
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count >::SquareMatrix( Type const * p_matrix )
		: my_matrix_type( p_matrix )
	{
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::~SquareMatrix()
	{
	}
	template< typename T, uint32_t Count >
	inline T SquareMatrix< T, Count >::get_determinant()const
	{
		T l_tReturn( Castor::Policy< T >::zero() );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			l_tReturn += this->operator[]( 0 )[i] * get_cofactor( i, 0 );
		}

		return l_tReturn;
	}
	template< typename T, uint32_t Count >
	inline bool SquareMatrix< T, Count >::is_orthogonal()const
	{
		bool l_bReturn = false;
		my_matrix_type l_mTmp( *this );
		SquareMatrix< T, Count > l_mId1;
		SquareMatrix< T, Count > l_mId2;
		l_mTmp.transpose();
		l_mId1.set_identity();
		l_mId2.set_identity();
		return ( l_mId1 == *this * l_mTmp ) && ( l_mId2 == l_mTmp * ( *this ) );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::transpose()
	{
		*this = my_matrix_type::get_transposed();
		return *this;
	}
	template< typename T, uint32_t Count >
	inline bool SquareMatrix< T, Count >::is_symmetrical()const
	{
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Count && l_bReturn; i++ )
		{
			for ( uint32_t j = 0; j < Count && l_bReturn; j++ )
			{
				l_bReturn = Castor::Policy< T >::equals( m_columns[i][j], m_columns[j][i] );
			}
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Count >
	inline bool SquareMatrix< T, Count >::is_anti_symmetrical()const
	{
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Count && l_bReturn; i++ )
		{
			for ( uint32_t j = 0; j < Count && l_bReturn; j++ )
			{
				if ( !Castor::Policy< T >::is_null( m_columns[i][j] + m_columns[j][i] ) )
				{
					l_bReturn = false;
				}
			}
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > SquareMatrix< T, Count >::get_inverse()const
	{
		return SqrMtxInverter< T, Count >::inverse( *this );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix < T, Count - 1 > SquareMatrix< T, Count >::get_minor( uint32_t x, uint32_t y )const
	{
		SquareMatrix < T, Count - 1 > l_return;
		l_return.set_identity();
		uint32_t l_i = 0, l_j = 0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			if ( i != x )
			{
				l_j = 0;

				for ( uint32_t j = 0; j < Count; j++ )
				{
					if ( j != y )
					{
						l_return[l_i][l_j++] = m_columns[i][j];
					}
				}

				l_i++;
			}
		}

		return l_return;
	}
	template< typename T, uint32_t Count >
	inline T SquareMatrix< T, Count >::get_cofactor( uint32_t p_column, uint32_t p_row )const
	{
		return CoFactorComputer< T, Count >::get( *this, p_column, p_row );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::invert()
	{
		T l_tDeterminant = get_determinant();

		if ( !Castor::Policy< T >::is_null( l_tDeterminant ) )
		{
			SquareMatrix< T, Count > l_mTmp;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				for ( uint32_t j = 0; j < Count; j++ )
				{
					l_mTmp[i][j] = get_cofactor( i, j ) / l_tDeterminant;
				}
			}

			std::memcpy( my_matrix_type::ptr(), l_mTmp.const_ptr(), my_matrix_type::size );
		}

		return *this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( Matrix< Type, Count, Count > const & p_matrix )
	{
		my_matrix_type::operator =( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( SquareMatrix< T, Count > const & p_matrix )
	{
		my_matrix_type::operator =( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( SquareMatrix< T, Count > && p_matrix )
	{
		my_matrix_type::operator =( std::move( p_matrix ) );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( SquareMatrix< Type, Count > const & p_matrix )
	{
		my_matrix_type::operator =( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator+=( SquareMatrix< Type, Count > const & p_matrix )
	{
		my_matrix_type::operator +=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator-=( SquareMatrix< Type, Count > const & p_matrix )
	{
		my_matrix_type::operator -=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator*=( SquareMatrix< Type, Count > const & p_matrix )
	{
		SqrMtxOperators< T, Count >::mul( *this, p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( Type const * p_matrix )
	{
		my_matrix_type::operator=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator+=( Type const * p_matrix )
	{
		my_matrix_type::operator+=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator-=( Type const * p_matrix )
	{
		my_matrix_type::operator-=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator*=( Type const * p_matrix )
	{
		my_matrix_type::operator*=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator+=( T const & p_value )
	{
		my_matrix_type::operator+=( p_value );
		return * this;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator-=( T const & p_value )
	{
		my_matrix_type::operator-=( p_value );
		return * this;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator*=( T const & p_value )
	{
		my_matrix_type::operator*=( p_value );
		return * this;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator/=( T const & p_value )
	{
		my_matrix_type::operator/=( p_value );
		return * this;
	}

//*************************************************************************************************

	template< typename T, uint32_t Count >
	inline bool operator==( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< T, Count > const & p_mtxB )
	{
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Count && l_bReturn; i++ )
		{
			for ( uint32_t j = 0; j < Count && l_bReturn; j++ )
			{
				l_bReturn = Castor::Policy< T >::equals( p_mtxA[i][j], p_mtxB[i][j] );
			}
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Count >
	inline bool operator!=( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< T, Count > const & p_mtxB )
	{
		return ! operator ==( p_mtxA, p_mtxB );
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB )
	{
		SquareMatrix< T, Count > l_return( p_mtxA );
		l_return += p_mtxB;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB )
	{
		SquareMatrix< T, Count > l_return( p_mtxA );
		l_return -= p_mtxB;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB )
	{
		SquareMatrix< T, Count > l_return( p_mtxA );
		l_return *= p_mtxB;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_matrix, U const * p_values )
	{
		SquareMatrix< T, Count > l_return( p_matrix );
		l_return += p_values;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_matrix, U const * p_values )
	{
		SquareMatrix< T, Count > l_return( p_matrix );
		l_return -= p_values;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_matrix, U const * p_values )
	{
		SquareMatrix< T, Count > l_return( p_matrix );
		l_return *= p_values;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > l_return( p_matrix );
		l_return += p_value;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > l_return( p_matrix );
		l_return -= p_value;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > l_return( p_matrix );
		l_return *= p_value;
		return l_return;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator/( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > l_return( p_matrix );
		l_return /= p_value;
		return l_return;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator+( T p_value, SquareMatrix< T, Count > const & p_matrix )
	{
		return operator +( p_matrix, p_value );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator-( T p_value, SquareMatrix< T, Count > const & p_matrix )
	{
		return operator -( p_matrix, p_value );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator*( T p_value, SquareMatrix< T, Count > const & p_matrix )
	{
		return operator *( p_matrix, p_value );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_matrix )
	{
		SquareMatrix< T, Count > l_return;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			for ( uint32_t j = 0; j < Count; j++ )
			{
				l_return[i][j] = -p_matrix[i][j];
			}
		}

		return l_return;
	}

//*************************************************************************************************
}
