/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SPHERICAL_VERTEX_H___
#define ___CASTOR_SPHERICAL_VERTEX_H___

#include "CastorUtils/Math/Point.hpp"

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
		 *\param[in]	radius	The distance from origin
		 *\param[in]	phi		Phi angle
		 *\param[in]	theta	Theta angle
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	radius	Distance à l'origine
		 *\param[in]	phi		Angle Phi
		 *\param[in]	theta	Angle Theta
		 */
		CU_API SphericalVertex( real radius = 0, real phi = 0, real theta = 0 );
		/**
		 *\~english
		 *\brief		Constructor from a cartesian point
		 *\param[in]	vertex	The vertex to convert
		 *\~french
		 *\brief		Constructeur à partir d'un point cartésien
		 *\param[in]	vertex	Le vertex à convertir
		 */
		CU_API explicit SphericalVertex( Point3r const & vertex );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		CU_API ~SphericalVertex();

	public:
		//!\~english	The radius (distance to origin).
		//!\~french		Rayon (distance à l'origine).
		real m_radius;
		//!\~english	The xy angle.
		//!\~french		Angle sur le plan XY.
		real m_phi;
		//!\~english	The xz angle.
		//!\~french		Angle sur le plan XZ.
		real m_theta;
	};
}

#endif
