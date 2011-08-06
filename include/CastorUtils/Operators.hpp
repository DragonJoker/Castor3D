/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___Castor_Operators___
#define ___Castor_Operators___

#include <limits>
#include <algorithm>

namespace Castor
{ namespace Templates
{
	typedef enum
	{	eOPERATOR_ADD	// addition
	,	eOPERATOR_SUB	// substraction
	,	eOPERATOR_MUL	// multiplication
	,	eOPERATOR_DIV	// division
	,	eOPERATOR_PLUS	// unary +
	,	eOPERATOR_MINUS	// unary -
	,	eOPERATOR_CONST	// constant
	,	eOPERATOR_COUNT
	}	eOPERATOR;

	// template class for unary operators
	template <class T, int Op> struct UnaryOp {};
	// base class for unary operators
	template <class T> struct UnaryOpBase
	{
		T It;

		UnaryOpBase(T t)
			:	It( t)
		{
		}
		inline void operator ++()
		{
			++It;
		}
	};
	// unary - specialisation
	template <class T> struct UnaryOp<T, eOPERATOR_MINUS> : public UnaryOpBase<T>
	{
		UnaryOp(T t)
			:	UnaryOpBase<T>(t)
		{
		}
		inline T operator *()
		{
			return -( * It);
		}
	};
	// unary + specialisation
	template <class T> struct UnaryOp<T, eOPERATOR_PLUS> : public UnaryOpBase<T>
	{
		UnaryOp(T t)
			:	UnaryOpBase<T>(t)
		{
		}
		inline T operator *()
		{
			return +( * It);
		}
	};

	// unary constants specialisation
	template <class T> struct UnaryOp<T, eOPERATOR_CONST> : public UnaryOpBase<T>
	{
		UnaryOp(T t)
			:	UnaryOpBase<T>( t)
		{
		}
		inline void operator ++()
		{
		}
		inline T operator *()
		{
			return It;
		}
	};

	// template class for binary operators
	template <class T, class U, int Op> struct BinaryOp {};
	// base class for binary operators
	template <class T, class U> struct BinaryOpBase
	{
		T It1;
		U It2;

		BinaryOpBase( T I1, U I2)
			:	It1(I1)
			,	It2(I2)
		{
		}
		inline void operator ++()
		{
			++It1;
			++It2;
		}
	};

	// + specialisation
	template <class T, class U> struct BinaryOp<T, U, eOPERATOR_ADD> : public BinaryOpBase<T, U>
	{
		BinaryOp( T I1, U I2)
			:	BinaryOpBase<T, U>( I1, I2)
		{
		}
		inline T operator *()
		{
			return *It1 + *It2;
		}
	};

	// - specialisation
	template <class T, class U> struct BinaryOp<T, U, eOPERATOR_SUB> : public BinaryOpBase<T, U>
	{
		BinaryOp( T I1, U I2)
			:	BinaryOpBase<T, U>( I1, I2)
		{
		}
		inline T operator *()
		{
			return *It1 - *It2;
		}
	};

	// * specialisation
	template <class T, class U> struct BinaryOp<T, U, eOPERATOR_MUL> : public BinaryOpBase<T, U>
	{
		BinaryOp( T I1, U I2)
			:	BinaryOpBase<T, U>( I1, I2)
		{
		}
		inline T operator *()
		{
			return *It1 * *It2;
		}
	};

	// * specialisation
	template <class T, class U> struct BinaryOp<T, U, eOPERATOR_DIV> : public BinaryOpBase<T, U>
	{
		BinaryOp( T I1, U I2)
			:	BinaryOpBase<T, U>( I1, I2)
		{
		}
		inline T operator *()
		{
			return *It1 / *It2;
		}
	};
	template <class T, class U> inline BinaryOp<T, U, eOPERATOR_ADD> MakeAdd( T const & t, U const & u)
	{
		return BinaryOp<T, U, eOPERATOR_ADD>(t, u);
	}
	template <class T, class U> inline BinaryOp<T, U, eOPERATOR_SUB> MakeSub( T const & t, U const & u)
	{
		return BinaryOp<T, U, eOPERATOR_SUB>(t, u);
	}
	template <class T, class U> inline BinaryOp<T, U, eOPERATOR_MUL> MakeMul( T const & t, U const & u)
	{
		return BinaryOp<T, U, eOPERATOR_MUL>(t, u);
	}
	template <class T, class U> inline BinaryOp<T, U, eOPERATOR_DIV> MakeDiv( T const & t, U const & u)
	{
		return BinaryOp<T, U, eOPERATOR_DIV>(t, u);
	}
	template <class T> inline UnaryOp<T, eOPERATOR_MINUS> MakeMinus( T const & t)
	{
		return UnaryOp<T, eOPERATOR_MINUS>(t);
	}
	template <class T> inline UnaryOp<T, eOPERATOR_PLUS> MakePlus( T const & t)
	{
		return UnaryOp<T, eOPERATOR_PLUS>(t);
	}
	template <class T> inline UnaryOp<T, eOPERATOR_CONST> MakeConst( T const & t)
	{
		return UnaryOp<T, eOPERATOR_CONST>(t);
	}
	template <class T, class U> BinaryOp<T, U, eOPERATOR_ADD> operator +( T const & t, U const & u)
	{
		return MakeAdd( t, u);
	}
	template <class T, class U> BinaryOp<T, U, eOPERATOR_SUB> operator -( T const & t, U const & u)
	{
		return MakeSub( t, u);
	}
	template <class T, class U> BinaryOp<T, U, eOPERATOR_MUL> operator -( T const & t, U const & u)
	{
		return MakeMul( t, u);
	}
	template <class T, class U> BinaryOp<T, U, eOPERATOR_DIV> operator -( T const & t, U const & u)
	{
		return MakeDiv( t, u);
	}
	template <class T> UnaryOp<T, eOPERATOR_MINUS> operator -( T const & t)
	{
		return MakeMinus( t);
	}
	template <class T> UnaryOp<T, eOPERATOR_PLUS> operator +( T const & t)
	{
		return MakePlus( t);
	}
}
}

#endif