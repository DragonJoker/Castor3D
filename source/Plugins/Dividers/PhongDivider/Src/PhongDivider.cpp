#include "PhongDivider.hpp"

using namespace Castor;
using namespace Castor3D;

namespace Phong
{
	namespace
	{
		Point3d Barycenter( double u, double v, Point3d const & p1, Point3d const & p2, Point3d const & p3 )
		{
			double w = 1 - u - v;
			ENSURE( u + v + w == 1 );
			return ( p1 * u + p2 * v + p3 * w );
		}
	}

	Patch::Patch( PlaneEquation< double > const & p_p1, PlaneEquation< double > const & p_p2, PlaneEquation< double > const & p_p3 )
		: pi( p_p1 )
		, pj( p_p2 )
		, pk( p_p3 )
	{
	}

	String const Subdivider::Name = cuT( "Phong Divider" );
	String const Subdivider::Type = cuT( "phong" );

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
		auto facesArray = m_submesh->GetFaces();
		m_submesh->ClearFaces();
		std::map< uint32_t, Castor::PlaneEquation< double > > posnml;
		uint32_t i = 0;

		for ( auto const & point : m_submesh->GetPoints() )
		{
			Point3r position, normal;
			Castor3D::Vertex::GetPosition( point, position );
			Castor3D::Vertex::GetNormal( point, normal );
			posnml.insert( std::make_pair( i++, Castor::PlaneEquation< double >( Point3d( normal[0], normal[1], normal[2] ), Point3d( position[0], position[1], position[2] ) ) ) );
		}

		for ( auto face : facesArray )
		{
			DoComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( posnml[face[0]], posnml[face[1]], posnml[face[2]] ) );
		}

		facesArray.clear();
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
			Castor3D::BufferElementGroupSPtr a = DoComputePoint( double( u0 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr b = DoComputePoint( double( u2 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr c = DoComputePoint( double( u0 ), double( v2 ), p_patch );
			Castor3D::BufferElementGroupSPtr d = DoComputePoint( double( u1 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr e = DoComputePoint( double( u1 ), double( v1 ), p_patch );
			Castor3D::BufferElementGroupSPtr f = DoComputePoint( double( u0 ), double( v1 ), p_patch );
			DoSetTextCoords( *a, *b, *c, *d, *e, *f );
		}
	}

	Castor3D::BufferElementGroupSPtr Subdivider::DoComputePoint( double u, double v, Patch const & p_patch )
	{
		Point3d b = Barycenter( u, v, p_patch.pi.GetPoint(), p_patch.pj.GetPoint(), p_patch.pk.GetPoint() );
		Point3d pi = p_patch.pi.Project( b );
		Point3d pj = p_patch.pj.Project( b );
		Point3d pk = p_patch.pk.Project( b );
		Point3r point( Barycenter( u, v, pi, pj, pk ) );
		return DoTryAddPoint( point );
	}
}
