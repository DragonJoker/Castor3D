/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LOOP_FACE_EDGES_H___
#define ___C3D_LOOP_FACE_EDGES_H___

#include "LoopDivider/LoopPrerequisites.hpp"

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
	public:
		/**
		 *\brief
		 * Constructor
		 *\param[in]		divider		The divider holding the face.
		 *\param[in]		face		The face.
		 *\param[in,out]	vertices	The vertices map, by index.
		 */
		FaceEdges( Subdivider * divider
			, castor3d::Face face
			,  VertexPtrUIntMap & vertices );
		/**
		 *\brief
		 * Constructor
		 *\param[in]	divider	The divider holding the face.
		 *\param[in]	face	The face.
		 *\param[in]	ab		Edge between 1st and 2nd vertex.
		 *\param[in]	bc		Edge between 2nd and 3rd vertex.
		 *\param[in]	ca		Edge between 3rd and 1st vertex.
		 */
		FaceEdges( Subdivider * divider
			, castor3d::Face face
			, EdgeSPtr ab
			, EdgeSPtr bc
			, EdgeSPtr ca );
		/**
		 *\brief
		 * Divides the edges held, creates needed faces to complete the division.
		 *\param[in]		value		The weight of division (if 0.5, divides all edges in the middle).
		 *\param[in,out]	vertices	The vertices map, by index.
		 *\param[in,out]	newFaces	The array of newly created faces. All faces created by the subdivision are put inside it.
		 */
		void divide( float value
			, VertexPtrUIntMap & vertices
			, FaceEdgesPtrArray & newFaces );

		/**@name Accessors */
		//@{
		inline EdgeSPtr getEdgesAB()
		{
			return m_edgeAB;
		}
		inline EdgeSPtr getEdgesBC()
		{
			return m_edgeBC;
		}
		inline EdgeSPtr getEdgesCA()
		{
			return m_edgeCA;
		}
		//@}

	private:
		void doAddFaceAndEdges( VertexSPtr a
			, VertexSPtr b
			, VertexSPtr c
			, castor::Point3f const & aTex
			, castor::Point3f const & bTex
			, castor::Point3f const & cTex
			, FaceEdgesPtrArray & newFaces );
		void doAddFaceAndEdges( VertexSPtr a
			, VertexSPtr b
			, VertexSPtr c
			, castor::Point3f const & aTex
			, castor::Point3f const & bTex
			, castor::Point3f const & cTex
			, EdgeSPtr edgeAB
			, EdgeSPtr edgeBC
			, EdgeSPtr edgeCA
			, FaceEdgesPtrArray & newFaces );
		EdgeSPtr doAddEdge( VertexSPtr v1
			, VertexSPtr v2
			, bool toDivide );
		void doRemoveEdge( Edge const & edge );

	private:
		Subdivider * m_divider;
		VertexWPtr m_vertex0;
		VertexWPtr m_vertex1;
		VertexWPtr m_vertex2;
		castor3d::Face m_face;
		EdgeSPtr m_edgeAB;
		EdgeSPtr m_edgeBC;
		EdgeSPtr m_edgeCA;
	};
}

#endif
