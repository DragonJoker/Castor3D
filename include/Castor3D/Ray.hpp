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
#ifndef ___C3D_Ray___
#define ___C3D_Ray___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//! Ray representation
	/*!
	A ray is an origin and a direction in 3D
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API Ray : public MemoryTraced<Ray>
	{
	private:
		typedef Templates::Policy<real> policy;

	public:
		Point3r m_ptOrigin;		//!< The ray origin
		Point3r m_ptDirection;	//!< The ray direction

	public:
		/**
		 * Constructor from mouse coordinates and a viewport
		 *@param p_ptPoint : [in] The mouse coordinates
		 *@param p_camera : [in] The camera from which to retrieve the ray
		 */
		Ray( Point2i const & p_ptPoint, const Camera & p_camera);
		/**
		 * Constructor from mouse coordinates and a viewport
		 *@param p_iX : [in] The mouse x
		 *@param p_iY : [in] The mouse y
		 *@param p_camera : [in] The camera from which to retrieve the ray
		 */
		Ray( int p_iX, int p_iY, const Camera & p_camera);
		/**
		 * Constructor from mouse coordinates and a viewport
		 *@param p_ptOrigin : [in] The origin of the ray
		 *@param p_ptDirection : [in] The The direction of the ray
		 */
		Ray( Point3r const & p_ptOrigin, Point3r const & p_ptDirection);
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
		*@param p_pt1 : [in] The first triangle vertex
		*@param p_pt2 : [in] The second triangle vertex
		*@param p_pt3 : [in] The third triangle vertex
		*@return true if the ray intersects the triangle, false if not
		*/
		real Intersects( Point3r const & p_pt1, Point3r const & p_pt2, Point3r const & p_pt3);
		/**
		 * Tells if the ray intersects the given face
		 *@param p_face : [in] The face to test
		 *@return true if the ray intersects the face, false if not
		 */
		real Intersects( const Face & p_face);
		/**
		* Tells if the vertex is on the ray
		*@param p_point : [in] The face to test
		*@return true if vertex is on the ray, false if not
		*/
		real Intersects( Point3r const & p_point);
		/**
		* Tells if the ray intersects the given Combo box
		*@param p_box : [in] The box to test
		*@return true if the ray intersects the face, false if not
		*/
		real Intersects( CubeBox const & p_box);
		/**
		* Tells if the ray intersects the given Sphere
		*@param p_sphere : [in] The sphere to test
		*@return true if the ray intersects the face, false if not
		*/
		real Intersects( SphereBox const & p_sphere);
		/**
		* Tells if the ray intersects the given Geometry
		*@param p_pGeometry : [in] The sphere to test
		*@param p_ppFace : [out] The intersected face
		*@param p_ppSubmesh : [out] The intersected submesh
		*@return true if the ray intersects the face, false if not
		*/
		real Intersects( Geometry * p_pGeometry, FacePtr * p_ppFace, SubmeshPtr * p_ppSubmesh);
		/**
		 * Projects the given vertex on the ray
		 *@param p_point : [in] The vertex we want to project
		 *@param p_result : [out] The projecion result
		 *@return true if the vertex can be projected on the ray, false if not
		 */
		bool ProjectVertex( Point3r const & p_point, Point3r & p_result);
	};
}

#endif