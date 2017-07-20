#include "LoopVertex.hpp"

using namespace Castor;

namespace Loop
{
	Vertex::Vertex( Castor3D::BufferElementGroupSPtr p_point )
		:	m_ptPoint( p_point	)
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
		EdgeSPtr result;
		EdgePtrUIntMap::iterator it = m_mapEdges.find( p_index );

		if ( it != m_mapEdges.end() )
		{
			result = it->second;
		}

		return result;
	}

	void Vertex::AddEdge( EdgeSPtr p_pEdge, uint32_t p_index )
	{
		EdgePtrUIntMap::iterator it = m_mapEdges.find( p_index );

		if ( it == m_mapEdges.end() )
		{
			m_mapEdges.insert( std::make_pair( p_index, p_pEdge ) );
		}
	}

	void Vertex::RemoveEdge( uint32_t p_index )
	{
		EdgePtrUIntMap::iterator it = m_mapEdges.find( p_index );

		if ( it != m_mapEdges.end() )
		{
			m_mapEdges.erase( it );
		}
	}
}
