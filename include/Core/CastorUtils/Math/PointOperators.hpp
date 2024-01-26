/*
See LICENSE file in root folder
*/
#ifndef ___CU_POINT_OPERATORS_H___
#define ___CU_POINT_OPERATORS_H___

#include "CastorUtils/Math/MathModule.hpp"

namespace castor
{
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	struct PtOperators
	{
		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > add( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > sub( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > mul( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > div( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > add( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > sub( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > mul( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1 >
		static Point< std::remove_cv_t< T1 >, C1 > div( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1, typename PtType2 >
		static Point< std::remove_cv_t< T1 >, C1 > add( PtType1 const & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static Point< std::remove_cv_t< T1 >, C1 > sub( PtType1 const & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static Point< std::remove_cv_t< T1 >, C1 > mul( PtType1 const & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static Point< std::remove_cv_t< T1 >, C1 > div( PtType1 const & lhs, PtType2 const & rhs );
	};

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	struct PtAssignOperators
	{
		template< typename PtType1 >
		static PtType1 & add( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static PtType1 & sub( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static PtType1 & mul( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static PtType1 & div( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static PtType1 & add( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1 >
		static PtType1 & sub( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1 >
		static PtType1 & mul( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1 >
		static PtType1 & div( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1, typename PtType2 >
		static PtType1 & add( PtType1 & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static PtType1 & sub( PtType1 & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static PtType1 & mul( PtType1 & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static PtType1 & div( PtType1 & lhs, PtType2 const & rhs );
	};
}

#include "PointOperators.inl"

#endif

