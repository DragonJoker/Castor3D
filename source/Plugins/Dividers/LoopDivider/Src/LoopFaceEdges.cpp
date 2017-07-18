#include "LoopFaceEdges.hpp"

#include "LoopEdge.hpp"
#include "LoopVertex.hpp"
#include "LoopDivider.hpp"

using namespace Castor;

namespace Loop
{
	FaceEdges::FaceEdges( Subdivider * p_pDivider, Castor3D::Face p_face, VertexPtrUIntMap & p_mapVertex )
		: m_pDivider( p_pDivider )
		, m_face( p_face )
		, m_bOwnFace( true )
		, m_pVertex0( p_mapVertex[p_face[0]] )
		, m_pVertex1( p_mapVertex[p_face[1]] )
		, m_pVertex2( p_mapVertex[p_face[2]] )
	{
		m_edgeAB = DoAddEdge( m_pVertex0.lock(), m_pVertex1.lock(), true );
		m_edgeBC = DoAddEdge( m_pVertex1.lock(), m_pVertex2.lock(), true );
		m_edgeCA = DoAddEdge( m_pVertex2.lock(), m_pVertex0.lock(), true );
	}

	FaceEdges::FaceEdges( Subdivider * p_pDivider, Castor3D::Face p_face, EdgeSPtr ab, EdgeSPtr bc, EdgeSPtr ca )
		: m_pDivider( p_pDivider )
		, m_face( p_face )
		, m_edgeAB( ab )
		, m_edgeBC( bc )
		, m_edgeCA( ca )
		, m_bOwnFace( false )
		, m_pVertex0( ab->GetVertex1() )
		, m_pVertex1( bc->GetVertex1() )
		, m_pVertex2( ca->GetVertex1() )
	{
	}

	void FaceEdges::Divide( real p_value, VertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces )
	{
		Point3r aTex, bTex, cTex, dTex, eTex, fTex;
		//// We first retrieve the 3 face's vertices
		VertexSPtr a = p_mapVertex[m_face[0]];
		VertexSPtr b = p_mapVertex[m_face[1]];
		VertexSPtr c = p_mapVertex[m_face[2]];
		// We divide the 3 edges of the face and retrieve the resulting vertices
		VertexSPtr d = m_edgeAB->Divide( m_pDivider, p_value );
		VertexSPtr e = m_edgeBC->Divide( m_pDivider, p_value );
		VertexSPtr f = m_edgeCA->Divide( m_pDivider, p_value );
		// We remove the 3 original edges
		DoRemoveEdge( m_edgeAB );
		DoRemoveEdge( m_edgeBC );
		DoRemoveEdge( m_edgeCA );
		m_edgeAB.reset();
		m_edgeBC.reset();
		m_edgeCA.reset();
		// We then compute the texture coordinates of the new 3 vertices
		Castor3D::Vertex::GetTexCoord( a->GetPoint(), aTex );
		Castor3D::Vertex::GetTexCoord( b->GetPoint(), bTex );
		Castor3D::Vertex::GetTexCoord( c->GetPoint(), cTex );
		dTex = aTex + ( bTex - aTex ) * p_value;
		eTex = bTex + ( cTex - bTex ) * p_value;
		fTex = cTex + ( aTex - cTex ) * p_value;
		// Then we add the 4 resulting faces
		DoAddFaceAndEdges( a, d, f, aTex, dTex, fTex, p_newFaces );
		DoAddFaceAndEdges( d, b, e, dTex, bTex, eTex, p_newFaces );
		DoAddFaceAndEdges( e, c, f, eTex, cTex, fTex, p_newFaces );
		DoAddFaceAndEdges( d, e, f, dTex, eTex, fTex, p_newFaces );
	}

	void FaceEdges::DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									   Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									   FaceEdgesPtrArray & p_newFaces )
	{
		// First we create the 3 edges of the face
		EdgeSPtr edgeAB = DoAddEdge( p_a, p_b,	true );
		EdgeSPtr edgeBC = DoAddEdge( p_b, p_c,	true );
		EdgeSPtr edgeCA = DoAddEdge( p_c, p_a,	true );
		// Then we add it
		DoAddFaceAndEdges( p_a, p_b, p_c, p_aTex, p_bTex, p_cTex, edgeAB, edgeBC, edgeCA, p_newFaces );
	}

	void FaceEdges::DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									   Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									   EdgeSPtr p_edgeAB, EdgeSPtr p_edgeBC, EdgeSPtr p_edgeCA,
									   FaceEdgesPtrArray & p_newFaces )
	{
		auto face = m_pDivider->AddFace( p_a->GetIndex(), p_b->GetIndex(), p_c->GetIndex() );
		Castor3D::Vertex::SetTexCoord( p_a->GetPoint(), p_aTex );
		Castor3D::Vertex::SetTexCoord( p_b->GetPoint(), p_bTex );
		Castor3D::Vertex::SetTexCoord( p_c->GetPoint(), p_cTex );
		p_newFaces.push_back( std::make_shared< FaceEdges >( m_pDivider, face, p_edgeAB, p_edgeBC, p_edgeCA ) );
	}

	EdgeSPtr FaceEdges::DoAddEdge( VertexSPtr p_v1, VertexSPtr p_v2, bool p_toDivide )
	{
		// First we check if we have the edge v1->v2
		EdgeSPtr result = p_v1->GetEdge( p_v2->GetIndex() );
		bool bCreated = false;

		if ( !result )
		{
			// We haven't it, we check if we have the edge v2->v1
			result = p_v2->GetEdge( p_v1->GetIndex() );

			if ( !result )
			{
				// We haven't it, so we create the edge
				result = std::make_shared< Edge >( p_v1, p_v2, m_face, p_toDivide );
				bCreated = true;
			}

			// We tell v1 it has an edge with v2
			p_v1->AddEdge( result, p_v2->GetIndex() );
		}

		if ( !p_v2->HasEdge( p_v1->GetIndex() ) )
		{
			// We tell v2 it has an edge with v1
			p_v2->AddEdge( result, p_v1->GetIndex() );
		}

		if ( !bCreated )
		{
			// We add the face to the edge's faces
			result->AddFace( m_face );
		}

		return result;
	}

	void FaceEdges::DoRemoveEdge( EdgeSPtr p_edge )
	{
		// We retrieve the two vertices of the edge
		VertexSPtr v1 = p_edge->GetVertex1();
		VertexSPtr v2 = p_edge->GetVertex2();
		// We tell each vertex it doesn't have anymore an edge with the other one
		v1->RemoveEdge( v2->GetIndex() );
		v2->RemoveEdge( v1->GetIndex() );
	}
}
