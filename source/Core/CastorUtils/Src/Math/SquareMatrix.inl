#include "Simd.hpp"

namespace castor
{
	namespace
	{
		template< typename Type, uint32_t Count > struct SqrMtxInverter;

		template< typename Type >
		struct SqrMtxInverter< Type, 4 >
		{
			static inline castor::SquareMatrix< Type, 4 > inverse( castor::SquareMatrix< Type, 4 > const & p_mtx )
			{
				Type tS00 = p_mtx[0][0];
				Type tS01 = p_mtx[0][1];
				Type tS02 = p_mtx[0][2];
				Type tS03 = p_mtx[0][3];
				Type tS10 = p_mtx[1][0];
				Type tS11 = p_mtx[1][1];
				Type tS12 = p_mtx[1][2];
				Type tS13 = p_mtx[1][3];
				Type tS20 = p_mtx[2][0];
				Type tS21 = p_mtx[2][1];
				Type tS22 = p_mtx[2][2];
				Type tS23 = p_mtx[2][3];
				Type tS30 = p_mtx[3][0];
				Type tS31 = p_mtx[3][1];
				Type tS32 = p_mtx[3][2];
				Type tS33 = p_mtx[3][3];
				Type tCoef00 = tS22 * tS33 - tS32 * tS23;
				Type tCoef02 = tS12 * tS33 - tS32 * tS13;
				Type tCoef03 = tS12 * tS23 - tS22 * tS13;
				Type tCoef04 = tS21 * tS33 - tS31 * tS23;
				Type tCoef06 = tS11 * tS33 - tS31 * tS13;
				Type tCoef07 = tS11 * tS23 - tS21 * tS13;
				Type tCoef08 = tS21 * tS32 - tS31 * tS22;
				Type tCoef10 = tS11 * tS32 - tS31 * tS12;
				Type tCoef11 = tS11 * tS22 - tS21 * tS12;
				Type tCoef12 = tS20 * tS33 - tS30 * tS23;
				Type tCoef14 = tS10 * tS33 - tS30 * tS13;
				Type tCoef15 = tS10 * tS23 - tS20 * tS13;
				Type tCoef16 = tS20 * tS32 - tS30 * tS22;
				Type tCoef18 = tS10 * tS32 - tS30 * tS12;
				Type tCoef19 = tS10 * tS22 - tS20 * tS12;
				Type tCoef20 = tS20 * tS31 - tS30 * tS21;
				Type tCoef22 = tS10 * tS31 - tS30 * tS11;
				Type tCoef23 = tS10 * tS21 - tS20 * tS11;
				castor::SquareMatrix< Type, 4 > mtxInverse = castor::SquareMatrix< Type, 4 >( castor::NoInit() );
				mtxInverse[0][0] = +1 * ( tS11 * tCoef00 - tS12 * tCoef04 + tS13 * tCoef08 );
				mtxInverse[0][1] = -1 * ( tS01 * tCoef00 - tS02 * tCoef04 + tS03 * tCoef08 );
				mtxInverse[0][2] = +1 * ( tS01 * tCoef02 - tS02 * tCoef06 + tS03 * tCoef10 );
				mtxInverse[0][3] = -1 * ( tS01 * tCoef03 - tS02 * tCoef07 + tS03 * tCoef11 );
				mtxInverse[1][0] = -1 * ( tS10 * tCoef00 - tS12 * tCoef12 + tS13 * tCoef16 );
				mtxInverse[1][1] = +1 * ( tS00 * tCoef00 - tS02 * tCoef12 + tS03 * tCoef16 );
				mtxInverse[1][2] = -1 * ( tS00 * tCoef02 - tS02 * tCoef14 + tS03 * tCoef18 );
				mtxInverse[1][3] = +1 * ( tS00 * tCoef03 - tS02 * tCoef15 + tS03 * tCoef19 );
				mtxInverse[2][0] = +1 * ( tS10 * tCoef04 - tS11 * tCoef12 + tS13 * tCoef20 );
				mtxInverse[2][1] = -1 * ( tS00 * tCoef04 - tS01 * tCoef12 + tS03 * tCoef20 );
				mtxInverse[2][2] = +1 * ( tS00 * tCoef06 - tS01 * tCoef14 + tS03 * tCoef22 );
				mtxInverse[2][3] = -1 * ( tS00 * tCoef07 - tS01 * tCoef15 + tS03 * tCoef23 );
				mtxInverse[3][0] = -1 * ( tS10 * tCoef08 - tS11 * tCoef16 + tS12 * tCoef20 );
				mtxInverse[3][1] = +1 * ( tS00 * tCoef08 - tS01 * tCoef16 + tS02 * tCoef20 );
				mtxInverse[3][2] = -1 * ( tS00 * tCoef10 - tS01 * tCoef18 + tS02 * tCoef22 );
				mtxInverse[3][3] = +1 * ( tS00 * tCoef11 - tS01 * tCoef19 + tS02 * tCoef23 );
				castor::Point< Type, 4 > ptRow0( mtxInverse[0][0], mtxInverse[1][0], mtxInverse[2][0], mtxInverse[3][0] );
				Type tDeterminant = castor::point::dot( p_mtx[0], ptRow0 );
				mtxInverse /= tDeterminant;
				return mtxInverse;
			}
		};

