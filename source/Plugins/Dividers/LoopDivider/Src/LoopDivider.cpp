#include "LoopDivider.hpp"

#include "LoopVertex.hpp"
#include "LoopFaceEdges.hpp"

#include <Math/Point.hpp>
#include <Math/Angle.hpp>

using namespace castor;

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
		:	castor3d::Subdivider()
	{
	}

	Subdivider::~Subdivider()
	{
		cleanup();
	}

	castor3d::SubdividerUPtr Subdivider::create()
	{
		return std::make_unique< Subdivider >();
	}

	void Subdivider::cleanup()
	{
		castor3d::Subdivider::cleanup();
		m_facesEdges.clear();
		m_mapVertex.clear();
	}

	VertexSPtr Subdivider::addPoint( real x, real y, real z )
	{
		VertexSPtr result = std::make_shared< Vertex >( castor3d::Subdivider::addPoint( x, y, z ) );
		m_mapVertex.insert( std::make_pair( result->getIndex(), result ) );
		return result;
	}

	VertexSPtr Subdivider::addPoint( Point3r const & p_v )
	{
		VertexSPtr result = std::make_shared< Vertex >( castor3d::Subdivider::addPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( result->getIndex(), result ) );
		return result;
	}

	VertexSPtr Subdivider::addPoint( real * p_v )
	{
		VertexSPtr result = std::make_shared< Vertex >( castor3d::Subdivider::addPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( result->getIndex(), result ) );
		return result;
	}

	void Subdivider::doInitialise()
	{
		castor3d::Subdivider::doInitialise();
		uint32_t index = 0;

		for ( auto & point : getPoints() )
		{
			m_mapVertex.insert( std::make_pair( index++, std::make_shared< Vertex >( point ) ) );
		}

		for ( auto & face : m_submesh->getFaces() )
		{
			m_facesEdges.push_back( std::make_shared< FaceEdges >( this, face, m_mapVertex ) );
		}

		m_submesh->clearFaces();
	}

	void Subdivider::doSubdivide()
	{
		doDivide();
		doAverage();
	}

	void Subdivider::doDivide()
	{
		FaceEdgesPtrArray old;
		std::swap( old, m_facesEdges );

		for ( auto & faceEdges : old )
		{
			faceEdges->divide( real( 0.5 ), m_mapVertex, m_facesEdges );
		}
	}

	void Subdivider::doAverage()
	{
		std::map< uint32_t, Point3r > positions;

		for ( auto & it : m_mapVertex )
		{
			Point3r point;
			castor3d::Vertex::getPosition( it.second->getPoint(), point );
			positions.insert( std::make_pair( it.first, point ) );
		}

		for ( auto & it : m_mapVertex )
		{
			VertexSPtr vertex = it.second;
			uint32_t nbEdges = vertex->size();
			Coords3r position;
			castor3d::Vertex::getPosition( vertex->getPoint(), position );
			real alpha = real( getAlpha( nbEdges ) );
			position *= alpha;

			for ( auto & itI : *vertex )
			{
				position += positions[itI.first];
			}

			position /= alpha + nbEdges;
		}

		for ( auto & face : m_submesh->getFaces() )
		{
			Coords3r dump;
			castor3d::Vertex::setPosition( m_submesh->getPoint( face[0] ), castor3d::Vertex::getPosition( m_mapVertex[face[0]]->getPoint(), dump ) );
			castor3d::Vertex::setPosition( m_submesh->getPoint( face[1] ), castor3d::Vertex::getPosition( m_mapVertex[face[1]]->getPoint(), dump ) );
			castor3d::Vertex::setPosition( m_submesh->getPoint( face[2] ), castor3d::Vertex::getPosition( m_mapVertex[face[2]]->getPoint(), dump ) );
		}

		m_mapVertex.clear();
	}
}
