#include "LoopVertex.hpp"

using namespace Castor;

namespace Loop
{
	Vertex::Vertex( Castor3D::BufferElementGroupSPtr p_ptPoint )
		:	m_ptPoint( p_ptPoint	)
	{
	}

	Vertex::~Vertex()
	{
		m_mapEdges.clear();
	}

	bool Vertex::HasEdge( uint32_t p_index )
	{
		return m_mapEdges.find( p_index ) != m_mapEdges.end();
	}

	EdgeSPtr Vertex::GetEdge( uint32_t p_index )
	{
		EdgeSPtr l_pReturn;
		EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_index );

		if ( l_it != m_mapEdges.end() )
		{
			l_pReturn = l_it->second;
		}

		return l_pReturn;
	}

	void Vertex::AddEdge( EdgeSPtr p_pEdge, uint32_t p_index )
	{
		EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_index );

		if ( l_it == m_mapEdges.end() )
		{
			m_mapEdges.insert( std::make_pair( p_index, p_pEdge ) );
		}
	}

	void Vertex::RemoveEdge( uint32_t p_index )
	{
		EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_index );

		if ( l_it != m_mapEdges.end() )
		{
			m_mapEdges.erase( l_it );
		}
	}
}
