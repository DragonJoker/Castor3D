#include "PnTrianglesDivider.hpp"

using namespace Castor;
using namespace Castor3D;

namespace PnTriangles
{
	namespace
	{
		Point3d Barycenter( double u, double v, Point3d const & p1, Point3d const & p2, Point3d const & p3 )
		{
			double w = 1.0 - u - v;
			ENSURE( u + v + w == 1 );
			return ( p1 * u + p2 * v + p3 * w );
		}
	}

	Patch::Patch( PlaneEquation< double >const & p_p1, PlaneEquation< double >const & p_p2, PlaneEquation< double >const & p_p3 )
	{
		b300 = Barycenter( 3 / 3.0, 0 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() );
		b030 = Barycenter( 0 / 3.0, 3 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() );
		b003 = Barycenter( 0 / 3.0, 0 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() );
		b201 = p_p1.Project( Barycenter( 2 / 3.0, 0 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() ) );
		b210 = p_p1.Project( Barycenter( 2 / 3.0, 1 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() ) );
		b120 = p_p2.Project( Barycenter( 1 / 3.0, 2 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() ) );
		b021 = p_p2.Project( Barycenter( 0 / 3.0, 2 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() ) );
		b102 = p_p3.Project( Barycenter( 1 / 3.0, 0 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() ) );
		b012 = p_p3.Project( Barycenter( 0 / 3.0, 1 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() ) );
		Point3d e = ( b210 + b120 + b021 + b012 + b102 + b201 ) / double( 6.0 );
		b111 = e + ( e - Barycenter( 1 / 3.0, 1 / 3.0, p_p1.GetPoint(), p_p2.GetPoint(), p_p3.GetPoint() ) ) / double( 2.0 );
	}

	String const Subdivider::Name = cuT( "PN-Triangles Divider" );
	String const Subdivider::Type = cuT( "pn_tri" );

	Subdivider::Subdivider()
		: Castor3D::Subdivider()
		, m_occurences( 1 )
	{
	}

	Subdivider::~Subdivider()
	{
		Cleanup();
	}

	SubdividerUPtr Subdivider::Create()
	{
		return std::make_unique< Subdivider >();
	}

	void Subdivider::Cleanup()
	{
		Castor3D::Subdivider::Cleanup();
	}

	void Subdivider::Subdivide( SubmeshSPtr p_submesh, int p_occurences, bool p_generateBuffers, bool p_threaded )
	{
		m_occurences = p_occurences;
		m_submesh = p_submesh;
		m_bGenerateBuffers = p_generateBuffers;
		m_submesh->ComputeContainers();

		DoInitialise();
		m_bThreaded = p_threaded;

		if ( p_threaded )
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
		auto l_facesArray = m_submesh->GetFaces();
		m_submesh->ClearFaces();
		std::map< uint32_t, Castor::PlaneEquation< double > > l_posnml;
		uint32_t i = 0;

		for ( auto const & l_point : m_submesh->GetPoints() )
		{
			Point3r l_position, l_normal;
			Castor3D::Vertex::GetPosition( l_point, l_position );
			Castor3D::Vertex::GetNormal( l_point, l_normal );
			l_posnml.insert( std::make_pair( i++, Castor::PlaneEquation< double >( Point3d( l_normal[0], l_normal[1], l_normal[2] ), Point3d( l_position[0], l_position[1], l_position[2] ) ) ) );
		}

		for ( auto const & l_face : l_facesArray )
		{
			DoComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( l_posnml[l_face[0]], l_posnml[l_face[1]], l_posnml[l_face[2]] ) );
		}
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
		double w = 1.0 - u - v;
		double u2 = double( u * u );
		double v2 = double( v * v );
		double w2 = double( w * w );
		double u3 = double( u2 * u );
		double v3 = double( v2 * v );
		double w3 = double( w2 * w );

		Point3r l_point = Point3r( p_patch.b300 * w3
								   + p_patch.b030 * u3
								   + p_patch.b003 * v3
								   + p_patch.b210 * 3.0 * w2 * u
								   + p_patch.b120 * 3.0 * w * u2
								   + p_patch.b201 * 3.0 * w2 * v
								   + p_patch.b021 * 3.0 * u2 * v
								   + p_patch.b102 * 3.0 * w * v2
								   + p_patch.b012 * 3.0 * u * v2
								   + p_patch.b111 * 6.0 * w * u * v );

		return DoTryAddPoint( l_point );
	}
}
