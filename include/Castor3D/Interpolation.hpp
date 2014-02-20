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
#ifndef ___Interpolation___
#define ___Interpolation___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	template< class Type, eINTERPOLATOR_MODE Mode > class C3D_API Interpolator
	{
	protected:
		//!\~english	The starting value	\~french	La valeur de départ
		Type m_tSrc;
		//!\~english	The current value	\~french	La valeur courante
		Type m_tCurrent;
		//!\~english	The ending value	\~french	La valeur d'arrivée
		Type m_tDest;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_tSrc	The start
		 *\param[in]	p_tDest	The end
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_tSrc	Le départ
		 *\param[in]	p_tDest	L'arrivée
		 */
		Interpolator( Type const & p_tSrc, Type const & p_tDest )
			:	m_tSrc		( p_tSrc	)
			,	m_tCurrent	( p_tSrc	)
			,	m_tDest		( p_tDest	)
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Interpolator()
		{
		}
		/**
		 *\~english
		 *\brief		Interpolation function
		 *\remark		Must be implemented by the interpolator you create
		 *\param[in]	p_rPercent	The percentage
		 *\~french
		 *\brief		Fonction d'interpolation
		 *\remark		Doit être implémentée par l'interpolateur que vous créez
		 *\param[in]	p_rPercent	Le pourcentage
		 */
		Type const & Interpolate( real p_rPercent )
		{
			if( p_rPercent <= 0.0 )
			{
				m_tCurrent = m_tSrc;
			}
			else if( p_rPercent >= 1.0 )
			{
				m_tCurrent = m_tDest;
			}
			else
			{
				m_tCurrent = m_tSrc + ((m_tDest - m_tSrc) * p_rPercent);
			}

			return m_tCurrent;
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which handles the Quaternion linear interpolations
	\~french
	\brief		Classe pour gérer les interpolations linéaires de Quaternion
	*/
	template<> class C3D_API Interpolator< Castor::Quaternion, eINTERPOLATOR_MODE_LINEAR >
	{
	protected:
		//!\~english	The starting value	\~french	La valeur de départ
		Castor::Quaternion m_tSrc;
		//!\~english	The current value	\~french	La valeur courante
		Castor::Quaternion m_tCurrent;
		//!\~english	The ending value	\~french	La valeur d'arrivée
		Castor::Quaternion m_tDest;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_qSrc	The start
		 *\param[in]	p_qDest	The end
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_qSrc	Le départ
		 *\param[in]	p_qDest	L'arrivée
		 */
		Interpolator( Castor::Quaternion const & p_qSrc, Castor::Quaternion const & p_qDest )
			:	m_tSrc		( p_qSrc	)
			,	m_tCurrent	( p_qSrc	)
			,	m_tDest		( p_qDest	)
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Interpolator()
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
		Castor::Quaternion const & Interpolate( real p_rPercent )
		{
			m_tCurrent = m_tSrc.Slerp( m_tDest, p_rPercent, true );
//			m_tCurrent = m_tCurrent * (m_tDest - m_tSrc).Slerp( Castor::Quaternion::Identity(), 1.0f - p_rPercent, true );
			return m_tCurrent;
		}
	};
}

#pragma warning( pop )

#endif
