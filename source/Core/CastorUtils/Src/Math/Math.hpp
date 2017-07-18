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

namespace Castor
{
	//************************************************************************************************
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Factorial recursive case
	\~french
	\brief		Cas récursif pour Factorielle
	*/
	template< typename T, int N > struct fact
	{
		inline T operator()()
		{
			return N * fact < T, N - 1 > ()();
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Factorial end case
	\~french
	\brief		Cas d'arrêt pour Factorielle
	*/
	template< typename T > struct fact< T, 0 >
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
	template< typename T, int N > struct power
	{
		inline T operator()( T x )
		{
			return power < T, N / 2 > ()( x ) * power < T, N / 2 > ()( x ) * power < T, N % 2 > ()( x );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Power end case
	\~french
	\brief		Cas d'arrêt pour Puissance
	*/
	template< typename T > struct power< T, 1 >
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
	template< typename T > struct power< T, 0 >
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
	template <int N, typename T> struct lnN
	{
		inline double operator()( T x )
		{
			return lnN < N - 1, T > ()( x ) + power < double, 2 * N + 1 > ()( ( double( x ) - 1 ) / ( double( x ) + 1 ) ) / ( 2 * N + 1 );
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Neperian Logarithm end case
	\~french
	\brief		Cas d'arrêt pour Logarithme Népérien
	*/
	template< typename T > struct lnN<0, T>
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
	template< typename T > struct ln
	{
	private:
		enum
		{
			Precision = 20
		};
	public:
		inline double operator()( T x )
		{
			return 2.0 * lnN<Precision, T>()( x );
		}
	};
	//************************************************************************************************
	/**
	 *\~english
	 *\brief		Puts the value in the range [min, max]
	 *\param[in]	p_min, p_max	The range
	 *\param[in]	p_value			The value to clamp
	 *\return		The result
	 *\~french
	 *\brief		Met la valeur entre les bornes [min,max]
	 *\param[in]	p_min, p_max	Les bornes
	 *\param[in]	p_value			La valeur à clamper
	 *\return		Le résultat
	 */
	template< typename T >
	inline void clamp( T & p_value, T const & p_min, T const & p_max )
	{
		if ( p_value < p_min )
		{
			p_value = p_min;
			return;
		}

		if ( p_value > p_max )
		{
			p_value = p_max;
		}
	}
	/**
	 *\~english
	 *\brief		Returns p_value if it is in the range [min,max], p_min if lower than it, p_max if larger than it
	 *\param[in]	p_min, p_max	The range
	 *\param[in]	p_value			The value to clamp
	 *\return		The result
	 *\~french
	 *\brief		Retourne p_value si elle est entre les bornes [min,max], p_min si elle lui est inférieure, p_max si elle lui est supérieure
	 *\param[in]	p_min, p_max	Les bornes
	 *\param[in]	p_value			La valeur à clamper
	 *\return		Le résultat
	 */
	template< typename T >
	inline T clamp( T const & p_value, T const & p_min, T const & p_max )
	{
		T tReturn = p_value;

		if ( p_value < p_min )
		{
			tReturn = p_min;
		}
		else if ( p_value > p_max )
		{
			tReturn = p_max;
		}

		return tReturn;
	}
	/**
	 *\~english
	 *\brief		Returns absolute value of given value
	 *\param[in]	p_val	The given value
	 *\return		The absolute value
	 *\~french
	 *\brief		Donne la valeur absolue d'une valeur donnée
	 *\param[in]	p_val	La valeur donnée
	 *\return		La valeur absolue
	 */
	template< typename T >
	inline T abs( T const & p_val )
	{
		return ( p_val >= 0 ? p_val : -p_val );
	}
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
	inline bool is_nan( double x )
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
	inline int is_inf( double x )
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
