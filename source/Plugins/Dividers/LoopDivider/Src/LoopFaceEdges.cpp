#include "LoopFaceEdges.hpp"

#include "LoopEdge.hpp"
#include "LoopVertex.hpp"
#include "LoopDivider.hpp"

using namespace castor;

namespace Loop
{
	FaceEdges::FaceEdges( Subdivider * p_pDivider, castor3d::Face p_face, VertexPtrUIntMap & p_mapVertex )
		: m_pDivider( p_pDivider )
		, m_face( p_face )
		, m_bOwnFace( true )
		, m_pVertex0( p_mapVertex[p_face[0]] )
		, m_pVertex1( p_mapVertex[p_face[1]] )
		, m_pVertex2( p_mapVertex[p_face[2]] )
	{
		m_edgeAB = doAddEdge( m_pVertex0.lock(), m_pVertex1.lock(), true );
		m_edgeBC = doAddEdge( m_pVertex1.lock(), m_pVertex2.lock(), true );
		m_edgeCA = doAddEdge( m_pVertex2.lock(), m_pVertex0.lock(), true );
	}

	FaceEdges::FaceEdges( Subdivider * p_pDivider, castor3d::Face p_face, EdgeSPtr ab, EdgeSPtr bc, EdgeSPtr ca )
		: m_pDivider( p_pDivider )
		, m_face( p_face )
		, m_edgeAB( ab )
		, m_edgeBC( bc )
		, m_edgeCA( ca )
		, m_bOwnFace( false )
		, m_pVertex0( ab->getVertex1() )
		, m_pVertex1( bc->getVertex1() )
		, m_pVertex2( ca->getVertex1() )
	{
	}

	void FaceEdges::divide( real p_value, VertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces )
	{
		Point3r aTex, bTex, cTex, dTex, eTex, fTex;
		//// We first retrieve the 3 face's vertices
		VertexSPtr a = p_mapVertex[m_face[0]];
		VertexSPtr b = p_mapVertex[m_face[1]];
		VertexSPtr c = p_mapVertex[m_face[2]];
		// We divide the 3 edges of the face and retrieve the resulting vertices
		VertexSPtr d = m_edgeAB->divide( m_pDivider, p_value );
		VertexSPtr e = m_edgeBC->divide( m_pDivider, p_value );
		VertexSPtr f = m_edgeCA->divide( m_pDivider, p_value );
		// We remove the 3 original edges
		doRemoveEdge( m_edgeAB );
		doRemoveEdge( m_edgeBC );
		doRemoveEdge( m_edgeCA );
		m_edgeAB.reset();
		m_edgeBC.reset();
		m_edgeCA.reset();
		// We then compute the texture coordinates of the new 3 vertices
		castor3d::Vertex::getTexCoord( a->getPoint(), aTex );
		castor3d::Vertex::getTexCoord( b->getPoint(), bTex );
		castor3d::Vertex::getTexCoord( c->getPoint(), cTex );
		dTex = aTex + ( bTex - aTex ) * p_value;
		eTex = bTex + ( cTex - bTex ) * p_value;
		fTex = cTex + ( aTex - cTex ) * p_value;
		// Then we add the 4 resulting faces
		doAddFaceAndEdges( a, d, f, aTex, dTex, fTex, p_newFaces );
		doAddFaceAndEdges( d, b, e, dTex, bTex, eTex, p_newFaces );
		doAddFaceAndEdges( e, c, f, eTex, cTex, fTex, p_newFaces );
		doAddFaceAndEdges( d, e, f, dTex, eTex, fTex, p_newFaces );
	}

	void FaceEdges::doAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									   Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									   FaceEdgesPtrArray & p_newFaces )
	{
		// First we create the 3 edges of the face
		EdgeSPtr edgeAB = doAddEdge( p_a, p_b,	true );
		EdgeSPtr edgeBC = doAddEdge( p_b, p_c,	true );
		EdgeSPtr edgeCA = doAddEdge( p_c, p_a,	true );
		// Then we add it
		doAddFaceAndEdges( p_a, p_b, p_c, p_aTex, p_bTex, p_cTex, edgeAB, edgeBC, edgeCA, p_newFaces );
	}

	void FaceEdges::doAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									   Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									   EdgeSPtr p_edgeAB, EdgeSPtr p_edgeBC, EdgeSPtr p_edgeCA,
									   FaceEdgesPtrArray & p_newFaces )
	{
		auto face = m_pDivider->addFace( p_a->getIndex(), p_b->getIndex(), p_c->getIndex() );
		castor3d::Vertex::setTexCoord( p_a->getPoint(), p_aTex );
		castor3d::Vertex::setTexCoord( p_b->getPoint(), p_bTex );
		castor3d::Vertex::setTexCoord( p_c->getPoint(), p_cTex );
		p_newFaces.push_back( std::make_shared< FaceEdges >( m_pDivider, face, p_edgeAB, p_edgeBC, p_edgeCA ) );
	}

	EdgeSPtr FaceEdges::doAddEdge( VertexSPtr p_v1, VertexSPtr p_v2, bool p_toDivide )
	{
		// First we check if we have the edge v1->v2
		EdgeSPtr result = p_v1->getEdge( p_v2->getIndex() );
		bool bCreated = false;

		if ( !result )
		{
			// We haven't it, we check if we have the edge v2->v1
			result = p_v2->getEdge( p_v1->getIndex() );

			if ( !result )
			{
				// We haven't it, so we create the edge
				result = std::make_shared< Edge >( p_v1, p_v2, m_face, p_toDivide );
				bCreated = true;
			}

			// We tell v1 it has an edge with v2
			p_v1->addEdge( result, p_v2->getIndex() );
		}

		if ( !p_v2->hasEdge( p_v1->getIndex() ) )
		{
			// We tell v2 it has an edge with v1
			p_v2->addEdge( result, p_v1->getIndex() );
		}

		if ( !bCreated )
		{
			// We add the face to the edge's faces
			result->addFace( m_face );
		}

		return result;
	}

	void FaceEdges::doRemoveEdge( EdgeSPtr p_edge )
	{
		// We retrieve the two vertices of the edge
		VertexSPtr v1 = p_edge->getVertex1();
		VertexSPtr v2 = p_edge->getVertex2();
		// We tell each vertex it doesn't have anymore an edge with the other one
		v1->removeEdge( v2->getIndex() );
		v2->removeEdge( v1->getIndex() );
	}
}
