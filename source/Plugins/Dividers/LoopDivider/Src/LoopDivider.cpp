#include "LoopDivider.hpp"

#include "LoopVertex.hpp"
#include "LoopFaceEdges.hpp"

#include <Math/Point.hpp>
#include <Math/Angle.hpp>

using namespace Castor;

namespace Loop
{
	namespace
	{
		double getAlpha( uint32_t n )
		{
			double tmp = 3.0 + 2.0 * cos( 2.0 * Angle::Pi / n );
			double beta = 1.25 - ( tmp * tmp ) / 32.0;
			return n * ( 1 - beta ) / beta;
		}
	}

	String const Subdivider::Name = cuT( "Loop Divider" );
	String const Subdivider::Type = cuT( "loop" );

	Subdivider::Subdivider()
		:	Castor3D::Subdivider()
	{
	}

	Subdivider::~Subdivider()
	{
		Cleanup();
	}

	Castor3D::SubdividerUPtr Subdivider::Create()
	{
		return std::make_unique< Subdivider >();
	}

	void Subdivider::Cleanup()
	{
		Castor3D::Subdivider::Cleanup();
		m_facesEdges.clear();
		m_mapVertex.clear();
	}

	VertexSPtr Subdivider::AddPoint( real x, real y, real z )
	{
		VertexSPtr result = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( x, y, z ) );
		m_mapVertex.insert( std::make_pair( result->GetIndex(), result ) );
		return result;
	}

	VertexSPtr Subdivider::AddPoint( Point3r const & p_v )
	{
		VertexSPtr result = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( result->GetIndex(), result ) );
		return result;
	}

	VertexSPtr Subdivider::AddPoint( real * p_v )
	{
		VertexSPtr result = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( result->GetIndex(), result ) );
		return result;
	}

	void Subdivider::DoInitialise()
	{
		Castor3D::Subdivider::DoInitialise();
		uint32_t index = 0;

		for ( auto & point : GetPoints() )
		{
			m_mapVertex.insert( std::make_pair( index++, std::make_shared< Vertex >( point ) ) );
		}

		for ( auto & face : m_submesh->GetFaces() )
		{
			m_facesEdges.push_back( std::make_shared< FaceEdges >( this, face, m_mapVertex ) );
		}

		m_submesh->ClearFaces();
	}

	void Subdivider::DoSubdivide()
	{
		DoDivide();
		DoAverage();
	}

	void Subdivider::DoDivide()
	{
		uint32_t size = uint32_t( m_facesEdges.size() );
		FaceEdgesPtrArray old;
		std::swap( old, m_facesEdges );

		for ( auto & faceEdges : old )
		{
			faceEdges->Divide( real( 0.5 ), m_mapVertex, m_facesEdges );
		}
	}

	void Subdivider::DoAverage()
	{
		std::map< uint32_t, Point3r > positions;

		for ( auto & it : m_mapVertex )
		{
			Point3r point;
			Castor3D::Vertex::GetPosition( it.second->GetPoint(), point );
			positions.insert( std::make_pair( it.first, point ) );
		}

		for ( auto & it : m_mapVertex )
		{
			VertexSPtr vertex = it.second;
			uint32_t nbEdges = vertex->size();
			Coords3r position;
			Castor3D::Vertex::GetPosition( vertex->GetPoint(), position );
			real alpha = real( getAlpha( nbEdges ) );
			position *= alpha;

			for ( auto & itI : *vertex )
			{
				position += positions[itI.first];
			}

			position /= alpha + nbEdges;
		}

		for ( auto & face : m_submesh->GetFaces() )
		{
			Coords3r dump;
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( face[0] ), Castor3D::Vertex::GetPosition( m_mapVertex[face[0]]->GetPoint(), dump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( face[1] ), Castor3D::Vertex::GetPosition( m_mapVertex[face[1]]->GetPoint(), dump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( face[2] ), Castor3D::Vertex::GetPosition( m_mapVertex[face[2]]->GetPoint(), dump ) );
		}

		m_mapVertex.clear();
	}
}
