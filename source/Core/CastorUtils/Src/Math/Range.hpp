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
#ifndef ___CastorUtils_Range_HPP___
#define ___CastorUtils_Range_HPP___
#pragma once

#include "CastorUtilsPrerequisites.hpp"

#include <cmath>
#include <limits>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		16/02/2017
	\~english
	\brief		A range class.
	\~french
	\brief		Une classe d'intervalle.
	*/
	template< typename T >
	class Range
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_min, p_max	The range.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_min, p_max	Les bornes de l'intervalle.
		 */
		inline Range( T const & p_min, T const & p_max )noexcept
			: m_min{ std::min( p_min, p_max ) }
			, m_max{ std::max( p_min, p_max ) }
		{
			assert( m_min != m_max );
		}
		/**
		 *\~english
		 *\brief		Puts a value in the range.
		 *\param[in]	p_value	The value.
		 *\return		\p m_min if \p p_value is less than it.\n
		 *				\p m_max if \p p_value is greater than it.\n
		 *				\p p_value otherwise.
		 *\~french
		 *\brief		Remet une valeur dans l'intervalle.
		 *\param[in]	p_value	La valeur.
		 *\return		\p m_min si la valeur y est inférieure.\n
		 *				\p m_max si la valeur y est supérieure.\n
		 *				\p value sinon.
		 */
		T const & clamp( T const & p_value )const noexcept
		{
			if ( p_value < m_min )
			{
				return m_min;
			}

			if ( p_value > m_max )
			{
				return m_max;
			}

			return p_value;
		}
		/**
		 *\~english
		 *\brief		Gives the percentage matching the given value.
		 *\param[in]	p_value	The value.
		 *\return		0.0 if \p p_value <= \p m_min.\n
		 *				1.0 if \p p_value >= \p m_max.\n
		 *				A value from 0.0 to 1.0, depending on how close the value is
		 *				from \p m_min or from \p m_max.
		 *\~french
		 *\brief		Retourne le pourcentage correspondant à la valeur donnée.
		 *\param[in]	p_value	La valeur.
		 *\return		0.0 si \p value est égal à ou inférieur à \p m_min.\n
		 *				1.0 si \p value est égal à ou supérieur à \p m_max.\n
		 *				Une pourcentage allant de 0.0 à 1.0, selon que la valeur est plus
		 *				proche de \p m_min ou de \p m_max.
		 */
		float percent( T const & p_value )const noexcept
		{
			return float( m_min - clamp( p_value ) ) / float( m_min - m_max );
		}
		/**
		 *\~english
		 *\brief		Gives the percentage matching the given value.
		 *\param[in]	p_value	The value.
		 *\return		1.0 if \p p_value <= \p m_min.\n
		 *				0.0 if \p p_value >= \p m_max.\n
		 *				A value from 0.0 to 1.0, depending on how close the value is
		 *				from \p m_max or from \p m_min.
		 *\~french
		 *\brief		Retourne le pourcentage correspondant à la valeur donnée.
		 *\param[in]	p_value	La valeur.
		 *\return		0.0 si \p value est égal à ou supérieur à \p m_max.\n
		 *				1.0 si \p value est égal à ou inférieur à \p m_min.\n
		 *				Une pourcentage allant de 0.0 à 1.0, selon que la valeur est plus
		 *				proche de \p m_max ou de \p m_min.
		 */
		float inv_percent( T const & p_value )const noexcept
		{
			return 1.0f - percent( p_value );
		}
		/**
		 *\~english
		 *\brief		Gives the value matching the given percentage.
		 *\param[in]	p_percent	The percentage.
		 *\return		\p m_min if \p p_percent <= \p 0.0.\n
		 *				\p m_max if \p p_percent >= \p 1.0.\n
		 *				A value between \p m_min and \p m_max.
		 *\~english
		 *\~french
		 *\brief		Retourne la valeur correspondant au pourcentage donné.
		 *\param[in]	p_percent	Le pourcentage.
		 *\return		\p m_min si \p p_percent vaut 0.0.\n
		 *				\p m_max si \p p_percent vaut 1.0.\n
		 *				Une valeur comprise entre \p m_min et \p m_max.
		 */
		T value( float const & p_percent )const noexcept
		{
			return T( m_min + p_percent * float( m_max - m_min ) );
		}
		/**
		 *\~english
		 *\brief		Gives the value matching the given percentage.
		 *\param[in]	p_percent	The percentage.
		 *\return		\p m_max if \p p_percent <= \p 0.0.\n
		 *				\p m_min if \p p_percent >= \p 1.0.\n
		 *				A value between \p m_min and \p m_max.
		 *\~english
		 *\~french
		 *\brief		Retourne la valeur correspondant au pourcentage donné.
		 *\param[in]	p_percent	Le pourcentage.
		 *\return		\p m_max si \p percent vaut 0.0.\n
		 *				\p m_min si \p percent vaut 1.0.\n
		 *				Une valeur comprise entre \p m_min et \p m_max.
		 */
		T inv_value( float const & p_percent )const noexcept
		{
			return T{ m_min + ( 1.0f - p_percent ) * float( m_max - m_min ) };
		}
		/**
		 *\~english
		 *\return		The lower bound.
		 *\~french
		 *\return		La borne minimale.
		 */
		inline T const & min()const noexcept
		{
			return m_min;
		}
		/**
		 *\~english
		 *\return		The upper bound.
		 *\~french
		 *\return		La borne maximale.
		 */
		inline T const & max()const noexcept
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
	 *\param[in]	p_min, p_max	The range.
	 *\return		The created range.
	 *\~french
	 *\brief		Fonction d'aide à la construction d'un intervalle.
	 *\param[in]	p_min, p_max	Les bornes de l'intervalle.
	 *\return		L'intervalle créé.
	 */
	template< typename T >
	inline Range< T > makeRange( T const & p_min, T const & p_max )noexcept
	{
		return Range< T >( p_min, p_max );
	}
}

#endif
