#include "Subdivider.hpp"

#include "Engine.hpp"
#include "Face.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	Subdivider::Subdivider()
		:	m_submesh( )
		,	m_bGenerateBuffers( true )
		,	m_pfnSubdivisionEnd( nullptr )
		,	m_bThreaded( false )
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

	BufferElementGroupSPtr Subdivider::addPoint( real x, real y, real z )
	{
		return m_submesh->addPoint( x, y, z );
	}

	BufferElementGroupSPtr Subdivider::addPoint( Point3r const & p_v )
	{
		return addPoint( p_v[0], p_v[1], p_v[2] );
	}

	BufferElementGroupSPtr Subdivider::addPoint( real * p_v )
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

	BufferElementGroupSPtr Subdivider::getPoint( uint32_t i )const
	{
		return m_submesh->getPoint( i );
	}

	VertexPtrArray const & Subdivider::getPoints()const
	{
		return m_submesh->getPoints();
	}

	castor3d::BufferElementGroupSPtr Subdivider::doTryAddPoint( Point3r const & p_point )
	{
		std::unique_lock< std::recursive_mutex > lock( m_mutex );
		int index = -1;
		castor3d::BufferElementGroupSPtr result;

		if ( ( index = isInMyPoints( p_point, 0.00001 ) ) < 0 )
		{
			result = addPoint( p_point );
		}
		else
		{
			result = getPoint( index );
			Coords3r coords;
			castor3d::Vertex::getPosition( *result, coords );

			if ( coords != p_point )
			{
				castor3d::Vertex::setPosition( *result, ( coords + p_point ) / real( 2 ) );
			}
		}

		return result;
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
			doSwapBuffers();
		}
	}

	void Subdivider::doInitialise()
	{
		if ( m_bGenerateBuffers )
		{
			// TODO Replace by something appropriate:
			// m_submesh->resetGpuBuffers();
		}
	}

	void Subdivider::doSwapBuffers()
	{
		for ( auto const & face : m_arrayFaces )
		{
			m_submesh->addFace( face[0], face[1], face[2] );
		}

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

	void Subdivider::doSetTextCoords( BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_d, BufferElementGroup & p_e, BufferElementGroup & p_f )
	{
		Point3r aTex;
		Point3r bTex;
		Point3r cTex;
		Vertex::getTexCoord( p_a, aTex );
		Vertex::getTexCoord( p_b, bTex );
		Vertex::getTexCoord( p_c, cTex );
		Vertex::setTexCoord( p_d, ( aTex + bTex ) / real( 2.0 ) );
		Vertex::setTexCoord( p_e, ( bTex + cTex ) / real( 2.0 ) );
		Vertex::setTexCoord( p_f, ( aTex + cTex ) / real( 2.0 ) );
		addFace( p_a.getIndex(), p_d.getIndex(), p_f.getIndex() );
		addFace( p_b.getIndex(), p_e.getIndex(), p_d.getIndex() );
		addFace( p_c.getIndex(), p_f.getIndex(), p_e.getIndex() );
		addFace( p_d.getIndex(), p_e.getIndex(), p_f.getIndex() );
	}

	void Subdivider::doSetTextCoords( BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_p )
	{
		Point3r aTex;
		Point3r bTex;
		Point3r cTex;
		Vertex::getTexCoord( p_a, aTex );
		Vertex::getTexCoord( p_b, bTex );
		Vertex::getTexCoord( p_c, cTex );
		Vertex::setTexCoord( p_p, ( aTex + bTex + cTex ) / real( 3.0 ) );
		addFace( p_a.getIndex(), p_b.getIndex(), p_p.getIndex() );
		addFace( p_b.getIndex(), p_c.getIndex(), p_p.getIndex() );
		addFace( p_c.getIndex(), p_a.getIndex(), p_p.getIndex() );
	}
}

String & operator << ( String & p_stream, castor3d::BufferElementGroup const & p_vertex )
{
	Point3r ptPos( reinterpret_cast< real const * >( p_vertex.constPtr() ) );
	p_stream += cuT( "Vertex[" );
	p_stream += string::toString( p_vertex.getIndex() );
	p_stream += cuT( "] - Buffer : [" );
	p_stream += string::toString( ptPos.constPtr() );
	p_stream += cuT( "] - (" );
	p_stream += string::toString( ptPos[0] );
	p_stream += cuT( "," );
	p_stream += string::toString( ptPos[1] );
	p_stream += cuT( "," );
	p_stream += string::toString( ptPos[2] );
	p_stream += cuT( ")" );
	return p_stream;
}
