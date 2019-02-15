/*
See LICENSE file in root folder
*/
#ifndef ___CU_PlaneEquation_HPP___
#define ___CU_PlaneEquation_HPP___

#include "Line3D.hpp"
#include "Point.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Plane equation implementation (ax + by + cz + d = 0).
	\~french
	\brief		Implémentation d'une équation de plan (ax + by + cz + d = 0).
	*/
	class PlaneEquation
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		CU_API PlaneEquation();
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p1	The first point belonging to the plane.
		 *\param[in]	p2	The second point belonging to the plane.
		 *\param[in]	p3	The third point belonging to the plane.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p1	Le premier point appartenant au plan.
		 *\param[in]	p2	Le second point appartenant au plan.
		 *\param[in]	p3	Le troisième point appartenant au plan.
		 */
		CU_API PlaneEquation( Point3r const & p1
			, Point3r const & p2
			, Point3r const & p3 );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	normal	The plane normal.
		 *\param[in]	point	A point belonging to the plane.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	normal	La normale du plan.
		 *\param[in]	point	Un point appartenant au plan.
		 */
		CU_API PlaneEquation( Point3r const & normal
			, Point3r const & point );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	normal	The plane normal.
		 *\param[in]	d		The plane distance.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	normal	La normale du plan.
		 *\param[in]	d		La distance du plan.
		 */
		CU_API PlaneEquation( Point3r const & normal
			, real d );
		/**
		 *\~english
		 *\brief		Compute plane's equation from 3 points.
		 *\param[in]	p1	The first point belonging to the plane.
		 *\param[in]	p2	The second point belonging to the plane.
		 *\param[in]	p3	The third point belonging to the plane.
		 *\~french
		 *\brief		Calcule l'équation du plan en fonction de 3 points.
		 *\param[in]	p1	Le premier point appartenant au plan.
		 *\param[in]	p2	Le second point appartenant au plan.
		 *\param[in]	p3	Le troisième point appartenant au plan.
		 */
		CU_API void set( Point3r const & p1
			, Point3r const & p2
			, Point3r const & p3 );
		/**
		 *\~english
		 *\brief		Defines plane's equation.
		 *\param[in]	normal	The plane normal.
		 *\param[in]	point	A point belonging to the plane.
		 *\~french
		 *\brief		Définit l'équation du plan.
		 *\param[in]	normal	La normale du plan.
		 *\param[in]	point	Un point appartenant au plan.
		 */
		CU_API void set( Point3r const & normal
			, Point3r const & point );
		/**
		 *\~english
		 *\brief		Defines plane's equation.
		 *\param[in]	normal	The plane normal.
		 *\param[in]	d		The plane distance.
		 *\~french
		 *\brief		Définit l'équation du plan.
		 *\param[in]	normal	La normale du plan.
		 *\param[in]	d		La distance du plan.
		 */
		CU_API void set( Point3r const & normal
			, real d );
		/**
		 *\~english
		 *\brief		Checks if this plane is parallel to another one, id est if their normals are parallel
		 *\param[in]	plane	The plane to test
		 *\~french
		 *\brief		Vérifie si ce plan est parallèle à un autre, id est leurs normales sont parallèles
		 *\param[in]	plane	Le plan a tester
		 */
		CU_API bool isParallel( PlaneEquation const & plane )const;
		/**
		 *\~english
		 *\brief		Computes distance of a point from this plane
		 *\param[in]	point	The point
		 *\return		The distance, positive if the point is on the same side as the plane's normal
		 *\~french
		 *\brief		Calcule la distance entre un point et ce plan
		 *\param[in]	point	Le point
		 *\return		La distance, positive si le point est du même côté que la normale du plan
		 */
		CU_API real distance( Point3r const & point )const;
		/**
		 *\~english
		 *\brief		Retrieves the projection of a point on this plane
		 *\param[in]	point	The point to project
		 *\return		The projection result
		 *\~french
		 *\brief		Récupère la projection d'un point sur ce plan
		 *\param[in]	point	Le point à projeter
		 *\return		Le résultat de la projection
		 */
		CU_API Point3r project( Point3r const & point )const;
		/**
		 *\todo			Find origin point
		 *\~english
		 *\brief		Checks if this plane intersects another one, gives the intersection line if intersection is not null
		 *\param[in]	plane	The plane to check
		 *\param[out]	line	The intersection line
		 *\return		\p true if there is an intersection, false if not
		 *\~french
		 *\brief		Vérifie si ce plan en croise un autre, donne la ligne d'intersetion si elle existe
		 *\param[in]	plane	Le plan à tester
		 *\param[out]	line	La ligne d'intersection
		 *\return		\p true s'il y a une intersection
		 */
		CU_API bool intersects( PlaneEquation const & plane
			, Line3D< real > & line )const;
		/**
		 *\~english
		 *\brief		Checks the intersection of this plane with 2 other ones, gives the intersection point
		 *\param[in]	plane1			The first plane to check
		 *\param[in]	plane2			The second plane to check
		 *\param[out]	intersection	The intersection point
		 *\return		\p true if there is an intersection point between the 3 planes
		 *\~french
		 *\brief		Vérifie si ce plan en croise deux autres, donne le point d'intersetion si il existe
		 *\param[in]	plane1			Le premier plan à tester
		 *\param[in]	plane2			Le second plan à tester
		 *\param[out]	intersection	Le point d'intersection
		 *\return		\p true s'il y a un point d'intersection entre les 3 plans
		 */
		CU_API bool intersects( PlaneEquation const & plane1
			, PlaneEquation const & plane2
			, Point3r & intersection )const;
		/**
		 *\~english
		 *\return		The plane's normal.
		 *\~french
		 *\return		La normale du plan.
		 */
		inline Point3r const & getNormal()const
		{
			return m_normal;
		}
		/**
		 *\~english
		 *\return		The plane's distance to origin.
		 *\~french
		 *\return		La distance entre le plan et l'origine.
		 */
		inline real getDistance()const
		{
			return m_d;
		}

	private:
		Point3r m_normal;
		real m_d;
		friend CU_API bool operator==( PlaneEquation const &, PlaneEquation const & );
		friend CU_API bool operator!=( PlaneEquation const &, PlaneEquation const & );
	};
	/**
	 *\~english
	 *\brief		Checks if this plane is equal to another one.
	 *\remarks		A plane is equal to another one if it is parallel to it and if normals and d coeff are equal
	 *\~french
	 *\brief		Vérifie si un plan est égal à un autre
	 *\remarks		Deux plans sont égaux si ils sont parallèles et si leurs normales et coeff sont égaux
	 */
	CU_API bool operator==( PlaneEquation const & lhs
		, PlaneEquation const & rhs );
	/**
	 *\~english
	 *\brief		Checks if this plane is different from another one
	 *\~french
	 *\brief		Vérifie si ce plan est différent d'un autre
	 */
	CU_API bool operator!=( PlaneEquation const & lhs
		, PlaneEquation const & rhs );
}

#endif
