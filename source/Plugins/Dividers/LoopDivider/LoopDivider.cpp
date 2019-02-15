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
			double tmp = 3.0 + 2.0 * cos( 2.0 * Pi< float > / n );
			double beta = 1.25 - ( tmp * tmp ) / 32.0;
			return n * ( 1 - beta ) / beta;
		}
	}

	String const Subdivider::Name = cuT( "Loop Divider" );
	String const Subdivider::Type = cuT( "loop" );

	Subdivider::Subdivider()
		: castor3d::Subdivider()
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
		m_vertices.clear();
	}

	VertexSPtr Subdivider::addPoint( real x, real y, real z )
	{
		VertexSPtr result = std::make_shared< Vertex >( castor3d::Subdivider::addPoint( x, y, z ) );
		m_vertices.emplace( result->getIndex(), result );
		return result;
	}

	VertexSPtr Subdivider::addPoint( Point3r const & v )
	{
		VertexSPtr result = std::make_shared< Vertex >( castor3d::Subdivider::addPoint( v ) );
		m_vertices.emplace( result->getIndex(), result );
		return result;
	}

	VertexSPtr Subdivider::addPoint( real * v )
	{
		VertexSPtr result = std::make_shared< Vertex >( castor3d::Subdivider::addPoint( v ) );
		m_vertices.emplace( result->getIndex(), result );
		return result;
	}

	void Subdivider::doInitialise()
	{
		castor3d::Subdivider::doInitialise();
		m_indexMapping = m_submesh->getComponent< castor3d::TriFaceMapping >();
		uint32_t index = 0;

		for ( auto & point : getPoints() )
		{
			m_vertices.emplace( index, std::make_shared< Vertex >( castor3d::SubmeshVertex{ index, point } ) );
			++index;
		}

		for ( auto & face : m_indexMapping->getFaces() )
		{
			m_facesEdges.emplace_back( std::make_shared< FaceEdges >( this, face, m_vertices ) );
		}

		m_indexMapping->clearFaces();
	}

	void Subdivider::doAddGeneratedFaces()
	{
		for ( auto const & face : m_arrayFaces )
		{
			m_indexMapping->addFace( face[0], face[1], face[2] );
		}
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
			faceEdges->divide( real( 0.5 ), m_vertices, m_facesEdges );
		}
	}

	void Subdivider::doAverage()
	{
		std::map< uint32_t, Point3r > positions;

		for ( auto & it : m_vertices )
		{
			Point3r point = it.second->getPoint().pos;
			positions.emplace( it.first, point );
		}

		for ( auto & it : m_vertices )
		{
			VertexSPtr vertex = it.second;
			uint32_t nbEdges = vertex->size();
			Point3r & position = vertex->getPoint().pos;
			real alpha = real( getAlpha( nbEdges ) );
			position *= alpha;

			for ( auto & itI : *vertex )
			{
				position += positions[itI.first];
			}

			position /= alpha + nbEdges;
		}

		for ( auto & face : m_arrayFaces )
		{
			Coords3r dump;
			m_submesh->getPoint( face[0] ).pos = m_vertices[face[0]]->getPoint().pos;
			m_submesh->getPoint( face[1] ).pos = m_vertices[face[1]]->getPoint().pos;
			m_submesh->getPoint( face[2] ).pos = m_vertices[face[2]]->getPoint().pos;
		}

		m_vertices.clear();
	}
}
