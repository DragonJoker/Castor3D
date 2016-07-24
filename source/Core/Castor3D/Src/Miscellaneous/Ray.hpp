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
#ifndef ___C3D_RAY_H___
#define ___C3D_RAY_H___

#include "Castor3DPrerequisites.hpp"

#include <Math/Point.hpp>

namespace Castor3D
{
	//! Ray representation
	/*!
	A ray is an origin and a direction in 3D
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class Ray
	{
	private:
		typedef Castor::Policy<real> policy;

	public:
		/**
		 *\brief		Constructor from mouse coordinates and a viewport
		 *\param[in]	p_ptPoint	The mouse coordinates
		 *\param[in]	p_camera	The camera from which to retrieve the ray
		 */
		C3D_API Ray( Castor::Point2i const & p_ptPoint, Camera const & p_camera );
		/**
		 *\brief		Constructor from mouse coordinates and a viewport
		 *\param[in]	p_x	The mouse x
		 *\param[in]	p_y	The mouse y
		 *\param[in]	p_camera	The camera from which to retrieve the ray
		 */
		C3D_API Ray( int p_x, int p_y, Camera const & p_camera );
		/**
		 *\brief		Constructor from mouse coordinates and a viewport
		 *\param[in]	p_ptOrigin	The origin of the ray
		 *\param[in]	p_ptDirection	The The direction of the ray
		 */
		C3D_API Ray( Castor::Point3r const & p_ptOrigin, Castor::Point3r const & p_ptDirection );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		C3D_API Ray( Ray const & p_copy );
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_copy	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_copy	L'objet à copier
		 */
		C3D_API Ray( Ray && p_copy );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Ray();
		/**
		 *\brief		Tells if the ray intersects the given triangle of vertices
		 *\param[in]	p_pt1	The first triangle vertex
		 *\param[in]	p_pt2	The second triangle vertex
		 *\param[in]	p_pt3	The third triangle vertex
		 *\return		\p true if the ray intersects the triangle, false if not
		 */
		C3D_API real Intersects( Castor::Point3r const & p_pt1, Castor::Point3r const & p_pt2, Castor::Point3r const & p_pt3 );
		/**
		 *\brief		Tells if the ray intersects the given face
		 *\param[in]	p_face		The face to test
		 *\param[in]	p_submesh	The submesh holding the face
		 *\return		\p true if the ray intersects the face, false if not
		 */
		C3D_API real Intersects( Face const & p_face, Submesh const & p_submesh );
		/**
		 *\brief		Tells if the vertex is on the ray
		 *\param[in]	p_point	The face to test
		 *\return		\p true if vertex is on the ray, false if not
		 */
		C3D_API real Intersects( Castor::Point3r const & p_point );
		/**
		 *\brief		Tells if the ray intersects the given Combo box
		 *\param[in]	p_box	The box to test
		 *\return		\p true if the ray intersects the face, false if not
		 */
		C3D_API real Intersects( Castor::CubeBox const & p_box );
		/**
		 *\brief		Tells if the ray intersects the given Sphere
		 *\param[in]	p_sphere	The sphere to test
		 *\return		\p true if the ray intersects the face, false if not
		 */
		C3D_API real Intersects( Castor::SphereBox const & p_sphere );
		/**
		 *\brief		Tells if the ray intersects the given Geometry
		 *\param[in]	p_pGeometry	The sphere to test
		 *\param[out]	p_nearestFace	The intersected face
		 *\param[out]	p_nearestSubmesh	The intersected submesh
		 *\return		\p true if the ray intersects the face, false if not
		 */
		C3D_API real Intersects( GeometrySPtr p_pGeometry, FaceSPtr * p_nearestFace, SubmeshSPtr * p_nearestSubmesh );
		/**
		 *\brief		Projects the given vertex on the ray
		 *\param[in]	p_point	The vertex we want to project
		 *\param[out]	p_result	The projecion result
		 *\return		\p true if the vertex can be projected on the ray, false if not
		 */
		C3D_API bool ProjectVertex( Castor::Point3r const & p_point, Castor::Point3r & p_result );

	public:
		//!\~english The ray origin	\~french L'origine du rayon
		Castor::Point3r m_ptOrigin;
		//!\~english The ray direction	\~french La direction du rayon
		Castor::Point3r m_ptDirection;
	};
}

#endif
