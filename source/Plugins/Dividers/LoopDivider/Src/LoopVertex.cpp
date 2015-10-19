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

	bool Vertex::HasEdge( uint32_t p_uiIndex )
	{
		return m_mapEdges.find( p_uiIndex ) != m_mapEdges.end();
	}

	EdgeSPtr Vertex::GetEdge( uint32_t p_uiIndex )
	{
		EdgeSPtr l_return;
		EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex );

		if ( l_it != m_mapEdges.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void Vertex::AddEdge( EdgeSPtr p_pEdge, uint32_t p_uiIndex )
	{
		EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex );

		if ( l_it == m_mapEdges.end() )
		{
			m_mapEdges.insert( std::make_pair( p_uiIndex, p_pEdge ) );
		}
	}

	void Vertex::RemoveEdge( uint32_t p_uiIndex )
	{
		EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex );

		if ( l_it != m_mapEdges.end() )
		{
			m_mapEdges.erase( l_it );
		}
	}
}
