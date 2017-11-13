#include "PnTrianglesDivider.hpp"

using namespace castor;
using namespace castor3d;

namespace PnTriangles
{
	namespace
	{
		Point3r barycenter( real u, real v, Point3r const & p1, Point3r const & p2, Point3r const & p3 )
		{
			double w = 1.0 - u - v;
			ENSURE( std::abs( u + v + w - 1.0 ) < 0.0001 );
			return ( p1 * u + p2 * v + p3 * w );
		}
	}

	Patch::Patch( Plane const & p_p1, Plane const & p_p2, Plane const & p_p3 )
	{
		b300 = barycenter( 3 / 3.0_r, 0 / 3.0_r, p_p1.point, p_p2.point, p_p3.point );
		b030 = barycenter( 0 / 3.0_r, 3 / 3.0_r, p_p1.point, p_p2.point, p_p3.point );
		b003 = barycenter( 0 / 3.0_r, 0 / 3.0_r, p_p1.point, p_p2.point, p_p3.point );
		b201 = p_p1.plane.project( barycenter( 2 / 3.0_r, 0 / 3.0_r, p_p1.point, p_p2.point, p_p3.point ) );
		b210 = p_p1.plane.project( barycenter( 2 / 3.0_r, 1 / 3.0_r, p_p1.point, p_p2.point, p_p3.point ) );
		b120 = p_p2.plane.project( barycenter( 1 / 3.0_r, 2 / 3.0_r, p_p1.point, p_p2.point, p_p3.point ) );
		b021 = p_p2.plane.project( barycenter( 0 / 3.0_r, 2 / 3.0_r, p_p1.point, p_p2.point, p_p3.point ) );
		b102 = p_p3.plane.project( barycenter( 1 / 3.0_r, 0 / 3.0_r, p_p1.point, p_p2.point, p_p3.point ) );
		b012 = p_p3.plane.project( barycenter( 0 / 3.0_r, 1 / 3.0_r, p_p1.point, p_p2.point, p_p3.point ) );
		Point3r e = ( b210 + b120 + b021 + b012 + b102 + b201 ) / 6.0_r;
		b111 = e + ( e - barycenter( 1 / 3.0_r, 1 / 3.0_r, p_p1.point, p_p2.point, p_p3.point ) ) / 2.0_r;
	}

	String const Subdivider::Name = cuT( "PN-Triangles Divider" );
	String const Subdivider::Type = cuT( "pn_tri" );

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
			doAddGeneratedFaces();
			doSwapBuffers();
		}
	}

	void Subdivider::doSubdivide()
	{
		auto facesArray = m_indexMapping->getFaces();
		m_indexMapping->clearFaces();
		std::map< uint32_t, Plane > posnml;
		uint32_t i = 0;

		for ( auto const & point : m_submesh->getPoints() )
		{
			Point3r position, normal;
			castor3d::Vertex::getPosition( point, position );
			castor3d::Vertex::getNormal( point, normal );
			posnml.emplace( i++, Plane{ castor::PlaneEquation( normal, position ), position } );
		}

		for ( auto const & face : facesArray )
		{
			doComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( posnml[face[0]], posnml[face[1]], posnml[face[2]] ) );
		}
	}

	void Subdivider::doInitialise()
	{
		castor3d::Subdivider::doInitialise();
		m_indexMapping = m_submesh->getComponent< TriFaceMapping >();
	}

	void Subdivider::doAddGeneratedFaces()
	{
		for ( auto const & face : m_arrayFaces )
		{
			m_indexMapping->addFace( face[0], face[1], face[2] );
		}
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
		double w = 1.0 - u - v;
		double u2 = double( u * u );
		double v2 = double( v * v );
		double w2 = double( w * w );
		double u3 = double( u2 * u );
		double v3 = double( v2 * v );
		double w3 = double( w2 * w );

		Point3r point = Point3r( p_patch.b300 * w3
			+ p_patch.b030 * u3
			+ p_patch.b003 * v3
			+ p_patch.b210 * 3.0 * w2 * u
			+ p_patch.b120 * 3.0 * w * u2
			+ p_patch.b201 * 3.0 * w2 * v
			+ p_patch.b021 * 3.0 * u2 * v
			+ p_patch.b102 * 3.0 * w * v2
			+ p_patch.b012 * 3.0 * u * v2
			+ p_patch.b111 * 6.0 * w * u * v );

		return doTryAddPoint( point );
	}
}
