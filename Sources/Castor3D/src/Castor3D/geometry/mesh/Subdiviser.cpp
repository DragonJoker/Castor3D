#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/Subdiviser.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"

using namespace Castor3D;

//*********************************************************************************************

Edge :: Edge( VertexPtr p_v1, VertexPtr p_v2, FacePtr p_f1, bool p_toDivide)
	:	m_firstVertex( p_v1),
		m_secondVertex( p_v2),
		m_firstFace( p_f1),
//		m_secondFace( NULL),
		m_divided( false),
//		m_createdVertex( NULL),
		m_toDivide( p_toDivide),
		m_toDelete( false),
		m_refCount( 1)
{
}

Edge :: ~Edge()
{
}

void Edge :: AddFace( FacePtr p_face)
{
	if (m_firstFace.null())
	{
		m_firstFace = p_face;
	}
	else if (m_secondFace.null())
	{
		m_secondFace = p_face;
	}
}

VertexPtr Edge :: Divide( SubmeshPtr p_submesh, real p_value)
{
	VertexPtr l_pReturn;

	if (m_toDivide)
	{
		if ( ! m_divided)
		{
			m_createdVertex = p_submesh->AddVertex( new Vertex( (* m_firstVertex + * m_secondVertex) * p_value));
			m_divided = true;
		}

		l_pReturn = m_createdVertex;
	}

	return l_pReturn;
}

//*********************************************************************************************

