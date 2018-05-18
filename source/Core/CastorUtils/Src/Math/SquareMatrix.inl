#include "Simd.hpp"

namespace castor
{
	namespace
	{
		template< typename Type, uint32_t Count > struct SqrMtxInverter;

		template< typename Type >
		struct SqrMtxInverter< Type, 4 >
		{
			static inline void inverse( castor::SquareMatrix< Type, 4 > const & input
				, castor::SquareMatrix< Type, 4 > & result )
			{
				auto tS0 = input[0];
				Type tS00 = input[0][0];
				Type tS01 = input[0][1];
				Type tS02 = input[0][2];
				Type tS03 = input[0][3];
				Type tS10 = input[1][0];
				Type tS11 = input[1][1];
				Type tS12 = input[1][2];
				Type tS13 = input[1][3];
				Type tS20 = input[2][0];
				Type tS21 = input[2][1];
				Type tS22 = input[2][2];
				Type tS23 = input[2][3];
				Type tS30 = input[3][0];
				Type tS31 = input[3][1];
				Type tS32 = input[3][2];
				Type tS33 = input[3][3];
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
				result[0][0] = +1 * ( tS11 * tCoef00 - tS12 * tCoef04 + tS13 * tCoef08 );
				result[0][1] = -1 * ( tS01 * tCoef00 - tS02 * tCoef04 + tS03 * tCoef08 );
				result[0][2] = +1 * ( tS01 * tCoef02 - tS02 * tCoef06 + tS03 * tCoef10 );
				result[0][3] = -1 * ( tS01 * tCoef03 - tS02 * tCoef07 + tS03 * tCoef11 );
				result[1][0] = -1 * ( tS10 * tCoef00 - tS12 * tCoef12 + tS13 * tCoef16 );
				result[1][1] = +1 * ( tS00 * tCoef00 - tS02 * tCoef12 + tS03 * tCoef16 );
				result[1][2] = -1 * ( tS00 * tCoef02 - tS02 * tCoef14 + tS03 * tCoef18 );
				result[1][3] = +1 * ( tS00 * tCoef03 - tS02 * tCoef15 + tS03 * tCoef19 );
				result[2][0] = +1 * ( tS10 * tCoef04 - tS11 * tCoef12 + tS13 * tCoef20 );
				result[2][1] = -1 * ( tS00 * tCoef04 - tS01 * tCoef12 + tS03 * tCoef20 );
				result[2][2] = +1 * ( tS00 * tCoef06 - tS01 * tCoef14 + tS03 * tCoef22 );
				result[2][3] = -1 * ( tS00 * tCoef07 - tS01 * tCoef15 + tS03 * tCoef23 );
				result[3][0] = -1 * ( tS10 * tCoef08 - tS11 * tCoef16 + tS12 * tCoef20 );
				result[3][1] = +1 * ( tS00 * tCoef08 - tS01 * tCoef16 + tS02 * tCoef20 );
				result[3][2] = -1 * ( tS00 * tCoef10 - tS01 * tCoef18 + tS02 * tCoef22 );
				result[3][3] = +1 * ( tS00 * tCoef11 - tS01 * tCoef19 + tS02 * tCoef23 );
				castor::Point< Type, 4 > ptRow0( result[0][0], result[1][0], result[2][0], result[3][0] );
				Type determinant = castor::point::dot( tS0, ptRow0 );
				result /= determinant;
			}
		};

		template< typename Type >
		struct SqrMtxInverter< Type, 3 >
		{
			static inline void inverse( castor::SquareMatrix< Type, 3 > const & input
				, castor::SquareMatrix< Type, 3 > & result )
			{
				Type tS00 = input[0][0];
				Type tS01 = input[0][1];
				Type tS02 = input[0][2];
				Type tS10 = input[1][0];
				Type tS11 = input[1][1];
				Type tS12 = input[1][2];
				Type tS20 = input[2][0];
				Type tS21 = input[2][1];
				Type tS22 = input[2][2];
				result[0][0] = tS11 * tS22 - tS21 * tS12;
				result[0][1] = tS12 * tS20 - tS22 * tS10;
				result[0][2] = tS10 * tS21 - tS20 * tS11;
				result[1][0] = tS02 * tS21 - tS01 * tS22;
				result[1][1] = tS00 * tS22 - tS02 * tS20;
				result[1][2] = tS01 * tS20 - tS00 * tS21;
				result[2][0] = tS12 * tS01 - tS11 * tS02;
				result[2][1] = tS10 * tS02 - tS12 * tS00;
				result[2][2] = tS11 * tS00 - tS10 * tS01;
				Type determinant = tS00 * ( tS11 * tS22 - tS21 * tS12 )
					- tS10 * ( tS01 * tS22 - tS21 * tS02 )
					+ tS20 * ( tS01 * tS12 - tS11 * tS02 );
				result /= determinant;
			}
		};

