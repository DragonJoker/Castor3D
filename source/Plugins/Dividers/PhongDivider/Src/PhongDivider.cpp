#include "PhongDivider.hpp"

using namespace castor;
using namespace castor3d;

namespace Phong
{
	namespace
	{
		Point3d barycenter( double u, double v, Point3d const & p1, Point3d const & p2, Point3d const & p3 )
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
		: castor3d::Subdivider()
		, m_occurences( 1 )
	{
	}

	Subdivider::~Subdivider()
	{
		cleanup();
	}

	SubdividerUPtr Subdivider::create()
	{
		return std::make_unique< Subdivider >();
	}

	void Subdivider::cleanup()
	{
		castor3d::Subdivider::cleanup();
	}

	void Subdivider::subdivide( SubmeshSPtr p_submesh, int p_occurences, bool p_generateBuffers, bool p_threaded )
	{
		m_occurences = p_occurences;
		m_submesh = p_submesh;
		m_bGenerateBuffers = p_generateBuffers;
		m_submesh->computeContainers();

		doInitialise();
		m_bThreaded = p_threaded;

		if ( p_threaded )
		{
			m_pThread = std::make_shared< std::thread >( std::bind( &Subdivider::doSubdivideThreaded, this ) );
		}
		else
		{
			doSubdivide();
			doSwapBuffers();
		}
	}

	void Subdivider::doSubdivide()
	{
		auto facesArray = m_submesh->getFaces();
		m_submesh->clearFaces();
		std::map< uint32_t, castor::PlaneEquation< double > > posnml;
		uint32_t i = 0;

		for ( auto const & point : m_submesh->getPoints() )
		{
			Point3r position, normal;
			castor3d::Vertex::getPosition( point, position );
			castor3d::Vertex::getNormal( point, normal );
			posnml.insert( std::make_pair( i++, castor::PlaneEquation< double >( Point3d( normal[0], normal[1], normal[2] ), Point3d( position[0], position[1], position[2] ) ) ) );
		}

		for ( auto face : facesArray )
		{
			doComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( posnml[face[0]], posnml[face[1]], posnml[face[2]] ) );
		}

		facesArray.clear();
	}

	void Subdivider::doComputeFaces( double u0, double v0, double u2, double v2, int p_occurences, Patch const & p_patch )
	{
		double u1 = ( u0 + u2 ) / 2.0;
		double v1 = ( v0 + v2 ) / 2.0;

		if ( p_occurences > 1 )
		{
			doComputeFaces( u0, v0, u1, v1, p_occurences - 1, p_patch );
			doComputeFaces( u0, v1, u1, v2, p_occurences - 1, p_patch );
			doComputeFaces( u1, v0, u2, v1, p_occurences - 1, p_patch );
			doComputeFaces( u1, v1, u0, v0, p_occurences - 1, p_patch );
		}
		else
		{
			castor3d::BufferElementGroupSPtr a = doComputePoint( double( u0 ), double( v0 ), p_patch );
			castor3d::BufferElementGroupSPtr b = doComputePoint( double( u2 ), double( v0 ), p_patch );
			castor3d::BufferElementGroupSPtr c = doComputePoint( double( u0 ), double( v2 ), p_patch );
			castor3d::BufferElementGroupSPtr d = doComputePoint( double( u1 ), double( v0 ), p_patch );
			castor3d::BufferElementGroupSPtr e = doComputePoint( double( u1 ), double( v1 ), p_patch );
			castor3d::BufferElementGroupSPtr f = doComputePoint( double( u0 ), double( v1 ), p_patch );
			doSetTextCoords( *a, *b, *c, *d, *e, *f );
		}
	}

	castor3d::BufferElementGroupSPtr Subdivider::doComputePoint( double u, double v, Patch const & p_patch )
	{
		Point3d b = barycenter( u, v, p_patch.pi.getPoint(), p_patch.pj.getPoint(), p_patch.pk.getPoint() );
		Point3d pi = p_patch.pi.project( b );
		Point3d pj = p_patch.pj.project( b );
		Point3d pk = p_patch.pk.project( b );
		Point3r point( barycenter( u, v, pi, pj, pk ) );
		return doTryAddPoint( point );
	}
}