FaceEdges :: FaceEdges( SubmeshPtr p_submesh, size_t p_sgIndex, FacePtr p_face,
						EdgePtrMapVPtrMap & p_existingEdges,
						IntVPtrMap & p_vertexNeighbourhood,
						std::set <EdgePtr> & p_allEdges)
	:	m_submesh( p_submesh),
		m_sgIndex( p_sgIndex),
		m_face( p_face)
{
	VertexPtr l_vertex1;
	VertexPtr l_vertex2;
	VertexPtr l_vertex3;

	l_vertex1 = m_face->m_vertex1;
	l_vertex2 = m_face->m_vertex2;
	l_vertex3 = m_face->m_vertex3;

	m_edgeAB = _addEdge( l_vertex1, l_vertex2, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	m_edgeBC = _addEdge( l_vertex2, l_vertex3, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	m_edgeCA = _addEdge( l_vertex3, l_vertex1, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
}

FaceEdges :: FaceEdges( SubmeshPtr p_submesh, size_t p_sgIndex, FacePtr p_face, EdgePtr l_ab, EdgePtr l_bc, EdgePtr l_ca)
	:	m_submesh( p_submesh),
		m_sgIndex( p_sgIndex),
		m_face( p_face),
		m_edgeAB( l_ab),
		m_edgeBC( l_bc),
		m_edgeCA( l_ca)
{
}

void FaceEdges :: Divide( real p_value, EdgePtrMapVPtrMap & p_existingEdges,
						  IntVPtrMap & p_vertexNeighbourhood,
						  std::set <EdgePtr> & p_allEdges, FaceEdgesPtrArray & p_newFaces,
						  VertexPtrArray & p_newVertices)
{
	Point2r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;
	VertexPtr l_a = m_face->m_vertex1;
	VertexPtr l_b = m_face->m_vertex2;
	VertexPtr l_c = m_face->m_vertex3;
	VertexPtr l_d = m_edgeAB->Divide( m_submesh, p_value);
	VertexPtr l_e = m_edgeBC->Divide( m_submesh, p_value);
	VertexPtr l_f = m_edgeCA->Divide( m_submesh, p_value);

	p_newVertices.push_back( l_d);
	p_newVertices.push_back( l_e);
	p_newVertices.push_back( l_f);

	_removeEdge( m_edgeAB, p_existingEdges, p_allEdges);
	_removeEdge( m_edgeBC, p_existingEdges, p_allEdges);
	_removeEdge( m_edgeCA, p_existingEdges, p_allEdges);

	l_aTex[0] = m_face->m_vertex1TexCoord[0];
	l_aTex[1] = m_face->m_vertex1TexCoord[1];
	l_bTex[0] = m_face->m_vertex2TexCoord[0];
	l_bTex[1] = m_face->m_vertex2TexCoord[1];
	l_cTex[0] = m_face->m_vertex3TexCoord[0];
	l_cTex[1] = m_face->m_vertex3TexCoord[1];

	l_dTex[0] = m_face->m_vertex1TexCoord[0] + (m_face->m_vertex2TexCoord[0] - m_face->m_vertex1TexCoord[0]) * p_value;
	l_dTex[1] = m_face->m_vertex1TexCoord[1] + (m_face->m_vertex2TexCoord[1] - m_face->m_vertex1TexCoord[1]) * p_value;
	l_eTex[0] = m_face->m_vertex2TexCoord[0] + (m_face->m_vertex3TexCoord[0] - m_face->m_vertex2TexCoord[0]) * p_value;
	l_eTex[1] = m_face->m_vertex2TexCoord[1] + (m_face->m_vertex3TexCoord[1] - m_face->m_vertex2TexCoord[1]) * p_value;
	l_fTex[0] = m_face->m_vertex3TexCoord[0] + (m_face->m_vertex1TexCoord[0] - m_face->m_vertex3TexCoord[0]) * p_value;
	l_fTex[1] = m_face->m_vertex3TexCoord[1] + (m_face->m_vertex1TexCoord[1] - m_face->m_vertex3TexCoord[1]) * p_value;

	FacePtr l_faceADF = m_submesh->AddFace( l_a, l_d, l_f, m_sgIndex);
	l_faceADF->SetTexCoordV1( l_aTex[0], l_aTex[1]);
	l_faceADF->SetTexCoordV2( l_dTex[0], l_dTex[1]);
	l_faceADF->SetTexCoordV3( l_fTex[0], l_fTex[1]);
	EdgePtr l_edgeAD = _addEdge( l_a, l_d, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeDF = _addEdge( l_d, l_f, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeFA = _addEdge( l_f, l_a, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceADF, l_edgeAD, l_edgeDF, l_edgeFA));

	FacePtr l_faceDBE = m_submesh->AddFace( l_d, l_b, l_e, m_sgIndex);
	l_faceDBE->SetTexCoordV1( l_dTex[0], l_dTex[1]);
	l_faceDBE->SetTexCoordV2( l_bTex[0], l_bTex[1]);
	l_faceDBE->SetTexCoordV3( l_eTex[0], l_eTex[1]);
	EdgePtr l_edgeDB = _addEdge( l_d, l_b, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeBE = _addEdge( l_b, l_e, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeED = _addEdge( l_e, l_d, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceDBE, l_edgeDB, l_edgeBE, l_edgeED));

	FacePtr l_faceECF = m_submesh->AddFace( l_e, l_c, l_f, m_sgIndex);
	l_faceECF->SetTexCoordV1( l_eTex[0], l_eTex[1]);
	l_faceECF->SetTexCoordV2( l_cTex[0], l_cTex[1]);
	l_faceECF->SetTexCoordV3( l_fTex[0], l_fTex[1]);
	EdgePtr l_edgeEC = _addEdge( l_e, l_c, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeCF = _addEdge( l_c, l_f, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeFE = _addEdge( l_f, l_e, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceECF, l_edgeEC, l_edgeCF, l_edgeFE));

	FacePtr l_faceDEF = m_submesh->AddFace( l_d, l_e, l_f, m_sgIndex);
	l_faceDEF->SetTexCoordV1( l_dTex[0], l_dTex[1]);
	l_faceDEF->SetTexCoordV2( l_eTex[0], l_eTex[1]);
	l_faceDEF->SetTexCoordV3( l_fTex[0], l_fTex[1]);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceDEF, l_edgeED, l_edgeFE, l_edgeDF));

//	delete m_face;
	m_face.reset();
}

EdgePtr FaceEdges :: _addEdge( VertexPtr p_v1, VertexPtr p_v2, bool p_toDivide, bool p_add1, bool p_add2,
							  EdgePtrMapVPtrMap & p_existingEdges,
							  IntVPtrMap & p_vertexNeighbourhood,
							  std::set <EdgePtr> & p_allEdges)
{
	EdgePtrMapVPtrMap::iterator l_it1;
	EdgePtrMapVPtrMap::iterator l_it2;
	IntVPtrMap::iterator l_nit1;
	IntVPtrMap::iterator l_nit2;
	EdgePtrMap::iterator l_edgeIt;

	if ((l_it1 = p_existingEdges.find( p_v1)) == p_existingEdges.end())
	{
		EdgePtrMap l_edges;
		p_vertexNeighbourhood.insert( IntVPtrMap::value_type( p_v1, 0));
		p_existingEdges.insert( EdgePtrMapVPtrMap::value_type( p_v1, l_edges));
		l_it1 = p_existingEdges.find( p_v1);
	}
	l_nit1 = p_vertexNeighbourhood.find( p_v1);

	if ((l_it2 = p_existingEdges.find( p_v2)) == p_existingEdges.end())
	{
		EdgePtrMap l_edges;
		p_vertexNeighbourhood.insert( IntVPtrMap::value_type( p_v2, 0));
		p_existingEdges.insert( EdgePtrMapVPtrMap::value_type( p_v2, l_edges));
		l_it2 = p_existingEdges.find( p_v2);
	}
	l_nit2 = p_vertexNeighbourhood.find( p_v2);

	if ((l_edgeIt = l_it1->second.find( p_v2)) == l_it1->second.end())
	{
		EdgePtr l_edge = new Edge( p_v1, p_v2, m_face, p_toDivide);
		p_allEdges.insert( l_edge);
		l_it1->second.insert( EdgePtrMap::value_type( p_v2, l_edge));
		l_it2->second.insert( EdgePtrMap::value_type( p_v1, l_edge));
		l_edgeIt = l_it1->second.find( p_v2);
		if (p_add1)
		{
			l_nit1->second++;
		}
		if (p_add2)
		{
			l_nit2->second++;
		}
	}
	else
	{
		l_edgeIt->second->AddFace( m_face);
		l_edgeIt->second->Ref();
	}

	return l_edgeIt->second;
}

void FaceEdges :: _removeEdge( EdgePtr p_edge, EdgePtrMapVPtrMap & p_existingEdges,
							   std::set <EdgePtr> & p_allEdges)
{
	if (p_allEdges.find( p_edge) != p_allEdges.end())
	{
		VertexPtr l_v1 = p_edge->GetVertex1();
		VertexPtr l_v2 = p_edge->GetVertex2();
		int l_count = p_edge->Release();

		if (l_count <= 0)
		{
			p_allEdges.erase( p_allEdges.find( p_edge));
			EdgePtrMapVPtrMap::iterator l_it;
			l_it = p_existingEdges.find( l_v1);
			l_it->second.erase( l_it->second.find( l_v2));
			l_it = p_existingEdges.find( l_v2);
			l_it->second.erase( l_it->second.find( l_v1));
			p_edge.reset();
//			delete p_edge;
		}
	}
}

//*********************************************************************************************

Subdiviser :: Subdiviser( Submesh * p_submesh)
	:	m_submesh( p_submesh),
		m_vertex( m_submesh->GetVertices()),
		m_smoothGroups( m_submesh->GetSmoothGroups())
{
}

void Subdiviser :: _setTextCoords( FacePtr p_face, VertexPtr p_a, VertexPtr p_b, VertexPtr p_c, 
								   VertexPtr p_d, VertexPtr p_e, VertexPtr p_f, size_t p_sgIndex)
{
	Point2r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;

	l_aTex[0] = p_face->m_vertex1TexCoord[0];
	l_aTex[1] = p_face->m_vertex1TexCoord[1];
	l_bTex[0] = p_face->m_vertex2TexCoord[0];
	l_bTex[1] = p_face->m_vertex2TexCoord[1];
	l_cTex[0] = p_face->m_vertex3TexCoord[0];
	l_cTex[1] = p_face->m_vertex3TexCoord[1];

	l_dTex[0] = p_face->m_vertex1TexCoord[0] + (p_face->m_vertex2TexCoord[0] - p_face->m_vertex1TexCoord[0]) / 2.0f;
	l_dTex[1] = p_face->m_vertex1TexCoord[1] + (p_face->m_vertex2TexCoord[1] - p_face->m_vertex1TexCoord[1]) / 2.0f;
	l_eTex[0] = p_face->m_vertex2TexCoord[0] + (p_face->m_vertex3TexCoord[0] - p_face->m_vertex2TexCoord[0]) / 2.0f;
	l_eTex[1] = p_face->m_vertex2TexCoord[1] + (p_face->m_vertex3TexCoord[1] - p_face->m_vertex2TexCoord[1]) / 2.0f;
	l_fTex[0] = p_face->m_vertex1TexCoord[0] + (p_face->m_vertex3TexCoord[0] - p_face->m_vertex1TexCoord[0]) / 2.0f;
	l_fTex[1] = p_face->m_vertex1TexCoord[1] + (p_face->m_vertex3TexCoord[1] - p_face->m_vertex1TexCoord[1]) / 2.0f;

	FacePtr l_face = m_submesh->AddFace( p_a, p_d, p_f, p_sgIndex);
	l_face->SetTexCoordV1( l_aTex[0], l_aTex[1]);
	l_face->SetTexCoordV2( l_dTex[0], l_dTex[1]);
	l_face->SetTexCoordV3( l_fTex[0], l_fTex[1]);
	l_face = m_submesh->AddFace( p_b, p_e, p_d, p_sgIndex);
	l_face->SetTexCoordV1( l_bTex[0], l_bTex[1]);
	l_face->SetTexCoordV2( l_eTex[0], l_eTex[1]);
	l_face->SetTexCoordV3( l_dTex[0], l_dTex[1]);
	l_face = m_submesh->AddFace( p_c, p_f, p_e, p_sgIndex);
	l_face->SetTexCoordV1( l_cTex[0], l_cTex[1]);
	l_face->SetTexCoordV2( l_fTex[0], l_fTex[1]);
	l_face->SetTexCoordV3( l_eTex[0], l_eTex[1]);
	l_face = m_submesh->AddFace( p_d, p_e, p_f, p_sgIndex);
	l_face->SetTexCoordV1( l_dTex[0], l_dTex[1]);
	l_face->SetTexCoordV2( l_eTex[0], l_eTex[1]);
	l_face->SetTexCoordV3( l_fTex[0], l_fTex[1]);
}

VertexPtr Subdiviser :: _computeCenterFrom( const Vertex & p_a, real * p_aNormal, const Vertex & p_b, real * p_bNormal)
{
	VertexPtr l_result = new Vertex;

	// Projection sur XY => Récupération du point d'intersection (X, Y) y = ax + b
	Line3D<real> l_aEq( p_a, p_aNormal);
	Line3D<real> l_bEq( p_b, p_bNormal);
	Vertex l_v1;

	if (l_aEq.Intersects( l_bEq, l_v1))
	{
		// Projection sur XZ => Récupération du point d'intersection (X, Z) z = ax + b
		l_aEq = Line3D<real>( p_a, p_aNormal);
		l_bEq = Line3D<real>( p_b, p_bNormal);
		Vertex l_v2;

		if (l_aEq.Intersects( l_bEq, l_v2))
		{
			l_result->m_coords[0] = l_v1[0] + (l_v2[0] - l_v1[0]) / 2.0f;
			l_result->m_coords[1] = l_v1[1] + (l_v2[1] - l_v1[1]) / 2.0f;
			l_result->m_coords[2] = l_v1[2] + (l_v2[2] - l_v1[2]) / 2.0f;
		}
	}

	return l_result;
}

VertexPtr Subdiviser :: _computeCenterFrom( const Vertex & p_a, real * p_aNormal,
											 const Vertex & p_b, real * p_bNormal,
											 const Vertex & p_c, real * p_cNormal)
{
	VertexPtr l_result = new Vertex;

	Plane<real> ABO( p_a, p_aNormal, p_bNormal);
	Plane<real> BCO( p_b, p_bNormal, p_cNormal);
	Plane<real> CAO( p_c, p_cNormal, p_aNormal);

	ABO.Intersects( BCO, CAO, *l_result);

	return l_result;
}

//*********************************************************************************************