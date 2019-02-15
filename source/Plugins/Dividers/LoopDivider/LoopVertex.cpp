#include "LoopVertex.hpp"

using namespace castor;

namespace Loop
{
	Vertex::Vertex( castor3d::SubmeshVertex vertex )
		: m_vertex{ std::move( vertex ) }
	{
	}

	bool Vertex::hasEdge( uint32_t p_index )
	{
		return m_edges.find( p_index ) != m_edges.end();
	}

	EdgeSPtr Vertex::getEdge( uint32_t p_index )
	{
		EdgeSPtr result;
		auto it = m_edges.find( p_index );

		if ( it != m_edges.end() )
		{
			result = it->second;
		}

		return result;
	}

	void Vertex::addEdge( EdgeSPtr p_pEdge, uint32_t p_index )
	{
		auto it = m_edges.find( p_index );

		if ( it == m_edges.end() )
		{
			m_edges.insert( std::make_pair( p_index, p_pEdge ) );
		}
	}

	void Vertex::removeEdge( uint32_t p_index )
	{
		auto it = m_edges.find( p_index );

		if ( it != m_edges.end() )
		{
			m_edges.erase( it );
		}
	}
}
