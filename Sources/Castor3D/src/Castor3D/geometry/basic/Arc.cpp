/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Arc.h - Arcf.cpp

 Desc:   Classe gérant les arcs ; une liste de points

*******************************************************************************/
#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/basic/Arc.h"

#include "main/Root.h"
#include "render_system/RenderSystem.h"

using namespace Castor3D;

Arc :: Arc ()
{
}

Arc :: ~Arc ()
{
	m_vertex.clear();
}

void Arc :: Display( DrawType p_displayMode)
{
	Root::GetRenderSystem()->DisplayArc( p_displayMode, m_vertex);
}

Castor3D::Arc * Castor3D::Arc :: GetReversed()
{
	Arc * l_result = new Arc();

	Vector3fPtrList::iterator l_it = m_vertex.begin();
	for (l_it = m_vertex.begin() ; l_it != m_vertex.end() ; l_it++)
	{
		l_result->m_vertex.push_front( *l_it);
	}

	return l_result;
}

void Arc :: AddVertex( Vector3f * p_vertex, int p_index)
{
	if ((unsigned)p_index >= m_vertex.size() || p_index < 0)
	{
		m_vertex.push_back( p_vertex);
		return;
	}

	Vector3fPtrList::iterator l_it = m_vertex.begin();

	for (int i = 0 ; i < p_index ; i++)
	{
		l_it++;
	}
	m_vertex.insert( l_it, p_vertex);

}

bool Arc :: IsClosed()
{
	return **m_vertex.begin() == **m_vertex.rbegin();
}

Vector3f * Arc :: GetVertex( unsigned int p_index)
{
	if( p_index >= m_vertex.size())
	{
		return NULL;
	}

	Vector3fPtrList::iterator l_it = m_vertex.begin();
	for (size_t i = 0 ; i < p_index ; i++)
	{
		l_it++;
	}

	return *l_it;
}