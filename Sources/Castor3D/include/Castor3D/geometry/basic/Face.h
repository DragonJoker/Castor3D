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
#include "Vertex.h"
#include "SmoothingGroup.h"

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
	class C3D_API Face : public Serialisable, public MemoryTraced<Face>
	{
	public:
		static unsigned long s_faceNumber;	//!< Defines the total number of created faces

	public:
		Vertex m_vertex[3];				//!< The vertices

		Point3r m_faceNormal;			//!< The face's normal, used to the three vertexes in face display mode
		Point3r m_faceTangent;			//!< The face's tangent, used to the three vertexes in face display mode
		Point3r m_smoothNormals[3];		//!< The face's normal, used to the three vertexes in smooth display mode
		Point3r m_smoothTangents[3];	//!< The face's tangent, used to the three vertexes in smooth display mode

	public:
		/**
		 * Constructor
		 *@param p_v1, p_v2, p_v3 : [in] The three verexes
		 */
		Face( const Point3r & p_v1=Point3r(), size_t p_uiIndex1 = 0, const Point3r & p_v2=Point3r(), size_t p_uiIndex2 = 0, const Point3r & p_v3=Point3r(), size_t p_uiIndex3 = 0);
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
		 *@param x,y : The UVW coordinates
		 */
		void SetVertexTexCoords( size_t p_iVertex, real x=0.0, real y=0.0);
		/**
		 * Set the texture coordinates for the first face vertex 
		 *@param p_ptUV : The UVW coordinates
		 */
		void SetVertexTexCoords( size_t p_iVertex, const Point2r & p_ptUV);
		/**
		 * Set the texture coordinates for the first face vertex 
		 *@param p_pCoords : The UVW coordinates
		 */
		void SetVertexTexCoords( size_t p_iVertex, const real * p_pCoords);
		void SetSmoothNormals();
		void SetFlatNormals();
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

		inline Vertex &			operator []	( size_t p_uiIndex)			{ return m_vertex[p_uiIndex];}
		inline const Vertex &	operator []	( size_t p_uiIndex)const	{ return m_vertex[p_uiIndex];}
	};
}

#endif