		template< typename Type >
		struct SqrMtxInverter< Type, 3 >
		{
			static inline castor::SquareMatrix< Type, 3 > inverse( castor::SquareMatrix< Type, 3 > const & p_mtx )
			{
				Type tS00 = p_mtx[0][0];
				Type tS01 = p_mtx[0][1];
				Type tS02 = p_mtx[0][2];
				Type tS10 = p_mtx[1][0];
				Type tS11 = p_mtx[1][1];
				Type tS12 = p_mtx[1][2];
				Type tS20 = p_mtx[2][0];
				Type tS21 = p_mtx[2][1];
				Type tS22 = p_mtx[2][2];
				castor::SquareMatrix< Type, 3 > mtxInverse = castor::SquareMatrix< Type, 3 >( castor::NoInit() );
				mtxInverse[0][0] = tS11 * tS22 - tS21 * tS12;
				mtxInverse[0][1] = tS12 * tS20 - tS22 * tS10;
				mtxInverse[0][2] = tS10 * tS21 - tS20 * tS11;
				mtxInverse[1][0] = tS02 * tS21 - tS01 * tS22;
				mtxInverse[1][1] = tS00 * tS22 - tS02 * tS20;
				mtxInverse[1][2] = tS01 * tS20 - tS00 * tS21;
				mtxInverse[2][0] = tS12 * tS01 - tS11 * tS02;
				mtxInverse[2][1] = tS10 * tS02 - tS12 * tS00;
				mtxInverse[2][2] = tS11 * tS00 - tS10 * tS01;
				Type tDeterminant = tS00 * ( tS11 * tS22 - tS21 * tS12 )
									  - tS10 * ( tS01 * tS22 - tS21 * tS02 )
									  + tS20 * ( tS01 * tS12 - tS11 * tS02 );
				mtxInverse /= tDeterminant;
				return mtxInverse;
			}
		};

		template< typename Type >
		struct SqrMtxInverter< Type, 2 >
		{
			static inline castor::SquareMatrix< Type, 2 > inverse( castor::SquareMatrix< Type, 2 > const & p_mtx )
			{
				Type tDeterminant = p_mtx[0][0] * p_mtx[1][1] - p_mtx[1][0] * p_mtx[0][1];
				castor::SquareMatrix< Type, 2 > mtxInverse = castor::SquareMatrix< Type, 2 >( castor::NoInit() );
				mtxInverse[1][1] = + p_mtx[1][1] / tDeterminant;
				mtxInverse[0][1] = - p_mtx[0][1] / tDeterminant;
				mtxInverse[1][0] = - p_mtx[1][0] / tDeterminant;
				mtxInverse[0][0] = + p_mtx[0][0] / tDeterminant;
				return mtxInverse;
			}
		};
		template< typename Type, uint32_t Count > struct SqrMtxOperators;

