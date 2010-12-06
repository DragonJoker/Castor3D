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

void PNTrianglesSubdiviser :: Subdivide( const Point3r & p_center)
{
	Vertex l_vectorOA, l_vectorOB, l_vectorOC, l_vectorOD, l_vectorOE, l_vectorOF;
	Vertex l_ptA, l_ptB, l_ptC, l_ptD, l_ptE, l_ptF;
	FaceArray l_facesArray;
	size_t i;

	VertexPtr l_center;

	for (size_t j = 0 ; j < m_smoothGroups.size() ; j++)
	{
		for (i = 0 ; i < m_smoothGroups[j].m_faces.size() ; i++)
		{
			l_facesArray.push_back( m_smoothGroups[j].m_faces[i]);
		}

		m_smoothGroups[j].m_faces.clear();

		for (i = 0 ; i < l_facesArray.size() ; i++)
		{
			Face l_face = l_facesArray[i];

			l_ptA = l_face[0];
			l_ptB = l_face[1];
			l_ptC = l_face[2];

			l_ptD = _computePointFrom( l_ptA, l_ptB, & p_center);
			l_ptE = _computePointFrom( l_ptB, l_ptC, & p_center);
			l_ptF = _computePointFrom( l_ptA, l_ptC, & p_center);

			_setTextCoords( l_face, l_ptA, l_ptB, l_ptC, l_ptD, l_ptE, l_ptF, j);
		}

		l_facesArray.clear();
	}
}

Vertex PNTrianglesSubdiviser :: _computePointFrom( const Vertex & p_a, const Vertex & p_b, const Point3r * p_center)
{
	Point3r l_center;

	if (p_center == NULL)
	{
		l_center = _computeCenterFrom( p_a, p_b);
	}
	else
	{
		l_center = * p_center;
	}

	Point3r l_oa = p_a.GetCoords() - l_center;
	Point3r l_ob = p_b.GetCoords() - l_center;
	real l_radius = (l_oa.GetLength() + l_ob.GetLength()) / 2.0f;
	Point3r l_oc = l_oa + l_ob;
	l_oc.Normalise();
	l_oc *= l_radius;
	l_oc += l_center;

	int l_index;
	Vertex l_vOC;

	if ((l_index = m_submesh->IsInMyVertex( l_oc)) == -1)
	{
		l_vOC.SetCoords( m_submesh->AddVertex( l_oc));
	}
	else
	{
		l_vOC.SetCoords( m_vertex[l_index]);
		l_vOC.m_index = static_cast <size_t>( l_index);
	}

	return l_vOC;
}
