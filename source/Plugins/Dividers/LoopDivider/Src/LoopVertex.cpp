#include "LoopVertex.hpp"

using namespace castor;

namespace Loop
{
	Vertex::Vertex( castor3d::BufferElementGroupSPtr p_point )
		:	m_ptPoint( p_point	)
	{
	}

	Vertex::~Vertex()
	{
		m_mapEdges.clear();
	}

	bool Vertex::hasEdge( uint32_t p_index )
	{
		return m_mapEdges.find( p_index ) != m_mapEdges.end();
	}

	EdgeSPtr Vertex::getEdge( uint32_t p_index )
	{
		EdgeSPtr result;
		EdgePtrUIntMap::iterator it = m_mapEdges.find( p_index );

		if ( it != m_mapEdges.end() )
		{
			result = it->second;
		}

		return result;
	}

	void Vertex::addEdge( EdgeSPtr p_pEdge, uint32_t p_index )
	{
		EdgePtrUIntMap::iterator it = m_mapEdges.find( p_index );

		if ( it == m_mapEdges.end() )
		{
			m_mapEdges.insert( std::make_pair( p_index, p_pEdge ) );
		}
	}

	void Vertex::removeEdge( uint32_t p_index )
	{
		EdgePtrUIntMap::iterator it = m_mapEdges.find( p_index );

		if ( it != m_mapEdges.end() )
		{
			m_mapEdges.erase( it );
		}
	}
}