		template<>
		struct SqrMtxOperators< float, 4 >
		{
			static const uint32_t Size = sizeof( float ) * 4;

#if CASTOR_USE_SSE2

			static inline void mul( castor::SquareMatrix< float, 4 > & p_lhs, castor::SquareMatrix< float, 4 > const & p_rhs )
			{
				Matrix4x4f result = Matrix4x4f( NoInit() );
				Float4 col0{ p_lhs[0].ptr() };		// col0 = p_lhs[0]
				Float4 col1{ p_lhs[1].ptr() };		// col1 = p_lhs[1]
				Float4 col2{ p_lhs[2].ptr() };		// col2 = p_lhs[2]
				Float4 col3{ p_lhs[3].ptr() };		// col3 = p_lhs[3]

				for ( int i = 0; i < 4; ++i )
				{
					Float4 colB{ p_rhs[i][0] };		// colB = { p_rhs[i][0], p_rhs[i][0], p_rhs[i][0], p_rhs[i][0] }
					Float4 colR{ col0 * colB };	// colR = col0 * colB
					colB = Float4( p_rhs[i][1] );		// colB = { p_rhs[i][1], p_rhs[i][1], p_rhs[i][1], p_rhs[i][1] }
					colR += col1 * colB;			// colR += col1 * colB
					colB = Float4( p_rhs[i][2] );		// colB = { p_rhs[i][2], p_rhs[i][2], p_rhs[i][2], p_rhs[i][2] }
					colR += col2 * colB;			// colR += col2 * colB
					colB = Float4( p_rhs[i][3] );		// colB = { p_rhs[i][3], p_rhs[i][3], p_rhs[i][3], p_rhs[i][3] }
					colR += col3 * colB;			// colR += col3 * colB
					colR.toPtr( result[i].ptr() );
				}

				p_lhs = result;
			}

#else

			static inline void mul( castor::SquareMatrix< float, 4 > & p_lhs, castor::SquareMatrix< float, 4 > const & p_rhs )
			{
				typedef typename Matrix4x4f::row_type row_type;
				Point4f ptSrcA0( p_lhs[0][0], p_lhs[0][1], p_lhs[0][2], p_lhs[0][3] );
				Point4f ptSrcA1( p_lhs[1][0], p_lhs[1][1], p_lhs[1][2], p_lhs[1][3] );
				Point4f ptSrcA2( p_lhs[2][0], p_lhs[2][1], p_lhs[2][2], p_lhs[2][3] );
				Point4f ptSrcA3( p_lhs[3][0], p_lhs[3][1], p_lhs[3][2], p_lhs[3][3] );
				std::memcpy( p_lhs[0].ptr(), ( ptSrcA0 * p_rhs[0][0] + ptSrcA1 * p_rhs[0][1] + ptSrcA2 * p_rhs[0][2] + ptSrcA3 * p_rhs[0][3] ).constPtr(), Size );
				std::memcpy( p_lhs[1].ptr(), ( ptSrcA0 * p_rhs[1][0] + ptSrcA1 * p_rhs[1][1] + ptSrcA2 * p_rhs[1][2] + ptSrcA3 * p_rhs[1][3] ).constPtr(), Size );
				std::memcpy( p_lhs[2].ptr(), ( ptSrcA0 * p_rhs[2][0] + ptSrcA1 * p_rhs[2][1] + ptSrcA2 * p_rhs[2][2] + ptSrcA3 * p_rhs[2][3] ).constPtr(), Size );
				std::memcpy( p_lhs[3].ptr(), ( ptSrcA0 * p_rhs[3][0] + ptSrcA1 * p_rhs[3][1] + ptSrcA2 * p_rhs[3][2] + ptSrcA3 * p_rhs[3][3] ).constPtr(), Size );
			}

#endif
		};