		template< typename Type >
		struct SqrMtxInverter< Type, 2 >
		{
			static inline void inverse( castor::SquareMatrix< Type, 2 > const & input
				, castor::SquareMatrix< Type, 2 > & result )
			{
				Type determinant = input[0][0] * input[1][1] - input[1][0] * input[0][1];
				result[1][1] = +input[1][1] / determinant;
				result[0][1] = -input[0][1] / determinant;
				result[1][0] = -input[1][0] / determinant;
				result[0][0] = +input[0][0] / determinant;
			}
		};
		template< typename Type, uint32_t Count > struct SqrMtxOperators;

		template< typename Type >
		struct SqrMtxOperators< Type, 4 >
		{
			static const uint32_t Size = sizeof( Type ) * 4;

			static inline void mul( castor::SquareMatrix< Type, 4 > & lhs, castor::SquareMatrix< Type, 4 > const & rhs )
			{
				auto const l11 = lhs[0][0];
				auto const l12 = lhs[0][1];
				auto const l13 = lhs[0][2];
				auto const l14 = lhs[0][3];
				auto const l21 = lhs[1][0];
				auto const l22 = lhs[1][1];
				auto const l23 = lhs[1][2];
				auto const l24 = lhs[1][3];
				auto const l31 = lhs[2][0];
				auto const l32 = lhs[2][1];
				auto const l33 = lhs[2][2];
				auto const l34 = lhs[2][3];
				auto const l41 = lhs[3][0];
				auto const l42 = lhs[3][1];
				auto const l43 = lhs[3][2];
				auto const l44 = lhs[3][3];
				auto const r11 = rhs[0][0];
				auto const r12 = rhs[0][1];
				auto const r13 = rhs[0][2];
				auto const r14 = rhs[0][3];
				auto const r21 = rhs[1][0];
				auto const r22 = rhs[1][1];
				auto const r23 = rhs[1][2];
				auto const r24 = rhs[1][3];
				auto const r31 = rhs[2][0];
				auto const r32 = rhs[2][1];
				auto const r33 = rhs[2][2];
				auto const r34 = rhs[2][3];
				auto const r41 = rhs[3][0];
				auto const r42 = rhs[3][1];
				auto const r43 = rhs[3][2];
				auto const r44 = rhs[3][3];

				lhs[0][0] = ( l11 * r11 + l21 * r12 + l31 * r13 + l41 * r14 );
				lhs[0][1] = ( l12 * r11 + l22 * r12 + l32 * r13 + l42 * r14 );
				lhs[0][2] = ( l13 * r11 + l23 * r12 + l33 * r13 + l43 * r14 );
				lhs[0][3] = ( l14 * r11 + l24 * r12 + l34 * r13 + l44 * r14 );

				lhs[1][0] = ( l11 * r21 + l21 * r22 + l31 * r23 + l41 * r24 );
				lhs[1][1] = ( l12 * r21 + l22 * r22 + l32 * r23 + l42 * r24 );
				lhs[1][2] = ( l13 * r21 + l23 * r22 + l33 * r23 + l43 * r24 );
				lhs[1][3] = ( l14 * r21 + l24 * r22 + l34 * r23 + l44 * r24 );

				lhs[2][0] = ( l11 * r31 + l21 * r32 + l31 * r33 + l41 * r34 );
				lhs[2][1] = ( l12 * r31 + l22 * r32 + l32 * r33 + l42 * r34 );
				lhs[2][2] = ( l13 * r31 + l23 * r32 + l33 * r33 + l43 * r34 );
				lhs[2][3] = ( l14 * r31 + l24 * r32 + l34 * r33 + l44 * r34 );

				lhs[3][0] = ( l11 * r41 + l21 * r42 + l31 * r43 + l41 * r44 );
				lhs[3][1] = ( l12 * r41 + l22 * r42 + l32 * r43 + l42 * r44 );
				lhs[3][2] = ( l13 * r41 + l23 * r42 + l33 * r43 + l43 * r44 );
				lhs[3][3] = ( l14 * r41 + l24 * r42 + l34 * r43 + l44 * r44 );
			}
		};

