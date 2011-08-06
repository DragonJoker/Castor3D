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
#ifndef ___C3D_LoopDivider___
#define ___C3D_LoopDivider___

#include <Castor3D/Subdivider.hpp>
#include <Castor3D/FrameListener.hpp>

#ifndef _WIN32
#	define C3D_Loop_API
#else
#	ifdef LoopDivider_EXPORTS
#		define C3D_Loop_API __declspec(dllexport)
#	else
#		define C3D_Loop_API __declspec(dllimport)
#	endif
#endif

namespace Castor3D
{
	class LoopSubdivider;
	class LoopVertex;
	class LoopEdge;
	class FaceEdges;

	typedef shared_ptr<	LoopVertex	>	LoopVertexPtr;
	typedef shared_ptr<	LoopEdge	>	LoopEdgePtr;
	typedef shared_ptr<	FaceEdges	>	FaceEdgesPtr;

	typedef Container<		FaceEdgesPtr						>::Vector	FaceEdgesPtrArray;
	typedef KeyedContainer<	size_t,			LoopEdgePtr			>::Map		LoopEdgePtrUIntMap;			//!< Map of edges, ordered by IdPoint3r pointer
	typedef KeyedContainer<	size_t,			LoopEdgePtrUIntMap	>::Map		LoopEdgePtrUIntMapUIntMap;
	typedef KeyedContainer< size_t,			LoopVertexPtr		>::Map		LoopVertexPtrUIntMap;

	//! Edge representation
	/*!
	An edge is defined between 2 vertices used to separate 2 faces
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class LoopEdge : public MemoryTraced<LoopEdge>
	{
	private:
		LoopVertexPtr m_firstVertex;
		LoopVertexPtr m_secondVertex;
		FacePtr m_firstFace;
		FacePtr m_secondFace;

		LoopVertexPtr m_createdVertex;
		bool m_divided;
		bool m_toDivide;
		bool m_toDelete;

	public:
		/**
		 * Constructor
		 *@param p_v1 : [in] The 1st point
		 *@param p_v2 : [in] The 2nd point
		 *@param p_f1 : [in] The 1st face of the edge
		 *@param p_toDivide : [in] Tells if the edge has to be divided
		 */
		LoopEdge( LoopVertexPtr p_v1, LoopVertexPtr p_v2, FacePtr p_f1, bool p_toDivide);
		/**
		 * Destructor
		 */
		~LoopEdge();
		/**
		 * Adds a face to the edge (max 2 faces, 1 at each side of the edge)
		 *@param p_face : [in] Pointer to the face to add
		 */
		void AddFace( FacePtr p_face);
		/**
		 * Divides the edge id est adds a vertex in a portion of the edge determined by p_value (0.5 = middle).
		 * Doesn't divide the faces
		 *@param p_submesh : [in] The submesh to which the newly created point is added
		 *@param p_value : [in] Percentage of the segment [m_firstVertex,m_secondVertex]
		 *@return The created point
		 */
		LoopVertexPtr Divide( LoopSubdivider * p_pDivider, real p_value);

	public:
		/**@name Accessors */
		//@{
		inline bool				IsDivided	()								{ return m_divided; }
		inline LoopVertexPtr	GetVertex1	()								{ return m_firstVertex; }
		inline LoopVertexPtr	GetVertex2	()								{ return m_secondVertex; }
		inline FacePtr			GetFace1	()								{ return m_firstFace; }
		inline FacePtr			GetFace2	()								{ return m_secondFace; }

