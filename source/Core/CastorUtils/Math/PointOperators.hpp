/*
See LICENSE file in root folder
*/
#ifndef ___CU_POINT_OPERATORS_H___
#define ___CU_POINT_OPERATORS_H___

#include "CastorUtilsPrerequisites.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		15/01/2016
	\~english
	\brief		Point operators.
	\~french
	\brief		Opérateurs de Point.
	*/
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	struct PtOperators
	{
		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > add( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > sub( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > mul( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > div( PtType1 const & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > add( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > sub( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > mul( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > div( PtType1 const & lhs, T2 const * rhs );

		template< typename PtType1, typename PtType2 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > add( PtType1 const & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > sub( PtType1 const & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > mul( PtType1 const & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static inline Point< typename std::remove_cv< T1 >::type, C1 > div( PtType1 const & lhs, PtType2 const & rhs );
	};
	/*!
	\author		Sylvain DOREMUS
	\date		15/01/2016
	\~english
	\brief		Point assignment operators.
	\~french
	\brief		Opérateurs d'affectation de Point.
	*/
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 >
	struct PtAssignOperators
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const & rhs );

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & lhs, T2 const * rhs );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & lhs, PtType2 const & rhs );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & lhs, PtType2 const & rhs );
	};
}

#include "PointOperators.inl"

#endif

