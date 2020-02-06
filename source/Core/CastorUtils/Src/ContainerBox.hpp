﻿/*
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
#ifndef ___CASTOR_CONTAINER_BOX_H___
#define ___CASTOR_CONTAINER_BOX_H___

#include "Point.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Box container class
	\remark		A container will be a simple object which surrounds a graphic object (2D or 3D)
				<br />It can be a parallelepiped, a sphere or other.
	\~french
	\brief		Classe de conteneur boîte
	\remark		Un conteneur boîte est un simple objet encadrant un objet graphique (2D ou 3D)
				<br />Ce peut être un parallélépipède, une sphère ou autre.
	*/
	template< uint8_t Dimension >
	class ContainerBox
	{
	protected:
		typedef Castor::Policy< real > policy;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		ContainerBox()
			:	m_ptCenter()
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_container	The container to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_container	Le conteneur à copier
		 */
		ContainerBox( ContainerBox const & p_container )
			:	m_ptCenter( p_container.m_ptCenter	)
		{
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_container	The container to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_container	Le conteneur à déplacer
		 */
		ContainerBox( ContainerBox && p_container )
			:	m_ptCenter( std::move( p_container.m_ptCenter )	)
		{
			p_container.m_ptCenter = Point< real, Dimension >();
		}
		/**
		 *\~english
		 *\brief		Constructor from center
		 *\param[in]	p_ptCenter	The center
		 *\~french
		 *\brief		Constructeur à partir du centre
		 *\param[in]	p_ptCenter	Le centre
		 */
		ContainerBox( Point< real, Dimension > const & p_ptCenter )
			:	m_ptCenter( p_ptCenter	)
		{
		}
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_container	The container to copy
		 *\return		A reference to this container
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_container	Le conteneur à copier
		 *\return		Une référence sur ce conteneur
		 */
		ContainerBox & operator =( ContainerBox const & p_container )
		{
			m_ptCenter = p_container.m_ptCenter;
			return * this;
		}
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_container	The container to copy
		 *\return		A reference to this container
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_container	Le conteneur à copier
		 *\return		Une référence sur ce conteneur
		 */
		ContainerBox & operator =( ContainerBox && p_container )
		{
			if ( this != &p_container )
			{
				m_ptCenter = std::move( p_container.m_ptCenter );
			}

			return * this;
		}
		/**
		 *\~english
		 *\brief		Tests if a vertex is within the container, id est inside it but not on it's limits
		 *\param[in]	p_v	The vertex to test
		 *\return		\p true if the vertex is within the container, false if not
		 *\~french
		 *\brief		Teste si un point est contenu dans le container (mais pas à la limite)
		 *\param[in]	p_v	Le point à tester
		 *\return		\p true si le point est dans le container
		 */
		virtual bool IsWithin( Point< real, Dimension > const & p_v ) = 0;
		/**
		 *\~english
		 *\brief		Tests if a vertex is on the limits of this container, and not within
		 *\param[in]	p_v	The vertex to test
		 *\return		\p true if the vertex is on the limits of the container, false if not
		 *\~french
		 *\brief		Teste si un point est sur la limite du container, et pas dedans
		 *\param[in]	p_v	Le point à tester
		 *\return		\p true si le point est sur la limite
		 */
		virtual bool IsOnLimits( Point< real, Dimension > const & p_v ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the center of this container
		 *\return		A constant reference to the center
		 *\~french
		 *\brief		Récupère le centre de ce conteneur
		 *\return		Une référence constante sur le centre
		 */
		inline Point< real, Dimension > const &	GetCenter()const
		{
			return m_ptCenter;
		}

	protected:
		//!\~english The center of the container	\~french Le centre de ce conteneur
		Point< real, Dimension > m_ptCenter;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Typedef on a 2 dimensions ContainerBox
	\~french
	\brief		Typedef sur une ContainerBox à 2 dimensions
	*/
	typedef ContainerBox< 2 >	ContainerBox2D;
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Typedef on a 3 dimensions ContainerBox
	\~french
	\brief		Typedef sur une ContainerBox à 3 dimensions
	*/
	typedef ContainerBox< 3 >	ContainerBox3D;
}

#endif