		template< typename Type >
		struct SqrMtxOperators< Type, 3 >
		{
			static const uint32_t Size = sizeof( Type ) * 3;

			static inline void mul( castor::SquareMatrix< Type, 3 > & lhs, castor::SquareMatrix< Type, 3 > const & rhs )
			{
				auto const l11 = lhs[0][0];
				auto const l12 = lhs[0][1];
				auto const l13 = lhs[0][2];
				auto const l21 = lhs[1][0];
				auto const l22 = lhs[1][1];
				auto const l23 = lhs[1][2];
				auto const l31 = lhs[2][0];
				auto const l32 = lhs[2][1];
				auto const l33 = lhs[2][2];
				auto const r11 = rhs[0][0];
				auto const r12 = rhs[0][1];
				auto const r13 = rhs[0][2];
				auto const r21 = rhs[1][0];
				auto const r22 = rhs[1][1];
				auto const r23 = rhs[1][2];
				auto const r31 = rhs[2][0];
				auto const r32 = rhs[2][1];
				auto const r33 = rhs[2][2];

				lhs[0][0] = ( l11 * r11 + l21 * r12 + l31 * r13 );
				lhs[0][1] = ( l12 * r11 + l22 * r12 + l32 * r13 );
				lhs[0][2] = ( l13 * r11 + l23 * r12 + l33 * r13 );

				lhs[1][0] = ( l11 * r21 + l21 * r22 + l31 * r23 );
				lhs[1][1] = ( l12 * r21 + l22 * r22 + l32 * r23 );
				lhs[1][2] = ( l13 * r21 + l23 * r22 + l33 * r23 );

				lhs[2][0] = ( l11 * r31 + l21 * r32 + l31 * r33 );
				lhs[2][1] = ( l12 * r31 + l22 * r32 + l32 * r33 );
				lhs[2][2] = ( l13 * r31 + l23 * r32 + l33 * r33 );
			}
		};

		template< typename Type >
		struct SqrMtxOperators< Type, 2 >
		{
			static const uint32_t Size = sizeof( Type ) * 2;

			static inline void mul( castor::SquareMatrix< Type, 2 > const & lhs, castor::SquareMatrix< Type, 2 > const & rhs )
			{
				auto const l11 = lhs[0][0];
				auto const l12 = lhs[0][1];
				auto const l21 = lhs[1][0];
				auto const l22 = lhs[1][1];
				auto const r11 = rhs[0][0];
				auto const r12 = rhs[0][1];
				auto const r21 = rhs[1][0];
				auto const r22 = rhs[1][1];

				lhs[0][0] = ( l11 * r11 + l21 * r12);
				lhs[0][1] = ( l12 * r11 + l22 * r12);
				lhs[0][2] = ( l13 * r11 + l23 * r12);

				lhs[1][0] = ( l11 * r21 + l21 * r22);
				lhs[1][1] = ( l12 * r21 + l22 * r22);
				lhs[1][2] = ( l13 * r21 + l23 * r22);
			}
		};