		template< typename Type >
		struct SqrMtxOperators< Type, 4 >
		{
			static const uint32_t Size = sizeof( Type ) * 4;

			static inline void mul( castor::SquareMatrix< Type, 4 > & p_mtxA, castor::SquareMatrix< Type, 4 > const & p_mtxB )
			{
				typedef typename castor::SquareMatrix< Type, 4 >::row_type row_type;
				castor::Point< Type, 4 > ptSrcA0( p_mtxA[0][0], p_mtxA[0][1], p_mtxA[0][2], p_mtxA[0][3] );
				castor::Point< Type, 4 > ptSrcA1( p_mtxA[1][0], p_mtxA[1][1], p_mtxA[1][2], p_mtxA[1][3] );
				castor::Point< Type, 4 > ptSrcA2( p_mtxA[2][0], p_mtxA[2][1], p_mtxA[2][2], p_mtxA[2][3] );
				castor::Point< Type, 4 > ptSrcA3( p_mtxA[3][0], p_mtxA[3][1], p_mtxA[3][2], p_mtxA[3][3] );
				std::memcpy( p_mtxA[0].ptr(), ( ptSrcA0 * p_mtxB[0][0] + ptSrcA1 * p_mtxB[0][1] + ptSrcA2 * p_mtxB[0][2] + ptSrcA3 * p_mtxB[0][3] ).constPtr(), Size );
				std::memcpy( p_mtxA[1].ptr(), ( ptSrcA0 * p_mtxB[1][0] + ptSrcA1 * p_mtxB[1][1] + ptSrcA2 * p_mtxB[1][2] + ptSrcA3 * p_mtxB[1][3] ).constPtr(), Size );
				std::memcpy( p_mtxA[2].ptr(), ( ptSrcA0 * p_mtxB[2][0] + ptSrcA1 * p_mtxB[2][1] + ptSrcA2 * p_mtxB[2][2] + ptSrcA3 * p_mtxB[2][3] ).constPtr(), Size );
				std::memcpy( p_mtxA[3].ptr(), ( ptSrcA0 * p_mtxB[3][0] + ptSrcA1 * p_mtxB[3][1] + ptSrcA2 * p_mtxB[3][2] + ptSrcA3 * p_mtxB[3][3] ).constPtr(), Size );
			}
		};

		template< typename Type >
		struct SqrMtxOperators< Type, 3 >
		{
			static const uint32_t Size = sizeof( Type ) * 3;

			static inline void mul( castor::SquareMatrix< Type, 3 > & p_mtxA, castor::SquareMatrix< Type, 3 > const & p_mtxB )
			{
				typedef typename castor::SquareMatrix< Type, 3 >::row_type row_type;
				castor::Point< Type, 3 > ptSrcA0( p_mtxA[0][0], p_mtxA[0][1], p_mtxA[0][2] );
				castor::Point< Type, 3 > ptSrcA1( p_mtxA[1][0], p_mtxA[1][1], p_mtxA[1][2] );
				castor::Point< Type, 3 > ptSrcA2( p_mtxA[2][0], p_mtxA[2][1], p_mtxA[2][2] );
				std::memcpy( p_mtxA[0].ptr(), ( ptSrcA0 * p_mtxB[0][0] + ptSrcA1 * p_mtxB[0][1] + ptSrcA2 * p_mtxB[0][2] ), Size );
				std::memcpy( p_mtxA[1].ptr(), ( ptSrcA0 * p_mtxB[1][0] + ptSrcA1 * p_mtxB[1][1] + ptSrcA2 * p_mtxB[1][2] ), Size );
				std::memcpy( p_mtxA[2].ptr(), ( ptSrcA0 * p_mtxB[2][0] + ptSrcA1 * p_mtxB[2][1] + ptSrcA2 * p_mtxB[2][2] ), Size );
			}
		};

