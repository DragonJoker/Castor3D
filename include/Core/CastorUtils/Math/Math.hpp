/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_MATH_H___
#define ___CASTOR_MATH_H___

namespace castor
{
	//************************************************************************************************
	/**
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
	/**
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
	/**
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
	/**
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
	/**
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
	/**
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
	/**
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
	/**
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

	namespace details
	{
		template< typename DataT >
		concept ValueTypeT = std::is_floating_point_v< DataT >
			|| std::is_integral_v< DataT >;


		template< ValueTypeT DataT, DataT ... ValuesT >
		struct MinRecT;

		template< ValueTypeT DataT, DataT ValueT >
		struct MinRecT< DataT, ValueT >
		{
			static constexpr DataT value = ValueT;
		};

		template< ValueTypeT DataT, DataT LhsT, DataT RhsT >
		struct MinRecT< DataT, LhsT, RhsT >
		{
			static constexpr DataT value = std::min( LhsT, RhsT );
		};

		template< ValueTypeT DataT, DataT LhsT, DataT RhsT, DataT ... ValuesT >
		struct MinRecT< DataT, LhsT, RhsT, ValuesT... >
		{
			static constexpr DataT value = MinRecT< DataT, std::max( LhsT, RhsT ), ValuesT... >::value;
		};


		template< ValueTypeT DataT, DataT ... ValuesT >
		struct MaxRecT;

		template< ValueTypeT DataT, DataT ValueT >
		struct MaxRecT< DataT, ValueT >
		{
			static constexpr DataT value = ValueT;
		};

		template< ValueTypeT DataT, DataT LhsT, DataT RhsT >
		struct MaxRecT< DataT, LhsT, RhsT >
		{
			static constexpr DataT value = std::max( LhsT, RhsT );
		};

		template< ValueTypeT DataT, DataT LhsT, DataT RhsT, DataT ... ValuesT >
		struct MaxRecT< DataT, LhsT, RhsT, ValuesT... >
		{
			static constexpr DataT value = MaxRecT< DataT, std::max( LhsT, RhsT ), ValuesT... >::value;
		};
	}

	template< details::ValueTypeT DataT, DataT ... ValuesT >
	static DataT minValueT = details::MinRecT< DataT, ValuesT... >::value;

	template< details::ValueTypeT DataT, DataT ... ValuesT >
	static DataT maxValueT = details::MaxRecT< DataT, ValuesT... >::value;

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
	/**
	 *\~english
	 *\brief		Division rounded up.
	 *\~french
	 *\brief		Division arrondie au supérieur.
	 */
	template< std::integral TypeT >
	inline constexpr TypeT divRoundUp( TypeT num, TypeT denom )
	{
		return num / denom + ( num % denom != 0 );
	}
}

#endif