		template< typename TypeA, typename TypeB, uint32_t Count >
		struct MtxMultiplicator
		{
			static inline castor::Point< TypeB, Count > mul( castor::SquareMatrix< TypeA, 4 > const & lhs, castor::Point< TypeB, Count > const & rhs )
			{
				castor::Point< TypeB, Count > result;
				result[0] = TypeB( lhs[0][0] * rhs[0] ) + TypeB( lhs[0][1] * rhs[1] ) + TypeB( lhs[0][2] * rhs[2] ) + TypeB( lhs[0][3] * rhs[3] );
				result[1] = TypeB( lhs[1][0] * rhs[0] ) + TypeB( lhs[1][1] * rhs[1] ) + TypeB( lhs[1][2] * rhs[2] ) + TypeB( lhs[1][3] * rhs[3] );
				result[2] = TypeB( lhs[2][0] * rhs[0] ) + TypeB( lhs[2][1] * rhs[1] ) + TypeB( lhs[2][2] * rhs[2] ) + TypeB( lhs[2][3] * rhs[3] );
				result[3] = TypeB( lhs[3][0] * rhs[0] ) + TypeB( lhs[3][1] * rhs[1] ) + TypeB( lhs[3][2] * rhs[2] ) + TypeB( lhs[3][3] * rhs[3] );
				return result;
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 0 >
		{
			static inline castor::Point< TypeB, 0 > mul( castor::SquareMatrix< TypeA, 4 > const & lhs, castor::Point< TypeB, 0 > const & rhs )
			{
				return castor::Point< TypeB, 0 >();
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 1 >
		{
			static inline castor::Point< TypeB, 1 > mul( castor::SquareMatrix< TypeA, 4 > const & lhs, castor::Point< TypeB, 1 > const & rhs )
			{
				castor::Point< TypeB, 1 > result;
				result[0]  = TypeB( lhs[0][0] * rhs[0] );
				return result;
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 2 >
		{
			static inline castor::Point< TypeB, 2 > mul( castor::SquareMatrix< TypeA, 4 > const & lhs, castor::Point< TypeB, 2 > const & rhs )
			{
				castor::Point< TypeB, 2 > result;
				result[0]  = TypeB( lhs[0][0] * rhs[0] ) + TypeB( lhs[0][1] * rhs[1] );
				result[1]  = TypeB( lhs[1][0] * rhs[0] ) + TypeB( lhs[1][1] * rhs[1] );
				return result;
			}
		};

		template< typename TypeA, typename TypeB >
		struct MtxMultiplicator< TypeA, TypeB, 3 >
		{
			static inline castor::Point< TypeB, 3 > mul( castor::SquareMatrix< TypeA, 4 > const & lhs, castor::Point< TypeB, 3 > const & rhs )
			{
				castor::Point< TypeB, 3 > result;
				result[0]  = TypeB( lhs[0][0] * rhs[0] ) + TypeB( lhs[0][1] * rhs[1] ) + TypeB( lhs[0][2] * rhs[2] );
				result[1]  = TypeB( lhs[1][0] * rhs[0] ) + TypeB( lhs[1][1] * rhs[1] ) + TypeB( lhs[1][2] * rhs[2] );
				result[2]  = TypeB( lhs[2][0] * rhs[0] ) + TypeB( lhs[2][1] * rhs[1] ) + TypeB( lhs[2][2] * rhs[2] );
				return result;
			}
		};

		template< typename T, uint32_t Count > struct CoFactorComputer;

		template< typename T >
		struct CoFactorComputer< T, 1 >
		{
			static inline T get( castor::SquareMatrix< T, 1 > const & CU_PARAM_UNUSED( matrix ), uint32_t CU_PARAM_UNUSED( column ), uint32_t CU_PARAM_UNUSED( row ) )
			{
				return T{ 1 };
			}
		};

		template< typename T, uint32_t Count >
		struct CoFactorComputer
		{
			static inline T get( castor::SquareMatrix< T, Count > const & matrix, uint32_t column, uint32_t row )
			{
				T tReturn = T();
				castor::SquareMatrix < T, Count - 1 > mtxTmp = matrix.getMinor( column, row );

				if ( ( row + column ) % 2 == 0 )
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
	inline SquareMatrix< T, Count >::SquareMatrix( NoInit const & noinit )
		: my_matrix_type( noinit )
	{
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( T const & rhs )
		: my_matrix_type( rhs )
	{
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< T, Count > const & rhs )
		: my_matrix_type( rhs )
	{
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< T, Count > && rhs )
		: my_matrix_type( std::move( rhs ) )
	{
	}

	template< typename T, uint32_t Count >
	template< uint32_t _Rows >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< T, _Rows > const & rhs )
		: my_matrix_type()
	{
		uint32_t constexpr count = std::min( Count, _Rows );

		for ( uint32_t i = 0; i < count; i++ )
		{
			for ( uint32_t j = 0; j < count; j++ )
			{
				this->operator[]( i )[j] = rhs[i][j];
			}
		}

		for ( int i = count; i < Count; i++ )
		{
			this->operator[]( i )[i] = T{ 1 };
		}
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count >::SquareMatrix( SquareMatrix< Type, Count > const & rhs )
		: my_matrix_type( rhs )
	{
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count >::SquareMatrix( Matrix< Type, Count, Count > const & rhs )
		: my_matrix_type( rhs )
	{
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count >::SquareMatrix( Type const * rhs )
		: my_matrix_type( rhs )
	{
	}

	template< typename T, uint32_t Count >
	inline T SquareMatrix< T, Count >::getDeterminant()const
	{
		T result{};

		for ( uint32_t i = 0; i < Count; i++ )
		{
			result += this->operator[]( 0 )[i] * getCofactor( i, 0 );
		}

		return result;
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
		SquareMatrix< T, Count > result{ NoInit{} };
		SqrMtxInverter< T, Count >::inverse( *this, result );
		return result;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix < T, Count - 1 > SquareMatrix< T, Count >::getMinor( uint32_t x, uint32_t y )const
	{
		SquareMatrix < T, Count - 1 > result;
		result.setIdentity();
		uint32_t idst = 0, jdst = 0;

		for ( uint32_t isrc = 0; isrc < Count; isrc++ )
		{
			if ( isrc != x )
			{
				jdst = 0;

				for ( uint32_t jsrc = 0; jsrc < Count; jsrc++ )
				{
					if ( jsrc != y )
					{
						result[idst][jdst++] = this->m_columns[isrc][jsrc];
					}
				}

				idst++;
			}
		}

		return result;
	}

	template< typename T, uint32_t Count >
	inline T SquareMatrix< T, Count >::getCofactor( uint32_t column, uint32_t _row )const
	{
		return CoFactorComputer< T, Count >::get( *this, column, row );
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::invert()
	{
		SqrMtxInverter< T, Count >::inverse( *this, *this );
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
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( Matrix< Type, Count, Count > const & rhs )
	{
		my_matrix_type::operator=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( SquareMatrix< T, Count > const & rhs )
	{
		my_matrix_type::operator=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( SquareMatrix< T, Count > && rhs )
	{
		my_matrix_type::operator=( std::move( rhs ) );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( SquareMatrix< Type, Count > const & rhs )
	{
		my_matrix_type::operator=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator=( Type const * rhs )
	{
		my_matrix_type::operator=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator+=( SquareMatrix< Type, Count > const & rhs )
	{
		my_matrix_type::operator+=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator-=( SquareMatrix< Type, Count > const & rhs )
	{
		my_matrix_type::operator-=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator*=( SquareMatrix< Type, Count > const & rhs )
	{
		SqrMtxOperators< T, Count >::mul( *this, rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator+=( Type const * rhs )
	{
		my_matrix_type::operator+=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator-=( Type const * rhs )
	{
		my_matrix_type::operator-=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	template< typename Type >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator*=( Type const * rhs )
	{
		my_matrix_type::operator*=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator+=( T const & rhs )
	{
		my_matrix_type::operator+=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator-=( T const & rhs )
	{
		my_matrix_type::operator-=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator*=( T const & rhs )
	{
		my_matrix_type::operator*=( rhs );
		return * this;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > & SquareMatrix< T, Count >::operator/=( T const & rhs )
	{
		my_matrix_type::operator/=( rhs );
		return * this;
	}

//*************************************************************************************************

	template< typename T, uint32_t Count >
	inline bool operator==( SquareMatrix< T, Count > const & lhs, SquareMatrix< T, Count > const & rhs )
	{
		bool result = true;

		for ( uint32_t i = 0; i < Count && result; i++ )
		{
			for ( uint32_t j = 0; j < Count && result; j++ )
			{
				result = lhs[i][j] == rhs[i][j];
			}
		}

		return result;
	}

	template< typename T, uint32_t Count >
	inline bool operator!=( SquareMatrix< T, Count > const & lhs, SquareMatrix< T, Count > const & rhs )
	{
		return !( lhs == rhs );
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & lhs, SquareMatrix< U, Count > const & rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result += rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & lhs, SquareMatrix< U, Count > const & rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result -= rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & lhs, SquareMatrix< U, Count > const & rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & lhs, U const * rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result += rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & lhs, U const * rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result -= rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & lhs, U const * rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator+( SquareMatrix< T, Count > const & lhs, T const & rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result += rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & lhs, T const & rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result -= rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator*( SquareMatrix< T, Count > const & lhs, T const & rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result *= rhs;
		return result;
	}

	template< typename T, uint32_t Count, typename U >
	SquareMatrix< T, Count > operator/( SquareMatrix< T, Count > const & lhs, T const & rhs )
	{
		SquareMatrix< T, Count > result( lhs );
		result /= rhs;
		return result;
	}

	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator+( T lhs, SquareMatrix< T, Count > const & rhs )
	{
		return rhs + lhs;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator-( T lhs, SquareMatrix< T, Count > const & rhs )
	{
		return rhs - lhs;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator*( T lhs, SquareMatrix< T, Count > const & rhs )
	{
		return rhs * lhs;
	}
	template< typename T, uint32_t Count >
	inline SquareMatrix< T, Count > operator-( SquareMatrix< T, Count > const & lhs )
	{
		SquareMatrix< T, Count > result;

		for ( uint32_t i = 0; i < Count; i++ )
		{
			for ( uint32_t j = 0; j < Count; j++ )
			{
				result[i][j] = -lhs[i][j];
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
