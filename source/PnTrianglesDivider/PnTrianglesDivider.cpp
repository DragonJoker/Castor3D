#include "PnTrianglesDivider/PnTrianglesDivider.hpp"

using namespace Castor;
using namespace PnTriangles;

//*************************************************************************************************

C3D_PnTriangles_API void GetRequiredVersion( Castor3D::Version & p_version)
{
	p_version = Castor3D::Version();
}

C3D_PnTriangles_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_DIVIDER;
}

C3D_PnTriangles_API String GetName()
{
	return cuT( "PN-Triangles Divider");
}

C3D_PnTriangles_API Castor3D::Subdivider * CreateDivider()
{
	Castor3D::Subdivider * l_pReturn( new Subdivider() );
	return l_pReturn;
}

C3D_PnTriangles_API void DestroyDivider( Castor3D::Subdivider * p_pDivider )
{
	delete p_pDivider;
}

C3D_PnTriangles_API String GetDividerType()
{
	return cuT( "pn_tri");
}

//*********************************************************************************************

Subdivider :: Subdivider()
	:	Castor3D::Subdivider()
{
}

Subdivider :: ~Subdivider()
{
	Cleanup();
}

void Subdivider :: Cleanup()
{
	Castor3D::Subdivider::Cleanup();
}

void Subdivider :: DoSubdivide()
{
	Castor3D::FacePtrArray l_facesArray;
	Point3r l_ptPos1;
	Point3r l_ptPos2;
	Point3r l_ptPos3;
	Point3r l_ptNml1;
	Point3r l_ptNml2;
	Point3r l_ptNml3;

	for( uint32_t i = 0; i < m_submesh->GetFacesCount(); i++ )
	{
		l_facesArray.push_back( m_submesh->GetFace( i ) );
	}

	m_submesh->ClearFaces();

	for (uint32_t i = 0; i < l_facesArray.size(); i++)
	{
		Castor3D::FaceSPtr l_face = l_facesArray[i];

		Castor3D::BufferElementGroup const & l_pt1 = *GetPoint( l_face->GetVertexIndex( 0 ) );
		Castor3D::BufferElementGroup const & l_pt2 = *GetPoint( l_face->GetVertexIndex( 1 ) );
		Castor3D::BufferElementGroup const & l_pt3 = *GetPoint( l_face->GetVertexIndex( 2 ) );

		Castor3D::Vertex::GetPosition( l_pt1, l_ptPos1 );
		Castor3D::Vertex::GetPosition( l_pt2, l_ptPos2 );
		Castor3D::Vertex::GetPosition( l_pt3, l_ptPos3 );

		Castor3D::Vertex::GetNormal( l_pt1, l_ptNml1 );
		Castor3D::Vertex::GetNormal( l_pt2, l_ptNml2 );
		Castor3D::Vertex::GetNormal( l_pt3, l_ptNml3 );

		Castor3D::BufferElementGroupSPtr l_ptD = DoComputePointFrom( l_ptPos1, l_ptPos2, l_ptNml1, l_ptNml2, m_ptDivisionCenter );
		Castor3D::BufferElementGroupSPtr l_ptE = DoComputePointFrom( l_ptPos2, l_ptPos3, l_ptNml2, l_ptNml3, m_ptDivisionCenter );
		Castor3D::BufferElementGroupSPtr l_ptF = DoComputePointFrom( l_ptPos1, l_ptPos3, l_ptNml1, l_ptNml3, m_ptDivisionCenter );

		DoSetTextCoords( l_face, l_pt1, l_pt2, l_pt3, *l_ptD, *l_ptE, *l_ptF );
	}

	l_facesArray.clear();
}

Castor3D::BufferElementGroupSPtr Subdivider :: DoComputePointFrom( Point3r const & p_ptPosA, Point3r const & p_ptPosB, Point3r const & p_ptNmlA, Point3r const & p_ptNmlB, Point3r const & p_ptCenter )
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
	if( (l_index = IsInMyPoints( l_ptPosC )) < 0 )
	{
		l_pReturn = AddPoint( l_ptPosC );
	}
	else
	{
		l_pReturn = GetPoint( l_index );
	}

	return l_pReturn;
}

//*********************************************************************************************
