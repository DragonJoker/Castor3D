#include <Castor3D/Prerequisites.h>

using namespace Castor::Templates;

#include <Castor3D/geometry/mesh/Submesh.h>
#include <Castor3D/geometry/basic/Vertex.h>
#include <Castor3D/geometry/basic/Face.h>
#include <Castor3D/geometry/basic/SmoothingGroup.h>
#include <Castor3D/main/Version.h>

#include "LoopDivider/LoopDivider.h"

using namespace Castor3D;

//*************************************************************************************************

extern "C" C3D_Loop_API void GetRequiredVersion( Version & p_version)
{
	p_version = Version( 0, 6);
}

extern "C" C3D_Loop_API Subdivider * CreateDivider( Submesh * p_pSubmesh)
{
	Subdivider * l_pReturn( new LoopSubdivider( p_pSubmesh));

	return l_pReturn;
}

//*********************************************************************************************

#define ALPHA_MAX 20
#define ALPHA_LIMIT 0.469 /* converges to ~ 0.469 */

real g_alpha[] = {	1.13333f, -0.358974f, -0.333333f, 0.129032f, 0.945783f, 2.0f,
					3.19889f, 4.47885f, 5.79946f, 7.13634f, 8.47535f, 9.80865f,
					11.1322f, 12.4441f, 13.7439f, 15.0317f, 16.3082f, 17.574f,
					18.83f, 20.0769f };

#define BETA_MAX 20
#define BETA_LIMIT 0.469 /* converges to ~ 0.469 */

real g_beta[] = {0.46875f, 1.21875f, 1.125f, 0.96875f, 0.840932f, 0.75f, 0.686349f,
				0.641085f, 0.60813f, 0.583555f, 0.564816f, 0.55024f, 0.5387f,
				0.529419f, 0.52185f, 0.515601f, 0.510385f, 0.505987f, 0.502247f, 0.49904f };

real _beta( unsigned int n)
{
	return (real( 5) / real( 4) - (real( 3) + real( 2) * cos( Angle::PiMult2 / n)) * (real( 3) + real( 2) * cos( Angle::PiMult2 / n)) / real( 32));
}

real _alpha( unsigned int n)
{
	real l_rBeta;

	if (n <= 20)
	{
		return g_alpha[n - 1];
	}

	l_rBeta = _beta( n);

	return n * (1 - l_rBeta) / l_rBeta;
}

//*********************************************************************************************

LoopEdge :: LoopEdge( LoopVertexPtr p_v1, LoopVertexPtr p_v2, FacePtr p_f1, bool p_toDivide)
	:	m_firstVertex( p_v1)
	,	m_secondVertex( p_v2)
	,	m_firstFace( p_f1)
	,	m_divided( false)
	,	m_toDivide( p_toDivide)
	,	m_toDelete( false)
{
}

LoopEdge :: ~LoopEdge()
{
}

void LoopEdge :: AddFace( FacePtr p_face)
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

LoopVertexPtr LoopEdge :: Divide( LoopSubdivider * p_pDivider, real p_value)
{
	LoopVertexPtr l_pReturn;

	if (m_toDivide)
	{
		if ( ! m_divided)
		{
			Point3r l_ptPoint = (*m_firstVertex->GetPoint() + *m_secondVertex->GetPoint()) * p_value;
			m_createdVertex = p_pDivider->AddPoint( l_ptPoint);
			m_divided = true;
		}

		l_pReturn = m_createdVertex;
	}

	return l_pReturn;
}

//*************************************************************************************************

FaceEdges :: FaceEdges( LoopSubdivider * p_pDivider,  size_t p_sgIndex, FacePtr p_face, LoopVertexPtrUIntMap & p_mapVertex)
	:	m_pDivider( p_pDivider)
	,	m_sgIndex( p_sgIndex)
	,	m_face( p_face)
	,	m_bOwnFace( true)
{
	LoopVertexPtr l_vertexA = p_mapVertex.find( m_face->GetVertex( 0).GetIndex())->second;
	LoopVertexPtr l_vertexB = p_mapVertex.find( m_face->GetVertex( 1).GetIndex())->second;
	LoopVertexPtr l_vertexC = p_mapVertex.find( m_face->GetVertex( 2).GetIndex())->second;

	m_edgeAB = _addEdge( l_vertexA, l_vertexB, true);
	m_edgeBC = _addEdge( l_vertexB, l_vertexC, true);
	m_edgeCA = _addEdge( l_vertexC, l_vertexA, true);
}

