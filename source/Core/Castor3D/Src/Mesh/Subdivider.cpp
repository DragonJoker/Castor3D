#include "Subdivider.hpp"

#include "Engine.hpp"
#include "Face.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Scene/Scene.hpp"

using namespace Castor;

namespace Castor3D
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
		Cleanup();
	}

	void Subdivider::Subdivide( SubmeshSPtr p_submesh, int p_occurences, bool p_generateBuffers, bool p_threaded )
	{
		for ( int i = 0; i < p_occurences; ++i )
		{
			DoSubdivide( p_submesh, p_generateBuffers, p_threaded );
		}
	}

	void Subdivider::Cleanup()
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

	BufferElementGroupSPtr Subdivider::AddPoint( real x, real y, real z )
	{
		return m_submesh->AddPoint( x, y, z );
	}

	BufferElementGroupSPtr Subdivider::AddPoint( Point3r const & p_v )
	{
		return AddPoint( p_v[0], p_v[1], p_v[2] );
	}

	BufferElementGroupSPtr Subdivider::AddPoint( real * p_v )
	{
		return AddPoint( p_v[0], p_v[1], p_v[2] );
	}

	Face Subdivider::AddFace( uint32_t a, uint32_t b, uint32_t c )
	{
		REQUIRE( a < GetPointsCount() && b < GetPointsCount() && c < GetPointsCount() );
		Face result{ a, b, c };
		m_arrayFaces.push_back( result );
		return result;
	}

	int Subdivider::IsInMyPoints( Point3r const & p_vertex, double p_precision )
	{
		return m_submesh->IsInMyPoints( p_vertex, p_precision );
	}

	uint32_t Subdivider::GetPointsCount()const
	{
		return m_submesh->GetPointsCount();
	}

	BufferElementGroupSPtr Subdivider::GetPoint( uint32_t i )const
	{
		return m_submesh->GetPoint( i );
	}

	VertexPtrArray const & Subdivider::GetPoints()const
	{
		return m_submesh->GetPoints();
	}

	Castor3D::BufferElementGroupSPtr Subdivider::DoTryAddPoint( Point3r const & p_point )
	{
		std::unique_lock< std::recursive_mutex > lock( m_mutex );
		int index = -1;
		Castor3D::BufferElementGroupSPtr result;

		if ( ( index = IsInMyPoints( p_point, 0.00001 ) ) < 0 )
		{
			result = AddPoint( p_point );
		}
		else
		{
			result = GetPoint( index );
			Coords3r coords;
			Castor3D::Vertex::GetPosition( *result, coords );

			if ( coords != p_point )
			{
				Castor3D::Vertex::SetPosition( *result, ( coords + p_point ) / real( 2 ) );
			}
		}

		return result;
	}

	void Subdivider::DoSubdivide( SubmeshSPtr p_submesh, bool p_generateBuffers, bool p_threaded )
	{
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

	void Subdivider::DoInitialise()
	{
		if ( m_bGenerateBuffers )
		{
			// TODO Replace by something appropriate:
			// m_submesh->ResetGpuBuffers();
		}
	}

	void Subdivider::DoSwapBuffers()
	{
		for ( auto const & face : m_arrayFaces )
		{
			m_submesh->AddFace( face[0], face[1], face[2] );
		}

		m_submesh->ComputeNormals( true );

		if ( m_bGenerateBuffers && !m_bThreaded )
		{
			m_submesh->Initialise();
		}

		Cleanup();
	}

	uint32_t Subdivider::DoSubdivideThreaded()
	{
		auto lock = Castor::make_unique_lock( m_mutex );
		DoSubdivide();
		DoSwapBuffers();

		if ( m_bGenerateBuffers )
		{
			m_submesh->GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this]()
			{
				m_submesh->ComputeNormals();
				m_submesh->Initialise();
			} ) );
		}

		if ( m_pfnSubdivisionEnd )
		{
			m_pfnSubdivisionEnd( *this );
		}

		return 0;
	}

	void Subdivider::DoSetTextCoords( BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_d, BufferElementGroup & p_e, BufferElementGroup & p_f )
	{
		Point3r aTex;
		Point3r bTex;
		Point3r cTex;
		Vertex::GetTexCoord( p_a, aTex );
		Vertex::GetTexCoord( p_b, bTex );
		Vertex::GetTexCoord( p_c, cTex );
		Vertex::SetTexCoord( p_d, ( aTex + bTex ) / real( 2.0 ) );
		Vertex::SetTexCoord( p_e, ( bTex + cTex ) / real( 2.0 ) );
		Vertex::SetTexCoord( p_f, ( aTex + cTex ) / real( 2.0 ) );
		AddFace( p_a.GetIndex(), p_d.GetIndex(), p_f.GetIndex() );
		AddFace( p_b.GetIndex(), p_e.GetIndex(), p_d.GetIndex() );
		AddFace( p_c.GetIndex(), p_f.GetIndex(), p_e.GetIndex() );
		AddFace( p_d.GetIndex(), p_e.GetIndex(), p_f.GetIndex() );
	}

	void Subdivider::DoSetTextCoords( BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_p )
	{
		Point3r aTex;
		Point3r bTex;
		Point3r cTex;
		Vertex::GetTexCoord( p_a, aTex );
		Vertex::GetTexCoord( p_b, bTex );
		Vertex::GetTexCoord( p_c, cTex );
		Vertex::SetTexCoord( p_p, ( aTex + bTex + cTex ) / real( 3.0 ) );
		AddFace( p_a.GetIndex(), p_b.GetIndex(), p_p.GetIndex() );
		AddFace( p_b.GetIndex(), p_c.GetIndex(), p_p.GetIndex() );
		AddFace( p_c.GetIndex(), p_a.GetIndex(), p_p.GetIndex() );
	}
}

String & operator << ( String & p_stream, Castor3D::BufferElementGroup const & p_vertex )
{
	Point3r ptPos( reinterpret_cast< real const * >( p_vertex.const_ptr() ) );
	p_stream += cuT( "Vertex[" );
	p_stream += string::to_string( p_vertex.GetIndex() );
	p_stream += cuT( "] - Buffer : [" );
	p_stream += string::to_string( ptPos.const_ptr() );
	p_stream += cuT( "] - (" );
	p_stream += string::to_string( ptPos[0] );
	p_stream += cuT( "," );
	p_stream += string::to_string( ptPos[1] );
	p_stream += cuT( "," );
	p_stream += string::to_string( ptPos[2] );
	p_stream += cuT( ")" );
	return p_stream;
}
