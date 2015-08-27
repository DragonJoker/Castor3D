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
#ifndef ___C3D_LOOP_FACE_EDGES_H___
#define ___C3D_LOOP_FACE_EDGES_H___

#include "LoopPrerequisites.hpp"

namespace Loop
{
	//! Face edges representation
	/*!
	Holds the three edges constituted by a face
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class FaceEdges
	{
	private:
		Subdivider * m_pDivider;
		VertexSPtr m_pVertex0;
		VertexSPtr m_pVertex1;
		VertexSPtr m_pVertex2;
		Castor3D::FaceSPtr m_face;
		EdgePtr m_edgeAB;
		EdgePtr m_edgeBC;
		EdgePtr m_edgeCA;
		bool m_bOwnFace;

	public:
		/**
		 * Constructor
		 *\param[in]	p_submesh	The submesh holding the face
		 *\param[in]	p_face	The face
		 *\param[in,out]	p_existingEdges	The map of already existing edges, sorted by vertex
		 *\param[in,out]	p_vertexNeighbourhood	The list of vertices neighbours
		 *\param[in,out]	p_allEdges	All the edges
		 */
		FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face,  VertexPtrUIntMap & p_mapVertex );
		/**
		 * Constructor
		 *\param[in]	p_submesh	The submesh holding the face
		 *\param[in]	p_face	The face
		 *\param[in]	l_ab	Edge between 1st and 2nd vertex
		 *\param[in]	l_bc	Edge between 2nd and 3rd vertex
		 *\param[in]	l_ca	Edge between 3rd and 1st vertex
		 */
		FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face, EdgePtr l_ab, EdgePtr l_bc, EdgePtr l_ca );
		/**
		 * Divides the edges held, creates needed faces to complete the division
		 *\param[in]	p_value	The weight of division (if 0.5, divides all edges in the middle)
		 *\param[in,out]	p_existingEdges	The map of already existing edges, sorted by vertex
		 *\param[in,out]	p_vertexNeighbourhood	The list of vertices neighbours
		 *\param[in,out]	p_allEdges	All the edges
		 *\param[in,out]	p_newFaces	The array of newly created faces. All faces created by the subdivision are put inside it
		 *\param[in,out]	p_newVertices	The array of newly created vertices. All vertices created by the subdivision are put inside it
		 */
		void Divide( Castor::real p_value, VertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces );

	public:
		/**@name Accessors */
		//@{
		inline EdgePtr GetEdgesAB()
		{
			return m_edgeAB;
		}
		inline EdgePtr GetEdgesBC()
		{
			return m_edgeBC;
		}
		inline EdgePtr GetEdgesCA()
		{
			return m_edgeCA;
		}
		//@}

	private:
		void DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c, Castor::Point3r const & p_aTex, Castor::Point3r const & p_bTex, Castor::Point3r const & p_cTex, FaceEdgesPtrArray & p_newFaces );
		void DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c, Castor::Point3r const & p_aTex, Castor::Point3r const & p_bTex, Castor::Point3r const & p_cTex, EdgePtr p_edgeAB, EdgePtr p_edgeBC, EdgePtr p_edgeCA, FaceEdgesPtrArray & p_newFaces );
		EdgePtr DoAddEdge( VertexSPtr p_v1, VertexSPtr p_v2, bool p_toDivide );
		void DoRemoveEdge( EdgePtr p_edge );
	};
}

#endif
