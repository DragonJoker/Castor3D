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
		uint32_t l_index = 0;

		for ( auto & l_point : GetPoints() )
		{
			m_mapVertex.insert( std::make_pair( l_index++, std::make_shared< Vertex >( l_point ) ) );
		}

		for ( auto & l_face : m_submesh->GetFaces() )
		{
			m_facesEdges.push_back( std::make_shared< FaceEdges >( this, l_face, m_mapVertex ) );
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
		VertexSPtr l_return = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( x, y, z ) );
		m_mapVertex.insert( std::make_pair( l_return->GetIndex(), l_return ) );
		return l_return;
	}

	VertexSPtr Subdivider::AddPoint( Point3r const & p_v )
	{
		VertexSPtr l_return = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( l_return->GetIndex(), l_return ) );
		return l_return;
	}

	VertexSPtr Subdivider::AddPoint( real * p_v )
	{
		VertexSPtr l_return = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( l_return->GetIndex(), l_return ) );
		return l_return;
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

		for ( auto & l_faceEdges : l_old )
		{
			l_faceEdges->Divide( real( 0.5 ), m_mapVertex, m_facesEdges );
		}
	}

	void Subdivider::DoAverage()
	{
		std::map< uint32_t, Point3r > l_positions;

		for ( auto & l_it : m_mapVertex )
		{
			Point3r l_point;
			Castor3D::Vertex::GetPosition( l_it.second->GetPoint(), l_point );
			l_positions.insert( std::make_pair( l_it.first, l_point ) );
		}

		for ( auto & l_it : m_mapVertex )
		{
			VertexSPtr l_vertex = l_it.second;
			uint32_t l_nbEdges = l_vertex->size();
			Coords3r l_position = Castor3D::Vertex::GetPosition( l_vertex->GetPoint(), l_position );
			real l_alpha = real( alpha( l_nbEdges ) );
			l_position *= l_alpha;

			for ( auto & l_it : *l_vertex )
			{
				l_position += l_positions[l_it.first];
			}

			l_position /= l_alpha + l_nbEdges;
		}

		for ( auto l_face : m_submesh->GetFaces() )
		{
			Coords3r l_dump;
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_face[0] ), Castor3D::Vertex::GetPosition( m_mapVertex[l_face[0]]->GetPoint(), l_dump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_face[1] ), Castor3D::Vertex::GetPosition( m_mapVertex[l_face[1]]->GetPoint(), l_dump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_face[2] ), Castor3D::Vertex::GetPosition( m_mapVertex[l_face[2]]->GetPoint(), l_dump ) );
		}

		m_mapVertex.clear();
	}
}
