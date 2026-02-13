/*
See LICENSE file in root folder
*/
#ifndef ___CastorUtils_RangedValue_HPP___
#define ___CastorUtils_RangedValue_HPP___
#pragma once

#include "CastorUtils/Math/Range.hpp"

namespace c3d
{
	template< typename T >
	class RangedValue
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	value	The value.
		 *\param[in]	range	The range.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	value	La valeur.
		 *\param[in]	range	Son intervalle.
		 */
		RangedValue( T const & value
			, Range< T > const & range )noexcept
			: m_range{ range }
			, m_value{ m_range.clamp( value ) }
		{
		}
		/**
		 *\~english
		 *\brief		Assignment from a value.
		 *\param[in]	value	The value.
		 *\~french
		 *\brief		Affectation depuis une valeur.
		 *\param[in]	value	La valeur.
		 */
		RangedValue & operator=( T const & value )noexcept
		{
			m_value = m_range.clamp( value );
			return *this;
		}
		/**
		 *\~english
		 *\brief		Redefines the range.
		 *\param[in]	range	The range.
		 *\~french
		 *\brief		Redéfinit l'intervalle.
		 *\param[in]	range	Le nouvel intervalle.
		 */
		void updateRange( Range< T > const & range )noexcept
		{
			m_range = range;
			m_value = m_range.clamp( m_value );
		}
		/**
		 *\~english
		 *\return		0.0 if \p m_value <= \p m_range.m_min.\n
		 *				1.0 if \p m_value >= \p m_range.m_max.\n
		 *				A value from 0.0 to 1.0, depending on how close the value is
		 *				from \p m_range.m_min or from \p m_range.m_max.
		 *\~french
		 *\return		0.0 si \p m_value <= \p m_range.m_min.\n
		 *				1.0 si \p m_value >= \p m_range.m_max.\n
		 *				Une valeur allant de 0.0 à 1.0, selon que la valeur est plus proche
		 *				de \p m_range.m_min ou de \p m_range.m_max.
		 */
		double percent()const noexcept
		{
			return m_range.percent( m_value );
		}
		/**
		 *\~english
		 *\return		The value.
		 *\~french
		 *\return		La valeur.
		 */
		T const & value()const noexcept
		{
			return m_value;
		}
		/**
		 *\~english
		 *\return		The range.
		 *\~french
		 *\return		L'intervalle.
		 */
		Range< T > const & range()const noexcept
		{
			return m_range;
		}
		/**
		 *\~english
		 *\name Arithmetic operators.
		 *\~french
		 *\name Opérateurs arithmétiques.
		 */
		/**@{*/
		template< typename ValueU >
		RangedValue< T > & operator+=( ValueU const & rhs )noexcept
		{
			if constexpr ( std::is_same_v< RangedValue< T >, ValueU > )
			{
				m_value = m_range.clamp( m_value + rhs.m_value );
			}
			else
			{
				m_value = m_range.clamp( m_value + rhs );
			}

			return *this;
		}

		template< typename ValueU >
		RangedValue< T > & operator-=( ValueU const & rhs )noexcept
		{
			if constexpr ( std::is_same_v< RangedValue< T >, ValueU > )
			{
				m_value = m_range.clamp( m_value - rhs.m_value );
			}
			else
			{
				m_value = m_range.clamp( m_value - rhs );
			}

			return *this;
		}

		template< typename ValueU >
		RangedValue< T > & operator*=( ValueU const & rhs )noexcept
		{
			if constexpr ( std::is_same_v< RangedValue< T >, ValueU > )
			{
				m_value = m_range.clamp( m_value * rhs.m_value );
			}
			else
			{
				m_value = m_range.clamp( m_value * rhs );
			}

			return *this;
		}

		template< typename ValueU >
		RangedValue< T > & operator/=( ValueU const & rhs )noexcept
		{
			if constexpr ( std::is_same_v< RangedValue< T >, ValueU > )
			{
				m_value = m_range.clamp( m_value / rhs.m_value );
			}
			else
			{
				m_value = m_range.clamp( m_value / rhs );
			}

			return *this;
		}
		/**@}*/

	private:
		//!\~english	The range.
		//!\~french		L'intervalle.
		Range< T > m_range;
		//!\~english	The value.
		//!\~french		La valeur.
		T m_value;

	private:
		/**
		 *\~english
		 *\name Logic operators.
		 *\~french
		 *\name Opérateurs logiques.
		 */
		/**@{*/
		friend bool operator==( RangedValue const & lhs
			, T const & rhs )noexcept
		{
			if constexpr ( std::is_integral_v< T > )
				return lhs.value() == rhs;
			else
				return std::abs( lhs.value() - rhs ) < std::numeric_limits< T >::epsilon();
		}

		friend bool operator>( RangedValue const & lhs
			, T const & rhs )noexcept
		{
			return lhs.value() > rhs;
		}

