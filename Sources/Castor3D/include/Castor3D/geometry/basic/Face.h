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
#ifndef ___C3D_Face___
#define ___C3D_Face___

#include "../../main/Serialisable.h"

namespace Castor3D
{
	//! The face handle class
	/*!
	A face is constituted from 3 vertexes, their faces normals, their vertexes normals, their texture coords
	The normals and texture coordinates must be held by each face
	to manage correctly the vertex which belong to different faces
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Face : public Serialisable
	{
	public:
		static unsigned long s_faceNumber;	//!< Defines the total number of created faces

	public:
		VertexPtr m_vertex1;			//!< The first vertex
		Point3rPtr m_vertex1Normal;		//!< First vertex normal coordinates
		Point3r m_vertex1TexCoord;		//!< First vertex texture coordinate
		Point3rPtr m_vertex1Tangent;	//!< First vertex texture coordinates
		VertexPtr m_vertex2;			//!< The second vertex
		Point3rPtr m_vertex2Normal;		//!< Second vertex normal coordinates
		Point3r m_vertex2TexCoord;		//!< First vertex texture coordinates
		Point3rPtr m_vertex2Tangent;	//!< First vertex texture coordinates
		VertexPtr m_vertex3;			//!< The third vertex
		Point3rPtr m_vertex3Normal;		//!< Third vertex normal coordinates
		Point3r m_vertex3TexCoord;		//!< First vertex texture coordinates
		Point3rPtr m_vertex3Tangent;	//!< First vertex texture coordinates

		Point3r m_faceNormal;		//!< The face's normal, used to the three vertexes in face display mode
		Point3r m_faceTangent;		//!< The face's normal, used to the three vertexes in face display mode

	public:
		/**
		 * Constructor
		 *@param p_v1, p_v2, p_v3 : [in] The three verexes
		 */
		Face( VertexPtr p_v1=NULL, VertexPtr p_v2=NULL, VertexPtr p_v3=NULL);
		/**
		 * Destructor
		 */
		~Face();
		/**
		 * Computes the center of a face, using the barycentre with the same weights
		 *@return The computed face center
		 */
		Point3r GetFaceCenter();
		/**
		 * Set the texture coordinates for the first face vertex 
		 *@param x,y : The UV coordinates
		 */
		void SetTexCoordV1( real x, real y);
		/**
		 * Set the texture coordinates for the first face vertex 
		 *@param p_ptUV : The UV coordinates
		 */
		void SetTexCoordV1( const Point2r & p_ptUV);
		/**
		 * Set the texture coordinates for the second face vertex 
		 *@param x,y : The UV coordinates
		 */
		void SetTexCoordV2( real x, real y);
		/**
		 * Set the texture coordinates for the second face vertex 
		 *@param p_ptUV : The UV coordinates
		 */
		void SetTexCoordV2( const Point2r & p_ptUV);
		/**
		 * Set the texture coordinates for the third face vertex
		 *@param x,y : The UV coordinates
		 */
		void SetTexCoordV3( real x, real y);
		/**
		 * Set the texture coordinates for the third face vertex 
		 *@param p_ptUV : The UV coordinates
		 */
		void SetTexCoordV3( const Point2r & p_ptUV);
		/**
		 * Writes a face in a file
		 *@param p_file : The file to write in
		 */
		virtual bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads a face from a file
		 *@param p_file : The file to read from
		 */
		virtual bool Read( Castor::Utils::File & p_file);
	};
}

#endif
