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
#ifndef ___C3D_Ray___
#define ___C3D_Ray___

#include "Module_Camera.h"
#include "../geometry/Module_Geometry.h"

namespace Castor3D
{
	//! Ray representation
	/*!
	A ray is an origin and a direction in 3D
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API Ray
	{
	private:
		typedef Templates::Value<real> value;

	public:
		Point3r m_origin;		//!< The ray origin
		Point3r m_direction;	//!< The ray direction

	public:
		/**
		 * Constructor from mouse coordinates and a viewport
		 *@param p_point : [in] The mouse coordinates
		 *@param p_camera : [in] The camera from which to retrieve the ray
		 */
		Ray( const Point<int, 2> & p_point, const Camera & p_camera);
		/**
		 * Constructor from mouse coordinates and a viewport
		 *@param p_x : [in] The mouse x
		 *@param p_y : [in] The mouse y
		 *@param p_camera : [in] The camera from which to retrieve the ray
		 */
		Ray( int p_x, int p_y, const Camera & p_camera);
		/**
		 * Constructor from mouse coordinates and a viewport
		 *@param p_rOrigin : [in] The origin of the ray
		 *@param p_rDirection : [in] The The direction of the ray
		 */
		Ray( const Point3r & p_rOrigin, const Point3r & p_rDirection);
		/**
		 * Copy constructor
		 *@param p_ray : [in] The ray to copy from
		 */
		Ray( const Ray & p_ray);
		/**
		 * Destructor
		 */
		~Ray();
		/**
		* Tells if the ray intersects the given triangle of vertices
		*@param p_v1 : [in] The first triangle vertex
		*@param p_v2 : [in] The second triangle vertex
		*@param p_v3 : [in] The third triangle vertex
		*@return true if the ray intersects the triangle, false if not
		*/
		real Intersects( const Point3r & p_v1, const Point3r & p_v2, const Point3r & p_v3);
		/**
		 * Tells if the ray intersects the given face
		 *@param p_face : [in] The face to test
		 *@return true if the ray intersects the face, false if not
		 */
		real Intersects( const Face & p_face);
		/**
		* Tells if the vertex is on the ray
		*@param p_vertex : [in] The face to test
		*@return true if vertex is on the ray, false if not
		*/
		real Intersects( const Point3r & p_vertex);
		/**
		* Tells if the ray intersects the given Combo box
		*@param p_box : [in] The box to test
		*@return true if the ray intersects the face, false if not
		*/
		real Intersects( const ComboBox & p_box);
		/**
		* Tells if the ray intersects the given Sphere
		*@param p_sphere : [in] The sphere to test
		*@return true if the ray intersects the face, false if not
		*/
		real Intersects( const Sphere & p_sphere);
		/**
		* Tells if the ray intersects the given Geometry
		*@param p_pGeometry : [in] The sphere to test
		*@param p_ppFace : [out] The intersected face
		*@param p_ppSubmesh : [out] The intersected submesh
		*@return true if the ray intersects the face, false if not
		*/
		real Intersects( GeometryPtr p_pGeometry, FacePtr* p_ppFace, SubmeshPtr* p_ppSubmesh);
		/**
		 * Projects the given vertex on the ray
		 *@param p_vertex : [in] The vertex we want to project
		 *@param p_result : [out] The projecion result
		 *@return true if the vertex can be projected on the ray, false if not
		 */
		bool ProjectVertex( const Point3r & p_vertex, Point3r & p_result);
	};
}

#endif