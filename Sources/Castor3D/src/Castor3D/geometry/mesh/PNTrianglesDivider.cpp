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

void PNTrianglesSubdiviser :: Subdivide( Point3rPtr p_center)
{
	Vertex l_vectorOA, l_vectorOB, l_vectorOC, l_vectorOD, l_vectorOE, l_vectorOF;
	VertexPtr l_ptA, l_ptB, l_ptC, l_ptD, l_ptE, l_ptF;
	FacePtr l_face;
	FacePtrArray l_facesArray;
	size_t i;

	VertexPtr l_center = NULL;

	for (size_t j = 0 ; j < m_smoothGroups.size() ; j++)
	{
		for (i = 0 ; i < m_smoothGroups[j]->m_faces.size() ; i++)
		{
			l_facesArray.push_back( m_smoothGroups[j]->m_faces[i]);
		}

		m_smoothGroups[j]->m_faces.clear();

		for (i = 0 ; i < l_facesArray.size() ; i++)
		{
			l_face = l_facesArray[i];

			l_ptA = l_face->m_vertex1;
			l_ptB = l_face->m_vertex2;
			l_ptC = l_face->m_vertex3;

			l_ptD = _computePointFrom( * l_ptA, l_face->m_vertex1Normal->ptr(), * l_ptB, l_face->m_vertex2Normal->ptr(), p_center);
			l_ptE = _computePointFrom( * l_ptB, l_face->m_vertex2Normal->ptr(), * l_ptC, l_face->m_vertex3Normal->ptr(), p_center);
			l_ptF = _computePointFrom( * l_ptA, l_face->m_vertex1Normal->ptr(), * l_ptC, l_face->m_vertex3Normal->ptr(), p_center);

			_setTextCoords( l_face, l_ptA, l_ptB, l_ptC, l_ptD, l_ptE, l_ptF, j);

//			delete l_face;
		}

		l_facesArray.clear();
	}
}

VertexPtr PNTrianglesSubdiviser :: _computePointFrom( const Vertex & p_a, real * p_aNormal,
													  const Vertex & p_b, real * p_bNormal, 
													  Point3rPtr p_center)
{
	Point3rPtr l_center;

	if (p_center.null())
	{
		l_center = _computeCenterFrom( p_a, p_aNormal, p_b, p_bNormal);
	}
	else
	{
		l_center = p_center;
	}

	Vertex l_oa = p_a - *l_center;
	Vertex l_ob = p_b - *l_center;
	real l_radius = (l_oa.GetLength() + l_ob.GetLength()) / 2.0f;
	Vertex l_oc = l_oa + l_ob;
	l_oc.Normalise();
	l_oc *= l_radius;
	l_oc += *l_center;

	int l_index;
	VertexPtr l_pOC;

	if ((l_index = m_submesh->IsInMyVertex( l_oc)) == -1)
	{
		l_pOC = m_submesh->AddVertex( new Vertex( l_oc));
	}
	else
	{
		l_pOC = m_vertex[l_index];
		l_pOC->m_index = static_cast <size_t>( l_index);
	}

	return l_pOC;
}