#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/PnTrianglesDivider.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/SmoothingGroup.hpp"

using namespace Castor3D;

PnTrianglesDivider :: PnTrianglesDivider( Submesh * p_submesh)
	:	Subdivider( p_submesh)
{
}

PnTrianglesDivider :: ~PnTrianglesDivider()
{
	Cleanup();
}

void PnTrianglesDivider :: Cleanup()
{
	Subdivider::Cleanup();
}

void PnTrianglesDivider :: _subdivide()
{
	FacePtrArray l_facesArray;

	for (size_t j = 0 ; j < m_smoothGroups.size() ; j++)
	{
		for (size_t i = 0 ; i < m_smoothGroups[j]->GetNbFaces() ; i++)
		{
			l_facesArray.push_back( m_smoothGroups[j]->GetFace( i));
		}

		m_smoothGroups[j]->ClearFaces();

		for (size_t i = 0 ; i < l_facesArray.size() ; i++)
		{
			FacePtr l_face = l_facesArray[i];

			const Vertex & l_ptA = l_face->GetVertex( 0);
			const Vertex & l_ptB = l_face->GetVertex( 1);
			const Vertex & l_ptC = l_face->GetVertex( 2);
			Vertex l_ptD;
			Vertex l_ptE;
			Vertex l_ptF;

			_computePointFrom( l_ptD, l_ptA, l_ptB, & m_ptDivisionCenter);
			_computePointFrom( l_ptE, l_ptB, l_ptC, & m_ptDivisionCenter);
			_computePointFrom( l_ptF, l_ptA, l_ptC, & m_ptDivisionCenter);

			_setTextCoords( l_face, l_ptA, l_ptB, l_ptC, l_ptD, l_ptE, l_ptF, j);
		}

		l_facesArray.clear();
	}
}

void PnTrianglesDivider :: _computePointFrom( Vertex & p_result, const Vertex & p_a, const Vertex & p_b, const Point3r * p_center)
{
	Point3r l_center;

	if (p_center == NULL)
	{
		_computeCenterFrom( p_a.GetCoords(), p_b.GetCoords(), p_a.GetNormal(), p_b.GetNormal(), l_center);
	}
	else
	{
		l_center.copy( * p_center);
	}

	Point3r l_oa;
	Point3r l_ob;
	real l_radius = real( (l_oa.length() + l_ob.length())) / 2;
	Point3r l_oc;
	l_oa.copy( p_a.GetCoords() - l_center);
	l_ob.copy( p_b.GetCoords() - l_center);
	l_oc.copy( l_oa + l_ob);
	l_oc.normalise();
	l_oc *= l_radius;
	l_oc += l_center;

	int l_index;

	if ((l_index = IsInMyPoints( l_oc)) == -1)
	{
		p_result.SetCoords( l_oc);
		l_index = AddPoint( l_oc)->GetIndex();
	}
	else
	{
		p_result.SetCoords( * m_points[l_index]);
	}

	p_result.SetIndex( static_cast <size_t>( l_index));
}
