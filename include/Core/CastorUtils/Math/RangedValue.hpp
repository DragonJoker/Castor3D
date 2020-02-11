/*
See LICENSE file in root folder
*/
#ifndef ___CastorUtils_RangedValue_HPP___
#define ___CastorUtils_RangedValue_HPP___
#pragma once

#include "CastorUtils/Math/Range.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\date		16/02/2017
	\~english
	\brief		A value inside a range.
	\remarks	The value can't get out of its range.
	\~french
	\brief		Représente une valeur dans un intervalle.
	\remarks	La valeur ne peut pas sortir de son intervalle.
	*/
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
		inline RangedValue( T const & value
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
		inline RangedValue & operator=( T const & value )noexcept
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
			m_range.clamp( m_value );
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
		float percent()const noexcept
		{
			return m_range.percent( m_value );
		}
		/**
		 *\~english
		 *\return		The value.
		 *\~french
		 *\return		La valeur.
		 */
		inline T const & value()const noexcept
		{
			return m_value;
		}
		/**
		 *\~english
		 *\return		The range.
		 *\~french
		 *\return		L'intervalle.
		 */
		inline Range< T > const & range()const noexcept
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
		inline RangedValue< T > & operator+=( RangedValue< T > const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value + rhs.m_value );
			return *this;
		}

		inline RangedValue< T > & operator-=( RangedValue< T > const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value - rhs.m_value );
			return *this;
		}

		inline RangedValue< T > & operator*=( RangedValue< T > const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value * rhs.m_value );
			return *this;
		}

		inline RangedValue< T > & operator/=( RangedValue< T > const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value / rhs.m_value );
			return *this;
		}

		inline RangedValue< T > & operator+=( T const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value + rhs );
			return *this;
		}

		inline RangedValue< T > & operator-=( T const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value - rhs );
			return *this;
		}

		inline RangedValue< T > & operator*=( T const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value * rhs );
			return *this;
		}

		inline RangedValue< T > & operator/=( T const & rhs )noexcept
		{
			m_value = m_range.clamp( m_value / rhs );
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
	/**
	 *\~english
	 *\name Logic operators.
	 *\~french
	 *\name Opérateurs logiques.
	 */
	/**@{*/
	namespace details
	{
		template< typename T >
		struct Equal
		{
			inline bool operator()( RangedValue< T > const & lhs
				, RangedValue< T > const & rhs )const noexcept
			{
				static constexpr auto eps = std::numeric_limits< T >::epsilon();
				return std::abs( lhs.value() - rhs.value() ) < eps;
			}
		};

		template<>
		struct Equal< uint32_t >
		{
			inline bool operator()( RangedValue< uint32_t > const & lhs
				, RangedValue< uint32_t > const & rhs )const noexcept
			{
				return lhs.value() == rhs.value();
			}
		};
	}

	template< typename T >
	inline bool operator==( RangedValue< T > const & lhs
		, T const & rhs )noexcept
	{
		static constexpr auto eps = std::numeric_limits< T >::epsilon();
		return std::abs( lhs.value() - rhs ) < eps;
	}

	template< typename T >
	inline bool operator!=( RangedValue< T > const & lhs
		, T const & rhs )noexcept
	{
		return !( lhs == rhs );
	}

	template< typename T >
	inline bool operator>( RangedValue< T > const & lhs
		, T const & rhs )noexcept
	{
		return lhs.value() > rhs;
	}

	template< typename T >
	inline bool operator<( RangedValue< T > const & lhs
		, T const & rhs )noexcept
	{
		return lhs.value() < rhs;
	}

	template< typename T >
	inline bool operator>=( RangedValue< T > const & lhs
		, T const & rhs )noexcept
	{
		return !( lhs < rhs );
	}

	template< typename T >
	inline bool operator<=( RangedValue< T > const & lhs
		, T const & rhs )noexcept
	{
		return !( lhs < rhs );
	}

	template< typename T >
	inline bool operator==( T const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		static constexpr auto eps = std::numeric_limits< T >::epsilon();
		return std::abs( lhs - rhs.value() ) < eps;
	}

	template< typename T >
	inline bool operator!=( T const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return !( lhs == rhs );
	}

	template< typename T >
	inline bool operator>( T const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs > rhs.value();
	}

	template< typename T >
	inline bool operator<( T const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs < rhs.value();
	}

	template< typename T >
	inline bool operator>=( T const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return !( lhs < rhs );
	}

	template< typename T >
	inline bool operator<=( T const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return !( lhs < rhs );
	}

	template< typename T >
	inline bool operator==( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		static const details::Equal< T > equals;
		return equals( lhs, rhs );
	}

	template< typename T >
	inline bool operator!=( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return !( lhs == rhs );
	}

	template< typename T >
	inline bool operator>( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs.value() > rhs.value();
	}

	template< typename T >
	inline bool operator<( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs.value() < rhs.value();
	}

	template< typename T >
	inline bool operator>=( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return !( lhs < rhs );
	}

	template< typename T >
	inline bool operator<=( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
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
	template< typename T >
	inline T operator+( T const & lhs, RangedValue< T > const & rhs )noexcept
	{
		return lhs + rhs.value();
	}

	template< typename T >
	inline T operator-( T const & lhs, RangedValue< T > const & rhs )noexcept
	{
		return lhs - rhs.value();
	}

	template< typename T >
	inline T operator*( T const & lhs, RangedValue< T > const & rhs )noexcept
	{
		return lhs * rhs.value();
	}

	template< typename T >
	inline T operator/( T const & lhs, RangedValue< T > const & rhs )noexcept
	{
		return lhs / rhs.value();
	}

	template< typename T >
	inline T operator+( RangedValue< T > const & lhs, T const & rhs )noexcept
	{
		return lhs.value() + rhs;
	}

	template< typename T >
	inline T operator-( RangedValue< T > const & lhs, T const & rhs )noexcept
	{
		return lhs.value() - rhs;
	}

	template< typename T >
	inline T operator*( RangedValue< T > const & lhs, T const & rhs )noexcept
	{
		return lhs.value() * rhs;
	}

	template< typename T >
	inline T operator/( RangedValue< T > const & lhs, T const & rhs )noexcept
	{
		return lhs.value() / rhs;
	}

	template< typename T >
	inline T operator+( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs.value() + rhs.value();
	}

	template< typename T >
	inline T operator-( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs.value() - rhs.value();
	}

	template< typename T >
	inline T operator*( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs.value() * rhs.value();
	}

	template< typename T >
	inline T operator/( RangedValue< T > const & lhs
		, RangedValue< T > const & rhs )noexcept
	{
		return lhs.value() / rhs.value();
	}
	/**@}*/
}

#endif
