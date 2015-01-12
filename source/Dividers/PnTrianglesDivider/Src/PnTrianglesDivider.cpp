#include "PnTrianglesDivider.hpp"

using namespace Castor;
using namespace PnTriangles;

Subdivider::Subdivider()
	:	Castor3D::Subdivider()
{
}

Subdivider::~Subdivider()
{
	Cleanup();
}

void Subdivider::Cleanup()
{
	Castor3D::Subdivider::Cleanup();
}

void Subdivider::DoSubdivide()
{
	Castor3D::FacePtrArray l_facesArray;
	Point3r l_ptPos1;
	Point3r l_ptPos2;
	Point3r l_ptPos3;
	Point3r l_ptNml1;
	Point3r l_ptNml2;
	Point3r l_ptNml3;

	for ( uint32_t i = 0; i < m_submesh->GetFaceCount(); i++ )
	{
		l_facesArray.push_back( m_submesh->GetFace( i ) );
	}

	m_submesh->ClearFaces();
	std::map< uint32_t, std::pair< Point3r, Point3r > > l_posnml;

	for ( uint32_t i = 0; i < m_submesh->GetPointsCount(); ++i )
	{
		Point3r l_position, l_normal;
		Castor3D::BufferElementGroup const & l_point = *GetPoint( i );
		Castor3D::Vertex::GetPosition( l_point, l_position );
		Castor3D::Vertex::GetPosition( l_point, l_normal );
		l_posnml.insert( std::make_pair( i, std::make_pair( l_position, l_normal ) ) );
	}

	for ( uint32_t i = 0; i < l_facesArray.size(); i++ )
	{
		Castor3D::FaceSPtr l_face = l_facesArray[i];
		std::pair< Point3r, Point3r > const & l_v1 = l_posnml[l_face->GetVertexIndex( 0 )];
		std::pair< Point3r, Point3r > const & l_v2 = l_posnml[l_face->GetVertexIndex( 1 )];
		std::pair< Point3r, Point3r > const & l_v3 = l_posnml[l_face->GetVertexIndex( 2 )];
		Castor3D::BufferElementGroupSPtr l_ptD = DoComputePointFrom( l_v1.first, l_v2.first, l_v1.second, l_v2.second, m_ptDivisionCenter );
		Castor3D::BufferElementGroupSPtr l_ptE = DoComputePointFrom( l_v2.first, l_v3.first, l_v2.second, l_v3.second, m_ptDivisionCenter );
		Castor3D::BufferElementGroupSPtr l_ptF = DoComputePointFrom( l_v1.first, l_v3.first, l_v1.second, l_v3.second, m_ptDivisionCenter );
		DoSetTextCoords( l_face, *GetPoint( l_face->GetVertexIndex( 0 ) ), *GetPoint( l_face->GetVertexIndex( 1 ) ), *GetPoint( l_face->GetVertexIndex( 2 ) ), *l_ptD, *l_ptE, *l_ptF );
	}

	l_facesArray.clear();
}

Castor3D::BufferElementGroupSPtr Subdivider::DoComputePointFrom( Point3r const & p_ptPosA, Point3r const & p_ptPosB, Point3r const & p_ptNmlA, Point3r const & p_ptNmlB, Point3r const & p_ptCenter )
{
	Castor3D::BufferElementGroupSPtr l_pReturn;
	int l_index;
	/**/
	Point3r l_ptOA = p_ptPosA - p_ptCenter;
	Point3r l_ptOB = p_ptPosB - p_ptCenter;
	real l_radius = real( point::distance( l_ptOA ) + point::distance( l_ptOB ) ) / 2;
	Point3r l_ptPosC = point::get_normalised( l_ptOA + l_ptOB );
	l_ptPosC *= l_radius;
	l_ptPosC += p_ptCenter;

	/**/
	/*
		Point3r l_ptPosC = (p_ptPosA + p_ptPosB) / real( 2 ) - (p_ptNmlA * point::dot( p_ptPosB - p_ptPosA, p_ptNmlA )) / real( 4 ) - (p_ptNmlB * point::dot( p_ptPosA - p_ptPosB, p_ptNmlB )) / real( 4 );
	/**/
	if ( ( l_index = IsInMyPoints( l_ptPosC ) ) < 0 )
	{
		l_pReturn = AddPoint( l_ptPosC );
	}
	else
	{
		l_pReturn = GetPoint( l_index );
	}

	return l_pReturn;
}
