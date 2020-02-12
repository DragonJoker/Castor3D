#include "Castor3D/Model/Mesh/Subdivider.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/Face.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	MeshSubdivider::MeshSubdivider()
		: m_submesh( )
		, m_generateBuffers( true )
		, m_onSubdivisionEnd( nullptr )
		, m_threaded( false )
	{
	}

	MeshSubdivider::~MeshSubdivider()
	{
		cleanup();
	}

	void MeshSubdivider::subdivide( SubmeshSPtr submesh
		, int occurences
		, bool generateBuffers
		, bool threaded )
	{
		for ( int i = 0; i < occurences; ++i )
		{
			doSubdivide( submesh, generateBuffers, threaded );
		}
	}

	void MeshSubdivider::cleanup()
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

	SubmeshVertex & MeshSubdivider::addPoint( float x, float y, float z )
	{
		auto index = m_submesh->getPointsCount();
		m_points.emplace_back( std::make_unique< SubmeshVertex >( SubmeshVertex{ index, m_submesh->addPoint( x, y, z ) } ) );
		return *m_points.back();
	}

	SubmeshVertex & MeshSubdivider::addPoint( castor::Point3f const & v )
	{
		return addPoint( v[0], v[1], v[2] );
	}

	SubmeshVertex & MeshSubdivider::addPoint( float * v )
	{
		return addPoint( v[0], v[1], v[2] );
	}

	Face MeshSubdivider::addFace( uint32_t a, uint32_t b, uint32_t c )
	{
		CU_Require( a < getPointsCount() && b < getPointsCount() && c < getPointsCount() );
		Face result{ a, b, c };
		m_arrayFaces.push_back( result );
		return result;
	}

	int MeshSubdivider::isInMyPoints( castor::Point3f const & vertex, double precision )
	{
		return m_submesh->isInMyPoints( vertex, precision );
	}

	uint32_t MeshSubdivider::getPointsCount()const
	{
		return m_submesh->getPointsCount();
	}

	SubmeshVertex & MeshSubdivider::getPoint( uint32_t i )const
	{
		return *m_points[i];
	}

	InterleavedVertexArray const & MeshSubdivider::getPoints()const
	{
		return m_submesh->getPoints();
	}

	SubmeshVertex & MeshSubdivider::doTryAddPoint( castor::Point3f const & point )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		int index = -1;

		if ( ( index = isInMyPoints( point, 0.00001 ) ) < 0 )
		{
			return addPoint( point );
		}

		auto & result = getPoint( index );
		auto position = result.m_vertex.pos;

		if ( position != point )
		{
			result.m_vertex.pos = ( position + point ) / 2.0f;
		}

		return result;
	}

	void MeshSubdivider::doSubdivide( SubmeshSPtr submesh, bool generateBuffers, bool threaded )
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

	void MeshSubdivider::doInitialise()
	{
	}

	void MeshSubdivider::doSwapBuffers()
	{
		m_submesh->computeNormals( true );

		if ( m_generateBuffers && !m_threaded )
		{
			m_submesh->initialise();
		}

		cleanup();
	}

	uint32_t MeshSubdivider::doSubdivideThreaded()
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		doSubdivide();
		doSwapBuffers();

		if ( m_generateBuffers )
		{
			m_submesh->getOwner()->getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this]()
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

	void MeshSubdivider::doSetTextCoords( SubmeshVertex const & a
		, SubmeshVertex const & b
		, SubmeshVertex const & c
		, SubmeshVertex & d
		, SubmeshVertex & e
		, SubmeshVertex & f )
	{
		castor::Point3f aTex = a.m_vertex.tex;
		castor::Point3f bTex = b.m_vertex.tex;
		castor::Point3f cTex = c.m_vertex.tex;
		d.m_vertex.tex = ( aTex + bTex ) / 2.0f;
		e.m_vertex.tex = ( bTex + cTex ) / 2.0f;
		f.m_vertex.tex = ( aTex + cTex ) / 2.0f;
		addFace( a.m_index, d.m_index, f.m_index );
		addFace( b.m_index, e.m_index, d.m_index );
		addFace( c.m_index, f.m_index, e.m_index );
		addFace( d.m_index, e.m_index, f.m_index );
	}

	void MeshSubdivider::doSetTextCoords( SubmeshVertex const & a
		, SubmeshVertex const & b
		, SubmeshVertex const & c
		, SubmeshVertex & p )
	{
		castor::Point3f aTex = a.m_vertex.tex;
		castor::Point3f bTex = b.m_vertex.tex;
		castor::Point3f cTex = c.m_vertex.tex;
		p.m_vertex.tex = ( aTex + bTex + cTex ) / 3.0f;
		addFace( a.m_index, b.m_index, p.m_index );
		addFace( b.m_index, c.m_index, p.m_index );
		addFace( c.m_index, a.m_index, p.m_index );
	}
}

castor::String & operator << ( castor::String & stream, castor3d::SubmeshVertex const & vertex )
{
	auto & ptPos = vertex.m_vertex.pos;
	stream += cuT( "Vertex[" );
	stream += castor::string::toString( vertex.m_index );
	stream += cuT( "] - Buffer : (" );
	stream += castor::string::toString( ptPos[0] );
	stream += cuT( "," );
	stream += castor::string::toString( ptPos[1] );
	stream += cuT( "," );
	stream += castor::string::toString( ptPos[2] );
	stream += cuT( ")" );
	return stream;
}
