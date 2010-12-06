#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/Subdiviser.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Vertex.h"
#include "geometry/basic/Face.h"

using namespace Castor3D;

//*********************************************************************************************
/*
Edge :: Edge( Vertex & p_v1, Vertex & p_v2, Face * p_f1, bool p_toDivide)
	:	m_firstVertex( p_v1),
		m_secondVertex( p_v2),
		m_firstFace( p_f1),
		m_secondFace( NULL),
		m_divided( false),
		m_toDivide( p_toDivide),
		m_toDelete( false),
		m_refCount( 1)
{
}

Edge :: ~Edge()
{
}

void Edge :: AddFace( Face * p_face)
{
	if (m_firstFace == NULL)
	{
		m_firstFace = p_face;
	}
	else if (m_secondFace == NULL)
	{
		m_secondFace = p_face;
	}
}

Vertex Edge :: Divide( Submesh * p_submesh, real p_value)
{
	Vertex l_pReturn;

	if (m_toDivide)
	{
		if ( ! m_divided)
		{
			m_createdVertex = p_submesh->AddVertex( Point3r( (m_firstVertex.GetCoords() + m_secondVertex.GetCoords()) * p_value));
			m_createdVertex.m_index = p_submesh->GetVertices().size() - 1;
			m_divided = true;
		}

		l_pReturn = m_createdVertex;
	}

	return l_pReturn;
}

//*********************************************************************************************

FaceEdges :: FaceEdges( Submesh * p_submesh, size_t p_sgIndex, Face * p_face,
						EdgePtrMapVPtrMap & p_existingEdges,
						IntVPtrMap & p_vertexNeighbourhood,
						std::set <EdgePtr> & p_allEdges)
	:	m_submesh( p_submesh),
		m_sgIndex( p_sgIndex),
		m_face( p_face)
{
	Vertex l_vertex1;
	Vertex l_vertex2;
	Vertex l_vertex3;

	l_vertex1 = m_face->m_vertex[0];
	l_vertex2 = m_face->m_vertex[1];
	l_vertex3 = m_face->m_vertex[2];

	m_edgeAB = _addEdge( l_vertex1, l_vertex2, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	m_edgeBC = _addEdge( l_vertex2, l_vertex3, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	m_edgeCA = _addEdge( l_vertex3, l_vertex1, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
}

FaceEdges :: FaceEdges( Submesh * p_submesh, size_t p_sgIndex, Face * p_face, EdgePtr l_ab, EdgePtr l_bc, EdgePtr l_ca)
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
						  VertexArray & p_newVertices)
{
	Point2r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;
	Vertex l_a = m_face->m_vertex[0];
	Vertex l_b = m_face->m_vertex[1];
	Vertex l_c = m_face->m_vertex[2];
	Vertex l_d = m_edgeAB->Divide( m_submesh, p_value);
	Vertex l_e = m_edgeBC->Divide( m_submesh, p_value);
	Vertex l_f = m_edgeCA->Divide( m_submesh, p_value);

	p_newVertices.push_back( l_d);
	p_newVertices.push_back( l_e);
	p_newVertices.push_back( l_f);

	_removeEdge( m_edgeAB, p_existingEdges, p_allEdges);
	_removeEdge( m_edgeBC, p_existingEdges, p_allEdges);
	_removeEdge( m_edgeCA, p_existingEdges, p_allEdges);

	l_aTex[0] = m_face->m_vertex[0].GetTexCoord()[0];
	l_aTex[1] = m_face->m_vertex[0].GetTexCoord()[1];
	l_bTex[0] = m_face->m_vertex[1].GetTexCoord()[0];
	l_bTex[1] = m_face->m_vertex[1].GetTexCoord()[1];
	l_cTex[0] = m_face->m_vertex[2].GetTexCoord()[0];
	l_cTex[1] = m_face->m_vertex[2].GetTexCoord()[1];

	l_dTex[0] = m_face->m_vertex[0].GetTexCoord()[0] + (m_face->m_vertex[1].GetTexCoord()[0] - m_face->m_vertex[0].GetTexCoord()[0]) * p_value;
	l_dTex[1] = m_face->m_vertex[0].GetTexCoord()[1] + (m_face->m_vertex[1].GetTexCoord()[1] - m_face->m_vertex[0].GetTexCoord()[1]) * p_value;
	l_eTex[0] = m_face->m_vertex[1].GetTexCoord()[0] + (m_face->m_vertex[2].GetTexCoord()[0] - m_face->m_vertex[1].GetTexCoord()[0]) * p_value;
	l_eTex[1] = m_face->m_vertex[1].GetTexCoord()[1] + (m_face->m_vertex[2].GetTexCoord()[1] - m_face->m_vertex[1].GetTexCoord()[1]) * p_value;
	l_fTex[0] = m_face->m_vertex[2].GetTexCoord()[0] + (m_face->m_vertex[0].GetTexCoord()[0] - m_face->m_vertex[2].GetTexCoord()[0]) * p_value;
	l_fTex[1] = m_face->m_vertex[2].GetTexCoord()[1] + (m_face->m_vertex[0].GetTexCoord()[1] - m_face->m_vertex[2].GetTexCoord()[1]) * p_value;

	Face & l_faceADF = m_submesh->AddFace( l_a.m_index, l_d.m_index, l_f.m_index, m_sgIndex);
	l_faceADF.SetVertexTexCoords( 0, l_aTex[0], l_aTex[1]);
	l_faceADF.SetVertexTexCoords( 1, l_dTex[0], l_dTex[1]);
	l_faceADF.SetVertexTexCoords( 2, l_fTex[0], l_fTex[1]);
	EdgePtr l_edgeAD = _addEdge( l_a, l_d, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeDF = _addEdge( l_d, l_f, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeFA = _addEdge( l_f, l_a, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( FaceEdgesPtr( new FaceEdges( m_submesh, m_sgIndex, & l_faceADF, l_edgeAD, l_edgeDF, l_edgeFA)));

	Face & l_faceDBE = m_submesh->AddFace( l_d.m_index, l_b.m_index, l_e.m_index, m_sgIndex);
	l_faceDBE.SetVertexTexCoords( 0, l_dTex[0], l_dTex[1]);
	l_faceDBE.SetVertexTexCoords( 1, l_bTex[0], l_bTex[1]);
	l_faceDBE.SetVertexTexCoords( 2, l_eTex[0], l_eTex[1]);
	EdgePtr l_edgeDB = _addEdge( l_d, l_b, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeBE = _addEdge( l_b, l_e, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeED = _addEdge( l_e, l_d, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( FaceEdgesPtr( new FaceEdges( m_submesh, m_sgIndex, & l_faceDBE, l_edgeDB, l_edgeBE, l_edgeED)));

	Face & l_faceECF = m_submesh->AddFace( l_e.m_index, l_c.m_index, l_f.m_index, m_sgIndex);
	l_faceECF.SetVertexTexCoords( 0, l_eTex[0], l_eTex[1]);
	l_faceECF.SetVertexTexCoords( 1, l_cTex[0], l_cTex[1]);
	l_faceECF.SetVertexTexCoords( 2, l_fTex[0], l_fTex[1]);
	EdgePtr l_edgeEC = _addEdge( l_e, l_c, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeCF = _addEdge( l_c, l_f, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	EdgePtr l_edgeFE = _addEdge( l_f, l_e, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( FaceEdgesPtr( new FaceEdges( m_submesh, m_sgIndex, & l_faceECF, l_edgeEC, l_edgeCF, l_edgeFE)));

	Face & l_faceDEF = m_submesh->AddFace( l_d.m_index, l_e.m_index, l_f.m_index, m_sgIndex);
	l_faceDEF.SetVertexTexCoords( 0, l_dTex[0], l_dTex[1]);
	l_faceDEF.SetVertexTexCoords( 1, l_eTex[0], l_eTex[1]);
	l_faceDEF.SetVertexTexCoords( 2, l_fTex[0], l_fTex[1]);
	p_newFaces.push_back( FaceEdgesPtr( new FaceEdges( m_submesh, m_sgIndex, & l_faceDEF, l_edgeED, l_edgeFE, l_edgeDF)));

	delete m_face;
}

EdgePtr FaceEdges :: _addEdge( const Vertex & p_v1, const Vertex & p_v2, bool p_toDivide, bool p_add1, bool p_add2,
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
		EdgePtr l_edge( new Edge( p_v1, p_v2, m_face, p_toDivide));
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
*/
//*********************************************************************************************