		friend bool operator<( RangedValue const & lhs
			, T const & rhs )noexcept
		{
			return lhs.value() < rhs;
		}

		friend bool operator>=( RangedValue const & lhs
			, T const & rhs )noexcept
		{
			return !( lhs < rhs );
		}

		friend bool operator<=( RangedValue const & lhs
			, T const & rhs )noexcept
		{
			return !( lhs < rhs );
		}

		friend bool operator==( T const & lhs
			, RangedValue const & rhs )noexcept
		{
			if constexpr ( std::is_integral_v< T > )
				return lhs == rhs.value();
			else
				return std::abs( lhs - rhs.value() ) < std::numeric_limits< T >::epsilon();
		}

		friend bool operator>( T const & lhs
			, RangedValue const & rhs )noexcept
		{
			return lhs > rhs.value();
		}

		friend bool operator<( T const & lhs
			, RangedValue const & rhs )noexcept
		{
			return lhs < rhs.value();
		}

		friend bool operator>=( T const & lhs
			, RangedValue const & rhs )noexcept
		{
			return !( lhs < rhs );
		}

		friend bool operator<=( T const & lhs
			, RangedValue const & rhs )noexcept
		{
			return !( lhs < rhs );
		}

		friend bool operator==( RangedValue const & lhs
			, RangedValue const & rhs )noexcept
		{
			if constexpr ( std::is_integral_v< T > )
				return lhs.value() == rhs.value();
			else
				return std::abs( lhs.value() - rhs.value() ) < std::numeric_limits< T >::epsilon();
		}

		friend bool operator>( RangedValue const & lhs
			, RangedValue const & rhs )noexcept
		{
			return lhs.value() > rhs.value();
		}

		friend bool operator<( RangedValue const & lhs
			, RangedValue const & rhs )noexcept
		{
			return lhs.value() < rhs.value();
		}

		friend bool operator>=( RangedValue const & lhs
			, RangedValue const & rhs )noexcept
		{
			return !( lhs < rhs );
		}

		friend bool operator<=( RangedValue const & lhs
			, RangedValue const & rhs )noexcept
		{
			return !( lhs < rhs );
		}
		/**@}*/
		/**
		 *\~english
		 *\name Arithmetic operators.
		 *\~french
		 *\name Opérateurs arithmétiques.
		 */
		/**@{*/
		friend T operator+( T const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs + rhs.value();
		}

		friend T operator-( T const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs - rhs.value();
		}

		friend T operator*( T const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs * rhs.value();
		}

		friend T operator/( T const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs / rhs.value();
		}

		friend T operator+( RangedValue const & lhs, T const & rhs )noexcept
		{
			return lhs.value() + rhs;
		}

		friend T operator-( RangedValue const & lhs, T const & rhs )noexcept
		{
			return lhs.value() - rhs;
		}

		friend T operator*( RangedValue const & lhs, T const & rhs )noexcept
		{
			return lhs.value() * rhs;
		}

		friend T operator/( RangedValue const & lhs, T const & rhs )noexcept
		{
			return lhs.value() / rhs;
		}

		friend T operator+( RangedValue const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs.value() + rhs.value();
		}

		friend T operator-( RangedValue const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs.value() - rhs.value();
		}

		friend T operator*( RangedValue const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs.value() * rhs.value();
		}

		friend T operator/( RangedValue const & lhs, RangedValue const & rhs )noexcept
		{
			return lhs.value() / rhs.value();
		}
		/**@}*/
	};
	/**
	 *\~english
	 *\brief		Helper function to create a ranged value.
	 *\param[in]	value		The value.
	 *\param[in]	min, max	The range.
	 *\return		The created ranged value.
	 *\~french
	 *\brief		Fonction d'aide à la construction d'une valeur dans un intervalle.
	 *\param[in]	value		La valeur.
	 *\param[in]	min, max	Les bornes de l'intervalle.
	 *\return		L'objet créé.
	 */
	template< typename T >
	inline RangedValue< T > makeRangedValue( T const & value
		, T const & min
		, T const & max )noexcept
	{
		return RangedValue< T >{ value, makeRange( min, max ) };
	}
	
	template< typename T >
	struct IsRangedValueT : std::false_type
	{
	};

	template< typename T >
	struct IsRangedValueT< RangedValue< T > > : std::true_type
	{
	};

	template< typename T >
	static inline bool constexpr isRangedValueT = IsRangedValueT< T >::value;
	
	template< typename T >
	struct RangedValueGetterT
	{
		using Type = T;
	};

	template< typename T >
	struct RangedValueGetterT< RangedValue< T > >
	{
		using Type = T;
	};

	template< typename T >
	using UnRangedValueT = typename RangedValueGetterT< T >::Type;
}

#endif
