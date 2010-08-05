#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/PNTrianglesDivider.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/SmoothingGroup.h"

using namespace Castor3D;

PNTrianglesSubdiviser :: ~PNTrianglesSubdiviser()
{
}

void PNTrianglesSubdiviser :: Subdivide( Vector3f * p_center)
{
	Vector3f l_vectorOA, l_vectorOB, l_vectorOC, l_vectorOD, l_vectorOE, l_vectorOF;
	Vector3f * l_ptA, * l_ptB, * l_ptC, * l_ptD, * l_ptE, * l_ptF;
	Face * l_face;
	FacePtrArray l_facesArray;
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