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
#ifndef ___CASTOR_PLANE_EQUATION_H___
#define ___CASTOR_PLANE_EQUATION_H___

#include "Line3D.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Plane equation
	\remark		Do you remember ax + by + cz + d = 0 ?
	\~french
	\brief		Equation de plan
	\remark		Vous connaissez ax + by + cz + d = 0 ?
	*/
	template< typename T >
	class PlaneEquation
	{
	private:
		using policy = Castor::Policy< T >;
		using point = Castor::Point3< T >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		PlaneEquation();
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_p1	The first point belonging to the plane
		 *\param[in]	p_p2	The second point belonging to the plane
		 *\param[in]	p_p3	The third point belonging to the plane
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_p1	Le premier point appartenant au plan
		 *\param[in]	p_p2	Le second point appartenant au plan
		 *\param[in]	p_p3	Le troisième point appartenant au plan
		 */
		PlaneEquation( point const & p_p1, point const & p_p2, point const & p_p3 );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_normal	The plane normal
		 *\param[in]	p_point	A point belonging to the plane
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_normal	La normale du plan
		 *\param[in]	p_point	Un point appartenant au plan
		 */
		PlaneEquation( point const & p_normal, point const & p_point );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PlaneEquation();
		/**
		 *\~english
		 *\brief		Compute plane's equation from 3 points
		 *\param[in]	p_p1	The first point belonging to the plane
		 *\param[in]	p_p2	The second point belonging to the plane
		 *\param[in]	p_p3	The third point belonging to the plane
		 *\~french
		 *\brief		Calcule l'équation du plan en fonction de 3 points
		 *\param[in]	p_p1	Le premier point appartenant au plan
		 *\param[in]	p_p2	Le second point appartenant au plan
		 *\param[in]	p_p3	Le troisième point appartenant au plan
		 */
		void Set( point const & p_p1, point const & p_p2, point const & p_p3 );
		/**
		 *\~english
		 *\brief		Defines plane's equation
		 *\param[in]	p_normal	The plane normal
		 *\param[in]	p_point	A point belonging to the plane
		 *\~french
		 *\brief		Définit l'équation du plan
		 *\param[in]	p_normal	La normale du plan
		 *\param[in]	p_point	Un point appartenant au plan
		 */
		void Set( point const & p_normal, point const & p_point );
		/**
		 *\~english
		 *\brief		Checks if this plane is parallel to another one, id est if their normals are parallel
		 *\param[in]	p_plane	The plane to test
		 *\~french
		 *\brief		Vérifie si ce plan est parallèle à un autre, id est leurs normales sont parallèles
		 *\param[in]	p_plane	Le plan a tester
		 */
		bool IsParallel( PlaneEquation const & p_plane )const;
		/**
		 *\~english
		 *\brief		Computes distance of a point from this plane
		 *\param[in]	p_point	The point
		 *\return		The distance, positive if the point is on the same side as the plane's normal
		 *\~french
		 *\brief		Calcule la distance entre un point et ce plan
		 *\param[in]	p_point	Le point
		 *\return		La distance, positive si le point est du même côté que la normale du plan
		 */
		T Distance( point const & p_point )const;
		/**
		 *\~english
		 *\brief		Retrieves the projection of a point on this plane
		 *\param[in]	p_point	The point to project
		 *\return		The projection result
		 *\~french
		 *\brief		Récupère la projection d'un point sur ce plan
		 *\param[in]	p_point	Le point à projeter
		 *\return		Le résultat de la projection
		 */
		point Project( point const & p_point )const;
		/**
		 *\todo			Find origin point
		 *\~english
		 *\brief		Checks if this plane intersects another one, gives the intersection line if intersection is not null
		 *\param[in]	p_plane	The plane to check
		 *\param[out]	p_line	The intersection line
		 *\return		\p true if there is an intersection, false if not
		 *\~french
		 *\brief		Vérifie si ce plan en croise un autre, donne la ligne d'intersetion si elle existe
		 *\param[in]	p_plane	Le plan à tester
		 *\param[out]	p_line	La ligne d'intersection
		 *\return		\p true s'il y a une intersection
		 */
		bool Intersects( PlaneEquation const & p_plane, Line3D< T > & p_line )const;
		/**
		 *\~english
		 *\brief		Checks the intersection of this plane with 2 other ones, gives the intersection point
		 *\param[in]	p_plane1	The first plane to check
		 *\param[in]	p_plane2	The second plane to check
		 *\param[out]	p_intersection	The intersection point
		 *\return		\p true if there is an intersection point between the 3 planes
		 *\~french
		 *\brief		Vérifie si ce plan en croise deux autres, donne le point d'intersetion si il existe
		 *\param[in]	p_plane1	Le permier plan à tester
		 *\param[in]	p_plane2	Le second plan à tester
		 *\param[out]	p_intersection	Le point d'intersection
		 *\return		\p true s'il y a un point d'intersection entre les 3 plans
		 */
		bool Intersects( PlaneEquation const & p_plane1, PlaneEquation const & p_plane2, point & p_intersection )const;
		/**
		 *\~english
		 *\brief		Checks if the given line belongs to the plane
		 *\param[out]	p_line	The line
		 *\return		\p true if the line belongs to the plane
		 *\~french
		 *\brief		Vérifie si la ligne donnée appartient à ce plan
		 *\param[out]	p_line	La ligne
		 *\return		\p true s'il la ligne appartient au plan
		 */
		bool LineOn( Line3D< T > const & p_line )const;
		/**
		 *\~english
		 *\return		The plane's normal.
		 *\~french
		 *\return		La normale du plan.
		 */
		inline point const & GetNormal()const
		{
			return m_normal;
		}
		/**
		 *\~english
		 *\return		The plane's reference point.
		 *\~french
		 *\return		Le point de référence du plan.
		 */
		inline point const & GetPoint()const
		{
			return m_point;
		}

	private:
		point m_normal;
		point m_point;
		T m_d;
		template< typename U > friend bool operator==( PlaneEquation< U > const & p_a, PlaneEquation< U > const & p_b );
		template< typename U > friend bool operator!=( PlaneEquation< U > const & p_a, PlaneEquation< U > const & p_b );
	};
	/**
	 *\~english
	 *\brief		Checks if this plane is equal to another one.
	 *\remarks		A plane is equal to another one if it is parallel to it and if normals and d coeff are equal
	 *\~french
	 *\brief		Vérifie si un plan est égal à un autre
	 *\remarks		Deux plans sont égaux si ils sont parallèles et si leurs normales et coeff sont égaux
	 */
	template< typename T >
	bool operator==( PlaneEquation< T > const & p_a, PlaneEquation< T > const & p_b );
	/**
	 *\~english
	 *\brief		Checks if this plane is different from another one
	 *\~french
	 *\brief		Vérifie si ce plan est différent d'un autre
	 */
	template< typename T >
	bool operator!=( PlaneEquation< T > const & p_a, PlaneEquation< T > const & p_b );
}

#include "PlaneEquation.inl"

#endif
