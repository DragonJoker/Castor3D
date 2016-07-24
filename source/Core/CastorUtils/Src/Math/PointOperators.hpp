/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

