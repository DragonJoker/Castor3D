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
#ifndef ___C3D_QUATERNION_INTERPOLATOR_H___
#define ___C3D_QUATERNION_INTERPOLATOR_H___

#include "Interpolator.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which handles the Quaternion linear interpolations
	\~french
	\brief		Classe pour gérer les interpolations linéaires de Quaternion
	*/
	template<>
	class Interpolator< Castor::Quaternion, eINTERPOLATOR_MODE_LINEAR >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_src	The start
		 *\param[in]	p_dst	The end
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_qSrc	Le départ
		 *\param[in]	p_dst	L'arrivée
		 */
		C3D_API Interpolator( Castor::Quaternion const & p_src, Castor::Quaternion const & p_dst )
			: m_src( p_src )
			, m_cur( p_src )
			, m_dst( p_dst )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function
		 *\param[in]	p_rPercent	The percentage
		 *\~french
		 *\brief		Fonction d'interpolation
		 *\param[in]	p_rPercent	Le pourcentage
		 */
		C3D_API Castor::Quaternion const & operator()( real p_percent )
		{
			m_cur = m_src.slerp( m_dst, p_percent );
			return m_cur;
		}

	protected:
		//!\~english The starting value	\~french La valeur de départ
		Castor::Quaternion m_src;
		//!\~english The current value	\~french La valeur courante
		Castor::Quaternion m_cur;
		//!\~english The ending value	\~french La valeur d'arrivée
		Castor::Quaternion m_dst;
	};
}

#endif
