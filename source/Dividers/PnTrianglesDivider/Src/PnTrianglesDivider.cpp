#include "PnTrianglesDivider.hpp"

using namespace Castor;
using namespace Castor3D;

namespace PnTriangles
{
	namespace
	{
		Point3d ComputeB( int i, int j, int k, Point3d const & p1, Point3d const & p2, Point3d const & p3 )
		{
			return ( p1 * double( i ) + p2 * double( j ) + p3 * double( k ) ) / double( 3.0 );
		}

		Point3d & ProjectB( Point3d & b, Point3d const & p, Point3d const & n )
		{
			double w = point::dot( ( b - p ), n );
			b = ( b - n * w );
			return b;
		}

		double ComputeV( std::pair< Point3d, Point3d > const & i, std::pair< Point3d, Point3d > const & j )
		{
			Point3d diff = j.first - i.first;
			return ( ( point::dot( diff, ( i.second + j.second ) ) ) / ( point::dot( diff, diff ) ) ) * double( 2.0 );
		}
	}

	Patch::Patch( std::pair< Point3d, Point3d > const & p_p1, std::pair< Point3d, Point3d > const & p_p2, std::pair< Point3d, Point3d > const & p_p3 )
	{
		b300 = ComputeB( 3, 0, 0, p_p1.first, p_p2.first, p_p3.first );
		b030 = ComputeB( 0, 3, 0, p_p1.first, p_p2.first, p_p3.first );
		b003 = ComputeB( 0, 0, 3, p_p1.first, p_p2.first, p_p3.first );
		b201 = ProjectB( ComputeB( 2, 0, 1, p_p1.first, p_p2.first, p_p3.first ), p_p1.first, p_p1.second );
		b210 = ProjectB( ComputeB( 2, 1, 0, p_p1.first, p_p2.first, p_p3.first ), p_p1.first, p_p1.second );
		b120 = ProjectB( ComputeB( 1, 2, 0, p_p1.first, p_p2.first, p_p3.first ), p_p2.first, p_p2.second );
		b021 = ProjectB( ComputeB( 0, 2, 1, p_p1.first, p_p2.first, p_p3.first ), p_p2.first, p_p2.second );
		b102 = ProjectB( ComputeB( 1, 0, 2, p_p1.first, p_p2.first, p_p3.first ), p_p3.first, p_p3.second );
		b012 = ProjectB( ComputeB( 0, 1, 2, p_p1.first, p_p2.first, p_p3.first ), p_p3.first, p_p3.second );
		Point3d e = ( b210 + b120 + b021 + b012 + b102 + b201 ) / double( 6.0 );
		b111 = e + ( e - ComputeB( 1, 1, 1, p_p1.first, p_p2.first, p_p3.first ) ) / double( 2.0 );
	}

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

	void Subdivider::Subdivide( SubmeshSPtr p_pSubmesh, int p_occurences, bool p_bGenerateBuffers, bool p_bThreaded )
	{
		m_occurences = p_occurences;
		m_submesh = p_pSubmesh;
		m_bGenerateBuffers = p_bGenerateBuffers;
		m_submesh->ComputeContainers();

		DoInitialise();
		m_bThreaded = p_bThreaded;

		if ( p_bThreaded )
		{
			m_pThread = std::make_shared< std::thread >( std::bind( &Subdivider::DoSubdivideThreaded, this ) );
		}
		else
		{
			DoSubdivide();
			DoSwapBuffers();
		}
	}