FaceEdges :: FaceEdges( LoopSubdivider * p_pDivider, size_t p_sgIndex, FacePtr p_face, LoopEdgePtr l_ab, LoopEdgePtr l_bc, LoopEdgePtr l_ca)
	:	m_pDivider( p_pDivider),
		m_sgIndex( p_sgIndex),
		m_face( p_face),
		m_edgeAB( l_ab),
		m_edgeBC( l_bc),
		m_edgeCA( l_ca),
		m_bOwnFace( false)
{
}

void FaceEdges :: Divide( real p_value, LoopVertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces)
{
	Point2r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;
	LoopVertexPtr l_a = p_mapVertex.find( m_face->GetVertex( 0).GetIndex())->second;
	LoopVertexPtr l_b = p_mapVertex.find( m_face->GetVertex( 1).GetIndex())->second;
	LoopVertexPtr l_c = p_mapVertex.find( m_face->GetVertex( 2).GetIndex())->second;
	LoopVertexPtr l_d = m_edgeAB->Divide( m_pDivider, p_value);
	LoopVertexPtr l_e = m_edgeBC->Divide( m_pDivider, p_value);
	LoopVertexPtr l_f = m_edgeCA->Divide( m_pDivider, p_value);

	_removeEdge( m_edgeAB);
	_removeEdge( m_edgeBC);
	_removeEdge( m_edgeCA);

	l_aTex = m_face->GetVertex( 0).GetTexCoord();
	l_bTex = m_face->GetVertex( 1).GetTexCoord();
	l_cTex = m_face->GetVertex( 2).GetTexCoord();

	l_dTex = l_aTex + (l_bTex - l_aTex) * p_value;
	l_eTex = l_bTex + (l_cTex - l_bTex) * p_value;
	l_fTex = l_cTex + (l_aTex - l_cTex) * p_value;

	_addFaceAndEdges( l_a, l_d, l_f, l_aTex, l_dTex, l_fTex, p_newFaces);
	_addFaceAndEdges( l_d, l_b, l_e, l_dTex, l_bTex, l_eTex, p_newFaces);
	_addFaceAndEdges( l_e, l_c, l_f, l_eTex, l_cTex, l_fTex, p_newFaces);
	_addFaceAndEdges( l_d, l_e, l_f, l_dTex, l_eTex, l_fTex, p_newFaces);
}

void FaceEdges :: _addFaceAndEdges( LoopVertexPtr p_a, LoopVertexPtr p_b, LoopVertexPtr p_c,
								   const Point2r & p_aTex, const Point2r & p_bTex, const Point2r & p_cTex,
								   FaceEdgesPtrArray & p_newFaces)
{
	LoopEdgePtr l_edgeAB = _addEdge( p_a, p_b,	true);
	LoopEdgePtr l_edgeBC = _addEdge( p_b, p_c,	true);
	LoopEdgePtr l_edgeCA = _addEdge( p_c, p_a,	true);
	_addFaceAndEdges( p_a, p_b, p_c, p_aTex, p_bTex, p_cTex, l_edgeAB, l_edgeBC, l_edgeCA, p_newFaces);
}

void FaceEdges :: _addFaceAndEdges( LoopVertexPtr p_a, LoopVertexPtr p_b, LoopVertexPtr p_c,
								   const Point2r & p_aTex, const Point2r & p_bTex, const Point2r & p_cTex,
								   LoopEdgePtr p_edgeAB, LoopEdgePtr p_edgeBC, LoopEdgePtr p_edgeCA,
								   FaceEdgesPtrArray & p_newFaces)
{
	FacePtr l_face = m_pDivider->AddFace( p_a->GetIndex(), p_b->GetIndex(), p_c->GetIndex(), m_sgIndex);
	l_face->SetTextureCoords( p_aTex, p_bTex, p_cTex);
	p_newFaces.push_back( FaceEdgesPtr( new FaceEdges( m_pDivider, m_sgIndex, l_face, p_edgeAB, p_edgeBC, p_edgeCA)));
}

