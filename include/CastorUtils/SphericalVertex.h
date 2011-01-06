/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Castor_SphericalVertex___
#define ___Castor_SphericalVertex___

#include "Point.h"

namespace Castor
{	namespace Math
{
	//! Vertex, expressed in rho theta and phi (euler angles)
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	class SphericalVertex : public MemoryTraced<SphericalVertex>
	{
	public:
		real m_radius;	//!< The radius (distance to origin)
		real m_phi;		//!< The xy angle
		real m_theta;	//!< The xz angle

	public:
		/**
		 * Specified constructor
		 */
		SphericalVertex( real p_radius=0, real p_phi=0, real p_theta=0);
		/**
		* Copy constructor
		*/
		SphericalVertex( const SphericalVertex & p_vertex);
		/**
		* Constructor from Point3r
		*/
		SphericalVertex( const Point3r & p_vertex);
		/**
		 * Destructor
		 */
		~SphericalVertex();

	public:
	};
}
}

#endif