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
#ifndef ___Castor_Plane___
#define ___Castor_Plane___

#include "Line.hpp"

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
	class PlaneEq
	{
	private:
		typedef Castor::Policy< T >		policy;
		typedef Castor::Point< T, 3 >	point;

	private:
		point	m_normal;
		real	m_d;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		PlaneEq();
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
		PlaneEq( point const & p_p1, point const & p_p2, point const & p_p3 );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_ptNormal	The plane normal
		 *\param[in]	p_ptPoint	A point belonging to the plane
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_ptNormal	La normale du plan
		 *\param[in]	p_ptPoint	Un point appartenant au plan
		 */
		PlaneEq( point const & p_ptNormal, point const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PlaneEq();
		/**
		 *\~english
		 *\brief		Checks if this plane is equal to another one.
		 \remark		A plane is equal to another one if it is parallel to it and if normals and d coeff are equal
		 *\~french
		 *\brief		Vérifie si un plan est égal à un autre
		 *\remark		Deux plans sont égaux si ils sont parallèles et si leurs normales et coeff sont égaux
		 */
		bool operator ==( PlaneEq const & p_plane)const;
		/**
		 *\~english
		 *\brief		Checks if this plane is different from another one
		 *\~french
		 *\brief		Vérifie si ce plan est différent d'un autre
		 */
		bool operator !=( PlaneEq const & p_plane)const;
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
		 *\param[in]	p_ptNormal	The plane normal
		 *\param[in]	p_ptPoint	A point belonging to the plane
		 *\~french
		 *\brief		Définit l'équation du plan
		 *\param[in]	p_ptNormal	La normale du plan
		 *\param[in]	p_ptPoint	Un point appartenant au plan
		 */
		void Set( point const & p_ptNormal, point const & p_ptPoint );
		/**
		 *\~english
		 *\brief		Checks if this plane is parallel to another one, id est if their normals are parallel
		 *\param[in]	p_plane	The plane to test
		 *\~french
		 *\brief		Vérifie si ce plan est parallèle à un autre, id est leurs normales sont parallèles
		 *\param[in]	p_plane	Le plan a tester
		 */
		bool IsParallel( PlaneEq const & p_plane)const;
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
		T Distance( Point< T, 3 > const & p_point )const;
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
		Point< T, 3 > Project( Point< T, 3 > const & p_point )const;
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
		bool Intersects( PlaneEq const & p_plane, Line< T > & p_line)const;
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
		bool Intersects( PlaneEq const & p_plane1, PlaneEq const & p_plane2, point & p_intersection)const;
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
		bool LineOn( Line3D< T > const & p_line)const;
		/**
		 *\~english
		 *\brief		Retrieves the plane's normal
		 *\return		The value
		 *\~french
		 *\brief		Récupère la normale du plan
		 *\return		La valeur
		 */
		inline Point< T, 3 > const & GetNormal()const { return m_normal; }
		/**
		 *\~english
		 *\brief		Retrieves the plane's reference point
		 *\return		The value
		 *\~french
		 *\brief		Récupère le point d'origine du plan
		 *\return		La valeur
		 */
		inline Point< T, 3 > const & GetPoint()const { return m_d; }
	};
}

#include "Plane.inl"

#endif
