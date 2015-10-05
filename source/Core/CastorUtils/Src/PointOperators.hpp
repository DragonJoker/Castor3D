#ifndef ___CU_POINT_OPERATORS_H___
#define ___CU_POINT_OPERATORS_H___

namespace Castor
{
	template< typename T1, typename T2, uint32_t C1, uint32_t C2 > struct PtOperators
	{
		template< typename PtType1 >
		static inline Point< T1, C1 > add( PtType1 const & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline Point< T1, C1 > sub( PtType1 const & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline Point< T1, C1 > mul( PtType1 const & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline Point< T1, C1 > div( PtType1 const & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline Point< T1, C1 > add( PtType1 const & p_ptA, T2 const * p_buffer );

		template< typename PtType1 >
		static inline Point< T1, C1 > sub( PtType1 const & p_ptA, T2 const * p_buffer );

		template< typename PtType1 >
		static inline Point< T1, C1 > mul( PtType1 const & p_ptA, T2 const * p_buffer );

		template< typename PtType1 >
		static inline Point< T1, C1 > div( PtType1 const & p_ptA, T2 const * p_buffer );

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, C1 > add( PtType1 const & p_ptA, PtType2 const & p_ptB );

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, C1 > sub( PtType1 const & p_ptA, PtType2 const & p_ptB );

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, C1 > mul( PtType1 const & p_ptA, PtType2 const & p_ptB );

		template< typename PtType1, typename PtType2 >
		static inline Point< T1, C1 > div( PtType1 const & p_ptA, PtType2 const & p_ptB );
	};

	template< typename T1, typename T2, uint32_t C1, uint32_t C2 > struct PtAssignOperators
	{
		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const & p_scalar );

		template< typename PtType1 >
		static inline PtType1 & add( PtType1 & p_ptA, T2 const * p_buffer );

		template< typename PtType1 >
		static inline PtType1 & sub( PtType1 & p_ptA, T2 const * p_buffer );

		template< typename PtType1 >
		static inline PtType1 & mul( PtType1 & p_ptA, T2 const * p_buffer );

		template< typename PtType1 >
		static inline PtType1 & div( PtType1 & p_ptA, T2 const * p_buffer );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & add( PtType1 & p_ptA, PtType2 const & p_ptB );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & sub( PtType1 & p_ptA, PtType2 const & p_ptB );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & mul( PtType1 & p_ptA, PtType2 const & p_ptB );

		template< typename PtType1, typename PtType2 >
		static inline PtType1 & div( PtType1 & p_ptA, PtType2 const & p_ptB );
	};
}

#include "PointOperators.inl"

#endif

