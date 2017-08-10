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
#ifndef ___CASTOR_SPHERICAL_VERTEX_H___
#define ___CASTOR_SPHERICAL_VERTEX_H___

#include "Point.hpp"

namespace castor
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
		real m_radius;
		//!\~english The xy angle	\~french Angle sur le plan XY
		real m_phi;
		//!\~english The xz angle	\~french Angle sur le plan XZ
		real m_theta;
	};
}

#endif
