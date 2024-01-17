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
		inline T operator()()const
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
		inline T operator()()const
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
		inline T operator()( T x )const
		{
			return Power< T, N / 2 > ()( x ) * Power< T, N / 2 > ()( x ) * Power< T, N % 2 > ()( x );
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
		inline T operator()( T x )const
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
		inline T operator()( T )const
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
		inline double operator()( T x )const
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
		inline double operator()( T x )const
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
		inline double operator()( T x )const
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
	 *\brief		Division rounded up.
	 *\~french
	 *\brief		Division arrondie au supérieur.
	 */
	template< std::integral TypeT >
	constexpr TypeT divRoundUp( TypeT num, TypeT denom )
	{
		return num / denom + ( num % denom != 0 );
	}
}

#endif
