#include "Castor3D/Mesh/Subdivider.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Mesh/SubmeshComponent/Face.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/Vertex.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	Subdivider::Subdivider()
		: m_submesh( )
		, m_generateBuffers( true )
		, m_onSubdivisionEnd( nullptr )
		, m_threaded( false )
	{
	}

	Subdivider::~Subdivider()
	{
		cleanup();
	}

	void Subdivider::subdivide( SubmeshSPtr submesh
		, int occurences
		, bool generateBuffers
		, bool threaded )
	{
		for ( int i = 0; i < occurences; ++i )
		{
			doSubdivide( submesh, generateBuffers, threaded );
		}
	}

	void Subdivider::cleanup()
	{
		m_onSubdivisionEnd = nullptr;

		if ( m_threaded && m_thread )
		{
			m_thread->join();
		}

		m_thread.reset();
		m_threaded = false;
		m_submesh.reset();
		m_arrayFaces.clear();
	}

	SubmeshVertex & Subdivider::addPoint( real x, real y, real z )
	{
		auto index = m_submesh->getPointsCount();
		m_points.emplace_back( std::make_unique< SubmeshVertex >( SubmeshVertex{ index, m_submesh->addPoint( x, y, z ) } ) );
		return *m_points.back();
	}

	SubmeshVertex & Subdivider::addPoint( Point3r const & v )
	{
		return addPoint( v[0], v[1], v[2] );
	}

	SubmeshVertex & Subdivider::addPoint( real * v )
	{
		return addPoint( v[0], v[1], v[2] );
	}

	Face Subdivider::addFace( uint32_t a, uint32_t b, uint32_t c )
	{
		CU_Require( a < getPointsCount() && b < getPointsCount() && c < getPointsCount() );
		Face result{ a, b, c };
		m_arrayFaces.push_back( result );
		return result;
	}

	int Subdivider::isInMyPoints( Point3r const & vertex, double precision )
	{
		return m_submesh->isInMyPoints( vertex, precision );
	}

	uint32_t Subdivider::getPointsCount()const
	{
		return m_submesh->getPointsCount();
	}

	SubmeshVertex & Subdivider::getPoint( uint32_t i )const
	{
		return *m_points[i];
	}

	InterleavedVertexArray const & Subdivider::getPoints()const
	{
		return m_submesh->getPoints();
	}

	SubmeshVertex & Subdivider::doTryAddPoint( Point3r const & point )
	{
		auto lock = makeUniqueLock( m_mutex );
		int index = -1;

		if ( ( index = isInMyPoints( point, 0.00001 ) ) < 0 )
		{
			return addPoint( point );
		}

		auto & result = getPoint( index );
		auto position = result.m_vertex.pos;

		if ( position != point )
		{
			result.m_vertex.pos = ( position + point ) / real( 2 );
		}

		return result;
	}

	void Subdivider::doSubdivide( SubmeshSPtr submesh, bool generateBuffers, bool threaded )
	{
		m_submesh = submesh;
		m_generateBuffers = generateBuffers;
		m_submesh->computeContainers();

		doInitialise();
		m_threaded = threaded;

		if ( threaded )
		{
			m_thread = std::make_shared< std::thread >( [this]()
				{
					doSubdivideThreaded();
				} );
		}
		else
		{
			doSubdivide();
			doAddGeneratedFaces();
			doSwapBuffers();
		}
	}

	void Subdivider::doInitialise()
	{
	}

	void Subdivider::doSwapBuffers()
	{
		m_submesh->computeNormals( true );

		if ( m_generateBuffers && !m_threaded )
		{
			m_submesh->initialise();
		}

		cleanup();
	}

	uint32_t Subdivider::doSubdivideThreaded()
	{
		auto lock = castor::makeUniqueLock( m_mutex );
		doSubdivide();
		doSwapBuffers();

		if ( m_generateBuffers )
		{
			m_submesh->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_submesh->computeNormals();
				m_submesh->initialise();
			} ) );
		}

		if ( m_onSubdivisionEnd )
		{
			m_onSubdivisionEnd( *this );
		}

		return 0;
	}

	void Subdivider::doSetTextCoords( SubmeshVertex const & a
		, SubmeshVertex const & b
		, SubmeshVertex const & c
		, SubmeshVertex & d
		, SubmeshVertex & e
		, SubmeshVertex & f )
	{
		Point3r aTex = a.m_vertex.tex;
		Point3r bTex = b.m_vertex.tex;
		Point3r cTex = c.m_vertex.tex;
		d.m_vertex.tex = ( aTex + bTex ) / real( 2.0 );
		e.m_vertex.tex = ( bTex + cTex ) / real( 2.0 );
		f.m_vertex.tex = ( aTex + cTex ) / real( 2.0 );
		addFace( a.m_index, d.m_index, f.m_index );
		addFace( b.m_index, e.m_index, d.m_index );
		addFace( c.m_index, f.m_index, e.m_index );
		addFace( d.m_index, e.m_index, f.m_index );
	}

	void Subdivider::doSetTextCoords( SubmeshVertex const & a
		, SubmeshVertex const & b
		, SubmeshVertex const & c
		, SubmeshVertex & p )
	{
		Point3r aTex = a.m_vertex.tex;
		Point3r bTex = b.m_vertex.tex;
		Point3r cTex = c.m_vertex.tex;
		p.m_vertex.tex = ( aTex + bTex + cTex ) / real( 3.0 );
		addFace( a.m_index, b.m_index, p.m_index );
		addFace( b.m_index, c.m_index, p.m_index );
		addFace( c.m_index, a.m_index, p.m_index );
	}
}

String & operator << ( String & stream, castor3d::SubmeshVertex const & vertex )
{
	auto & ptPos = vertex.m_vertex.pos;
	stream += cuT( "Vertex[" );
	stream += string::toString( vertex.m_index );
	stream += cuT( "] - Buffer : (" );
	stream += string::toString( ptPos[0] );
	stream += cuT( "," );
	stream += string::toString( ptPos[1] );
	stream += cuT( "," );
	stream += string::toString( ptPos[2] );
	stream += cuT( ")" );
	return stream;
}
