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
		VertexWPtr m_pVertex0;
		VertexWPtr m_pVertex1;
		VertexWPtr m_pVertex2;
		Castor3D::Face m_face;
		EdgeSPtr m_edgeAB;
		EdgeSPtr m_edgeBC;
		EdgeSPtr m_edgeCA;
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
		FaceEdges( Subdivider * p_pDivider, Castor3D::Face p_face,  VertexPtrUIntMap & p_mapVertex );
		/**
		 * Constructor
		 *\param[in]	p_submesh	The submesh holding the face
		 *\param[in]	p_face	The face
		 *\param[in]	ab	Edge between 1st and 2nd vertex
		 *\param[in]	bc	Edge between 2nd and 3rd vertex
		 *\param[in]	ca	Edge between 3rd and 1st vertex
		 */
		FaceEdges( Subdivider * p_pDivider, Castor3D::Face p_face, EdgeSPtr ab, EdgeSPtr bc, EdgeSPtr ca );
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
		inline EdgeSPtr GetEdgesAB()
		{
			return m_edgeAB;
		}
		inline EdgeSPtr GetEdgesBC()
		{
			return m_edgeBC;
		}
		inline EdgeSPtr GetEdgesCA()
		{
			return m_edgeCA;
		}
		//@}

	private:
		void DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c, Castor::Point3r const & p_aTex, Castor::Point3r const & p_bTex, Castor::Point3r const & p_cTex, FaceEdgesPtrArray & p_newFaces );
		void DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c, Castor::Point3r const & p_aTex, Castor::Point3r const & p_bTex, Castor::Point3r const & p_cTex, EdgeSPtr p_edgeAB, EdgeSPtr p_edgeBC, EdgeSPtr p_edgeCA, FaceEdgesPtrArray & p_newFaces );
		EdgeSPtr DoAddEdge( VertexSPtr p_v1, VertexSPtr p_v2, bool p_toDivide );
		void DoRemoveEdge( EdgeSPtr p_edge );
	};
}

#endif
