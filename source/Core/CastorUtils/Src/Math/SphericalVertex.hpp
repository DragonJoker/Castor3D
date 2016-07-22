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
#ifndef ___CASTOR_SPHERICAL_VERTEX_H___
#define ___CASTOR_SPHERICAL_VERTEX_H___

#include "Point.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Vertex, expressed in rho theta and phi (Euler angles)
	\~french
	\brief		Vertex, exprimé en rho theta et phi (angles d'Euler)
	*/
	class SphericalVertex
	{
	public:
		/**
		 *\~english
		 *\brief		Specified Constructor
		 *\param[in]	p_radius	The distance from origin
		 *\param[in]	p_phi		Phi angle
		 *\param[in]	p_theta		Theta angle
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_radius	Distance à l'origine
		 *\param[in]	p_phi		Angle Phi
		 *\param[in]	p_theta		Angle Theta
		 */
		CU_API SphericalVertex( real p_radius = 0, real p_phi = 0, real p_theta = 0 );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_vertex	The vertex to copy
		 *\~french
		 *\brief		Constructeur par recopie
		 *\param[in]	p_vertex	Le vertex à copier
		 */
		CU_API SphericalVertex( SphericalVertex const & p_vertex );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_vertex	The vertex to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_vertex	Le vertex à déplacer
		 */
		CU_API SphericalVertex( SphericalVertex && p_vertex );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_vertex	The vertex to copy
		 *\return		A reference to this SphericalVertex
		 *\~french
		 *\brief		Opérateur d'affectation par recopie
		 *\param[in]	p_vertex	Le vertex à copier
		 *\return		Une référence sur ce SphericalVertex
		 */
		CU_API SphericalVertex & operator =( SphericalVertex const & p_vertex );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_vertex	The vertex to move
		 *\return		A reference to this SphericalVertex
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_vertex	Le vertex à déplacer
		 *\return		Une référence sur ce SphericalVertex
		 */
		CU_API SphericalVertex & operator =( SphericalVertex && p_vertex );
		/**
		 *\~english
		 *\brief		Constructor from a cartesian point
		 *\param[in]	p_vertex	The vertex to convert
		 *\~french
		 *\brief		Constructeur à partir d'un point cartésien
		 *\param[in]	p_vertex	Le vertex à convertir
		 */
		CU_API SphericalVertex( Point3r const & p_vertex );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~SphericalVertex();

	public:
		//!\~english The radius (distance to origin)	\~french Rayon (distance à l'origine)
		real m_rRadius;
		//!\~english The xy angle	\~french Angle sur le plan XY
		real m_rPhi;
		//!\~english The xz angle	\~french Angle sur le plan XZ
		real m_rTheta;
	};
}

#endif
