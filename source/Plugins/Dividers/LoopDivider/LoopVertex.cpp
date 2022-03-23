#include "LoopDivider/LoopVertex.hpp"

namespace Loop
{
	Vertex::Vertex( castor3d::SubmeshVertex vertex )
		: m_vertex{ std::move( vertex ) }
	{
	}

	bool Vertex::hasEdge( uint32_t index )
	{
		return m_edges.find( index ) != m_edges.end();
	}

	EdgeSPtr Vertex::getEdge( uint32_t index )
	{
		EdgeSPtr result;
		auto it = m_edges.find( index );

		if ( it != m_edges.end() )
		{
			result = it->second;
		}

		return result;
	}

	void Vertex::addEdge( EdgeSPtr edge, uint32_t index )
	{
		auto it = m_edges.find( index );

		if ( it == m_edges.end() )
		{
			m_edges.insert( std::make_pair( index, edge ) );
		}
	}

	void Vertex::removeEdge( uint32_t index )
	{
		auto it = m_edges.find( index );

		if ( it != m_edges.end() )
		{
			m_edges.erase( it );
		}
	}
}