LoopEdgePtr FaceEdges :: _addEdge( LoopVertexPtr p_v1, LoopVertexPtr p_v2, bool p_toDivide)
{
	LoopEdgePtr l_pReturn = p_v1->GetEdge( p_v2->GetIndex());
	bool l_bCreated = false;

	if (l_pReturn == NULL)
	{
		l_pReturn = p_v2->GetEdge( p_v1->GetIndex());

		if (l_pReturn == NULL)
		{
			l_pReturn = LoopEdgePtr( new LoopEdge( p_v1, p_v2, m_face, p_toDivide));
			l_bCreated = true;
		}

		p_v1->AddEdge( l_pReturn, p_v2->GetIndex());
	}

	if ( ! p_v2->HasEdge( p_v1->GetIndex()))
	{
		p_v2->AddEdge( l_pReturn, p_v1->GetIndex());
	}

	if ( ! l_bCreated)
	{
		l_pReturn->AddFace( m_face);
	}

	return l_pReturn;
}

void FaceEdges :: _removeEdge( LoopEdgePtr p_edge)
{
	LoopVertexPtr l_v1 = p_edge->GetVertex1();
	LoopVertexPtr l_v2 = p_edge->GetVertex2();
	l_v1->RemoveEdge( l_v2->GetIndex());
	l_v2->RemoveEdge( l_v1->GetIndex());
	p_edge.reset();
}

//*********************************************************************************************

LoopVertex :: LoopVertex( IdPoint3rPtr p_ptPoint)
	:	m_ptPoint( p_ptPoint)
{
}

LoopVertex :: ~LoopVertex()
{
	m_mapEdges.clear();
}

bool LoopVertex :: HasEdge( size_t p_uiIndex)
{
	return m_mapEdges.find( p_uiIndex) != m_mapEdges.end();
}

LoopEdgePtr LoopVertex :: GetEdge( size_t p_uiIndex)
{
	LoopEdgePtr l_pReturn;
	LoopEdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex);

	if (l_it != m_mapEdges.end())
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

void LoopVertex :: AddEdge( LoopEdgePtr p_pLoopEdge, size_t p_uiIndex)
{
	LoopEdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex);

	if (l_it == m_mapEdges.end())
	{
		m_mapEdges.insert( LoopEdgePtrUIntMap::value_type( p_uiIndex, p_pLoopEdge));
	}
}

void LoopVertex :: RemoveEdge( size_t p_uiIndex)
{
	LoopEdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex);

	if (l_it != m_mapEdges.end())
	{
		m_mapEdges.erase( l_it);
	}
}

//*********************************************************************************************

LoopSubdivider :: LoopSubdivider( Submesh * p_submesh)
	:	Subdivider( p_submesh)
{
}

LoopSubdivider :: ~LoopSubdivider()
{
	Cleanup();
}

void LoopSubdivider :: Initialise()
{
	Subdivider::Initialise();

	for (size_t i = 0 ; i < m_points.size() ; i++)
	{
		m_mapVertex.insert( LoopVertexPtrUIntMap::value_type( m_points[i]->GetIndex(), LoopVertexPtr( new LoopVertex( m_points[i]))));
	}

	FaceEdgesPtr l_faceLoopEdges;

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		SmoothingGroupPtr l_group = m_smoothGroups[i];

		for (size_t j = 0 ; j < l_group->GetNbFaces() ; j++)
		{
			FacePtr l_face = l_group->GetFace( j);
			l_faceLoopEdges.reset( new FaceEdges( this, i, l_face, m_mapVertex));
			m_facesEdges.push_back( l_faceLoopEdges);
		}

		l_group->ClearFaces();
	}
}

void LoopSubdivider :: Cleanup()
{
	Subdivider::Cleanup();

	m_facesEdges.clear();
	m_mapVertex.clear();
}

LoopVertexPtr LoopSubdivider :: AddPoint( real x, real y, real z)
{
	LoopVertexPtr l_pReturn( new LoopVertex( Subdivider::AddPoint( x, y, z)));
	m_mapVertex.insert( LoopVertexPtrUIntMap::value_type( l_pReturn->GetIndex(), l_pReturn));
	return l_pReturn;
}

