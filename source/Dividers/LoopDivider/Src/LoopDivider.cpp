#include "LoopDivider.hpp"

#include "LoopVertex.hpp"
#include "LoopFaceEdges.hpp"

#include <Point.hpp>
#include <Angle.hpp>

using namespace Castor;

namespace Loop
{
	namespace
	{
		double alpha( uint32_t n )
		{
			double l_tmp = 3.0 + 2.0 * cos( 2.0 * Angle::Pi / n );
			double l_beta = 1.25 - ( l_tmp * l_tmp ) / 32.0;
			return n * ( 1 - l_beta ) / l_beta;
		}
	}

	Subdivider::Subdivider()
		:	Castor3D::Subdivider()
	{
	}

	Subdivider::~Subdivider()
	{
		Cleanup();
	}

	void Subdivider::DoInitialise()
	{
		Castor3D::Subdivider::DoInitialise();

		for ( uint32_t i = 0; i < GetNbPoints(); i++ )
		{
			m_mapVertex.insert( std::make_pair( i, std::make_shared< Vertex >( GetPoint( i ) ) ) );
		}

		for ( uint32_t j = 0; j < m_submesh->GetFaceCount(); j++ )
		{
			m_facesEdges.push_back( std::make_shared< FaceEdges >( this, m_submesh->GetFace( j ), m_mapVertex ) );
		}

		m_submesh->ClearFaces();
	}

	void Subdivider::Cleanup()
	{
		Castor3D::Subdivider::Cleanup();
		m_facesEdges.clear();
		m_mapVertex.clear();
	}

	VertexSPtr Subdivider::AddPoint( real x, real y, real z )
	{
		VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( x, y, z ) );
		m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
		return l_pReturn;
	}

	VertexSPtr Subdivider::AddPoint( Point3r const & p_v )
	{
		VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
		return l_pReturn;
	}

	VertexSPtr Subdivider::AddPoint( real * p_v )
	{
		VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
		return l_pReturn;
	}

	void Subdivider::DoSubdivide()
	{
		DoDivide();
		DoAverage();
	}

	void Subdivider::DoDivide()
	{
		uint32_t l_size = uint32_t( m_facesEdges.size() );
		FaceEdgesPtrArray l_old;
		std::swap( l_old, m_facesEdges );

		for ( FaceEdgesPtrArray::iterator l_it = l_old.begin(); l_it != l_old.end(); ++l_it )
		{
			( *l_it )->Divide( real( 0.5 ), m_mapVertex, m_facesEdges );
		}
	}

	void Subdivider::DoAverage()
	{
		std::map< uint32_t, Point3r > l_positions;

		for ( VertexPtrUIntMap::iterator l_it = m_mapVertex.begin(); l_it != m_mapVertex.end(); ++l_it )
		{
			Point3r l_point;
			Castor3D::Vertex::GetPosition( l_it->second->GetPoint(), l_point );
			l_positions.insert( std::make_pair( l_it->first, l_point ) );
		}

		for ( VertexPtrUIntMap::iterator l_itVertex = m_mapVertex.begin(); l_itVertex != m_mapVertex.end(); ++l_itVertex )
		{
			VertexSPtr l_pVertex = l_itVertex->second;
			uint32_t l_nbEdges = l_pVertex->Size();
			Coords3r l_position = Castor3D::Vertex::GetPosition( l_pVertex->GetPoint(), l_position );
			real l_alpha = real( alpha( l_nbEdges ) );
			l_position *= l_alpha;

			for ( EdgePtrUIntMap::iterator l_it = l_pVertex->Begin(); l_it != l_pVertex->End(); ++l_it )
			{
				l_position += l_positions[l_it->first];
			}

			l_position /= l_alpha + l_nbEdges;
		}

		for ( uint32_t j = 0; j < m_submesh->GetFaceCount(); j++ )
		{
			Coords3r l_dump;
			Castor3D::FaceSPtr l_pFace = m_submesh->GetFace( j );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 0 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 0 )]->GetPoint(), l_dump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 1 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 1 )]->GetPoint(), l_dump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 2 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 2 )]->GetPoint(), l_dump ) );
		}

		m_mapVertex.clear();
	}
}
