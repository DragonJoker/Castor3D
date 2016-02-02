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
	template< class Type, eINTERPOLATOR_MODE Mode > class Interpolator;
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
	class Interpolator< Type, eINTERPOLATOR_MODE_NONE >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_src	The start.
		 *\param[in]	p_dst	The end.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_src	Le départ.
		 *\param[in]	p_dst	L'arrivée.
		 */
		Interpolator( Type const & p_src, Type const & CU_PARAM_UNUSED( p_dst ) )
			: m_src( p_src )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		Type const & Interpolate( real CU_PARAM_UNUSED( p_percent ) )
		{
			return m_src;
		}

	protected:
		//!\~english The starting value.	\~french La valeur de départ.
		Type m_src;
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
	class Interpolator< Type, eINTERPOLATOR_MODE_LINEAR >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_src	The start.
		 *\param[in]	p_dst	The end.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_src	Le départ.
		 *\param[in]	p_dst	L'arrivée.
		 */
		Interpolator( Type const & p_src, Type const & p_dst )
			: m_src( p_src )
			, m_cur( p_src )
			, m_dst( p_dst )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function.
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		Type const & Interpolate( real p_percent )
		{
			if ( p_percent <= 0.0 )
			{
				m_cur = m_src;
			}
			else if ( p_percent >= 1.0 )
			{
				m_cur = m_dst;
			}
			else
			{
				m_cur = m_src + ( ( m_dst - m_src ) * p_percent );
			}

			return m_cur;
		}

	protected:
		//!\~english The starting value.	\~french La valeur de départ.
		Type m_src;
		//!\~english The current value.	\~french La valeur courante.
		Type m_cur;
		//!\~english The ending value.	\~french La valeur d'arrivée.
		Type m_dst;
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
	class Interpolator< Castor::Quaternion, eINTERPOLATOR_MODE_LINEAR >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_src	The start.
		 *\param[in]	p_dst	The end.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_src	Le départ.
		 *\param[in]	p_dst	L'arrivée.
		 */
		inline Interpolator( Castor::Quaternion const & p_src, Castor::Quaternion const & p_dst )
			: m_src( p_src )
			, m_cur( p_src )
			, m_dst( p_dst )
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
		 *\param[in]	p_percent	The percentage.
		 *\~french
		 *\brief		Fonction d'interpolation.
		 *\param[in]	p_percent	Le pourcentage.
		 */
		inline Castor::Quaternion const & Interpolate( real p_percent )
		{
			if ( p_percent <= 0.0 )
			{
				m_cur = m_src;
			}
			else if ( p_percent >= 1.0 )
			{
				m_cur = m_dst;
			}
			else
			{
				m_cur = m_src.slerp( m_dst, p_percent );
			}

			return m_cur;
		}

	protected:
		//!\~english The starting value.	\~french La valeur de départ.
		Castor::Quaternion m_src;
		//!\~english The current value.	\~french La valeur courante.
		Castor::Quaternion m_cur;
		//!\~english The ending value.	\~french La valeur d'arrivée.
		Castor::Quaternion m_dst;
	};
}

#endif
