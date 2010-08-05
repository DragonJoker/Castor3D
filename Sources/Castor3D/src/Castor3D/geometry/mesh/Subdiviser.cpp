#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/Subdiviser.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"

using namespace Castor3D;

//*********************************************************************************************

Edge :: Edge( Vector3f * p_v1, Vector3f * p_v2, Face * p_f1, bool p_toDivide)
	:	m_firstVertex( p_v1),
		m_secondVertex( p_v2),
		m_firstFace( p_f1),
		m_secondFace( NULL),
		m_divided( false),
		m_createdVertex( NULL),
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

Vector3f * Edge :: Divide( Submesh * p_submesh, float p_value)
{
	if ( ! m_toDivide)
	{
		return NULL;
	}

	if (m_divided)
	{
		return m_createdVertex;
	}

	m_createdVertex = p_submesh->AddVertex( new Vector3f( (*m_firstVertex + *m_secondVertex) * p_value));

	m_divided = true;
	return m_createdVertex;
}

//*********************************************************************************************

FaceEdges :: FaceEdges( Submesh * p_submesh, size_t p_sgIndex, Face * p_face,
						std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
						std::map <Vector3f *, int> & p_vertexNeighbourhood,
						std::set <Edge *> & p_allEdges)
	:	m_submesh( p_submesh),
		m_sgIndex( p_sgIndex),
		m_face( p_face)
{
	Vector3f * l_vertex1;
	Vector3f * l_vertex2;
	Vector3f * l_vertex3;

	l_vertex1 = m_face->m_vertex1;
	l_vertex2 = m_face->m_vertex2;
	l_vertex3 = m_face->m_vertex3;

	m_edgeAB = _addEdge( l_vertex1, l_vertex2, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	m_edgeBC = _addEdge( l_vertex2, l_vertex3, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	m_edgeCA = _addEdge( l_vertex3, l_vertex1, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
}

FaceEdges :: FaceEdges( Submesh * p_submesh, size_t p_sgIndex, Face * p_face, Edge * l_ab, Edge * l_bc, Edge * l_ca)
	:	m_submesh( p_submesh),
		m_sgIndex( p_sgIndex),
		m_face( p_face),
		m_edgeAB( l_ab),
		m_edgeBC( l_bc),
		m_edgeCA( l_ca)
{
}

void FaceEdges :: Divide( float p_value, std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
						  std::map <Vector3f *, int> & p_vertexNeighbourhood,
						  std::set <Edge *> & p_allEdges, std::vector <FaceEdges *> & p_newFaces,
						  std::vector <Vector3f *> & p_newVertices)
{
	Point2D<float> l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;
	Vector3f * l_a = m_face->m_vertex1;
	Vector3f * l_b = m_face->m_vertex2;
	Vector3f * l_c = m_face->m_vertex3;
	Vector3f * l_d = m_edgeAB->Divide( m_submesh, p_value);
	Vector3f * l_e = m_edgeBC->Divide( m_submesh, p_value);
	Vector3f * l_f = m_edgeCA->Divide( m_submesh, p_value);

	p_newVertices.push_back( l_d);
	p_newVertices.push_back( l_e);
	p_newVertices.push_back( l_f);

	_removeEdge( m_edgeAB, p_existingEdges, p_allEdges);
	_removeEdge( m_edgeBC, p_existingEdges, p_allEdges);
	_removeEdge( m_edgeCA, p_existingEdges, p_allEdges);

	l_aTex.x = m_face->m_vertex1TexCoord[0];
	l_aTex.y = m_face->m_vertex1TexCoord[1];
	l_bTex.x = m_face->m_vertex2TexCoord[0];
	l_bTex.y = m_face->m_vertex2TexCoord[1];
	l_cTex.x = m_face->m_vertex3TexCoord[0];
	l_cTex.y = m_face->m_vertex3TexCoord[1];

	l_dTex.x = m_face->m_vertex1TexCoord[0] + (m_face->m_vertex2TexCoord[0] - m_face->m_vertex1TexCoord[0]) * p_value;
	l_dTex.y = m_face->m_vertex1TexCoord[1] + (m_face->m_vertex2TexCoord[1] - m_face->m_vertex1TexCoord[1]) * p_value;
	l_eTex.x = m_face->m_vertex2TexCoord[0] + (m_face->m_vertex3TexCoord[0] - m_face->m_vertex2TexCoord[0]) * p_value;
	l_eTex.y = m_face->m_vertex2TexCoord[1] + (m_face->m_vertex3TexCoord[1] - m_face->m_vertex2TexCoord[1]) * p_value;
	l_fTex.x = m_face->m_vertex3TexCoord[0] + (m_face->m_vertex1TexCoord[0] - m_face->m_vertex3TexCoord[0]) * p_value;
	l_fTex.y = m_face->m_vertex3TexCoord[1] + (m_face->m_vertex1TexCoord[1] - m_face->m_vertex3TexCoord[1]) * p_value;

	Face * l_faceADF = m_submesh->AddFace( l_a, l_d, l_f, m_sgIndex);
	SetTexCoordV1( l_faceADF, l_aTex.x, l_aTex.y);
	SetTexCoordV2( l_faceADF, l_dTex.x, l_dTex.y);
	SetTexCoordV3( l_faceADF, l_fTex.x, l_fTex.y);
	Edge * l_edgeAD = _addEdge( l_a, l_d, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	Edge * l_edgeDF = _addEdge( l_d, l_f, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	Edge * l_edgeFA = _addEdge( l_f, l_a, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceADF, l_edgeAD, l_edgeDF, l_edgeFA));

	Face * l_faceDBE = m_submesh->AddFace( l_d, l_b, l_e, m_sgIndex);
	SetTexCoordV1( l_faceDBE, l_dTex.x, l_dTex.y);
	SetTexCoordV2( l_faceDBE, l_bTex.x, l_bTex.y);
	SetTexCoordV3( l_faceDBE, l_eTex.x, l_eTex.y);
	Edge * l_edgeDB = _addEdge( l_d, l_b, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	Edge * l_edgeBE = _addEdge( l_b, l_e, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	Edge * l_edgeED = _addEdge( l_e, l_d, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceDBE, l_edgeDB, l_edgeBE, l_edgeED));

	Face * l_faceECF = m_submesh->AddFace( l_e, l_c, l_f, m_sgIndex);
	SetTexCoordV1( l_faceECF, l_eTex.x, l_eTex.y);
	SetTexCoordV2( l_faceECF, l_cTex.x, l_cTex.y);
	SetTexCoordV3( l_faceECF, l_fTex.x, l_fTex.y);
	Edge * l_edgeEC = _addEdge( l_e, l_c, true, true, false, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	Edge * l_edgeCF = _addEdge( l_c, l_f, true, false, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	Edge * l_edgeFE = _addEdge( l_f, l_e, true, true, true, p_existingEdges, p_vertexNeighbourhood, p_allEdges);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceECF, l_edgeEC, l_edgeCF, l_edgeFE));

	Face * l_faceDEF = m_submesh->AddFace( l_d, l_e, l_f, m_sgIndex);
	SetTexCoordV1( l_faceDEF, l_dTex.x, l_dTex.y);
	SetTexCoordV2( l_faceDEF, l_eTex.x, l_eTex.y);
	SetTexCoordV3( l_faceDEF, l_fTex.x, l_fTex.y);
	p_newFaces.push_back( new FaceEdges( m_submesh, m_sgIndex, l_faceDEF, l_edgeED, l_edgeFE, l_edgeDF));

	delete m_face;
}

Edge * FaceEdges :: _addEdge( Vector3f * p_v1, Vector3f * p_v2, bool p_toDivide, bool p_add1, bool p_add2,
							  std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
							  std::map <Vector3f *, int> & p_vertexNeighbourhood,
							  std::set <Edge *> & p_allEdges)
{
	std::map <Vector3f *, EdgePtrMap>::iterator l_it1;
	std::map <Vector3f *, EdgePtrMap>::iterator l_it2;
	std::map <Vector3f *, int>::iterator l_nit1;
	std::map <Vector3f *, int>::iterator l_nit2;
	EdgePtrMap::iterator l_edgeIt;

	if ((l_it1 = p_existingEdges.find( p_v1)) == p_existingEdges.end())
	{
		EdgePtrMap l_edges;
		p_vertexNeighbourhood.insert( std::map <Vector3f *, int>::value_type( p_v1, 0));
		p_existingEdges.insert( std::map <Vector3f *, EdgePtrMap>::value_type( p_v1, l_edges));
		l_it1 = p_existingEdges.find( p_v1);
	}
	l_nit1 = p_vertexNeighbourhood.find( p_v1);

	if ((l_it2 = p_existingEdges.find( p_v2)) == p_existingEdges.end())
	{
		EdgePtrMap l_edges;
		p_vertexNeighbourhood.insert( std::map <Vector3f *, int>::value_type( p_v2, 0));
		p_existingEdges.insert( std::map <Vector3f *, EdgePtrMap>::value_type( p_v2, l_edges));
		l_it2 = p_existingEdges.find( p_v2);
	}
	l_nit2 = p_vertexNeighbourhood.find( p_v2);

	if ((l_edgeIt = l_it1->second.find( p_v2)) == l_it1->second.end())
	{
		Edge * l_edge = new Edge( p_v1, p_v2, m_face, p_toDivide);
		p_allEdges.insert( std::set <Edge *>::value_type( l_edge));
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

void FaceEdges :: _removeEdge( Edge * p_edge, std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
							   std::set <Edge *> & p_allEdges)
{
	if (p_allEdges.find( p_edge) != p_allEdges.end())
	{
		Vector3f * l_v1 = p_edge->GetVertex1();
		Vector3f * l_v2 = p_edge->GetVertex2();
		int l_count = p_edge->Release();
		if (l_count <= 0)
		{
			p_allEdges.erase( p_allEdges.find( p_edge));
			std::map <Vector3f *, EdgePtrMap>::iterator l_it;
			l_it = p_existingEdges.find( l_v1);
			l_it->second.erase( l_it->second.find( l_v2));
			l_it = p_existingEdges.find( l_v2);
			l_it->second.erase( l_it->second.find( l_v1));
			delete p_edge;
		}
	}
}

//*********************************************************************************************

Subdiviser :: Subdiviser( Submesh * p_submesh)
	:	m_submesh( p_submesh)
{
	m_vertex = m_submesh->GetVertices();
	m_smoothGroups = m_submesh->GetSmoothGroups();
}

void Subdiviser :: _setTextCoords( Face * p_face, Vector3f * p_a, Vector3f * p_b, Vector3f * p_c, 
								   Vector3f * p_d, Vector3f * p_e, Vector3f * p_f, size_t p_sgIndex)
{
	Point2D<float> l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;

	l_aTex.x = p_face->m_vertex1TexCoord[0];
	l_aTex.y = p_face->m_vertex1TexCoord[1];
	l_bTex.x = p_face->m_vertex2TexCoord[0];
	l_bTex.y = p_face->m_vertex2TexCoord[1];
	l_cTex.x = p_face->m_vertex3TexCoord[0];
	l_cTex.y = p_face->m_vertex3TexCoord[1];

	l_dTex.x = p_face->m_vertex1TexCoord[0] + (p_face->m_vertex2TexCoord[0] - p_face->m_vertex1TexCoord[0]) / 2.0f;
	l_dTex.y = p_face->m_vertex1TexCoord[1] + (p_face->m_vertex2TexCoord[1] - p_face->m_vertex1TexCoord[1]) / 2.0f;
	l_eTex.x = p_face->m_vertex2TexCoord[0] + (p_face->m_vertex3TexCoord[0] - p_face->m_vertex2TexCoord[0]) / 2.0f;
	l_eTex.y = p_face->m_vertex2TexCoord[1] + (p_face->m_vertex3TexCoord[1] - p_face->m_vertex2TexCoord[1]) / 2.0f;
	l_fTex.x = p_face->m_vertex1TexCoord[0] + (p_face->m_vertex3TexCoord[0] - p_face->m_vertex1TexCoord[0]) / 2.0f;
	l_fTex.y = p_face->m_vertex1TexCoord[1] + (p_face->m_vertex3TexCoord[1] - p_face->m_vertex1TexCoord[1]) / 2.0f;

	Face * l_face = m_submesh->AddFace( p_a, p_d, p_f, p_sgIndex);
	SetTexCoordV1( l_face, l_aTex.x, l_aTex.y);
	SetTexCoordV2( l_face, l_dTex.x, l_dTex.y);
	SetTexCoordV3( l_face, l_fTex.x, l_fTex.y);
	l_face = m_submesh->AddFace( p_b, p_e, p_d, p_sgIndex);
	SetTexCoordV1( l_face, l_bTex.x, l_bTex.y);
	SetTexCoordV2( l_face, l_eTex.x, l_eTex.y);
	SetTexCoordV3( l_face, l_dTex.x, l_dTex.y);
	l_face = m_submesh->AddFace( p_c, p_f, p_e, p_sgIndex);
	SetTexCoordV1( l_face, l_cTex.x, l_cTex.y);
	SetTexCoordV2( l_face, l_fTex.x, l_fTex.y);
	SetTexCoordV3( l_face, l_eTex.x, l_eTex.y);
	l_face = m_submesh->AddFace( p_d, p_e, p_f, p_sgIndex);
	SetTexCoordV1( l_face, l_dTex.x, l_dTex.y);
	SetTexCoordV2( l_face, l_eTex.x, l_eTex.y);
	SetTexCoordV3( l_face, l_fTex.x, l_fTex.y);
}

Vector3f * Subdiviser :: _computeCenterFrom( const Vector3f & p_a, float * p_aNormal, const Vector3f & p_b, float * p_bNormal)
{
	Vector3f * l_result = new Vector3f;

	// Projection sur XY => Récupération du point d'intersection (X, Y) y = ax + b
	Line3D l_aEq( p_a, p_aNormal);
	Line3D l_bEq( p_b, p_bNormal);
	Vector3f l_v1;
	if (l_aEq.Intersects( l_bEq, l_v1))
	{
		// Projection sur XZ => Récupération du point d'intersection (X, Z) z = ax + b
		l_aEq = Line3D( p_a, p_aNormal);
		l_bEq = Line3D( p_b, p_bNormal);
		Vector3f l_v2;
		if (l_aEq.Intersects( l_bEq, l_v2))
		{
			l_result->x = l_v1.x + (l_v2.x - l_v1.x) / 2.0f;
			l_result->y = l_v1.y + (l_v2.y - l_v1.y) / 2.0f;
			l_result->z = l_v1.z + (l_v2.z - l_v1.z) / 2.0f;
		}
	}

	return l_result;
}

Vector3f * Subdiviser :: _computeCenterFrom( const Vector3f & p_a, float * p_aNormal,
											 const Vector3f & p_b, float * p_bNormal,
											 const Vector3f & p_c, float * p_cNormal)
{
	Vector3f * l_result = new Vector3f;

	Plane ABO( p_a, p_aNormal, p_bNormal);
	Plane BCO( p_b, p_bNormal, p_cNormal);
	Plane CAO( p_c, p_cNormal, p_aNormal);

	ABO.Intersects( BCO, CAO, *l_result);

	return l_result;
}

//*********************************************************************************************