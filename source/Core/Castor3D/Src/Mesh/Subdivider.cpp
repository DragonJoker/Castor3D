#include "Subdivider.hpp"

#include "Engine.hpp"
#include "SubmeshComponent/Face.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	Subdivider::Subdivider()
		: m_submesh( )
		, m_bGenerateBuffers( true )
		, m_pfnSubdivisionEnd( nullptr )
		, m_bThreaded( false )
	{
	}

	Subdivider::~Subdivider()
	{
		cleanup();
	}

	void Subdivider::subdivide( SubmeshSPtr p_submesh, int p_occurences, bool p_generateBuffers, bool p_threaded )
	{
		for ( int i = 0; i < p_occurences; ++i )
		{
			doSubdivide( p_submesh, p_generateBuffers, p_threaded );
		}
	}

	void Subdivider::cleanup()
	{
		m_pfnSubdivisionEnd = nullptr;

		if ( m_bThreaded && m_pThread )
		{
			m_pThread->join();
		}

		m_pThread.reset();
		m_bThreaded = false;
		m_submesh.reset();
		m_arrayFaces.clear();
	}

	SubmeshVertex Subdivider::addPoint( real x, real y, real z )
	{
		auto index = m_submesh->getPointsCount();
		return
		{
			index,
			m_submesh->addPoint( x, y, z )
		};
	}

	SubmeshVertex Subdivider::addPoint( Point3r const & p_v )
	{
		return addPoint( p_v[0], p_v[1], p_v[2] );
	}

	SubmeshVertex Subdivider::addPoint( real * p_v )
	{
		return addPoint( p_v[0], p_v[1], p_v[2] );
	}

	Face Subdivider::addFace( uint32_t a, uint32_t b, uint32_t c )
	{
		REQUIRE( a < getPointsCount() && b < getPointsCount() && c < getPointsCount() );
		Face result{ a, b, c };
		m_arrayFaces.push_back( result );
		return result;
	}

	int Subdivider::isInMyPoints( Point3r const & p_vertex, double p_precision )
	{
		return m_submesh->isInMyPoints( p_vertex, p_precision );
	}

	uint32_t Subdivider::getPointsCount()const
	{
		return m_submesh->getPointsCount();
	}

	SubmeshVertex Subdivider::getPoint( uint32_t i )const
	{
		return
		{
			i,
			m_submesh->getPoint( i )
		};
	}

	InterleavedVertexArray const & Subdivider::getPoints()const
	{
		return m_submesh->getPoints();
	}

	SubmeshVertex Subdivider::doTryAddPoint( Point3r const & point )
	{
		std::unique_lock< std::recursive_mutex > lock( m_mutex );
		int index = -1;

		if ( ( index = isInMyPoints( point, 0.00001 ) ) < 0 )
		{
			return addPoint( point );
		}
		else
		{
			auto & result = getPoint( index );
			auto position = result.m_vertex.m_pos;

			if ( position != point )
			{
				result.m_vertex.m_pos = ( position + point ) / real( 2 );
			}

			return result;
		}
	}

	void Subdivider::doSubdivide( SubmeshSPtr p_submesh, bool p_generateBuffers, bool p_threaded )
	{
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

	void Subdivider::doInitialise()
	{
	}

	void Subdivider::doSwapBuffers()
	{
		m_submesh->computeNormals( true );

		if ( m_bGenerateBuffers && !m_bThreaded )
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

		if ( m_bGenerateBuffers )
		{
			m_submesh->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_submesh->computeNormals();
				m_submesh->initialise();
			} ) );
		}

		if ( m_pfnSubdivisionEnd )
		{
			m_pfnSubdivisionEnd( *this );
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
		Point3r aTex = a.m_vertex.m_tex;
		Point3r bTex = b.m_vertex.m_tex;
		Point3r cTex = c.m_vertex.m_tex;
		d.m_vertex.m_tex = ( aTex + bTex ) / real( 2.0 );
		e.m_vertex.m_tex = ( bTex + cTex ) / real( 2.0 );
		f.m_vertex.m_tex = ( aTex + cTex ) / real( 2.0 );
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
		Point3r aTex = a.m_vertex.m_tex;
		Point3r bTex = b.m_vertex.m_tex;
		Point3r cTex = c.m_vertex.m_tex;
		p.m_vertex.m_tex = ( aTex + bTex + cTex ) / real( 3.0 );
		addFace( a.m_index, b.m_index, p.m_index );
		addFace( b.m_index, c.m_index, p.m_index );
		addFace( c.m_index, a.m_index, p.m_index );
	}
}

String & operator << ( String & p_stream, castor3d::SubmeshVertex const & vertex )
{
	auto & ptPos = vertex.m_vertex.m_pos;
	p_stream += cuT( "Vertex[" );
	p_stream += string::toString( vertex.m_index );
	p_stream += cuT( "] - Buffer : (" );
	p_stream += string::toString( ptPos[0] );
	p_stream += cuT( "," );
	p_stream += string::toString( ptPos[1] );
	p_stream += cuT( "," );
	p_stream += string::toString( ptPos[2] );
	p_stream += cuT( ")" );
	return p_stream;
}
