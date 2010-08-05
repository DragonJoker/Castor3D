#include "PrecompiledHeader.h"

#include "geometry/mesh/Subdiviser.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "math/Line.h"
#include "math/Plane.h"

using namespace Castor3D;

//*********************************************************************************************

bool VertexNeighbours :: AddNeighbour( Vector3f * p_vertex)
{
	if (m_neighbours.find( p_vertex) != m_neighbours.end())
	{
		return false;
	}
	m_neighbours.insert( std::set <Vector3f *>::value_type( p_vertex));
	return true;
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

PNTrianglesSubdiviser :: ~PNTrianglesSubdiviser()
{
}

void PNTrianglesSubdiviser :: Subdivide( Vector3f * p_center)
{
	Vector3f l_vectorOA, l_vectorOB, l_vectorOC, l_vectorOD, l_vectorOE, l_vectorOF;
	Vector3f * l_ptA, * l_ptB, * l_ptC, * l_ptD, * l_ptE, * l_ptF;
	Face * l_face;
	FacefPtrArray l_facesArray;
	size_t i;

	Vector3f * l_center = NULL;

	for (size_t j = 0 ; j < m_smoothGroups->size() ; j++)
	{
		for (i = 0 ; i < (*m_smoothGroups)[j]->m_faces.size() ; i++)
		{
			l_facesArray.push_back( (*m_smoothGroups)[j]->m_faces[i]);
		}
		(*m_smoothGroups)[j]->m_faces.clear();

		for (i = 0 ; i < l_facesArray.size() ; i++)
		{
			l_face = l_facesArray[i];

			l_ptA = l_face->m_vertex1;
			l_ptB = l_face->m_vertex2;
			l_ptC = l_face->m_vertex3;

			l_ptD = _computePointFrom( * l_ptA, l_face->m_vertex1Normal, * l_ptB, l_face->m_vertex2Normal, p_center);
			l_ptE = _computePointFrom( * l_ptB, l_face->m_vertex2Normal, * l_ptC, l_face->m_vertex3Normal, p_center);
			l_ptF = _computePointFrom( * l_ptA, l_face->m_vertex1Normal, * l_ptC, l_face->m_vertex3Normal, p_center);

			_setTextCoords( l_face, l_ptA, l_ptB, l_ptC, l_ptD, l_ptE, l_ptF, j);

			delete l_face;
		}
		l_facesArray.clear();
	}
}

Vector3f * PNTrianglesSubdiviser :: _computePointFrom( const Vector3f & p_a, float * p_aNormal,
													   const Vector3f & p_b, float * p_bNormal, 
													   Vector3f * p_center)
{
	Vector3f * l_center;
	if ((l_center = p_center) == NULL)
	{
		l_center = _computeCenterFrom( p_a, p_aNormal, p_b, p_bNormal);
	}

	Vector3f l_oa = p_a - *l_center;
	Vector3f l_ob = p_b - *l_center;
	float l_radius = (l_oa.GetLength() + l_ob.GetLength()) / 2.0f;
	Vector3f l_oc = l_oa + l_ob;
	l_oc.Normalise();
	l_oc *= l_radius;
	l_oc += *l_center;

	int l_index;
	Vector3f * l_pOC;
	if ((l_index = m_submesh->IsInMyVertex( l_oc)) == -1)
	{
		l_pOC = m_submesh->AddVertex( new Vector3f( l_oc.x, l_oc.y, l_oc.z));
	}
	else
	{
		l_pOC = (*m_vertex)[l_index];
		l_pOC->m_index = static_cast <size_t>( l_index);
	}
	return l_pOC;
}

//*********************************************************************************************