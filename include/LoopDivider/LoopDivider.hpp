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

#pragma warning( push )
#pragma warning( disable:4311 )
#pragma warning( disable:4312 )

#include <Castor3D/Prerequisites.hpp>

#include <Castor3D/Submesh.hpp>
#include <Castor3D/Vertex.hpp>
#include <Castor3D/Face.hpp>
#include <Castor3D/Version.hpp>
#include <Castor3D/Plugin.hpp>

#include <Castor3D/Subdivider.hpp>
#include <Castor3D/FrameListener.hpp>

#pragma warning( pop )

#ifndef _WIN32
#	define C3D_Loop_API
#else
#	ifdef LoopDivider_EXPORTS
#		define C3D_Loop_API __declspec(dllexport)
#	else
#		define C3D_Loop_API __declspec(dllimport)
#	endif
#endif

namespace Loop
{
	class Subdivider;
	class Vertex;
	class Edge;
	class FaceEdges;

	typedef std::shared_ptr<	Vertex		>	VertexSPtr;
	typedef std::shared_ptr<	Edge		>	EdgePtr;
	typedef std::shared_ptr<	FaceEdges	>	FaceEdgesPtr;

	typedef std::vector<	FaceEdgesPtr					>	FaceEdgesPtrArray;
	typedef std::map<		uint32_t,		EdgePtr			>	EdgePtrUIntMap;			//!< Map of edges, ordered by index
	typedef std::map<		uint32_t,		EdgePtrUIntMap	>	EdgePtrUIntMapUIntMap;
	typedef std::map<		uint32_t,		VertexSPtr		>	VertexPtrUIntMap;

	//! Edge representation
	/*!
	An edge is defined between 2 vertices used to separate 2 faces
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Edge
	{
	private:
		VertexSPtr			m_firstVertex;
		VertexSPtr			m_secondVertex;
		Castor3D::FaceSPtr	m_firstFace;
		Castor3D::FaceSPtr	m_secondFace;

		VertexSPtr			m_createdVertex;
		bool				m_divided;
		bool				m_toDivide;
		bool				m_toDelete;

	public:
		/**
		 * Constructor
		 *\param[in]	p_v1	The 1st point
		 *\param[in]	p_v2	The 2nd point
		 *\param[in]	p_f1	The 1st face of the edge
		 *\param[in]	p_toDivide	Tells if the edge has to be divided
		 */
		Edge( VertexSPtr p_v1, VertexSPtr p_v2, Castor3D::FaceSPtr p_f1, bool p_toDivide);
		/**
		 * Destructor
		 */
		~Edge();
		/**
		 * Adds a face to the edge (max 2 faces, 1 at each side of the edge)
		 *\param[in]	p_face	Pointer to the face to add
		 */
		void AddFace( Castor3D::FaceSPtr p_face);
		/**
		 * Divides the edge id est adds a vertex in a portion of the edge determined by p_value (0.5 = middle).
		 * Doesn't divide the faces
		 *\param[in]	p_submesh	The submesh to which the newly created point is added
		 *\param[in]	p_value	Percentage of the segment [m_firstVertex,m_secondVertex]
		 *\return	The created point
		 */
		VertexSPtr Divide( Subdivider * p_pDivider, Castor::real p_value);

	public:
		/**@name Accessors */
		//@{
		inline bool					IsDivided	()const	{ return m_divided;			}
		inline VertexSPtr			GetVertex1	()const	{ return m_firstVertex;		}
		inline VertexSPtr			GetVertex2	()const	{ return m_secondVertex;	}
		inline Castor3D::FaceSPtr	GetFace1	()const	{ return m_firstFace;		}
		inline Castor3D::FaceSPtr	GetFace2	()const	{ return m_secondFace;		}

