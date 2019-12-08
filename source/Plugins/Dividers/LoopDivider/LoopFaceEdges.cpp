#include "LoopDivider/LoopFaceEdges.hpp"

#include "LoopDivider/LoopEdge.hpp"
#include "LoopDivider/LoopVertex.hpp"
#include "LoopDivider/LoopDivider.hpp"

namespace Loop
{
	FaceEdges::FaceEdges( Subdivider * divider
		, castor3d::Face face
		, VertexPtrUIntMap & vertices )
		: m_divider( divider )
		, m_face( face )
		, m_vertex0( vertices[face[0]] )
		, m_vertex1( vertices[face[1]] )
		, m_vertex2( vertices[face[2]] )
	{
		m_edgeAB = doAddEdge( m_vertex0.lock(), m_vertex1.lock(), true );
		m_edgeBC = doAddEdge( m_vertex1.lock(), m_vertex2.lock(), true );
		m_edgeCA = doAddEdge( m_vertex2.lock(), m_vertex0.lock(), true );
	}

	FaceEdges::FaceEdges( Subdivider * divider
		, castor3d::Face face
		, EdgeSPtr ab
		, EdgeSPtr bc
		, EdgeSPtr ca )
		: m_divider( divider )
		, m_face( face )
		, m_edgeAB( ab )
		, m_edgeBC( bc )
		, m_edgeCA( ca )
		, m_vertex0( ab->getVertex1() )
		, m_vertex1( bc->getVertex1() )
		, m_vertex2( ca->getVertex1() )
	{
	}

	void FaceEdges::divide( float value
		, VertexPtrUIntMap & vertices
		, FaceEdgesPtrArray & newFaces )
	{
		//// We first retrieve the 3 face's vertices
		VertexSPtr a = vertices[m_face[0]];
		VertexSPtr b = vertices[m_face[1]];
		VertexSPtr c = vertices[m_face[2]];
		// We divide the 3 edges of the face and retrieve the resulting vertices
		VertexSPtr d = m_edgeAB->divide( m_divider, value );
		VertexSPtr e = m_edgeBC->divide( m_divider, value );
		VertexSPtr f = m_edgeCA->divide( m_divider, value );
		// We remove the 3 original edges
		doRemoveEdge( *m_edgeAB );
		doRemoveEdge( *m_edgeBC );
		doRemoveEdge( *m_edgeCA );
		m_edgeAB.reset();
		m_edgeBC.reset();
		m_edgeCA.reset();
		// We then compute the texture coordinates of the new 3 vertices
		castor::Point3f aTex = a->getPoint().tex;
		castor::Point3f bTex = b->getPoint().tex;
		castor::Point3f cTex = c->getPoint().tex;
		castor::Point3f dTex = aTex + ( bTex - aTex ) * value;
		castor::Point3f eTex = bTex + ( cTex - bTex ) * value;
		castor::Point3f fTex = cTex + ( aTex - cTex ) * value;
		// Then we add the 4 resulting faces
		doAddFaceAndEdges( a, d, f, aTex, dTex, fTex, newFaces );
		doAddFaceAndEdges( d, b, e, dTex, bTex, eTex, newFaces );
		doAddFaceAndEdges( e, c, f, eTex, cTex, fTex, newFaces );
		doAddFaceAndEdges( d, e, f, dTex, eTex, fTex, newFaces );
	}

	void FaceEdges::doAddFaceAndEdges( VertexSPtr a
		, VertexSPtr b
		, VertexSPtr c
		, castor::Point3f const & aTex
		, castor::Point3f const & bTex
		, castor::Point3f const & cTex
		, FaceEdgesPtrArray & newFaces )
	{
		// First we create the 3 edges of the face
		EdgeSPtr edgeAB = doAddEdge( a, b, true );
		EdgeSPtr edgeBC = doAddEdge( b, c, true );
		EdgeSPtr edgeCA = doAddEdge( c, a, true );
		// Then we add it
		doAddFaceAndEdges( a, b, c, aTex, bTex, cTex, edgeAB, edgeBC, edgeCA, newFaces );
	}

	void FaceEdges::doAddFaceAndEdges( VertexSPtr a
		, VertexSPtr b
		, VertexSPtr c
		, castor::Point3f const & aTex
		, castor::Point3f const & bTex
		, castor::Point3f const & cTex
		, EdgeSPtr edgeAB
		, EdgeSPtr edgeBC
		, EdgeSPtr edgeCA
		, FaceEdgesPtrArray & newFaces )
	{
		auto face = m_divider->addFace( a->getIndex(), b->getIndex(), c->getIndex() );
		a->getPoint().tex = aTex;
		b->getPoint().tex = bTex;
		c->getPoint().tex = cTex;
		newFaces.push_back( std::make_shared< FaceEdges >( m_divider, face, edgeAB, edgeBC, edgeCA ) );
	}

	EdgeSPtr FaceEdges::doAddEdge( VertexSPtr v1
		, VertexSPtr v2
		, bool toDivide )
	{
		// First we check if we have the edge v1->v2
		EdgeSPtr result = v1->getEdge( v2->getIndex() );
		bool created = false;

		if ( !result )
		{
			// We haven't it, we check if we have the edge v2->v1
			result = v2->getEdge( v1->getIndex() );

			if ( !result )
			{
				// We haven't it, so we create the edge
				result = std::make_shared< Edge >( v1, v2, m_face, toDivide );
				created = true;
			}

			// We tell v1 it has an edge with v2
			v1->addEdge( result, v2->getIndex() );
		}

		if ( !v2->hasEdge( v1->getIndex() ) )
		{
			// We tell v2 it has an edge with v1
			v2->addEdge( result, v1->getIndex() );
		}

		if ( !created )
		{
			// We add the face to the edge's faces
			result->addFace( m_face );
		}

		return result;
	}

	void FaceEdges::doRemoveEdge( Edge const & edge )
	{
		// We retrieve the two vertices of the edge
		VertexSPtr v1 = edge.getVertex1();
		VertexSPtr v2 = edge.getVertex2();
		// We tell each vertex it doesn't have anymore an edge with the other one
		v1->removeEdge( v2->getIndex() );
		v2->removeEdge( v1->getIndex() );
	}
}
