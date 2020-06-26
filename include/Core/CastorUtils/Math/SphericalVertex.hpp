/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_SPHERICAL_VERTEX_H___
#define ___CASTOR_SPHERICAL_VERTEX_H___

#include "CastorUtils/Math/Point.hpp"

namespace castor
{
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
		CU_API SphericalVertex( float radius = 0, float phi = 0, float theta = 0 );
		/**
		 *\~english
		 *\brief		Constructor from a cartesian point
		 *\param[in]	vertex	The vertex to convert
		 *\~french
		 *\brief		Constructeur à partir d'un point cartésien
		 *\param[in]	vertex	Le vertex à convertir
		 */
		CU_API explicit SphericalVertex( Point3f const & vertex );
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
		float m_radius;
		//!\~english	The xy angle.
		//!\~french		Angle sur le plan XY.
		float m_phi;
		//!\~english	The xz angle.
		//!\~french		Angle sur le plan XZ.
		float m_theta;
	};
}

#endif