		inline void	SetToDivide	() { m_toDivide = true;m_divided = false;m_createdVertex.reset(); }
		inline void	SetToDelete	() { m_toDelete = true; }
		//@}
	};
	//! Loop Algorithm vertex representation
	/*!
	It's a vertex with additional information : neighbourhood and associated edges
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 12/01/2011
	*/
	class LoopVertex
	{
	private:
		IdPoint3rPtr m_ptPoint;
		LoopEdgePtrUIntMap m_mapEdges;

	public:
		LoopVertex( IdPoint3rPtr p_ptPoint);
		~LoopVertex();

		bool HasEdge( size_t p_uiIndex);
		LoopEdgePtr GetEdge( size_t p_uiIndex);
		void AddEdge( LoopEdgePtr p_pEdge, size_t p_uiIndex);
		void RemoveEdge( size_t p_uiIndex);

		/**@name Accessors */
		//@{
		inline IdPoint3rPtr							GetPoint		()const									{ return m_ptPoint; }
		inline size_t								GetIndex		()const									{ return m_ptPoint->GetIndex(); }
		inline size_t								Size			()const									{ return m_mapEdges.size(); }
		inline LoopEdgePtrUIntMap::iterator			Begin			()										{ return m_mapEdges.begin(); }
		inline LoopEdgePtrUIntMap::const_iterator	End				()const									{ return m_mapEdges.end(); }
		inline void									Erase			( LoopEdgePtrUIntMap::iterator p_it)	{ m_mapEdges.erase( p_it); }
		inline void									Clear			()										{ m_mapEdges.clear(); }
		//@}
	};
	//! Face edges representation
	/*!
	Holds the three edges constituted by a face
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class FaceEdges : public MemoryTraced<FaceEdges>
	{
	private:
		LoopSubdivider * m_pDivider;
		FacePtr m_face;
		size_t m_sgIndex;
		LoopEdgePtr m_edgeAB;
		LoopEdgePtr m_edgeBC;
		LoopEdgePtr m_edgeCA;
		bool m_bOwnFace;

	public:
		/**
		 * Constructor
		 *@param p_submesh : [in] The submesh holding the face
		 *@param p_sgIndex : [in] The smoothing group index in the submesh
		 *@param p_face : [in] The face
		 *@param p_existingEdges : [in/out] The map of already existing edges, sorted by vertex
		 *@param p_vertexNeighbourhood : [in/out] The list of vertices neighbours
		 *@param p_allEdges : [in/out] All the edges
		 */
		FaceEdges( LoopSubdivider * p_pDivider, size_t p_sgIndex, FacePtr p_face,  LoopVertexPtrUIntMap & p_mapVertex);
		/**
		 * Constructor
		 *@param p_submesh : [in] The submesh holding the face
		 *@param p_sgIndex : [in] The smoothing group index in the submesh
		 *@param p_face : [in] The face
		 *@param l_ab : [in] Edge between 1st and 2nd vertex
		 *@param l_bc : [in] Edge between 2nd and 3rd vertex
		 *@param l_ca : [in] Edge between 3rd and 1st vertex
		 */
		FaceEdges( LoopSubdivider * p_pDivider, size_t p_sgIndex, FacePtr p_face, LoopEdgePtr l_ab, LoopEdgePtr l_bc, LoopEdgePtr l_ca);
		/**
		 * Divides the edges held, creates needed faces to complete the division
		 *@param p_value : [in] The weight of division (if 0.5, divides all edges in the middle)
		 *@param p_existingEdges : [in/out] The map of already existing edges, sorted by vertex
		 *@param p_vertexNeighbourhood : [in/out] The list of vertices neighbours
		 *@param p_allEdges : [in/out] All the edges
		 *@param p_newFaces : [in/out] The array of newly created faces. All faces created by the subdivision are put inside it
		 *@param p_newVertices : [in/out] The array of newly created vertices. All vertices created by the subdivision are put inside it
		 */
		void Divide( real p_value, LoopVertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces);

	public:
		/**@name Accessors */
		//@{
		inline LoopEdgePtr GetEdgesAB() { return m_edgeAB; }
		inline LoopEdgePtr GetEdgesBC() { return m_edgeBC; }
		inline LoopEdgePtr GetEdgesCA() { return m_edgeCA; }
		//@}

	private:
		void _addFaceAndEdges( LoopVertexPtr p_a, LoopVertexPtr p_b, LoopVertexPtr p_c,
							   const Point2r & p_aTex, const Point2r & p_bTex, const Point2r & p_cTex,
							   FaceEdgesPtrArray & p_newFaces);
		void _addFaceAndEdges( LoopVertexPtr p_a, LoopVertexPtr p_b, LoopVertexPtr p_c,
							   const Point2r & p_aTex, const Point2r & p_bTex, const Point2r & p_cTex,
							   LoopEdgePtr p_edgeAB, LoopEdgePtr p_edgeBC, LoopEdgePtr p_edgeCA,
							   FaceEdgesPtrArray & p_newFaces);
		LoopEdgePtr _addEdge( LoopVertexPtr p_v1, LoopVertexPtr p_v2, bool p_toDivide);
		void _removeEdge( LoopEdgePtr p_edge);
	};
	//! Loop subdivision algorithm Subdivider
	/*!
	This class implements the Loop subdivision algorithm
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class LoopSubdivider : public Subdivider, public MemoryTraced<LoopSubdivider>
	{
	private:
		LoopVertexPtrUIntMap	m_mapVertex;
		FaceEdgesPtrArray		m_facesEdges;

	public:
		LoopSubdivider( Submesh * p_submesh);
		virtual ~LoopSubdivider();
		virtual void Initialise();
		virtual void Cleanup();
		/**
		 * Creates and adds a vertex to my list
		 *@param x : [in] The vertex X coordinate
		 *@param y : [in] The vertex Y coordinate
		 *@param z : [in] The vertex Z coordinate
		 *@return The created vertex
		 */
		LoopVertexPtr AddPoint( real x, real y, real z);
		/**
		 * Adds a vertex to my list
		 *@param p_v : [in] The vertex to add
		 *@return The vertex
		 */
		LoopVertexPtr AddPoint( Point3r const & p_v);
		/**
		 * Creates and adds a vertex to my list
		 *@param p_v : [in] The vertex coordinates
		 *@return The created vertex
		 */
		LoopVertexPtr AddPoint( real * p_v);

	private:
		virtual void _subdivide();
		void _divide();
		void _average( Point3r const & p_center);
	};

	String & operator << ( String & p_stream, const LoopVertex & p_vertex);
}

#endif
