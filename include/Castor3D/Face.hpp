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
#ifndef ___C3D_Face___
#define ___C3D_Face___

#include "Vertex.hpp"

namespace Castor
{	namespace Resources
{
	//! Face loader
	/*!
	Loads and saves faces from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::Face>
	{
	public:
		/**
		 * Reads a face from a text file
		 *@param p_face : [in/out] The face to read
		 *@param p_file : [in/out] The file where to read the face
		 */
		static bool Read( Castor3D::Face & p_face, Utils::File & p_file);
		/**
		 * Writes a face into a text file
		 *@param p_face : [in] The face to write
		 *@param p_file : [in/out] The file where to write the face
		 */
		static bool Write( const Castor3D::Face & p_face, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! The face handle class C3D_API
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
		array <Vertex, 3> m_vertex;				//!< The 3 vertices from which the face is made
		Point3r m_ptFaceNormal;					//!< The face's normal, used to the three vertexes in face display mode
		Point3r m_ptFaceTangent;				//!< The face's tangent, used to the three vertexes in face display mode
		Point3r m_ptSmoothNormals[3];			//!< The face's normal, used to the three vertexes in smooth display mode
		Point3r m_ptSmoothTangents[3];			//!< The face's tangent, used to the three vertexes in smooth display mode
		mutable Point3r m_ptCenter;				//!< The face's center, mutable because it has no influence over a face
		mutable bool m_bCenterComputed;			//!< Tells if the center has already been computed. Mutable because it has no influence over a face.

	public:
		/**@name Construction / Destruction */
		//@{
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
		//@}

		/**
		 * Computes the center of a face, using the barycentre with the same weights
		 *@return The computed face center
		 */
		Point3r const & GetFaceCenter()const;
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
		void SetVertexTexCoords( size_t p_uiVertex, real const * p_pCoords, bool p_bManual = false);
		/**
		 * Set the normal for the three vertex of the face
		 *@param p_ptNormalA : The normal coordinates for first vertex
		 *@param p_ptNormalB : The normal coordinates for second vertex
		 *@param p_ptNormalC : The normal coordinates for third vertex
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetNormals( Point3r const & p_ptNormalA, Point3r const & p_ptNormalB, Point3r const & p_ptNormalC, bool p_bManual = false);
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
		void SetVertexNormal( size_t p_uiVertex, Point3r const & p_ptNormal, bool p_bManual = false);
		/**
		 * Set the normal for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_pCoords : The normal coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexNormal( size_t p_uiVertex, real const * p_pCoords, bool p_bManual = false);
		/**
		 * Set the tangent for the three vertex of the face
		 *@param p_ptTangentA : The tangent coordinates for first vertex
		 *@param p_ptTangentB : The tangent coordinates for second vertex
		 *@param p_ptTangentC : The tangent coordinates for third vertex
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetTangents( Point3r const & p_ptTangentA, Point3r const & p_ptTangentB, Point3r const & p_ptTangentC, bool p_bManual = false);
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
		void SetVertexTangent( size_t p_uiVertex, Point3r const & p_ptUV, bool p_bManual = false);
		/**
		 * Set the tangent for the given vertex 
		 *@param p_uiVertex : The vertex index
		 *@param p_pCoords : The tangent coordinates
		 *@param p_bManual : Tells if the coordinates are user coordinates
		 */
		void SetVertexTangent( size_t p_uiVertex, real const * p_pCoords, bool p_bManual = false);
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
		void SetSmoothNormal( size_t p_uiIndex, Point3r const & p_ptNormal);
		/**
		 * Sets the face normal
		 *@param p_ptNormal : [in] The normal
		 */
		void SetFlatNormal( Point3r const & p_ptNormal);
		/**
		 * Sets the smooth normals for the wante vertex
		 *@param p_uiIndex : [in] The index of the wanted vertex
		 *@param p_ptNormal : [in] The normal
		 */
		void SetSmoothTangent( size_t p_uiIndex, Point3r const & p_ptTangent);
		/**
		 * Sets the face normal
		 *@param p_ptNormal : [in] The normal
		 */
		void SetFlatTangent( Point3r const & p_ptTangent);

		/**@name Accessors */
		//@{
		inline Vertex *			GetVertexPtr	( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return & m_vertex[p_uiIndex]; }
		inline const Vertex *	GetVertexPtr	( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return & m_vertex[p_uiIndex]; }
		inline Vertex &			GetVertex		( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return m_vertex[p_uiIndex]; }
		inline const Vertex &	GetVertex		( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_vertex[p_uiIndex]; }
		inline Vertex &			operator []		( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());m_bCenterComputed = false;return m_vertex[p_uiIndex]; }
		inline const Vertex &	operator []		( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_vertex[p_uiIndex]; }
		inline Point3r &		GetSmoothNormal	( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothNormals[p_uiIndex]; }
		inline Point3r const &	GetSmoothNormal	( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothNormals[p_uiIndex]; }
		inline Point3r &		GetSmoothTangent( size_t p_uiIndex)			{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothTangents[p_uiIndex]; }
		inline Point3r const &	GetSmoothTangent( size_t p_uiIndex)const	{ CASTOR_ASSERT( p_uiIndex < m_vertex.size());return m_ptSmoothTangents[p_uiIndex]; }
		inline Point3r &		GetFlatNormal	()							{ return m_ptFaceNormal; }
		inline Point3r const &	GetFlatNormal	()const						{ return m_ptFaceNormal; }
		inline Point3r &		GetFlatTangent	()							{ return m_ptFaceTangent; }
		inline Point3r const &	GetFlatTangent	()const						{ return m_ptFaceTangent; }
		//@}

		DECLARE_SERIALISE_MAP()
		DECLARE_POST_UNSERIALISE()
	};
}

#endif
