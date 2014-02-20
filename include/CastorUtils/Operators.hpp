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
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Operators enumeration
	\~french
	\brief		Enumération des opérateurs
	*/
	typedef enum eOPERATOR CASTOR_TYPE( uint8_t )
	{	eOPERATOR_ADD	//!<\~english	addition		\~french	addition
	,	eOPERATOR_SUB	//!<\~english	substraction	\~french	soustraction
	,	eOPERATOR_MUL	//!<\~english	multiplication	\~french	multiplication
	,	eOPERATOR_DIV	//!<\~english	division		\~french	division
	,	eOPERATOR_PLUS	//!<\~english	unary +			\~french	+ unaire
	,	eOPERATOR_MINUS	//!<\~english	unary -			\~french	- unaire
	,	eOPERATOR_CONST	//!<\~english	constant		\~french	constante unaire
	,	eOPERATOR_COUNT
	}	eOPERATOR;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Template class for unary operators
	\~french
	\brief		Classe template pour les opérateurs unaires
	*/
	template <class T, int Op> struct UnaryOp {};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Base class for unary operators
	\~french
	\brief		Classe de base pour les opérateurs unaires
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Unary - Specialisation of UnaryOp
	\~french
	\brief		Spécialisation de UnaryOp pour le - unaire
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Unary + Specialisation of UnaryOp
	\~french
	\brief		Spécialisation de UnaryOp pour le + unaire
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Unary constant Specialisation of UnaryOp
	\~french
	\brief		Spécialisation de UnaryOp pour la constante unaire
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Template class for binary operators
	\~french
	\brief		Classe template pour les opérateurs binaires
	*/
	template <class T, class U, int Op> struct BinaryOp {};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Base class for binary operators
	\~french
	\brief		Classe de base pour les opérateurs binaires
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Binary + Specialisation of BinaryOp
	\~french
	\brief		Spécialisation de BinaryOp pour le + binaire
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Binary - Specialisation of BinaryOp
	\~french
	\brief		Spécialisation de BinaryOp pour le - binaire
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Binary * Specialisation of BinaryOp
	\~french
	\brief		Spécialisation de BinaryOp pour le * binaire
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Binary / Specialisation of BinaryOp
	\~french
	\brief		Spécialisation de BinaryOp pour le / binaire
	*/
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
	template< class T, class U > inline BinaryOp< T, U, eOPERATOR_ADD > MakeAdd( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_ADD >( t, u );
	}
	template< class T, class U > inline BinaryOp< T, U, eOPERATOR_SUB > MakeSub( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_SUB >( t, u );
	}
	template< class T, class U > inline BinaryOp< T, U, eOPERATOR_MUL > MakeMul( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_MUL >( t, u );
	}
	template< class T, class U > inline BinaryOp< T, U, eOPERATOR_DIV > MakeDiv( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_DIV >( t, u );
	}
	template< class T > inline UnaryOp< T, eOPERATOR_MINUS > MakeMinus( T const & t )
	{
		return UnaryOp< T, eOPERATOR_MINUS >( t );
	}
	template< class T > inline UnaryOp< T, eOPERATOR_PLUS > MakePlus( T const & t )
	{
		return UnaryOp< T, eOPERATOR_PLUS >( t );
	}
	template< class T > inline UnaryOp< T, eOPERATOR_CONST > MakeConst( T const & t )
	{
		return UnaryOp< T, eOPERATOR_CONST >( t );
	}
	template< class T, class U > BinaryOp< T, U, eOPERATOR_ADD > operator +( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_ADD >( t, u );
	}
	template< class T, class U > BinaryOp< T, U, eOPERATOR_SUB > operator -( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_SUB >( t, u );
	}
	template< class T, class U > BinaryOp< T, U, eOPERATOR_MUL > operator -( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_MUL >( t, u );
	}
	template< class T, class U > BinaryOp< T, U, eOPERATOR_DIV > operator -( T const & t, U const & u )
	{
		return BinaryOp< T, U, eOPERATOR_DIV >( t, u );
	}
	template< class T > UnaryOp< T, eOPERATOR_MINUS > operator -( T const & t )
	{
		return UnaryOp< T, eOPERATOR_MINUS >( t );
	}
	template< class T > UnaryOp< T, eOPERATOR_PLUS > operator +( T const & t )
	{
		return UnaryOp< T, eOPERATOR_PLUS >( t );
	}
}

#endif
