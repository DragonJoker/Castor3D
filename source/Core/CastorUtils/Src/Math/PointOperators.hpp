/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_POINT_OPERATORS_H___
#define ___CU_POINT_OPERATORS_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
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

