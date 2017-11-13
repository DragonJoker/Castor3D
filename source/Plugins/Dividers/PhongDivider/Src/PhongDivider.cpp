#include "PhongDivider.hpp"

using namespace castor;
using namespace castor3d;

namespace Phong
{
	namespace
	{
		Point3r barycenter( real u, real v, Point3r const & p1, Point3r const & p2, Point3r const & p3 )
		{
			double w = 1 - u - v;
			ENSURE( u + v + w == 1 );
			return ( p1 * u + p2 * v + p3 * w );
		}
	}

	Patch::Patch( Plane const & p_p1, Plane const & p_p2, Plane const & p_p3 )
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

		for ( auto face : facesArray )
		{
			doComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( posnml[face[0]], posnml[face[1]], posnml[face[2]] ) );
		}

		facesArray.clear();
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

	void Subdivider::doComputeFaces( real u0, real v0, real u2, real v2, int p_occurences, Patch const & p_patch )
	{
		real u1 = ( u0 + u2 ) / 2.0_r;
		real v1 = ( v0 + v2 ) / 2.0_r;

		if ( p_occurences > 1 )
		{
			doComputeFaces( u0, v0, u1, v1, p_occurences - 1, p_patch );
			doComputeFaces( u0, v1, u1, v2, p_occurences - 1, p_patch );
			doComputeFaces( u1, v0, u2, v1, p_occurences - 1, p_patch );
			doComputeFaces( u1, v1, u0, v0, p_occurences - 1, p_patch );
		}
		else
		{
			castor3d::BufferElementGroupSPtr a = doComputePoint( u0, v0, p_patch );
			castor3d::BufferElementGroupSPtr b = doComputePoint( u2, v0, p_patch );
			castor3d::BufferElementGroupSPtr c = doComputePoint( u0, v2, p_patch );
			castor3d::BufferElementGroupSPtr d = doComputePoint( u1, v0, p_patch );
			castor3d::BufferElementGroupSPtr e = doComputePoint( u1, v1, p_patch );
			castor3d::BufferElementGroupSPtr f = doComputePoint( u0, v1, p_patch );
			doSetTextCoords( *a, *b, *c, *d, *e, *f );
		}
	}

	castor3d::BufferElementGroupSPtr Subdivider::doComputePoint( real u, real v, Patch const & p_patch )
	{
		Point3r b = barycenter( u, v, p_patch.pi.point, p_patch.pj.point, p_patch.pk.point );
		Point3r pi = p_patch.pi.plane.project( b );
		Point3r pj = p_patch.pj.plane.project( b );
		Point3r pk = p_patch.pk.plane.project( b );
		Point3r point( barycenter( u, v, pi, pj, pk ) );
		return doTryAddPoint( point );
	}
}
