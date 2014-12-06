#include "Subdivider.hpp"
#include "SubdivisionFrameEvent.hpp"
#include "Submesh.hpp"
#include "Face.hpp"
#include "Vertex.hpp"

#include <Line3D.hpp>
#include <PlaneEquation.hpp>

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

	void Subdivider::Subdivide( SubmeshSPtr p_pSubmesh, Point3r * p_pCenter, bool p_bGenerateBuffers, bool p_bThreaded )
	{
		m_submesh = p_pSubmesh;
		m_bGenerateBuffers = p_bGenerateBuffers;

		if ( !p_pCenter )
		{
			m_submesh->ComputeContainers();
			m_ptDivisionCenter = m_submesh->GetCubeBox().GetCenter();
		}
		else
		{
			m_ptDivisionCenter = *p_pCenter;
		}

		DoInitialise();
		m_bThreaded = p_bThreaded;

		if ( p_bThreaded )
		{
			m_pThread = std::make_shared< std::thread >( DoSubdivideThreaded, this );
		}
		else
		{
			DoSubdivide();
			DoSwapBuffers();
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

	FaceSPtr Subdivider::AddFace( uint32_t a, uint32_t b, uint32_t c )
	{
		CASTOR_ASSERT( a < GetNbPoints() && b < GetNbPoints() && c < GetNbPoints() );
		FaceSPtr l_pReturn = std::make_shared< Face >( a, b, c );
		m_arrayFaces.push_back( l_pReturn );
		return l_pReturn;
	}

	int Subdivider::IsInMyPoints( Point3r const & p_vertex )
	{
		return m_submesh->IsInMyPoints( p_vertex );
	}

	uint32_t Subdivider::GetNbPoints()const
	{
		return m_submesh->GetPointsCount();
	}

	BufferElementGroupSPtr Subdivider::GetPoint( uint32_t i )const
	{
		return m_submesh->GetPoint( i );
	}

	void Subdivider::DoInitialise()
	{
		if ( m_bGenerateBuffers )
		{
			m_submesh->ResetGpuBuffers();
		}
	}

	void Subdivider::DoSwapBuffers()
	{
		FaceSPtr l_pFace;

		for ( FacePtrArray::iterator l_it = m_arrayFaces.begin(); l_it != m_arrayFaces.end(); ++l_it )
		{
			l_pFace = *l_it;
			m_submesh->AddFace( l_pFace->GetVertexIndex( 0 ), l_pFace->GetVertexIndex( 1 ), l_pFace->GetVertexIndex( 2 ) );
		}

		m_submesh->ComputeNormals( true );

		if ( m_bGenerateBuffers && !m_bThreaded )
		{
			m_submesh->GenerateBuffers();
			m_submesh->GetRenderer()->Initialise();
		}

		Cleanup();
	}

	uint32_t Subdivider::DoSubdivideThreaded( Subdivider * p_pThis )
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( p_pThis->m_mutex );
		p_pThis->DoSubdivide();
		p_pThis->DoSwapBuffers();

		if ( p_pThis->m_bGenerateBuffers )
		{
			p_pThis->m_submesh->GetEngine()->PostEvent( std::make_shared< SubdivisionFrameEvent >( p_pThis->m_submesh ) );
		}

		if ( p_pThis->m_pfnSubdivisionEnd )
		{
			p_pThis->m_pfnSubdivisionEnd( p_pThis->m_pArg, p_pThis );
		}

		return 0;
	}

	void Subdivider::DoSetTextCoords( FaceSPtr p_face, BufferElementGroup const & p_a, BufferElementGroup const & p_b, BufferElementGroup const & p_c, BufferElementGroup & p_d, BufferElementGroup & p_e, BufferElementGroup & p_f )
	{
		Point3r l_aTex;
		Point3r l_bTex;
		Point3r l_cTex;
		Vertex::GetTexCoord( p_a, l_aTex );
		Vertex::GetTexCoord( p_b, l_bTex );
		Vertex::GetTexCoord( p_c, l_cTex );
		Vertex::SetTexCoord( p_d, l_aTex + ( l_bTex - l_aTex ) / real( 2.0 ) );
		Vertex::SetTexCoord( p_e, l_bTex + ( l_cTex - l_bTex ) / real( 2.0 ) );
		Vertex::SetTexCoord( p_f, l_aTex + ( l_cTex - l_aTex ) / real( 2.0 ) );
		AddFace( p_a.GetIndex(), p_d.GetIndex(), p_f.GetIndex() );
		AddFace( p_b.GetIndex(), p_e.GetIndex(), p_d.GetIndex() );
		AddFace( p_c.GetIndex(), p_f.GetIndex(), p_e.GetIndex() );
		AddFace( p_d.GetIndex(), p_e.GetIndex(), p_f.GetIndex() );
	}

	void Subdivider::DoComputeCenterFrom( Point3r const & p_a, Point3r const & p_b, Point3r const & p_ptANormal, Point3r const & p_ptBNormal, Point3r & p_ptResult )
	{
		/**/
		Line3D< real > l_aEq = Line3D< real >::FromPointAndSlope( p_a, p_ptANormal );
		Line3D< real > l_bEq = Line3D< real >::FromPointAndSlope( p_b, p_ptBNormal );
		l_aEq.Intersects( l_bEq, p_ptResult );
		/**/
		/*
			// Projection sur XY => R�cup�ration du point d'intersection (X, Y) y = ax + b
			Line3D< real > l_aEq = Line3D< real >::FromPointAndSlope( p_a, p_ptANormal );
			Line3D< real > l_bEq = Line3D< real >::FromPointAndSlope( p_b, p_ptBNormal );
			Point3r l_pt1;

			if( l_aEq.Intersects( l_bEq, l_pt1 ) )
			{
				// Projection sur XZ => R�cup�ration du point d'intersection (X, Z) z = ax + b
				l_aEq = Line3D< real >::FromPointAndSlope( p_a, p_ptANormal );
				l_bEq = Line3D< real >::FromPointAndSlope( p_b, p_ptBNormal );
				Point3r l_pt2;

				if( l_aEq.Intersects( l_bEq, l_pt2 ) )
				{
					p_ptResult[0] = l_pt1[0] + (l_pt2[0] - l_pt1[0]) / 2.0f;
					p_ptResult[1] = l_pt1[1] + (l_pt2[1] - l_pt1[1]) / 2.0f;
					p_ptResult[2] = l_pt1[2] + (l_pt2[2] - l_pt1[2]) / 2.0f;
				}
			}
		/**/
	}

	void Subdivider::DoComputeCenterFrom( Point3r const & p_a, Point3r const & p_b, Point3r const & p_c, Point3r const & p_ptANormal, Point3r const & p_ptBNormal, Point3r const & p_ptCNormal, Point3r & p_ptResult )
	{
		PlaneEquation<real> ABO( p_a, p_ptANormal, p_ptBNormal );
		PlaneEquation<real> BCO( p_b, p_ptBNormal, p_ptCNormal );
		PlaneEquation<real> CAO( p_c, p_ptCNormal, p_ptANormal );
		ABO.Intersects( BCO, CAO, p_ptResult );
	}
}

String & operator << ( String & p_stream, Castor3D::BufferElementGroup const & p_vertex )
{
	Point3r l_ptPos( reinterpret_cast< real const * >( p_vertex.const_ptr() ) );
	p_stream += cuT( "Vertex[" );
	p_stream += str_utils::to_string( p_vertex.GetIndex() );
	p_stream += cuT( "] - Buffer : [" );
	p_stream += str_utils::to_string( l_ptPos.const_ptr() );
	p_stream += cuT( "] - (" );
	p_stream += str_utils::to_string( l_ptPos[0] );
	p_stream += cuT( "," );
	p_stream += str_utils::to_string( l_ptPos[1] );
	p_stream += cuT( "," );
	p_stream += str_utils::to_string( l_ptPos[2] );
	p_stream += cuT( ")" );
	return p_stream;
}
