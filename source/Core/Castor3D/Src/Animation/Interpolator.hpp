/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_INTERPOLATOR_H___
#define ___C3D_INTERPOLATOR_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	\~french
	\brief		Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< class Type >
	class Interpolator
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	p_src		The start.
		 *\param[in]	p_dst		The end.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 *\param[in]	p_dst		L'arrivée.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		C3D_API virtual Type Interpolate( Type const & p_src, Type const & p_dst, real p_percent )const = 0;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Template class which handles the interpolations, would it be quaternion, point, real, ... interpolations
	\~french
	\brief		Classe modèle pour gérer les interpolations (de point, quaternion, real, ...)
	*/
	template< class Type, InterpolatorType Mode > class InterpolatorT;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		24/01/2016
	\~english
	\brief		Partial specialisation of Interpolator, for eINTERPOLATOR_MODE_NONE.
	\~french
	\brief		Spécialisation partielle de Interpolator, pour eINTERPOLATOR_MODE_NONE.
	*/
	template< class Type >
	class InterpolatorT< Type, InterpolatorType::Nearest >
		: public Interpolator< Type >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	p_src		The start.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 */
		inline Type Interpolate( Type const & p_src, Type const &, real )const
		{
			return p_src;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		24/01/2016
	\~english
	\brief		Partial specialisation of Interpolator, for eINTERPOLATOR_MODE_LINEAR.
	\~french
	\brief		Spécialisation partielle de Interpolator, pour eINTERPOLATOR_MODE_LINEAR.
	*/
	template< class Type >
	class InterpolatorT< Type, InterpolatorType::Linear >
		: public Interpolator< Type >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		inline InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	p_src		The start.
		 *\param[in]	p_dst		The end.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 *\param[in]	p_dst		L'arrivée.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		inline Type Interpolate( Type const & p_src, Type const & p_dst, real p_percent )const
		{
			Type l_return;

			if ( p_percent <= 0.0 )
			{
				l_return = p_src;
			}
			else if ( p_percent >= 1.0 )
			{
				l_return = p_dst;
			}
			else
			{
				l_return = p_src + ( ( p_dst - p_src ) * p_percent );
			}

			return l_return;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which handles the Quaternion linear interpolations.
	\~french
	\brief		Classe pour gérer les interpolations linéaires de Quaternion.
	*/
	template<>
	class InterpolatorT< Castor::Quaternion, InterpolatorType::Linear >
		: public Interpolator< Castor::Quaternion >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		inline InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~InterpolatorT()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	p_src		The start.
		 *\param[in]	p_dst		The end.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_src		Le départ.
		 *\param[in]	p_dst		L'arrivée.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		inline Castor::Quaternion Interpolate( Castor::Quaternion const & p_src, Castor::Quaternion const & p_dst, real p_percent )const
		{
			Castor::Quaternion l_return;

			if ( p_percent <= 0.0 )
			{
				l_return = p_src;
			}
			else if ( p_percent >= 1.0 )
			{
				l_return = p_dst;
			}
			else
			{
				l_return = p_src.slerp( p_dst, p_percent );
			}

			return l_return;
		}
	};
}

#endif