	void Subdivider::DoSubdivide()
	{
		Castor3D::FacePtrArray l_facesArray;

		for ( uint32_t i = 0; i < m_submesh->GetFaceCount(); i++ )
		{
			l_facesArray.push_back( m_submesh->GetFace( i ) );
		}

		m_submesh->ClearFaces();
		std::map< uint32_t, std::pair< Point3d, Point3d > > l_posnml;

		for ( uint32_t i = 0; i < m_submesh->GetPointsCount(); ++i )
		{
			Point3r l_position, l_normal;
			Castor3D::BufferElementGroup const & l_point = *GetPoint( i );
			Castor3D::Vertex::GetPosition( l_point, l_position );
			Castor3D::Vertex::GetNormal( l_point, l_normal );
			l_posnml.insert( std::make_pair( i, std::make_pair( Point3d( l_position[0], l_position[1], l_position[2] ), Point3d( l_normal[0], l_normal[1], l_normal[2] ) ) ) );
		}
		
		for ( Castor3D::FacePtrArray::iterator l_it = l_facesArray.begin(); l_it != l_facesArray.end(); ++l_it )
		{
			Castor3D::FaceSPtr l_face = *l_it;
			DoComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( l_posnml[l_face->GetVertexIndex( 0 )], l_posnml[l_face->GetVertexIndex( 1 )], l_posnml[l_face->GetVertexIndex( 2 )] ) );
		}

		l_facesArray.clear();
	}

	void Subdivider::DoComputeFaces( double u0, double v0, double u2, double v2, int p_occurences, Patch const & p_patch )
	{
		double u1 = ( u0 + u2 ) / 2.0;
		double v1 = ( v0 + v2 ) / 2.0;

		if ( p_occurences > 1 )
		{
			DoComputeFaces( u0, v0, u1, v1, p_occurences - 1, p_patch );
			DoComputeFaces( u0, v1, u1, v2, p_occurences - 1, p_patch );
			DoComputeFaces( u1, v0, u2, v1, p_occurences - 1, p_patch );
			DoComputeFaces( u1, v1, u0, v0, p_occurences - 1, p_patch );
		}
		else
		{
			Castor3D::BufferElementGroupSPtr l_a = DoComputePoint( double( u0 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_b = DoComputePoint( double( u2 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_c = DoComputePoint( double( u0 ), double( v2 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_d = DoComputePoint( double( u1 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_e = DoComputePoint( double( u1 ), double( v1 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_f = DoComputePoint( double( u0 ), double( v1 ), p_patch );
			DoSetTextCoords( *l_a, *l_b, *l_c, *l_d, *l_e, *l_f );
		}
	}

	Castor3D::BufferElementGroupSPtr Subdivider::DoComputePoint( double u, double v, Patch const & p_patch )
	{
		Castor3D::BufferElementGroupSPtr l_pReturn;
		int l_index;
		double w = 1.0 - u - v;
		double u2 = double( u * u );
		double v2 = double( v * v );
		double w2 = double( w * w );
		double u3 = double( u2 * u );
		double v3 = double( v2 * v );
		double w3 = double( w2 * w );

		Point3d l_point = p_patch.b300 * w3
			+ p_patch.b030 * u3
			+ p_patch.b003 * v3
			+ p_patch.b210 * 3.0 * w2 * u
			+ p_patch.b120 * 3.0 * w * u2
			+ p_patch.b201 * 3.0 * w2 * v
			+ p_patch.b021 * 3.0 * u2 * v
			+ p_patch.b102 * 3.0 * w * v2
			+ p_patch.b012 * 3.0 * u * v2
			+ p_patch.b111 * 6.0 * w * u * v;

		Point3r l_tmp( l_point[0], l_point[1], l_point[2] );

		if ( ( l_index = IsInMyPoints( l_tmp, 0.001 ) ) < 0 )
		{
			l_pReturn = AddPoint( l_tmp );
		}
		else
		{
			l_pReturn = GetPoint( l_index );
			Coords3r l_tmp2;
			Castor3D::Vertex::GetPosition( *l_pReturn, l_tmp2 );

			if ( l_tmp2 != l_tmp )
			{
				Castor3D::Vertex::SetPosition( *l_pReturn, ( l_tmp2 + l_tmp ) / real( 2 ) );
			}
		}

		return l_pReturn;
	}
}