		inline void					SetToDivide	()		{ m_toDivide = true;m_divided = false;m_createdVertex.reset(); }
		inline void					SetToDelete	()		{ m_toDelete = true; }
		//@}
	};
	//! Loop Algorithm vertex representation
	/*!
	It's a vertex with additional information : neighbourhood and associated edges
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 12/01/2011
	*/
	class Vertex
	{
	private:
		Castor3D::BufferElementGroupSPtr	m_ptPoint;
		EdgePtrUIntMap						m_mapEdges;

	public:
		Vertex( Castor3D::BufferElementGroupSPtr p_ptPoint);
		~Vertex();

		bool	HasEdge		( uint32_t p_uiIndex);
		EdgePtr	GetEdge		( uint32_t p_uiIndex);
		void	AddEdge		( EdgePtr p_pEdge, uint32_t p_uiIndex);
		void	RemoveEdge	( uint32_t p_uiIndex);

		/**@name Accessors */
		//@{
		inline Castor3D::BufferElementGroupSPtr	GetPoint		()const									{ return m_ptPoint;							}
		inline uint32_t							GetIndex		()const									{ return m_ptPoint->GetIndex();				}
		inline uint32_t							Size			()const									{ return uint32_t( m_mapEdges.size() );		}
		inline EdgePtrUIntMap::iterator			Begin			()										{ return m_mapEdges.begin();				}
		inline EdgePtrUIntMap::const_iterator	End				()const									{ return m_mapEdges.end();					}
		inline void								Erase			( EdgePtrUIntMap::iterator p_it )		{ m_mapEdges.erase( p_it);					}
		inline void								Clear			()										{ m_mapEdges.clear();						}
		//@}
	};
	//! Face edges representation
	/*!
	Holds the three edges constituted by a face
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class FaceEdges
	{
	private:
		Subdivider *		m_pDivider;
		VertexSPtr			m_pVertex0;
		VertexSPtr			m_pVertex1;
		VertexSPtr			m_pVertex2;
		Castor3D::FaceSPtr	m_face;
		EdgePtr				m_edgeAB;
		EdgePtr				m_edgeBC;
		EdgePtr				m_edgeCA;
		bool				m_bOwnFace;

	public:
		/**
		 * Constructor
		 *\param[in]	p_submesh	The submesh holding the face
		 *\param[in]	p_face	The face
		 *\param[in,out]	p_existingEdges	The map of already existing edges, sorted by vertex
		 *\param[in,out]	p_vertexNeighbourhood	The list of vertices neighbours
		 *\param[in,out]	p_allEdges	All the edges
		 */
		FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face,  VertexPtrUIntMap & p_mapVertex);
		/**
		 * Constructor
		 *\param[in]	p_submesh	The submesh holding the face
		 *\param[in]	p_face	The face
		 *\param[in]	l_ab	Edge between 1st and 2nd vertex
		 *\param[in]	l_bc	Edge between 2nd and 3rd vertex
		 *\param[in]	l_ca	Edge between 3rd and 1st vertex
		 */
		FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face, EdgePtr l_ab, EdgePtr l_bc, EdgePtr l_ca);
		/**
		 * Divides the edges held, creates needed faces to complete the division
		 *\param[in]	p_value	The weight of division (if 0.5, divides all edges in the middle)
		 *\param[in,out]	p_existingEdges	The map of already existing edges, sorted by vertex
		 *\param[in,out]	p_vertexNeighbourhood	The list of vertices neighbours
		 *\param[in,out]	p_allEdges	All the edges
		 *\param[in,out]	p_newFaces	The array of newly created faces. All faces created by the subdivision are put inside it
		 *\param[in,out]	p_newVertices	The array of newly created vertices. All vertices created by the subdivision are put inside it
		 */
		void Divide( Castor::real p_value, VertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces);

	public:
		/**@name Accessors */
		//@{
		inline EdgePtr GetEdgesAB() { return m_edgeAB; }
		inline EdgePtr GetEdgesBC() { return m_edgeBC; }
		inline EdgePtr GetEdgesCA() { return m_edgeCA; }
		//@}

	private:
		void DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c, Castor::Point3r const & p_aTex, Castor::Point3r const & p_bTex, Castor::Point3r const & p_cTex, FaceEdgesPtrArray & p_newFaces);
		void DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c, Castor::Point3r const & p_aTex, Castor::Point3r const & p_bTex, Castor::Point3r const & p_cTex, EdgePtr p_edgeAB, EdgePtr p_edgeBC, EdgePtr p_edgeCA, FaceEdgesPtrArray & p_newFaces);
		EdgePtr DoAddEdge( VertexSPtr p_v1, VertexSPtr p_v2, bool p_toDivide);
		void DoRemoveEdge( EdgePtr p_edge);
	};
	//! Loop subdivision algorithm Subdivider
	/*!
	This class implements the Loop subdivision algorithm
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Subdivider : public Castor3D::Subdivider
	{
	private:
		VertexPtrUIntMap	m_mapVertex;
		FaceEdgesPtrArray	m_facesEdges;

	public:
		Subdivider();
		virtual ~Subdivider();
		virtual void Cleanup();
		/**
		 * Creates and adds a vertex to my list
		 *\param[in]	x	The vertex X coordinate
		 *\param[in]	y	The vertex Y coordinate
		 *\param[in]	z	The vertex Z coordinate
		 *\return	The created vertex
		 */
		VertexSPtr AddPoint( Castor::real x, Castor::real y, Castor::real z);
		/**
		 * Adds a vertex to my list
		 *\param[in]	p_v	The vertex to add
		 *\return	The vertex
		 */
		VertexSPtr AddPoint( Castor::Point3r const & p_v);
		/**
		 * Creates and adds a vertex to my list
		 *\param[in]	p_v	The vertex coordinates
		 *\return	The created vertex
		 */
		VertexSPtr AddPoint( Castor::real * p_v);

		inline Castor3D::SubmeshSPtr GetSubmesh()const { return m_submesh; }

	private:
		virtual void DoInitialise();
		virtual void DoSubdivide();
		void DoDivide();
		void DoAverage( Castor::Point3r const & p_center);
	};
}

Castor::String & operator << ( Castor::String & p_stream, Loop::Vertex const & p_vertex);

#endif