LoopVertexPtr LoopSubdivider :: AddPoint( const Point3r & p_v)
{
	LoopVertexPtr l_pReturn( new LoopVertex( Subdivider::AddPoint( p_v)));
	m_mapVertex.insert( LoopVertexPtrUIntMap::value_type( l_pReturn->GetIndex(), l_pReturn));
	return l_pReturn;
}

LoopVertexPtr LoopSubdivider :: AddPoint( real * p_v)
{
	LoopVertexPtr l_pReturn( new LoopVertex( Subdivider::AddPoint( p_v)));
	m_mapVertex.insert( LoopVertexPtrUIntMap::value_type( l_pReturn->GetIndex(), l_pReturn));
	return l_pReturn;
}

void LoopSubdivider :: _subdivide()
{
	_divide();
	_average( m_ptDivisionCenter);
}

void LoopSubdivider :: _divide()
{
	FaceEdgesPtrArray l_newFaces;
	size_t l_size = m_facesEdges.size();

	for (size_t i = 0 ; i < l_size ; i++)
	{
		l_newFaces.clear();
		m_facesEdges[0]->Divide( 0.5f, m_mapVertex, l_newFaces);
		m_facesEdges[0].reset();
		m_facesEdges.erase( m_facesEdges.begin());

		for (size_t j = 0 ; j < l_newFaces.size() ; j++)
		{
			m_facesEdges.push_back( l_newFaces[j]);
		}
	}
}

void LoopSubdivider :: _average( const Point3r & p_center)
{
	VertexPtrUIntMap::iterator l_origIt;
	int l_nb;
	LoopVertexPtr l_pVertex;
	LoopEdgePtr l_pEdge;
	LoopVertexPtrUIntMap::iterator l_itVertex2;

	for (LoopVertexPtrUIntMap::iterator l_itVertex = m_mapVertex.begin() ; l_itVertex != m_mapVertex.end() ; ++l_itVertex)
	{
		l_pVertex = l_itVertex->second;

		l_nb = l_pVertex->Size();
		IdPoint3r & l_ptPoint = *l_pVertex->GetPoint();
		l_ptPoint -= p_center;
		l_ptPoint *= _alpha( l_nb);

		for (LoopEdgePtrUIntMap::iterator l_it = l_pVertex->Begin() ; l_it != l_pVertex->End() ; ++l_it)
		{
			l_ptPoint += (*m_points[l_it->first] - p_center);
		}

		l_ptPoint /= (_alpha( l_nb) + l_nb);
		l_ptPoint += p_center;
	}

	for (size_t i = 0 ; i < m_smoothGroups.size() ; i++)
	{
		SmoothingGroupPtr l_pGroup = m_smoothGroups[i];

		for (size_t j = 0 ; j < l_pGroup->GetNbFaces() ; j++)
		{
			FacePtr l_pFace = l_pGroup->GetFace( j);

			Vertex & l_v1 = l_pFace->GetVertex( 0);
			Vertex & l_v2 = l_pFace->GetVertex( 1);
			Vertex & l_v3 = l_pFace->GetVertex( 2);

			l_v1.SetCoords( * m_mapVertex.find( l_v1.GetIndex())->second->GetPoint());
			l_v2.SetCoords( * m_mapVertex.find( l_v2.GetIndex())->second->GetPoint());
			l_v3.SetCoords( * m_mapVertex.find( l_v3.GetIndex())->second->GetPoint());
		}
	}

	m_mapVertex.clear();
}

//*********************************************************************************************

String & Castor3D::operator << ( String & p_stream, const LoopVertex & p_vertex)
{
	p_stream << CU_T( "LoopVertex[") << p_vertex.GetIndex() << CU_T( "] - Buffer : [") << p_vertex.GetPoint()->const_ptr() << CU_T( "] - (") << (*p_vertex.GetPoint())[0] << CU_T( ",") << (*p_vertex.GetPoint())[1] << CU_T( ",") << (*p_vertex.GetPoint())[2] << CU_T( ")");
	return p_stream;
}

//*********************************************************************************************