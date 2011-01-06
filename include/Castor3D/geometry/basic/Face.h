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
		static unsigned long long FacesCount;	//!< Defines the total number of created faces

	private:
		Array <Vertex, 3>::Value m_vertex;		//!< The 3 vertices from which the face is made
		Point3r m_ptFaceNormal;					//!< The face's normal, used to the three vertexes in face display mode
		Point3r m_ptFaceTangent;				//!< The face's tangent, used to the three vertexes in face display mode
		Point3r m_ptSmoothNormals[3];			//!< The face's normal, used to the three vertexes in smooth display mode
		Point3r m_ptSmoothTangents[3];			//!< The face's tangent, used to the three vertexes in smooth display mode
		mutable Point3r m_ptCenter;				//!< The face's center, mutable because it has no influence over a face
		mutable bool m_bCenterComputed;			//!< Tells if the center has already been computed. Mutable because it has no influence over a face.

	public:
		/**
		 * Constructor
		 *@param p_pt1, p_pt2, p_pt3 : [in] The three verexes
		 */
		Face( const IdPoint3r & p_pt1=IdPoint3r(), const IdPoint3r & p_pt2=IdPoint3r(), const IdPoint3r & p_pt3=IdPoint3r());
		/**
		 * Copy Constructor
		 *@param p_face : [in] The face to copy
		 */
		Face( const Face & p_face);
		/**
		 * Destructor
		 */
		~Face();
		/**
		 * Computes the center of a face, using the barycentre with the same weights
		 *@return The computed face center
		 */
		const Point3r & GetFaceCenter()const;
		/**
		 * Set the texture coordinates for the three vertex of the face
		 *@param p_ptUVA : The UV coordinates for first vertex
		 *@param p_ptUVB : The UV coordinates for second vertex
		 *@param p_ptUVC : The UV coordinates for third vertex
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetTextureCoords( const Point2r & p_ptUVA, const Point2r & p_ptUVB, const Point2r & p_ptUVC, bool p_bManual = false);
		/**
		 * Set the texture coordinates for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param x,y : The UV coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexTexCoords( size_t p_uiVertex, real x=0.0, real y=0.0, bool p_bManual = false);
		/**
		 * Set the texture coordinates for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_ptUV : The UV coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexTexCoords( size_t p_uiVertex, const Point2r & p_ptUV, bool p_bManual = false);
		/**
		 * Set the texture coordinates for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_pCoords : The UV coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexTexCoords( size_t p_uiVertex, const real * p_pCoords, bool p_bManual = false);
		/**
		 * Set the normal for the three vertex of the face
		 *@param p_ptNormalA : The normal coordinates for first vertex
		 *@param p_ptNormalB : The normal coordinates for second vertex
		 *@param p_ptNormalC : The normal coordinates for third vertex
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetNormals( const Point2r & p_ptNormalA, const Point2r & p_ptNormalB, const Point2r & p_ptNormalC, bool p_bManual = false);
		/**
		 * Set the normal for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param x,y,z : The normal coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexNormal( size_t p_uiVertex, real x=0.0, real y=0.0, real z=0.0, bool p_bManual = false);
		/**
		 * Set the normal for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_ptNormal : The normal coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexNormal( size_t p_uiVertex, const Point3r & p_ptNormal, bool p_bManual = false);
		/**
		 * Set the normal for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_pCoords : The normal coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexNormal( size_t p_uiVertex, const real * p_pCoords, bool p_bManual = false);
		/**
		 * Set the tangent for the three vertex of the face
		 *@param p_ptTangentA : The tangent coordinates for first vertex
		 *@param p_ptTangentB : The tangent coordinates for second vertex
		 *@param p_ptTangentC : The tangent coordinates for third vertex
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetTangents( const Point2r & p_ptTangentA, const Point2r & p_ptTangentB, const Point2r & p_ptTangentC, bool p_bManual = false);
		/**
		 * Set the tangent for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param x,y,z : The tangent coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexTangent( size_t p_uiVertex, real x=0.0, real y=0.0, real z=0.0, bool p_bManual = false);
		/**
		 * Set the tangent for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_ptUV : The tangent coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexTangent( size_t p_uiVertex, const Point3r & p_ptUV, bool p_bManual = false);
		/**
		 * Set the tangent for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_pCoords : The tangent coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexTangent( size_t p_uiVertex, const real * p_pCoords, bool p_bManual = false);
		/**
		 * Sets the vertex normals to the smooth ones (per vertex)
		 */
		void SetSmoothNormals();
		/**
		 * Sets the vertex normals to the flat ones (per face)
		 */
		void SetFlatNormals();
		/**
		 * Sets the smooth normals for the wante vertex
		 *@param p_uiIndex : [in] The index of the wanted vertex
		 *@param p_ptNormal : [in] The normal
		 */
		void SetSmoothNormal( size_t p_uiIndex, const Point3r & p_ptNormal);
		/**
		 * Sets the face normal
		 *@param p_ptNormal : [in] The normal
		 */
		void SetFlatNormal( const Point3r & p_ptNormal);
		/**
		 * Sets the smooth normals for the wante vertex
		 *@param p_uiIndex : [in] The index of the wanted vertex
		 *@param p_ptNormal : [in] The normal
		 */
		void SetSmoothTangent( size_t p_uiIndex, const Point3r & p_ptTangent);
		/**
		 * Sets the face normal
		 *@param p_ptNormal : [in] The normal
		 */
		void SetFlatTangent( const Point3r & p_ptTangent);
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

	public:
		/**@name Accessors */
		//@{
		inline Vertex *			GetVertexPtr	( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return & m_vertex[p_uiIndex]; }
		inline const Vertex *	GetVertexPtr	( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return & m_vertex[p_uiIndex]; }
		inline Vertex &			GetVertex		( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return m_vertex[p_uiIndex]; }
		inline const Vertex &	GetVertex		( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_vertex[p_uiIndex]; }
		inline Vertex &			operator []		( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return m_vertex[p_uiIndex]; }
		inline const Vertex &	operator []		( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_vertex[p_uiIndex]; }
		inline Point3r &		GetSmoothNormal	( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothNormals[p_uiIndex]; }
		inline const Point3r &	GetSmoothNormal	( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothNormals[p_uiIndex]; }
		inline Point3r &		GetSmoothTangent( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothTangents[p_uiIndex]; }
		inline const Point3r &	GetSmoothTangent( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothTangents[p_uiIndex]; }
		inline Point3r &		GetFlatNormal	()							{ return m_ptFaceNormal; }
		inline const Point3r &	GetFlatNormal	()const						{ return m_ptFaceNormal; }
		inline Point3r &		GetFlatTangent	()							{ return m_ptFaceTangent; }
		inline const Point3r &	GetFlatTangent	()const						{ return m_ptFaceTangent; }
		//@}
	};
}

#endif
