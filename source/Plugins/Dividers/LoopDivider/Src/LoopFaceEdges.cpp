#include "LoopFaceEdges.hpp"

#include "LoopEdge.hpp"
#include "LoopVertex.hpp"
#include "LoopDivider.hpp"

using namespace Castor;

namespace Loop
{
	FaceEdges::FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face, VertexPtrUIntMap & p_mapVertex )
		:	m_pDivider( p_pDivider	)
		,	m_face( p_face	)
		,	m_bOwnFace( true	)
		,	m_pVertex0( p_mapVertex[p_face->GetVertexIndex( 0 )]	)
		,	m_pVertex1( p_mapVertex[p_face->GetVertexIndex( 1 )]	)
		,	m_pVertex2( p_mapVertex[p_face->GetVertexIndex( 2 )]	)
	{
		m_edgeAB = DoAddEdge( m_pVertex0.lock(), m_pVertex1.lock(), true );
		m_edgeBC = DoAddEdge( m_pVertex1.lock(), m_pVertex2.lock(), true );
		m_edgeCA = DoAddEdge( m_pVertex2.lock(), m_pVertex0.lock(), true );
	}

	FaceEdges::FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face, EdgeSPtr l_ab, EdgeSPtr l_bc, EdgeSPtr l_ca )
		:	m_pDivider( p_pDivider	)
		,	m_face( p_face	)
		,	m_edgeAB( l_ab	)
		,	m_edgeBC( l_bc	)
		,	m_edgeCA( l_ca	)
		,	m_bOwnFace( false	)
		,	m_pVertex0( l_ab->GetVertex1()	)
		,	m_pVertex1( l_bc->GetVertex1()	)
		,	m_pVertex2( l_ca->GetVertex1()	)
	{
	}

	void FaceEdges::Divide( real p_value, VertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces )
	{
		Point3r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;
		//// We first retrieve the 3 face's vertices
		VertexSPtr l_a = p_mapVertex[m_face->GetVertexIndex( 0 )];
		VertexSPtr l_b = p_mapVertex[m_face->GetVertexIndex( 1 )];
		VertexSPtr l_c = p_mapVertex[m_face->GetVertexIndex( 2 )];
		// We divide the 3 edges of the face and retrieve the resulting vertices
		VertexSPtr l_d = m_edgeAB->Divide( m_pDivider, p_value );
		VertexSPtr l_e = m_edgeBC->Divide( m_pDivider, p_value );
		VertexSPtr l_f = m_edgeCA->Divide( m_pDivider, p_value );
		// We remove the 3 original edges
		DoRemoveEdge( m_edgeAB );
		DoRemoveEdge( m_edgeBC );
		DoRemoveEdge( m_edgeCA );
		m_edgeAB.reset();
		m_edgeBC.reset();
		m_edgeCA.reset();
		// We then compute the texture coordinates of the new 3 vertices
		Castor3D::Vertex::GetTexCoord( l_a->GetPoint(), l_aTex );
		Castor3D::Vertex::GetTexCoord( l_b->GetPoint(), l_bTex );
		Castor3D::Vertex::GetTexCoord( l_c->GetPoint(), l_cTex );
		l_dTex = l_aTex + ( l_bTex - l_aTex ) * p_value;
		l_eTex = l_bTex + ( l_cTex - l_bTex ) * p_value;
		l_fTex = l_cTex + ( l_aTex - l_cTex ) * p_value;
		// Then we add the 4 resulting faces
		DoAddFaceAndEdges( l_a, l_d, l_f, l_aTex, l_dTex, l_fTex, p_newFaces );
		DoAddFaceAndEdges( l_d, l_b, l_e, l_dTex, l_bTex, l_eTex, p_newFaces );
		DoAddFaceAndEdges( l_e, l_c, l_f, l_eTex, l_cTex, l_fTex, p_newFaces );
		DoAddFaceAndEdges( l_d, l_e, l_f, l_dTex, l_eTex, l_fTex, p_newFaces );
	}

	void FaceEdges::DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									   Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									   FaceEdgesPtrArray & p_newFaces )
	{
		// First we create the 3 edges of the face
		EdgeSPtr l_edgeAB = DoAddEdge( p_a, p_b,	true );
		EdgeSPtr l_edgeBC = DoAddEdge( p_b, p_c,	true );
		EdgeSPtr l_edgeCA = DoAddEdge( p_c, p_a,	true );
		// Then we add it
		DoAddFaceAndEdges( p_a, p_b, p_c, p_aTex, p_bTex, p_cTex, l_edgeAB, l_edgeBC, l_edgeCA, p_newFaces );
	}

	void FaceEdges::DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									   Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									   EdgeSPtr p_edgeAB, EdgeSPtr p_edgeBC, EdgeSPtr p_edgeCA,
									   FaceEdgesPtrArray & p_newFaces )
	{
		Castor3D::FaceSPtr l_pFace = m_pDivider->AddFace( p_a->GetIndex(), p_b->GetIndex(), p_c->GetIndex() );
		Castor3D::Vertex::SetTexCoord( p_a->GetPoint(), p_aTex );
		Castor3D::Vertex::SetTexCoord( p_b->GetPoint(), p_bTex );
		Castor3D::Vertex::SetTexCoord( p_c->GetPoint(), p_cTex );
		p_newFaces.push_back( std::make_shared< FaceEdges >( m_pDivider, l_pFace, p_edgeAB, p_edgeBC, p_edgeCA ) );
	}

	EdgeSPtr FaceEdges::DoAddEdge( VertexSPtr p_v1, VertexSPtr p_v2, bool p_toDivide )
	{
		// First we check if we have the edge v1->v2
		EdgeSPtr l_pReturn = p_v1->GetEdge( p_v2->GetIndex() );
		bool l_bCreated = false;

		if ( !l_pReturn )
		{
			// We haven't it, we check if we have the edge v2->v1
			l_pReturn = p_v2->GetEdge( p_v1->GetIndex() );

			if ( !l_pReturn )
			{
				// We haven't it, so we create the edge
				l_pReturn = std::make_shared< Edge >( p_v1, p_v2, m_face, p_toDivide );
				l_bCreated = true;
			}

			// We tell v1 it has an edge with v2
			p_v1->AddEdge( l_pReturn, p_v2->GetIndex() );
		}

		if ( !p_v2->HasEdge( p_v1->GetIndex() ) )
		{
			// We tell v2 it has an edge with v1
			p_v2->AddEdge( l_pReturn, p_v1->GetIndex() );
		}

		if ( !l_bCreated )
		{
			// We add the face to the edge's faces
			l_pReturn->AddFace( m_face );
		}

		return l_pReturn;
	}

	void FaceEdges::DoRemoveEdge( EdgeSPtr p_edge )
	{
		// We retrieve the two vertices of the edge
		VertexSPtr l_v1 = p_edge->GetVertex1();
		VertexSPtr l_v2 = p_edge->GetVertex2();
		// We tell each vertex it doesn't have anymore an edge with the other one
		l_v1->RemoveEdge( l_v2->GetIndex() );
		l_v2->RemoveEdge( l_v1->GetIndex() );
	}
}
