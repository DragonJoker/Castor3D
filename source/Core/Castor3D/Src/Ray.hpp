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
#ifndef ___C3D_RAY_H___
#define ___C3D_RAY_H___

#include "Castor3DPrerequisites.hpp"

#include <Point.hpp>

namespace Castor3D
{
	//! Ray representation
	/*!
	A ray is an origin and a direction in 3D
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Ray
	{
	private:
		typedef Castor::Policy<real> policy;

	public:
		/**
		 *\brief		Constructor from mouse coordinates and a viewport
		 *\param[in]	p_ptPoint	The mouse coordinates
		 *\param[in]	p_camera	The camera from which to retrieve the ray
		 */
		Ray( Castor::Point2i const & p_ptPoint, Camera const & p_camera );
		/**
		 *\brief		Constructor from mouse coordinates and a viewport
		 *\param[in]	p_x	The mouse x
		 *\param[in]	p_y	The mouse y
		 *\param[in]	p_camera	The camera from which to retrieve the ray
		 */
		Ray( int p_x, int p_y, Camera const & p_camera );
		/**
		 *\brief		Constructor from mouse coordinates and a viewport
		 *\param[in]	p_ptOrigin	The origin of the ray
		 *\param[in]	p_ptDirection	The The direction of the ray
		 */
		Ray( Castor::Point3r const & p_ptOrigin, Castor::Point3r const & p_ptDirection );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		Ray( Ray const & p_copy );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		Ray( Ray && p_copy );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Ray();
		/**
		 *\brief		Tells if the ray intersects the given triangle of vertices
		 *\param[in]	p_pt1	The first triangle vertex
		 *\param[in]	p_pt2	The second triangle vertex
		 *\param[in]	p_pt3	The third triangle vertex
		 *\return		\p true if the ray intersects the triangle, false if not
		 */
		real Intersects( Castor::Point3r const & p_pt1, Castor::Point3r const & p_pt2, Castor::Point3r const & p_pt3 );
		/**
		 *\brief		Tells if the ray intersects the given face
		 *\param[in]	p_face		The face to test
		 *\param[in]	p_submesh	The submesh holding the face
		 *\return		\p true if the ray intersects the face, false if not
		 */
		real Intersects( Face const & p_face, Submesh const & p_submesh );
		/**
		 *\brief		Tells if the vertex is on the ray
		 *\param[in]	p_point	The face to test
		 *\return		\p true if vertex is on the ray, false if not
		 */
		real Intersects( Castor::Point3r const & p_point );
		/**
		 *\brief		Tells if the ray intersects the given Combo box
		 *\param[in]	p_box	The box to test
		 *\return		\p true if the ray intersects the face, false if not
		 */
		real Intersects( Castor::CubeBox const & p_box );
		/**
		 *\brief		Tells if the ray intersects the given Sphere
		 *\param[in]	p_sphere	The sphere to test
		 *\return		\p true if the ray intersects the face, false if not
		 */
		real Intersects( Castor::SphereBox const & p_sphere );
		/**
		 *\brief		Tells if the ray intersects the given Geometry
		 *\param[in]	p_pGeometry	The sphere to test
		 *\param[out]	p_ppFace	The intersected face
		 *\param[out]	p_ppSubmesh	The intersected submesh
		 *\return		\p true if the ray intersects the face, false if not
		 */
		real Intersects( GeometrySPtr p_pGeometry, FaceSPtr * p_ppFace, SubmeshSPtr * p_ppSubmesh );
		/**
		 *\brief		Projects the given vertex on the ray
		 *\param[in]	p_point	The vertex we want to project
		 *\param[out]	p_result	The projecion result
		 *\return		\p true if the vertex can be projected on the ray, false if not
		 */
		bool ProjectVertex( Castor::Point3r const & p_point, Castor::Point3r & p_result );

	public:
		//!\~english The ray origin	\~french L'origine du rayon
		Castor::Point3r m_ptOrigin;
		//!\~english The ray direction	\~french La direction du rayon
		Castor::Point3r m_ptDirection;
	};
}

#endif
