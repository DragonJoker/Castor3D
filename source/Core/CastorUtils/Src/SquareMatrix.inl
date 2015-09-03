namespace
{
	template< typename Type, uint32_t Rows > struct SqrMtxOperators;

	template< typename Type > struct SqrMtxOperators< Type, 4 >
	{
		static const uint32_t Size = sizeof( Type ) * 4;

		static inline Castor::SquareMatrix< Type, 4 > mul( Castor::SquareMatrix< Type, 4 > & p_mtxA, Castor::SquareMatrix< Type, 4 > const & p_mtxB )
		{
			typedef typename Castor::SquareMatrix< Type, 4 >::col_type col_type;
			Castor::SquareMatrix< Type, 4 > l_mtxReturn;
			col_type const l_ptSrcA0( p_mtxA[0] );
			col_type const l_ptSrcA1( p_mtxA[1] );
			col_type const l_ptSrcA2( p_mtxA[2] );
			col_type const l_ptSrcA3( p_mtxA[3] );
			std::memcpy( l_mtxReturn[0], ( l_ptSrcA0 * p_mtxB[0][0] + l_ptSrcA1 * p_mtxB[0][1] + l_ptSrcA2 * p_mtxB[0][2] + l_ptSrcA3 * p_mtxB[0][3] ).const_ptr(), Size );
			std::memcpy( l_mtxReturn[1], ( l_ptSrcA0 * p_mtxB[1][0] + l_ptSrcA1 * p_mtxB[1][1] + l_ptSrcA2 * p_mtxB[1][2] + l_ptSrcA3 * p_mtxB[1][3] ).const_ptr(), Size );
			std::memcpy( l_mtxReturn[2], ( l_ptSrcA0 * p_mtxB[2][0] + l_ptSrcA1 * p_mtxB[2][1] + l_ptSrcA2 * p_mtxB[2][2] + l_ptSrcA3 * p_mtxB[2][3] ).const_ptr(), Size );
			std::memcpy( l_mtxReturn[3], ( l_ptSrcA0 * p_mtxB[3][0] + l_ptSrcA1 * p_mtxB[3][1] + l_ptSrcA2 * p_mtxB[3][2] + l_ptSrcA3 * p_mtxB[3][3] ).const_ptr(), Size );
			return l_mtxReturn;
		}

		static inline Castor::SquareMatrix< Type, 4 > inverse( Castor::SquareMatrix< Type, 4 > const & p_mtx )
		{
			Type l_tCoef00 = p_mtx[2][2] * p_mtx[3][3] - p_mtx[3][2] * p_mtx[2][3];
			Type l_tCoef02 = p_mtx[1][2] * p_mtx[3][3] - p_mtx[3][2] * p_mtx[1][3];
			Type l_tCoef03 = p_mtx[1][2] * p_mtx[2][3] - p_mtx[2][2] * p_mtx[1][3];
			Type l_tCoef04 = p_mtx[2][1] * p_mtx[3][3] - p_mtx[3][1] * p_mtx[2][3];
			Type l_tCoef06 = p_mtx[1][1] * p_mtx[3][3] - p_mtx[3][1] * p_mtx[1][3];
			Type l_tCoef07 = p_mtx[1][1] * p_mtx[2][3] - p_mtx[2][1] * p_mtx[1][3];
			Type l_tCoef08 = p_mtx[2][1] * p_mtx[3][2] - p_mtx[3][1] * p_mtx[2][2];
			Type l_tCoef10 = p_mtx[1][1] * p_mtx[3][2] - p_mtx[3][1] * p_mtx[1][2];
			Type l_tCoef11 = p_mtx[1][1] * p_mtx[2][2] - p_mtx[2][1] * p_mtx[1][2];
			Type l_tCoef12 = p_mtx[2][0] * p_mtx[3][3] - p_mtx[3][0] * p_mtx[2][3];
			Type l_tCoef14 = p_mtx[1][0] * p_mtx[3][3] - p_mtx[3][0] * p_mtx[1][3];
			Type l_tCoef15 = p_mtx[1][0] * p_mtx[2][3] - p_mtx[2][0] * p_mtx[1][3];
			Type l_tCoef16 = p_mtx[2][0] * p_mtx[3][2] - p_mtx[3][0] * p_mtx[2][2];
			Type l_tCoef18 = p_mtx[1][0] * p_mtx[3][2] - p_mtx[3][0] * p_mtx[1][2];
			Type l_tCoef19 = p_mtx[1][0] * p_mtx[2][2] - p_mtx[2][0] * p_mtx[1][2];
			Type l_tCoef20 = p_mtx[2][0] * p_mtx[3][1] - p_mtx[3][0] * p_mtx[2][1];
			Type l_tCoef22 = p_mtx[1][0] * p_mtx[3][1] - p_mtx[3][0] * p_mtx[1][1];
			Type l_tCoef23 = p_mtx[1][0] * p_mtx[2][1] - p_mtx[2][0] * p_mtx[1][1];
			Castor::Point< Type, 4 > const l_ptSignA( +1, -1, +1, -1 );
			Castor::Point< Type, 4 > const l_ptSignB( -1, +1, -1, +1 );
			Castor::Point< Type, 4 > l_ptFac0( l_tCoef00, l_tCoef00, l_tCoef02, l_tCoef03 );
			Castor::Point< Type, 4 > l_ptFac1( l_tCoef04, l_tCoef04, l_tCoef06, l_tCoef07 );
			Castor::Point< Type, 4 > l_ptFac2( l_tCoef08, l_tCoef08, l_tCoef10, l_tCoef11 );
			Castor::Point< Type, 4 > l_ptFac3( l_tCoef12, l_tCoef12, l_tCoef14, l_tCoef15 );
			Castor::Point< Type, 4 > l_ptFac4( l_tCoef16, l_tCoef16, l_tCoef18, l_tCoef19 );
			Castor::Point< Type, 4 > l_ptFac5( l_tCoef20, l_tCoef20, l_tCoef22, l_tCoef23 );
			Castor::Point< Type, 4 > l_ptVec0( p_mtx[1][0], p_mtx[0][0], p_mtx[0][0], p_mtx[0][0] );
			Castor::Point< Type, 4 > l_ptVec1( p_mtx[1][1], p_mtx[0][1], p_mtx[0][1], p_mtx[0][1] );
			Castor::Point< Type, 4 > l_ptVec2( p_mtx[1][2], p_mtx[0][2], p_mtx[0][2], p_mtx[0][2] );
			Castor::Point< Type, 4 > l_ptVec3( p_mtx[1][3], p_mtx[0][3], p_mtx[0][3], p_mtx[0][3] );
			Castor::Point< Type, 4 > l_ptInv0 = l_ptSignA * ( l_ptVec1 * l_ptFac0 - l_ptVec2 * l_ptFac1 + l_ptVec3 * l_ptFac2 );
			Castor::Point< Type, 4 > l_ptInv1 = l_ptSignB * ( l_ptVec0 * l_ptFac0 - l_ptVec2 * l_ptFac3 + l_ptVec3 * l_ptFac4 );
			Castor::Point< Type, 4 > l_ptInv2 = l_ptSignA * ( l_ptVec0 * l_ptFac1 - l_ptVec1 * l_ptFac3 + l_ptVec3 * l_ptFac5 );
			Castor::Point< Type, 4 > l_ptInv3 = l_ptSignB * ( l_ptVec0 * l_ptFac2 - l_ptVec1 * l_ptFac4 + l_ptVec2 * l_ptFac5 );
			Castor::SquareMatrix< Type, 4 > l_mtxInverse;
			l_mtxInverse[0][0] = l_ptInv0[0];
			l_mtxInverse[0][1] = l_ptInv0[1];
			l_mtxInverse[0][2] = l_ptInv0[2];
			l_mtxInverse[0][3] = l_ptInv0[3];
			l_mtxInverse[1][0] = l_ptInv1[0];
			l_mtxInverse[1][1] = l_ptInv1[1];
			l_mtxInverse[1][2] = l_ptInv1[2];
			l_mtxInverse[1][3] = l_ptInv1[3];
			l_mtxInverse[2][0] = l_ptInv2[0];
			l_mtxInverse[2][1] = l_ptInv2[1];
			l_mtxInverse[2][2] = l_ptInv2[2];
			l_mtxInverse[2][3] = l_ptInv2[3];
			l_mtxInverse[3][0] = l_ptInv3[0];
			l_mtxInverse[3][1] = l_ptInv3[1];
			l_mtxInverse[3][2] = l_ptInv3[2];
			l_mtxInverse[3][3] = l_ptInv3[3];
			Castor::Point< Type, 4 > l_ptRow0( l_mtxInverse[0][0], l_mtxInverse[1][0], l_mtxInverse[2][0], l_mtxInverse[3][0] );
			Type l_tDeterminant = Castor::point::dot( Castor::Point< Type, 4 >( p_mtx[0] ), l_ptRow0 );
			l_mtxInverse /= l_tDeterminant;
			return l_mtxInverse;
		}
	};

	template< typename Type > struct SqrMtxOperators< Type, 3 >
	{
		static const uint32_t Size = sizeof( Type ) * 3;

		static inline Castor::SquareMatrix< Type, 3 > mul( Castor::SquareMatrix< Type, 3 > const & p_mtxA, Castor::SquareMatrix< Type, 3 > const & p_mtxB )
		{
			typedef typename Castor::SquareMatrix< Type, 3 >::col_type col_type;
			Castor::SquareMatrix< Type, 3 > l_mtxReturn;
			col_type const l_ptSrcA0( p_mtxA[0] );
			col_type const l_ptSrcA1( p_mtxA[1] );
			col_type const l_ptSrcA2( p_mtxA[2] );
			std::memcpy( l_mtxReturn[0], ( l_ptSrcA0 * p_mtxB[0][0] + l_ptSrcA1 * p_mtxB[0][1] + l_ptSrcA2 * p_mtxB[0][2] ).const_ptr(), Size );
			std::memcpy( l_mtxReturn[1], ( l_ptSrcA0 * p_mtxB[1][0] + l_ptSrcA1 * p_mtxB[1][1] + l_ptSrcA2 * p_mtxB[1][2] ).const_ptr(), Size );
			std::memcpy( l_mtxReturn[2], ( l_ptSrcA0 * p_mtxB[2][0] + l_ptSrcA1 * p_mtxB[2][1] + l_ptSrcA2 * p_mtxB[2][2] ).const_ptr(), Size );
			return l_mtxReturn;
		}

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
			Castor::SquareMatrix< Type, 3 > l_mtxInverse;
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

	template< typename Type > struct SqrMtxOperators< Type, 2 >
	{
		static const uint32_t Size = sizeof( Type ) * 2;

		static inline Castor::SquareMatrix< Type, 2 > mul( Castor::SquareMatrix< Type, 2 > const & p_mtxA, Castor::SquareMatrix< Type, 2 > const & p_mtxB )
		{
			typedef typename Castor::SquareMatrix< Type, 2 >::col_type col_type;
			Castor::SquareMatrix< Type, 2 > l_mtxReturn;
			col_type const l_ptSrcA0( p_mtxA[0] );
			col_type const l_ptSrcA1( p_mtxA[1] );
			std::memcpy( l_mtxReturn[0], ( l_ptSrcA0 * p_mtxB[0][0] + l_ptSrcA1 * p_mtxB[0][1] ).const_ptr(), Size );
			std::memcpy( l_mtxReturn[1], ( l_ptSrcA0 * p_mtxB[1][0] + l_ptSrcA1 * p_mtxB[1][1] ).const_ptr(), Size );
			return l_mtxReturn;
		}

		static inline Castor::SquareMatrix< Type, 2 > inverse( Castor::SquareMatrix< Type, 2 > const & p_mtx )
		{
			Type l_tDeterminant = p_mtx[0][0] * p_mtx[1][1] - p_mtx[1][0] * p_mtx[0][1];
			Castor::SquareMatrix< Type, 2 > l_mtxInverse;
			l_mtxInverse[1][1] = + p_mtx[1][1] / l_tDeterminant;
			l_mtxInverse[0][1] = - p_mtx[0][1] / l_tDeterminant;
			l_mtxInverse[1][0] = - p_mtx[1][0] / l_tDeterminant;
			l_mtxInverse[0][0] = + p_mtx[0][0] / l_tDeterminant;
			return l_mtxInverse;
		}
	};

	template< typename TypeA, typename TypeB, uint32_t Count > struct MtxMultiplicator
	{
		Castor::Point< TypeB, Count > operator()( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, Count > const & p_vertex )
		{
			Castor::Point< TypeB, Count > l_ptReturn;

			for ( uint32_t i = 0; i < Count; i++ )
			{
				l_ptReturn[i]  = TypeB( p_matrix[0][i] * p_vertex[0] );
				l_ptReturn[i] += TypeB( p_matrix[1][i] * p_vertex[1] );
				l_ptReturn[i] += TypeB( p_matrix[2][i] * p_vertex[2] );
				l_ptReturn[i] += TypeB( p_matrix[3][i] * p_vertex[3] );
			}

			return l_ptReturn;
		}
	};

	template< typename TypeA, typename TypeB > struct MtxMultiplicator< TypeA, TypeB, 0 >
	{
		Castor::Point< TypeB, 0 > operator()( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 0 > const & p_vertex )
		{
			return Castor::Point<TypeB, 0>();
		}
	};

	template< typename TypeA, typename TypeB > struct MtxMultiplicator< TypeA, TypeB, 1 >
	{
		Castor::Point< TypeB, 1 > operator()( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 1 > const & p_vertex )
		{
			Castor::Point<TypeB, 1> l_ptReturn;
			l_ptReturn[0]  = TypeB( p_matrix[0][0] * p_vertex[0] );
			return l_ptReturn;
		}
	};

	template< typename TypeA, typename TypeB > struct MtxMultiplicator< TypeA, TypeB, 2 >
	{
		Castor::Point< TypeB, 2 > operator()( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 2 > const & p_vertex )
		{
			Castor::Point<TypeB, 2> l_ptReturn;
			l_ptReturn[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[1][0] * p_vertex[1] );
			l_ptReturn[1]  = TypeB( p_matrix[0][1] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] );
			return l_ptReturn;
		}
	};

	template< typename TypeA, typename TypeB > struct MtxMultiplicator< TypeA, TypeB, 3 >
	{
		Castor::Point< TypeB, 3 > operator()( Castor::SquareMatrix< TypeA, 4 > const & p_matrix, Castor::Point< TypeB, 3 > const & p_vertex )
		{
			Castor::Point<TypeB, 3> l_ptReturn;
			l_ptReturn[0]  = TypeB( p_matrix[0][0] * p_vertex[0] ) + TypeB( p_matrix[1][0] * p_vertex[1] ) + TypeB( p_matrix[2][0] * p_vertex[2] );
			l_ptReturn[1]  = TypeB( p_matrix[0][1] * p_vertex[0] ) + TypeB( p_matrix[1][1] * p_vertex[1] ) + TypeB( p_matrix[2][1] * p_vertex[2] );
			l_ptReturn[2]  = TypeB( p_matrix[0][2] * p_vertex[0] ) + TypeB( p_matrix[1][2] * p_vertex[1] ) + TypeB( p_matrix[2][2] * p_vertex[2] );
			return l_ptReturn;
		}
	};

	template< typename T, uint32_t Rows > struct CoFactorComputer;

	template< typename T > struct CoFactorComputer< T, 1 >
	{
		T operator()( Castor::SquareMatrix< T, 1 > const & CU_PARAM_UNUSED( p_matrix ), uint32_t CU_PARAM_UNUSED( p_uiRow ), uint32_t CU_PARAM_UNUSED( p_uiColumn ) )
		{
			return Castor::Policy< T >::unit();
		}
	};

	template< typename T, uint32_t Rows > struct CoFactorComputer
	{
		T operator()( Castor::SquareMatrix< T, Rows > const & p_matrix, uint32_t p_uiRow, uint32_t p_uiColumn )
		{
			T l_tReturn = T();
			Castor::SquareMatrix < T, Rows - 1 > l_mtxTmp = p_matrix.get_minor( p_uiRow, p_uiColumn );

			if ( ( p_uiRow + p_uiColumn ) % 2 == 0 )
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

	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows >::SquareMatrix()
		:	Matrix< T, Rows, Rows >	()
	{
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows >::SquareMatrix( T const & p_tValue )
		:	Matrix< T, Rows, Rows >	( p_tValue	)
	{
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows >::SquareMatrix( SquareMatrix< T, Rows > const & p_matrix )
		:	Matrix< T, Rows, Rows >	( p_matrix	)
	{
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows >::SquareMatrix( SquareMatrix< T, Rows > && p_matrix )
		:	Matrix< T, Rows, Rows >	( std::move( p_matrix ) )
	{
	}
	template< typename T, uint32_t Rows >
	template< uint32_t _Rows >
	inline SquareMatrix< T, Rows >::SquareMatrix( SquareMatrix< T, _Rows > const & p_matrix )
		:	Matrix< T, Rows, Rows >	()
	{
		uint32_t l_uiCount = std::min( Rows, _Rows );

		for ( uint32_t i = 0; i < l_uiCount; i++ )
		{
			for ( uint32_t j = 0; j < l_uiCount; j++ )
			{
				this->operator[]( i )[j] = p_matrix[i][j];
			}
		}

		for ( int i = l_uiCount; i < Rows; i++ )
		{
			this->operator[]( i )[i] = Castor::Policy< T >::unit();
		}

#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows >::SquareMatrix( SquareMatrix< Type, Rows > const & p_matrix )
		:	Matrix< T, Rows, Rows >	( p_matrix	)
	{
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows >::SquareMatrix( Matrix< Type, Rows, Rows > const & p_matrix )
		:	Matrix< T, Rows, Rows >	( p_matrix	)
	{
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows >::SquareMatrix( Type const * p_matrix )
		:	Matrix< T, Rows, Rows >	( p_matrix	)
	{
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows >::~SquareMatrix()
	{
	}
	template< typename T, uint32_t Rows >
	inline T SquareMatrix< T, Rows >::get_determinant()const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		T l_tReturn(	Castor::Policy< T >::zero() );

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			l_tReturn += this->operator[]( 0 )[i] * get_cofactor( i, 0 );
		}

		if ( Castor::Policy< T >::is_null( l_tReturn ) )
		{
			l_tReturn = T();
		}

		return l_tReturn;
	}
	template< typename T, uint32_t Rows >
	inline bool SquareMatrix< T, Rows >::is_orthogonal()const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		bool l_bReturn = false;
		Matrix< T, Rows, Rows > l_mTmp( * this );
		SquareMatrix< T, Rows > l_mId1;
		SquareMatrix< T, Rows > l_mId2;
		l_mTmp.transpose();
		l_mId1.set_identity();
		l_mId2.set_identity();

		if ( ( l_mId1 != *this * l_mTmp ) || ( l_mId2 != l_mTmp * ( * this ) ) )
		{
			l_bReturn = false;
		}
		else
		{
			l_bReturn = true;
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::transpose()
	{
		*this = my_matrix_type::get_transposed();
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows >
	inline bool SquareMatrix< T, Rows >::is_symmetrical()const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Rows && l_bReturn; i++ )
		{
			for ( uint32_t j = 0; j < Rows && l_bReturn; j++ )
			{
				l_bReturn = Castor::Policy< T >::equals( this->operator[]( i )[j], this->operator[]( j )[i] );
			}
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Rows >
	inline bool SquareMatrix< T, Rows >::is_anti_symmetrical()const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Rows && l_bReturn; i++ )
		{
			for ( uint32_t j = 0; j < Rows && l_bReturn; j++ )
			{
				if ( !Castor::Policy< T >::is_null( this->operator[]( i )[j] + this->operator[]( j )[i] ) )
				{
					l_bReturn = false;
				}
			}
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > SquareMatrix< T, Rows >::get_inverse()const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		return SqrMtxOperators< T, Rows >::inverse( *this );
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix < T, Rows - 1 > SquareMatrix< T, Rows >::get_minor( uint32_t x, uint32_t y )const
	{
		SquareMatrix < T, Rows - 1 > l_mReturn;
		l_mReturn.set_identity();
		uint32_t l_i = 0, l_j = 0;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			if ( i != x )
			{
				l_j = 0;

				for ( uint32_t j = 0; j < Rows; j++ )
				{
					if ( j != y )
					{
						l_mReturn[l_j++][l_i] = this->operator[]( j )[i];
					}
				}

				l_i++;
			}
		}

		return l_mReturn;
	}
	template< typename T, uint32_t Rows >
	inline T SquareMatrix< T, Rows >::get_cofactor( uint32_t p_uiRow, uint32_t p_uiColumn )const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		return CoFactorComputer< T, Rows >()( *this, p_uiRow, p_uiColumn );
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::invert()
	{
		T l_tDeterminant = get_determinant();

		if ( !Castor::Policy< T >::is_null( l_tDeterminant ) )
		{
			SquareMatrix< T, Rows > l_mTmp;

			for ( uint32_t i = 0; i < Rows; i++ )
			{
				for ( uint32_t j = 0; j < Rows; j++ )
				{
					l_mTmp[j][i] = get_cofactor( j, i ) / l_tDeterminant;
				}
			}

			for ( uint32_t i = 0; i < Rows; i++ )
			{
				for ( uint32_t j = 0; j < Rows; j++ )
				{
					if ( Castor::Policy< T >::is_null( l_mTmp[j][i] ) )
					{
						this->operator[]( j )[i] = T();
					}
					else
					{
						this->operator[]( j )[i] = l_mTmp[j][i];
					}
				}
			}
		}

#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		return *this;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > SquareMatrix< T, Rows >::multiply( SquareMatrix< T, Rows > const & p_matrix )const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		SquareMatrix< T, Rows > l_mtxReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_mtxReturn.set_column( i, l_mtxReturn.get_column( i ) + my_matrix_type::get_column( j ) * p_matrix[i][j] );
			}
		}

		return l_mtxReturn;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > SquareMatrix< T, Rows >::multiply( SquareMatrix< Type, Rows > const & p_matrix )const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		SquareMatrix< T, Rows > l_mtxReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_mtxReturn.set_column( i, l_mtxReturn.get_column( i ) + my_matrix_type::get_column( j ) * T( p_matrix[i][j] ) );
			}
		}

		return l_mtxReturn;
	}
	template< typename T, uint32_t Rows >
	template <uint32_t _Columns>
	inline Matrix< T, Rows, _Columns > SquareMatrix< T, Rows >::multiply( Matrix< T, Rows, _Columns > const & p_matrix )const
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		Matrix< T, Rows, _Columns > l_mtxReturn;

		for ( uint32_t i = 0; i < _Columns; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_mtxReturn.set_column( i, l_mtxReturn.get_column( i ) + my_matrix_type::get_column( j ) * p_matrix[i][j] );
			}
		}

		return l_mtxReturn;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator =( Matrix< Type, Rows, Rows > const & p_matrix )
	{
		Matrix< T, Rows, Rows >::operator =( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator =( SquareMatrix< T, Rows > const & p_matrix )
	{
		Matrix< T, Rows, Rows >::operator =( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator =( SquareMatrix< T, Rows > && p_matrix )
	{
		Matrix< T, Rows, Rows >::operator =( std::move( p_matrix ) );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator =( SquareMatrix< Type, Rows > const & p_matrix )
	{
		Matrix< T, Rows, Rows >::operator =( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator += ( SquareMatrix< Type, Rows > const & p_matrix )
	{
		Matrix< T, Rows, Rows >::operator +=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator -= ( SquareMatrix< Type, Rows > const & p_matrix )
	{
		Matrix< T, Rows, Rows >::operator -=( p_matrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator *= ( SquareMatrix< Type, Rows > const & p_matrix )
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		*this = SqrMtxOperators< T, Rows >::mul( *this, p_matrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator =( Type const * p_pMatrix )
	{
		Matrix< T, Rows, Rows >::operator =( p_pMatrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator += ( Type const * p_pMatrix )
	{
		Matrix< T, Rows, Rows >::operator +=( p_pMatrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator -= ( Type const * p_pMatrix )
	{
		Matrix< T, Rows, Rows >::operator -=( p_pMatrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	template< typename Type >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator *= ( Type const * p_pMatrix )
	{
#if !defined( NDEBUG )
		Matrix< T, Rows, Rows >::do_update_debug();
#endif
		*this = multiply( p_pMatrix );
		return * this;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator += ( T const & p_tValue )
	{
		Matrix< T, Rows, Rows >::operator +=( p_tValue );
		return * this;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator -= ( T const & p_tValue )
	{
		Matrix< T, Rows, Rows >::operator -=( p_tValue );
		return * this;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator *= ( T const & p_tValue )
	{
		Matrix< T, Rows, Rows >::operator *=( p_tValue );
		return * this;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix< T, Rows > & SquareMatrix< T, Rows >::operator /= ( T const & p_tValue )
	{
		Matrix< T, Rows, Rows >::operator /=( p_tValue );
		return * this;
	}

//*************************************************************************************************

	template< typename T, uint32_t Rows >
	inline bool operator ==( SquareMatrix< T, Rows > const & p_mtxA, SquareMatrix< T, Rows > const & p_mtxB )
	{
		bool l_bReturn = true;

		for ( uint32_t i = 0; i < Rows && l_bReturn; i++ )
		{
			for ( uint32_t j = 0; j < Rows && l_bReturn; j++ )
			{
				l_bReturn = Castor::Policy< T >::equals( p_mtxA[j][i], p_mtxB[j][i] );
			}
		}

		return l_bReturn;
	}
	template< typename T, uint32_t Rows >
	inline bool operator !=( SquareMatrix< T, Rows > const & p_mtxA, SquareMatrix< T, Rows > const & p_mtxB )
	{
		return ! operator ==( p_mtxA, p_mtxB );
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator + ( SquareMatrix <T, Rows> const & p_mtxA, SquareMatrix <U, Rows> const & p_mtxB )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_mtxA );
		l_mtxReturn += p_mtxB;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_mtxA, SquareMatrix <U, Rows> const & p_mtxB )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_mtxA );
		l_mtxReturn -= p_mtxB;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix< T, Rows > operator * ( SquareMatrix <T, Rows> const & p_mtxA, SquareMatrix <U, Rows> const & p_mtxB )
	{
		SquareMatrix< T, Rows > l_mtxReturn( p_mtxA );
		l_mtxReturn *= p_mtxB;
		return l_mtxReturn;
//		return SqrMtxOperators< T, Rows >::mul( p_mtxA, p_mtxB );
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator + ( SquareMatrix <T, Rows> const & p_matrix, U const * p_pMatrix )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix );
		l_mtxReturn += p_pMatrix;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_matrix, U const * p_pMatrix )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix );
		l_mtxReturn -= p_pMatrix;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator * ( SquareMatrix <T, Rows> const & p_matrix, U const * p_pMatrix )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix );
		l_mtxReturn *= p_pMatrix;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator + ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix );
		l_mtxReturn += p_tValue;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix );
		l_mtxReturn -= p_tValue;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator * ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix );
		l_mtxReturn *= p_tValue;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	SquareMatrix <T, Rows> operator / ( SquareMatrix <T, Rows> const & p_matrix, T const & p_tValue )
	{
		SquareMatrix <T, Rows> l_mtxReturn( p_matrix );
		l_mtxReturn /= p_tValue;
		return l_mtxReturn;
	}
	template <typename T, uint32_t Rows, typename U>
	inline Point <T, Rows> operator * ( SquareMatrix <T, Rows> const & p_matrix, Point<U, Rows> const & p_ptVector )
	{
		Point<T, Rows> l_ptReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_ptReturn[i] += p_matrix[j][i] * T( p_ptVector[j] );
			}
		}

		return l_ptReturn;
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix <T, Rows> operator + ( T p_tValue, SquareMatrix <T, Rows> const & p_matrix )
	{
		return operator +( p_matrix, p_tValue );
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix <T, Rows> operator - ( T p_tValue, SquareMatrix <T, Rows> const & p_matrix )
	{
		return operator -( p_matrix, p_tValue );
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix <T, Rows> operator * ( T p_tValue, SquareMatrix <T, Rows> const & p_matrix )
	{
		return operator *( p_matrix, p_tValue );
	}
	template< typename T, uint32_t Rows >
	inline SquareMatrix <T, Rows> operator - ( SquareMatrix <T, Rows> const & p_matrix )
	{
		Matrix< T, Rows, Rows > l_mtxReturn;

		for ( uint32_t i = 0; i < Rows; i++ )
		{
			for ( uint32_t j = 0; j < Rows; j++ )
			{
				l_mtxReturn[i][j] = -p_matrix[i][j];
			}
		}

		return l_mtxReturn;
	}

//*************************************************************************************************
}
