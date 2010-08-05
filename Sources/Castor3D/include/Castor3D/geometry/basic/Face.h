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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Face___
#define ___C3D_Face___

namespace Castor3D
{
	//! The face handle class
	/*!
	A face is constituted from 3 vertexes, their faces normals, their vertexes normals, their texture coords
	No functions in this class in order to optimise memory
	The normals and texture coordinates must be held by a face, to manage correctly the vertex which belong to different faces
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Face
	{
	public:
		static unsigned long s_faceNumber;	//!< Defines the total number of created faces

	public:
		Vector3f * m_vertex1;				//!< The first vertex
		Vector3f * m_vertex1Normal;			//!< First vertex normal coordinates
		Point3D<float> m_vertex1TexCoord;	//!< First vertex texture coordinate
		Vector3f * m_vertex1Tangent;		//!< First vertex texture coordinates
		Vector3f * m_vertex2;				//!< The second vertex
		Vector3f * m_vertex2Normal;			//!< Second vertex normal coordinates
		Point3D<float> m_vertex2TexCoord;	//!< First vertex texture coordinates
		Vector3f * m_vertex2Tangent;		//!< First vertex texture coordinates
		Vector3f * m_vertex3;				//!< The third vertex
		Vector3f * m_vertex3Normal;			//!< Third vertex normal coordinates
		Point3D<float> m_vertex3TexCoord;	//!< First vertex texture coordinates
		Vector3f * m_vertex3Tangent;		//!< First vertex texture coordinates

		Point3D<float> m_faceNormal;		//!< The face's normal, used to the three vertexes in face display mode
		Point3D<float> m_faceTangent;		//!< The face's normal, used to the three vertexes in face display mode

	public:
		/**
		 * Constructor
		 *@param p_v1, p_v2, p_v3 : [in] The three verexes
		 */
		Face( Vector3f * p_v1=NULL, Vector3f * p_v2=NULL, Vector3f * p_v3=NULL);
		/**
		 * Copy constructor
		 */
//		Face( const Face & p_face);
		/**
		 * Destructor
		 */
		~Face();
	};
}

#endif
