#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/Pattern.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"

using namespace Castor3D;

Pattern :: Pattern ()
{
}

Pattern :: ~Pattern ()
{
	m_vertex.clear();
}

void Pattern :: Display( eDRAW_TYPE p_displayMode)
{
	RenderSystem::GetSingletonPtr<RenderSystem>()->DisplayArc( p_displayMode, m_vertex);
}

PatternPtr Pattern :: GetReversed()
{
	PatternPtr l_result = new Pattern();
	VertexPtrList::iterator l_it = m_vertex.begin();

	for (l_it = m_vertex.begin() ; l_it != m_vertex.end() ; l_it++)
	{
		l_result->m_vertex.push_front( *l_it);
	}

	return l_result;
}

void Pattern :: AddVertex( VertexPtr p_vertex, size_t p_index)
{
	if (p_index >= m_vertex.size())
	{
		m_vertex.push_back( p_vertex);
		return;
	}

	VertexPtrList::iterator l_it = m_vertex.begin();

	for (size_t i = 0 ; i < p_index ; i++)
	{
		l_it++;
	}
	m_vertex.insert( l_it, p_vertex);

}

bool Pattern :: IsClosed()
{
	Vertex l_v1, l_v2;
	l_v1 = **m_vertex.begin();
	l_v2 = **m_vertex.rbegin();
	return l_v1 == l_v2;
}

VertexPtr Pattern :: GetVertex( size_t p_index)const
{
	if( p_index >= m_vertex.size())
	{
		return NULL;
	}

	VertexPtrList::const_iterator l_it = m_vertex.begin();

	for (size_t i = 0 ; i < p_index ; i++)
	{
		l_it++;
	}

	return *l_it;
}