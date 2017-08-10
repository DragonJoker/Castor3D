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
#ifndef ___CASTOR_MATH_H___
#define ___CASTOR_MATH_H___

//In case someone tries to create macros for these -_- template versions are better...
#ifdef min
#	undef min
#endif

#ifdef max
#	undef max
#endif

#ifdef abs
#	undef abs
#endif

#ifdef nabs
#	undef nabs
#endif

#ifdef abs
#	undef abs
#endif

namespace castor
{
	//************************************************************************************************
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Factorial recursive case
	\~french
	\brief		Cas récursif pour Factorielle
	*/
	template< typename T, int N > struct Fact
	{
		inline T operator()()
		{
			return N * Fact < T, N - 1 > ()();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Factorial end case
	\~french
	\brief		Cas d'arrêt pour Factorielle
	*/
	template< typename T > struct Fact< T, 0 >
	{
		inline T operator()()
		{
			return T( 1 );
		}
	};
	//************************************************************************************************
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Power recursive case
	\~french
	\brief		Cas récursif pour Puissance
	*/
	template< typename T, int N > struct Power
	{
		inline T operator()( T x )
		{
			return Power < T, N / 2 > ()( x ) * Power < T, N / 2 > ()( x ) * Power < T, N % 2 > ()( x );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Power end case
	\~french
	\brief		Cas d'arrêt pour Puissance
	*/
	template< typename T > struct Power< T, 1 >
	{
		inline T operator()( T x )
		{
			return x;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Power end case
	\~french
	\brief		Cas d'arrêt pour Puissance
	*/
	template< typename T > struct Power< T, 0 >
	{
		inline T operator()( T x )
		{
			return T( 1 );
		}
	};
	//************************************************************************************************
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Neperian Logarithm recursive case
	\~french
	\brief		Cas récursif pour Logarithme Népérien
	*/
	template <int N, typename T> struct LogN
	{
		inline double operator()( T x )
		{
			return LogN < N - 1, T > ()( x ) + Power < double, 2 * N + 1 > ()( ( double( x ) - 1 ) / ( double( x ) + 1 ) ) / ( 2 * N + 1 );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Neperian Logarithm end case
	\~french
	\brief		Cas d'arrêt pour Logarithme Népérien
	*/
	template< typename T > struct LogN<0, T>
	{
		inline double operator()( T x )
		{
			return ( double( x ) - 1 ) / ( double( x ) + 1 );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Neperian Logarithm using template implementation
	\~french
	\brief		Logarithme Népérien utilisant une implémentation template
	*/
	template< typename T > struct Ln
	{
	private:
		enum
		{
			Precision = 20
		};
	public:
		inline double operator()( T x )
		{
			return 2.0 * LogN<Precision, T>()( x );
		}
	};

	//************************************************************************************************

	/**
	 *\~english
	 *\brief		Tests if a double is a number
	 *\remarks		It is a number as long as x==x
	 *\param[in]	x	Number to test
	 *\return		The result
	 *\~french
	 *\brief		Teste si un double est un nombre
	 *\remarks		C'est un nombre tant que x==x
	 *\param[in]	x	Le nombre à tester
	 *\return		Le résultat
	 */
	inline bool isNan( double x )
	{
		volatile double temp = x;
		return temp != x;
	}
	/**
	 *\~english
	 *\brief		Tests if a double is infinite
	 *\remarks		It is infinite if x==x and x-x!=0
	 *\param[in]	x	Number to test
	 *\return		-1 if negative infinite, 1 if positive infinite, 0 if not infinite
	 *\~french
	 *\brief		Teste si un double est infini
	 *\remarks		Il est infini si x==x et x-x!=0
	 *\param[in]	x	Le nombre à tester
	 *\return		-1 si infini négatif, 1 si infini positif, 0 si fini
	 */
	inline int isInf( double x )
	{
		volatile double temp = x;

		if ( ( temp == x ) && ( ( temp - x ) != 0.0 ) )
		{
			return ( x < 0.0 ? -1 : 1 );
		}
		else
		{
			return 0;
		}
	}
}

#endif