		template< typename Type >
		struct SqrMtxOperators< Type, 2 >
		{
			static const uint32_t Size = sizeof( Type ) * 2;

			static inline void mul( castor::SquareMatrix< Type, 2 > const & p_mtxA, castor::SquareMatrix< Type, 2 > const & p_mtxB )
			{
				typedef typename castor::SquareMatrix< Type, 2 >::row_type row_type;
				castor::Point< Type, 2 > ptSrcA0( p_mtxA[0][0], p_mtxA[0][1] );
				castor::Point< Type, 2 > ptSrcA1( p_mtxA[1][0], p_mtxA[1][1] );
				std::memcpy( p_mtxA[0].ptr(), ( ptSrcA0 * p_mtxB[0][0] + ptSrcA1 * p_mtxB[0][1] ), Size );
				std::memcpy( p_mtxA[1].ptr(), ( ptSrcA0 * p_mtxB[1][0] + ptSrcA1 * p_mtxB[1][1] ), Size );
			}
		};

		template< typename TypeA, typename TypeB, uint32_t Count >
		struct MtxMultiplicator
		{
			static inline castor::Point< TypeB, Count > mul( castor::SquareMatrix< TypeA, 4 > const & p_matrix, castor::Point< TypeB, Count > const & p_vertex )
			{
				castor::Point< TypeB, Count > result;
				result[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[0][1] * p_vertex[1] ) + TypeB( p_matrix[0][2] * p_vertex[2] ) + TypeB( p_matrix[0][3] * p_vertex[3] );
				result[1]  = TypeB( p_matrix[1][0] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] ) + TypeB( p_matrix[1][2] * p_vertex[2] ) + TypeB( p_matrix[1][3] * p_vertex[3] );
				result[2]  = TypeB( p_matrix[2][0] * p_vertex[0] ) + TypeB( p_matrix[2][1] * p_vertex[1] ) + TypeB( p_matrix[2][2] * p_vertex[2] ) + TypeB( p_matrix[2][3] * p_vertex[3] );
				result[3]  = TypeB( p_matrix[3][0] * p_vertex[0] ) + TypeB( p_matrix[3][1] * p_vertex[1] ) + TypeB( p_matrix[3][2] * p_vertex[2] ) + TypeB( p_matrix[3][3] * p_vertex[3] );
				return result;
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 0 >
		{
			static inline castor::Point< TypeB, 0 > mul( castor::SquareMatrix< TypeA, 4 > const & p_matrix, castor::Point< TypeB, 0 > const & p_vertex )
			{
				return castor::Point< TypeB, 0 >();
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 1 >
		{
			static inline castor::Point< TypeB, 1 > mul( castor::SquareMatrix< TypeA, 4 > const & p_matrix, castor::Point< TypeB, 1 > const & p_vertex )
			{
				castor::Point< TypeB, 1 > result;
				result[0]  = TypeB( p_matrix[0][0] * p_vertex[0] );
				return result;
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 2 >
		{
			static inline castor::Point< TypeB, 2 > mul( castor::SquareMatrix< TypeA, 4 > const & p_matrix, castor::Point< TypeB, 2 > const & p_vertex )
			{
				castor::Point< TypeB, 2 > result;
				result[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[0][1] * p_vertex[1] );
				result[1]  = TypeB( p_matrix[1][0] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] );
				return result;
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 3 >
		{
			static inline castor::Point< TypeB, 3 > mul( castor::SquareMatrix< TypeA, 4 > const & p_matrix, castor::Point< TypeB, 3 > const & p_vertex )
			{
				castor::Point< TypeB, 3 > result;
				result[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[0][1] * p_vertex[1] ) + TypeB( p_matrix[0][2] * p_vertex[2] );
				result[1]  = TypeB( p_matrix[1][0] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] ) + TypeB( p_matrix[1][2] * p_vertex[2] );
				result[2]  = TypeB( p_matrix[2][0] * p_vertex[0] ) + TypeB( p_matrix[2][1] * p_vertex[1] ) + TypeB( p_matrix[2][2] * p_vertex[2] );
				return result;
			}
		};

		template< typename T, uint32_t Count > struct CoFactorComputer;

		template< typename T >
		struct CoFactorComputer< T, 1 >
		{
			static inline T get( castor::SquareMatrix< T, 1 > const & CU_PARAM_UNUSED( p_matrix ), uint32_t CU_PARAM_UNUSED( p_column ), uint32_t CU_PARAM_UNUSED( p_row ) )
			{
				return T{ 1 };
			}
		};

		template< typename T, uint32_t Count >
		struct CoFactorComputer
		{
			static inline T get( castor::SquareMatrix< T, Count > const & p_matrix, uint32_t p_column, uint32_t p_row )
			{
				T tReturn = T();
				castor::SquareMatrix < T, Count - 1 > mtxTmp = p_matrix.getMinor( p_column, p_row );

				if ( ( p_row + p_column ) % 2 == 0 )
				{
					tReturn = mtxTmp.getDeterminant();
				}
				else
				{
					tReturn = -mtxTmp.getDeterminant();
				}

				return tReturn;
			}
		};
	}

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix()
		: my_matrix_type()
	{
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( NoInit const & p_noinit )
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
		uint32_t count = std::min( Count, _Rows );

		for ( uint32_t i = 0; i < count; i++ )
		{
			for ( uint32_t j = 0; j < count; j++ )
			{
				this->operator[]( i )[j] = p_matrix[i][j];
			}
		}

		for ( int i = count; i < Count; i++ )
		{
			this->operator[]( i )[i] = T{ 1 };
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
	inline T SquareMatrix< T, Count >::getDeterminant()const
	{
		T tReturn{};

		for ( uint32_t i = 0; i < Count; i++ )
		{
			tReturn += this->operator[]( 0 )[i] * getCofactor( i, 0 );
		}

		return tReturn;
	}
	template< typename T, uint32_t Count >
	inline bool SquareMatrix< T, Count >::isOrthogonal()const
	{
		bool result = false;
		my_matrix_type mTmp( *this );
		SquareMatrix< T, Count > mId1;
		SquareMatrix< T, Count > mId2;
		mTmp.transpose();
		mId1.setIdentity();
		mId2.setIdentity();
		return ( mId1 == *this * mTmp ) && ( mId2 == mTmp * ( *this ) );
	}
	template< typename T, uint32_t Count >
	inline bool SquareMatrix< T, Count >::isSymmetrical()const
	{
		bool result = true;

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			for ( uint32_t j = 0; j < Count && result; j++ )
			{
				result = this->m_columns[i][j] == this->m_columns[j][i];
			}
		}

		return result;
	}
	template< typename T, uint32_t Count >
	inline bool SquareMatrix< T, Count >::isAntiSymmetrical()const
	{
		bool result = true;

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			for ( uint32_t j = 0; j < Count && result; j++ )
			{
				if ( this->m_columns[i][j] + this->m_columns[j][i] != T{} )
				{
					result = false;
				}
			}
		}

		return result;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > SquareMatrix< T, Count >::getInverse()const
	{
		return SqrMtxInverter< T, Count >::inverse( *this );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix < T, Count - 1 > SquareMatrix< T, Count >::getMinor( uint32_t x, uint32_t y )const
	{
		SquareMatrix < T, Count - 1 > result;
		result.setIdentity();
		uint32_t i = 0, j = 0;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			if ( i != x )
			{
				j = 0;

				for ( uint32_t j = 0; j < Count; j++ )
				{
					if ( j != y )
					{
						result[i][j++] = this->m_columns[i][j];
					}
				}

				i++;
			}
		}

		return result;
	}
	template< typename T, uint32_t Count >
	inline T SquareMatrix< T, Count >::getCofactor( uint32_t p_column, uint32_t p_row )const
	{
		return CoFactorComputer< T, Count >::get( *this, p_column, p_row );
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::invert()
	{
		T tDeterminant = getDeterminant();

		if ( tDeterminant != T{} )
		{
			SquareMatrix< T, Count > mTmp;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				for ( uint32_t j = 0; j < Count; j++ )
				{
					mTmp[i][j] = getCofactor( i, j ) / tDeterminant;
				}
			}

			std::memcpy( my_matrix_type::ptr(), mTmp.constPtr(), my_matrix_type::size );
		}

		return *this;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > SquareMatrix< T, Count >::getTransposed()const
	{
		SquareMatrix< T, Count > result( *this );
		return result.transpose();
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::transpose()
	{
		T const * buffer = my_matrix_type::constPtr();

		for ( uint32_t i = 0; i < Count; i++ )
		{
			for ( uint32_t j = 0; j < i; j++ )
			{
				std::swap( ( *this )[j][i], ( *this )[i][j] );
			}
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
		bool result = true;

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			for ( uint32_t j = 0; j < Count && result; j++ )
			{
				result = p_mtxA[i][j] == p_mtxB[i][j];
			}
		}

		return result;
	}
	template< typename T, uint32_t Count >
	inline bool operator!=( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< T, Count > const & p_mtxB )
	{
		return ! operator ==( p_mtxA, p_mtxB );
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB )
	{
		SquareMatrix< T, Count > result( p_mtxA );
		result += p_mtxB;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB )
	{
		SquareMatrix< T, Count > result( p_mtxA );
		result -= p_mtxB;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_mtxA, SquareMatrix< U, Count > const & p_mtxB )
	{
		SquareMatrix< T, Count > result( p_mtxA );
		result *= p_mtxB;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_matrix, U const * p_values )
	{
		SquareMatrix< T, Count > result( p_matrix );
		result += p_values;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_matrix, U const * p_values )
	{
		SquareMatrix< T, Count > result( p_matrix );
		result -= p_values;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_matrix, U const * p_values )
	{
		SquareMatrix< T, Count > result( p_matrix );
		result *= p_values;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > result( p_matrix );
		result += p_value;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > result( p_matrix );
		result -= p_value;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > result( p_matrix );
		result *= p_value;
		return result;
	}
	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator/( SquareMatrix< T, Count > const & p_matrix, T const & p_value )
	{
		SquareMatrix< T, Count > result( p_matrix );
		result /= p_value;
		return result;
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
		SquareMatrix< T, Count > result;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			for ( uint32_t j = 0; j < Count; j++ )
			{
				result[i][j] = -p_matrix[i][j];
			}
		}

		return result;
	}

//*************************************************************************************************

	template< typename CharT, typename T, uint32_t Count >
	inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & p_streamOut, castor::SquareMatrix< T, Count > const & p_matrix )
	{
		auto precision = p_streamOut.precision( 10 );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			for ( uint32_t j = 0; j < Count; j++ )
			{
				p_streamOut.width( 15 );
				p_streamOut << std::right << p_matrix[i][j];
			}

			p_streamOut << std::endl;
		}

		p_streamOut.precision( precision );
		return p_streamOut;
	}

	template< typename CharT, typename T, uint32_t Count >
	inline std::basic_istream< CharT > & operator>>( std::basic_istream< CharT > & p_streamIn, castor::SquareMatrix< T, Count > & p_matrix )
	{
		for ( uint32_t i = 0; i < Count; i++ )
		{
			for ( uint32_t j = 0; j < Count; j++ )
			{
				p_streamIn >> p_matrix[i][j];
			}

			p_streamIn.ignore();
		}

		return p_streamIn;
	}

//*************************************************************************************************
}