Subdiviser :: Subdiviser( Submesh * p_submesh)
	:	m_submesh( p_submesh),
		m_vertex( m_submesh->GetVertices()),
		m_smoothGroups( m_submesh->GetSmoothGroups())
{
}

void Subdiviser :: _setTextCoords( Face & p_face, const Vertex & p_a, const Vertex & p_b, const Vertex & p_c, 
								   const Vertex & p_d, const Vertex & p_e, const Vertex & p_f, size_t p_sgIndex)
{
	Point2r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;

	l_aTex[0] = p_face[0].GetTexCoord()[0];
	l_aTex[1] = p_face[0].GetTexCoord()[1];
	l_bTex[0] = p_face[1].GetTexCoord()[0];
	l_bTex[1] = p_face[1].GetTexCoord()[1];
	l_cTex[0] = p_face[2].GetTexCoord()[0];
	l_cTex[1] = p_face[2].GetTexCoord()[1];

	l_dTex[0] = p_face[0].GetTexCoord()[0] + (p_face[1].GetTexCoord()[0] - p_face[0].GetTexCoord()[0]) / 2.0f;
	l_dTex[1] = p_face[0].GetTexCoord()[1] + (p_face[1].GetTexCoord()[1] - p_face[0].GetTexCoord()[1]) / 2.0f;
	l_eTex[0] = p_face[1].GetTexCoord()[0] + (p_face[2].GetTexCoord()[0] - p_face[1].GetTexCoord()[0]) / 2.0f;
	l_eTex[1] = p_face[1].GetTexCoord()[1] + (p_face[2].GetTexCoord()[1] - p_face[1].GetTexCoord()[1]) / 2.0f;
	l_fTex[0] = p_face[0].GetTexCoord()[0] + (p_face[2].GetTexCoord()[0] - p_face[0].GetTexCoord()[0]) / 2.0f;
	l_fTex[1] = p_face[0].GetTexCoord()[1] + (p_face[2].GetTexCoord()[1] - p_face[0].GetTexCoord()[1]) / 2.0f;

	Face & l_faceA = m_submesh->AddFace( p_a.m_index, p_d.m_index, p_f.m_index, p_sgIndex);
	l_faceA.SetVertexTexCoords( 0, l_aTex[0], l_aTex[1]);
	l_faceA.SetVertexTexCoords( 1, l_dTex[0], l_dTex[1]);
	l_faceA.SetVertexTexCoords( 2, l_fTex[0], l_fTex[1]);
	Face & l_faceB = m_submesh->AddFace( p_b.m_index, p_e.m_index, p_d.m_index, p_sgIndex);
	l_faceB.SetVertexTexCoords( 0, l_bTex[0], l_bTex[1]);
	l_faceB.SetVertexTexCoords( 1, l_eTex[0], l_eTex[1]);
	l_faceB.SetVertexTexCoords( 2, l_dTex[0], l_dTex[1]);
	Face & l_faceC = m_submesh->AddFace( p_c.m_index, p_f.m_index, p_e.m_index, p_sgIndex);
	l_faceC.SetVertexTexCoords( 0, l_cTex[0], l_cTex[1]);
	l_faceC.SetVertexTexCoords( 1, l_fTex[0], l_fTex[1]);
	l_faceC.SetVertexTexCoords( 2, l_eTex[0], l_eTex[1]);
	Face & l_faceD = m_submesh->AddFace( p_d.m_index, p_e.m_index, p_f.m_index, p_sgIndex);
	l_faceD.SetVertexTexCoords( 0, l_dTex[0], l_dTex[1]);
	l_faceD.SetVertexTexCoords( 1, l_eTex[0], l_eTex[1]);
	l_faceD.SetVertexTexCoords( 2, l_fTex[0], l_fTex[1]);
}
/*
Point3r Subdiviser :: _computeCenterFrom( const Vertex & p_a,const Vertex & p_b)
{
	Point3r l_result;

	// Projection sur XY => Récupération du point d'intersection (X, Y) y = ax + b
	Line3D<real> l_aEq( p_a.GetCoords(), p_a.GetNormal());
	Line3D<real> l_bEq( p_b.GetCoords(), p_b.GetNormal());
	Point3r l_pt1;

	if (l_aEq.Intersects( l_bEq, l_pt1))
	{
		// Projection sur XZ => Récupération du point d'intersection (X, Z) z = ax + b
		l_aEq = Line3D<real>( p_a.GetCoords(), p_a.GetNormal());
		l_bEq = Line3D<real>( p_b.GetCoords(), p_b.GetNormal());
		Point3r l_pt2;

		if (l_aEq.Intersects( l_bEq, l_pt2))
		{
			l_result[0] = l_pt1[0] + (l_pt2[0] - l_pt1[0]) / 2.0f;
			l_result[1] = l_pt1[1] + (l_pt2[1] - l_pt1[1]) / 2.0f;
			l_result[2] = l_pt1[2] + (l_pt2[2] - l_pt1[2]) / 2.0f;
		}
	}

	return l_result;
}

Point3r Subdiviser :: _computeCenterFrom( const Vertex & p_a, const Vertex & p_b, const Vertex & p_c)
{
	Point3r l_result;

	Plane<real> ABO( p_a.GetCoords(), p_a.GetNormal(), p_b.GetNormal());
	Plane<real> BCO( p_b.GetCoords(), p_b.GetNormal(), p_c.GetNormal());
	Plane<real> CAO( p_c.GetCoords(), p_c.GetNormal(), p_a.GetNormal());

	ABO.Intersects( BCO, CAO, l_result);

	return l_result;
}
*/
//*********************************************************************************************