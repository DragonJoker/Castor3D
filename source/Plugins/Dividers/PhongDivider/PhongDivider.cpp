#include "PhongDivider/PhongDivider.hpp"

using namespace castor;
using namespace castor3d;

namespace Phong
{
	namespace
	{
		Point3r barycenter( real u
			, real v
			, Point3r const & p1
			, Point3r const & p2
			, Point3r const & p3 )
		{
			real w = real( 1 - u - v );
			CU_Ensure( std::abs( u + v + w - 1.0 ) < 0.0001 );
			return Point3r{ p1 * u + p2 * v + p3 * w };
		}
	}

	Patch::Patch( Plane const & p1
		, Plane const & p2
		, Plane const & p3 )
		: pi( p1 )
		, pj( p2 )
		, pk( p3 )
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

	void Subdivider::subdivide( SubmeshSPtr submesh
		, int occurences
		, bool generateBuffers
		, bool threaded )
	{
		m_occurences = occurences;
		m_submesh = submesh;
		m_generateBuffers = generateBuffers;
		m_submesh->computeContainers();

		doInitialise();
		m_threaded = threaded;

		if ( threaded )
		{
			m_thread = std::make_shared< std::thread >( std::bind( &Subdivider::doSubdivideThreaded, this ) );
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
			Point3r position = point.pos;
			Point3r normal = point.nml;
			posnml.emplace( i++, Plane{ castor::PlaneEquation( normal, position ), position } );
		}

		for ( auto face : facesArray )
		{
			doComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( posnml[face[0]], posnml[face[1]], posnml[face[2]] ) );
		}

		for ( auto & point : m_points )
		{
			m_submesh->getPoint( point->m_index ).pos = point->m_vertex.pos;
		}

		facesArray.clear();
	}

	void Subdivider::doInitialise()
	{
		for ( uint32_t i = 0; i < m_submesh->getPointsCount(); ++i )
		{
			m_points.emplace_back( std::make_unique< SubmeshVertex >( SubmeshVertex{ i, m_submesh->getPoint( i ) } ) );
		}

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

	void Subdivider::doComputeFaces( real u0
		, real v0
		, real u2
		, real v2
		, int occurences
		, Patch const & patch )
	{
		real u1 = ( u0 + u2 ) / 2.0_r;
		real v1 = ( v0 + v2 ) / 2.0_r;

		if ( occurences > 1 )
		{
			doComputeFaces( u0, v0, u1, v1, occurences - 1, patch );
			doComputeFaces( u0, v1, u1, v2, occurences - 1, patch );
			doComputeFaces( u1, v0, u2, v1, occurences - 1, patch );
			doComputeFaces( u1, v1, u0, v0, occurences - 1, patch );
		}
		else
		{
			auto & a = doComputePoint( u0, v0, patch );
			auto & b = doComputePoint( u2, v0, patch );
			auto & c = doComputePoint( u0, v2, patch );
			auto & d = doComputePoint( u1, v0, patch );
			auto & e = doComputePoint( u1, v1, patch );
			auto & f = doComputePoint( u0, v1, patch );
			doSetTextCoords( a, b, c, d, e, f );
		}
	}

	castor3d::SubmeshVertex & Subdivider::doComputePoint( real u, real v, Patch const & patch )
	{
		Point3r b = barycenter( u, v, patch.pi.point, patch.pj.point, patch.pk.point );
		Point3r pi = patch.pi.plane.project( b );
		Point3r pj = patch.pj.plane.project( b );
		Point3r pk = patch.pk.plane.project( b );
		Point3r point( barycenter( u, v, pi, pj, pk ) );
		return doTryAddPoint( point );
	}
}
