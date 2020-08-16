/*
See LICENSE file in root folder
*/
#ifndef ___CastorUtils_Range_HPP___
#define ___CastorUtils_Range_HPP___
#pragma once

#include "CastorUtils/Math/MathModule.hpp"

#include <cmath>
#include <limits>

#ifdef min
#	undef min
#	undef max
#endif

namespace castor
{
	template< typename T >
	class Range
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	min, max	The range.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	min, max	Les bornes de l'intervalle.
		 */
		inline Range( T const & min, T const & max )noexcept
			: m_min{ std::min( min, max ) }
			, m_max{ std::max( min, max ) }
		{
		}
		/**
		 *\~english
		 *\brief		Puts a value in the range.
		 *\param[in]	value	The value.
		 *\return		\p m_min if \p value is less than it.\n
		 *				\p m_max if \p value is greater than it.\n
		 *				\p value otherwise.
		 *\~french
		 *\brief		Remet une valeur dans l'intervalle.
		 *\param[in]	value	La valeur.
		 *\return		\p m_min si la valeur y est inférieure.\n
		 *				\p m_max si la valeur y est supérieure.\n
		 *				\p value sinon.
		 */
		T const & clamp( T const & value )const noexcept
		{
			if ( value < m_min )
			{
				return m_min;
			}

			if ( value > m_max )
			{
				return m_max;
			}

			return value;
		}
		/**
		 *\~english
		 *\return		\p true if \p value is inside the range.
		 *\~french
		 *\return		\p true si \p value est dans l'intervalle.
		 */
		bool has( T const & value )const noexcept
		{
			return value >= m_min && value <= m_max;
		}
		/**
		 *\~english
		 *\brief		Gives the percentage matching the given value.
		 *\param[in]	value	The value.
		 *\return		0.0 if \p value <= \p m_min.\n
		 *				1.0 if \p value >= \p m_max.\n
		 *				A value from 0.0 to 1.0, depending on how close the value is
		 *				from \p m_min or from \p m_max.
		 *\~french
		 *\brief		Retourne le pourcentage correspondant à la valeur donnée.
		 *\param[in]	value	La valeur.
		 *\return		0.0 si \p value est égal à ou inférieur à \p m_min.\n
		 *				1.0 si \p value est égal à ou supérieur à \p m_max.\n
		 *				Une pourcentage allant de 0.0 à 1.0, selon que la valeur est plus
		 *				proche de \p m_min ou de \p m_max.
		 */
		float percent( T const & value )const noexcept
		{
			return ( m_min == m_max )
				? 0.0f
				: float( m_min - clamp( value ) ) / float( m_min - m_max );
		}
		/**
		 *\~english
		 *\brief		Gives the percentage matching the given value.
		 *\param[in]	value	The value.
		 *\return		1.0 if \p value <= \p m_min.\n
		 *				0.0 if \p value >= \p m_max.\n
		 *				A value from 0.0 to 1.0, depending on how close the value is
		 *				from \p m_max or from \p m_min.
		 *\~french
		 *\brief		Retourne le pourcentage correspondant à la valeur donnée.
		 *\param[in]	value	La valeur.
		 *\return		0.0 si \p value est égal à ou supérieur à \p m_max.\n
		 *				1.0 si \p value est égal à ou inférieur à \p m_min.\n
		 *				Une pourcentage allant de 0.0 à 1.0, selon que la valeur est plus
		 *				proche de \p m_max ou de \p m_min.
		 */
		float invPercent( T const & value )const noexcept
		{
			return 1.0f - percent( value );
		}
		/**
		 *\~english
		 *\brief		Gives the value matching the given percentage.
		 *\param[in]	percent	The percentage.
		 *\return		\p m_min if \p percent <= \p 0.0.\n
		 *				\p m_max if \p percent >= \p 1.0.\n
		 *				A value between \p m_min and \p m_max.
		 *\~english
		 *\~french
		 *\brief		Retourne la valeur correspondant au pourcentage donné.
		 *\param[in]	percent	Le pourcentage.
		 *\return		\p m_min si \p percent vaut 0.0.\n
		 *				\p m_max si \p percent vaut 1.0.\n
		 *				Une valeur comprise entre \p m_min et \p m_max.
		 */
		T value( float const & percent )const noexcept
		{
			return T( m_min + percent * float( m_max - m_min ) );
		}
		/**
		 *\~english
		 *\brief		Gives the value matching the given percentage.
		 *\param[in]	percent	The percentage.
		 *\return		\p m_max if \p percent <= \p 0.0.\n
		 *				\p m_min if \p percent >= \p 1.0.\n
		 *				A value between \p m_min and \p m_max.
		 *\~english
		 *\~french
		 *\brief		Retourne la valeur correspondant au pourcentage donné.
		 *\param[in]	percent	Le pourcentage.
		 *\return		\p m_max si \p percent vaut 0.0.\n
		 *				\p m_min si \p percent vaut 1.0.\n
		 *				Une valeur comprise entre \p m_min et \p m_max.
		 */
		T invValue( float const & percent )const noexcept
		{
			return T{ m_min + ( 1.0f - percent ) * float( m_max - m_min ) };
		}
		/**
		 *\~english
		 *\return		The lower bound.
		 *\~french
		 *\return		La borne minimale.
		 */
		inline T const & getMin()const noexcept
		{
			return m_min;
		}
		/**
		 *\~english
		 *\return		The upper bound.
		 *\~french
		 *\return		La borne maximale.
		 */
		inline T const & getMax()const noexcept
		{
			return m_max;
		}

	private:
		//!\~english	The upper bound.
		//!\~french		La borne inférieure.
		T m_min;
		//!\~english	The lower bound.
		//!\~french		La borne supérieure.
		T m_max;
	};
	/**
	 *\~english
	 *\brief		Helper function to create a range.
	 *\param[in]	min, max	The range.
	 *\return		The created range.
	 *\~french
	 *\brief		Fonction d'aide à la construction d'un intervalle.
	 *\param[in]	min, max	Les bornes de l'intervalle.
	 *\return		L'intervalle créé.
	 */
	template< typename T >
	inline Range< T > makeRange( T const & min, T const & max )noexcept
	{
		return Range< T >( min, max );
	}
}

#endif
