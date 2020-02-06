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
#ifndef ___CASTOR_SPHERE_BOX_H___
#define ___CASTOR_SPHERE_BOX_H___

#include "ContainerBox.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Sphere container class
	\~french
	\brief		Classe de conteneur sphérique
	*/
	class SphereBox
		:	public ContainerBox3D
	{
	private:
		//!\~english The radius of the sphere	\~french Le rayon de la sphère
		real m_radius;

	public:
		/**
		 *\~english
		 *\brief		Default constructor
		 *\~french
		 *\brief		Constructeur par défaut
		 */
		SphereBox()
			:	ContainerBox3D(	)
			,	m_radius( 0	)
		{
		}
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_sphere	The sphere box to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_sphere	La sphere box à copier
		 */
		SphereBox( SphereBox const & p_sphere )
			:	ContainerBox3D( p_sphere	)
			,	m_radius( p_sphere.m_radius	)
		{
		}
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_sphere	The sphere box to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_sphere	La sphere box à déplacer
		 */
		SphereBox( SphereBox && p_sphere )
			:	ContainerBox3D( std::move( p_sphere )	)
			,	m_radius( std::move( p_sphere.m_radius )	)
		{
			p_sphere.m_radius = 0;
		}
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	p_center	The center of the sphere
		 *\param[in]	p_radius	The radius of the sphere
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_center	Le centre de la sphère
		 *\param[in]	p_radius	Le rayon de la sphère
		 */
		SphereBox( Point3r const & p_center, real p_radius )
			:	ContainerBox3D( p_center	)
			,	m_radius( p_radius	)
		{
		}
		/**
		 *\~english
		 *\brief		Constructor from a CubeBox
		 *\param[in]	p_box	The CubeBox
		 *\~french
		 *\brief		Constructeur à partir d'une CubeBox
		 *\param[in]	p_box	La CubeBox
		 */
		SphereBox( CubeBox const & p_box )
			:	ContainerBox3D( p_box.GetCenter()	)
			,	m_radius( real( point::distance( p_box.GetMax() - m_ptCenter ) )	)
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
		SphereBox & operator =( SphereBox const & p_container )
		{
			ContainerBox3D::operator =( p_container );
			m_radius = p_container.m_radius;
			return *this;
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
		SphereBox & operator =( SphereBox && p_container )
		{
			ContainerBox3D::operator =( std::move( p_container ) );

			if ( this != &p_container )
			{
				m_radius = std::move( p_container.m_radius );
			}

			return *this;
		}
		/**
		 *\~english
		 *\brief		Reinitialises the sphere box
		 *\param[in]	p_center	The center
		 *\param[in]	p_radius	The radius
		 *\~french
		 *\brief		Réinitialise la sphere box
		 *\param[in]	p_center	Le centre
		 *\param[in]	p_radius	Le rayon
		 */
		void Load( Point3r const & p_center, real p_radius )
		{
			m_ptCenter = p_center;
			m_radius = p_radius;
		}
		/**
		 *\~english
		 *\brief		Reinitialises the sphere box from a CubeBox
		 *\param[in]	p_box	The CubeBox
		 *\~french
		 *\brief		Réinitialise à partir d'une CubeBox
		 *\param[in]	p_box	La CubeBox
		 */
		void Load( CubeBox const & p_box )
		{
			m_ptCenter = p_box.GetCenter();
			m_radius = real( point::distance( p_box.GetMax() - m_ptCenter ) );
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
		virtual bool IsWithin( Point3r const & p_v )
		{
			return point::distance( p_v - m_ptCenter ) < m_radius;
		}
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
		virtual bool IsOnLimits( Point3r const & p_v )
		{
			return policy::equals( real( point::distance( p_v - m_ptCenter ) ), m_radius );
		}
		/**
		 *\~english
		 *\brief		Retrieves the radius
		 *\return		The radius
		 *\~french
		 *\brief		Récupère le rayon
		 *\return		Le rayon
		 */
		inline real	GetRadius()const
		{
			return m_radius;
		}
	};
}

#endif
