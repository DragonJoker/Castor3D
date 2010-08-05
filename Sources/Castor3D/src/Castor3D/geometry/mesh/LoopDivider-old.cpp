#include "PrecompiledHeader.h"

#include "geometry/mesh/LoopDivider.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"

using namespace Castor3D;

//*********************************************************************************************

#define ALPHA_MAX 20
#define ALPHA_LIMIT 0.469 /* converges to ~ 0.469 */

float ALPHA[] = {	1.13333f, -0.358974f, -0.333333f, 0.129032f, 0.945783f, 2.0f,
					3.19889f, 4.47885f, 5.79946f, 7.13634f, 8.47535f, 9.80865f,
					11.1322f, 12.4441f, 13.7439f, 15.0317f, 16.3082f, 17.574f,
					18.83f, 20.0769f };

#define BETA_MAX 20
#define BETA_LIMIT 0.469 /* converges to ~ 0.469 */

float BETA[] = {0.46875f, 1.21875f, 1.125f, 0.96875f, 0.840932f, 0.75f, 0.686349f,
				0.641085f, 0.60813f, 0.583555f, 0.564816f, 0.55024f, 0.5387f,
				0.529419f, 0.52185f, 0.515601f, 0.510385f, 0.505987f, 0.502247f, 0.49904f };

float beta( unsigned int n)
{
	return (5.0f/4.0f - (3.0f+2.0f*cos( M_PI_MULT_2 / n)) * (3.0f+2.0f*cos( M_PI_MULT_2 / n)) / 32.0f);
}

float alpha( unsigned int n)
{
	float b;

	if (n <= 20) return ALPHA[n-1];

	b = beta(n);

	return n * (1 - b) / b;
}

//*********************************************************************************************

LoopEdge :: LoopEdge( Vector3f * p_v1, Vector3f * p_v2, Face * p_f1)
	:	Edge( p_v1, p_v2, p_f1),
		m_divided( false),
		m_createdVertex( NULL)
{
}

Vector3f * LoopEdge :: Divide()
{
	if (m_divided)
	{
		return m_createdVertex;
	}

	m_createdVertex = new Vector3f( (*m_firstVertex + *m_secondVertex) / 2.0f);

	m_divided = true;
	return m_createdVertex;
}

//*********************************************************************************************

LoopEdgeList :: ~LoopEdgeList()
{
	multimap::deleteAll( m_edges);
}

void LoopEdgeList :: DivideFace( Submesh * p_submesh, Face * p_face, size_t p_sgIndex)
{
	Vector3f * l_ptA = p_face->m_vertex1;
	Vector3f * l_ptB = p_face->m_vertex2;
	Vector3f * l_ptC = p_face->m_vertex3;

	LoopEdge *l_edge12, *l_edge23, *l_edge31;

	bool l_1and2 = false;
	bool l_1and3 = false;
	bool l_2and3 = false;
	std::multimap <Vector3f *, LoopEdge *> ::iterator l_it = m_edges.find( l_ptA);
	if (l_it != m_edges.end())
	{
		while (l_it != m_edges.end() && l_it->first == l_ptA)// && ! (l_1and2 && l_1and3))
		{
			if (l_it->second->IsInVertex( l_ptB) && ! l_1and2)
			{
				l_1and2 = true;
				l_edge12 = l_it->second;
			}
			if (l_it->second->IsInVertex( l_ptC) && ! l_1and3)
			{
				l_1and3 = true;
				l_edge31 = l_it->second;
			}
			++l_it;
		}
	}
	l_it = m_edges.find( l_ptB);
	if (l_it != m_edges.end())
	{
		while (l_it != m_edges.end() && l_it->first == l_ptB)// && ! (l_1and2 && l_2and3))
		{
			if (l_it->second->IsInVertex( l_ptA) && ! l_1and2)
			{
				l_1and2 = true;
				l_edge12 = l_it->second;
			}
			if (l_it->second->IsInVertex( l_ptC) && ! l_2and3)
			{
				l_2and3 = true;
				l_edge23 = l_it->second;
			}
			++l_it;
		}
	}
	l_it = m_edges.find( l_ptC);
	if (l_it != m_edges.end())
	{
		while (l_it != m_edges.end() && l_it->first == l_ptC)// && ! (l_1and3 && l_2and3))
		{
			if (l_it->second->IsInVertex( l_ptA) && ! l_1and3)
			{
				l_1and3 = true;
				l_edge31 = l_it->second;
			}
			if (l_it->second->IsInVertex( l_ptB) && ! l_2and3)
			{
				l_2and3 = true;
				l_edge23 = l_it->second;
			}
			++l_it;
		}
	}

	if (l_1and2 && l_2and3 && l_1and3)
	{
		Vector3f * l_ptD;
		Vector3f * l_ptE;
		Vector3f * l_ptF;

		if (l_edge12->IsDivided())
		{
			l_ptD = l_edge12->Divide();
		}
		else
		{
			l_ptD = p_submesh->AddVertex( l_edge12->Divide());
		}

		if (l_edge23->IsDivided())
		{
			l_ptE = l_edge23->Divide();
		}
		else
		{
			l_ptE = p_submesh->AddVertex( l_edge23->Divide());
		}

		if (l_edge31->IsDivided())
		{
			l_ptF = l_edge31->Divide();
		}
		else
		{
			l_ptF = p_submesh->AddVertex( l_edge31->Divide());
		}

		Face * l_face = p_submesh->AddFace( l_ptA, l_ptD, l_ptF, p_sgIndex);
		AddFace( l_face);
		l_face = p_submesh->AddFace( l_ptB, l_ptE, l_ptD, p_sgIndex);
		AddFace( l_face);
		l_face = p_submesh->AddFace( l_ptC, l_ptF, l_ptE, p_sgIndex);
		AddFace( l_face);
		l_face = p_submesh->AddFace( l_ptD, l_ptE, l_ptF, p_sgIndex);
		AddFace( l_face);

	}
}

//*********************************************************************************************

LoopSubdivMask :: ~LoopSubdivMask()
{
	set::deleteAll( m_originalNeighbours);
}

bool LoopSubdivMask :: AddNeighbour( Vector3f * p_vertex)
{
	if (VertexNeighbours::AddNeighbour( p_vertex))
	{
		m_originalNeighbours.insert( std::set <Vector3f *>::value_type( new Vector3f( p_vertex->x, p_vertex->y, p_vertex->z, 0)));

		float l_nbNeighbours = (float)m_neighbours.size();
		m_factor = (1.0f / l_nbNeighbours)*(0.625f * (0.375f + 0.25f * cos( M_PI_MULT_2 / l_nbNeighbours))*(0.375f + 0.25f * cos( M_PI_MULT_2 / l_nbNeighbours)));
		return true;
	}
	return false;
}

void LoopSubdivMask :: ReplaceVertex( Vector3f * p_center)
{
	size_t l_size = m_neighbours.size();

	*m_vertex -= *p_center;

	*m_vertex *= alpha( l_size);
	Vector3f l_vertex;
	std::set<Vector3f *>::iterator l_it = m_originalNeighbours.begin();
	while (l_it != m_originalNeighbours.end())
	{
		*m_vertex += (*(*l_it) - *p_center);
		++l_it;
	}
	*m_vertex /= (alpha( l_size) + l_size);

	*m_vertex += *p_center;
}

//*********************************************************************************************

LoopSubdiviser :: ~LoopSubdiviser()
{
	if (m_edges != NULL)
	{
		delete m_edges;
	}
}

void LoopSubdiviser :: Subdivide( Vector3f * p_center)
{
	if (m_edges != NULL)
	{
		delete m_edges;
		m_edges = NULL;
	}
	m_edges = new LoopEdgeList();

	Vector3f l_vectorOA, l_vectorOB, l_vectorOC, l_vectorOD, l_vectorOE, l_vectorOF;
	Vector3f * l_ptA, * l_ptB, * l_ptC;
	Face * l_face;
	std::vector <FacefPtrArray> l_facesArrays;
	std::map <Vector3f *, LoopSubdivMask *> l_loopMasks;
	std::map <Vector3f *, LoopSubdivMask *>::iterator l_it;
	FacefPtrArray l_facesArray;
	LoopSubdivMask * l_mask;
	Vector3f * l_center = NULL;

	// Step 1 : Fill the edges
	for (size_t i = 0 ; i < m_vertex->size() ; i++)
	{
		l_loopMasks.insert( std::map <Vector3f *, LoopSubdivMask *>::value_type( (*m_vertex)[i], new LoopSubdivMask( (*m_vertex)[i])));
	}
	for (size_t j = 0 ; j < m_smoothGroups->size() ; j++)
	{
		FacefPtrArray l_facesArray;
		for (size_t i = 0 ; i < (*m_smoothGroups)[j]->m_faces.size() ; i++)
		{
			l_face = (*m_smoothGroups)[j]->m_faces[i];
			l_facesArray.push_back( l_face);

			m_edges->AddFace( l_face);
		}
		l_facesArrays.push_back( l_facesArray);
		(*m_smoothGroups)[j]->m_faces.clear();
	}

	// Step 2 : Create new vertices
	for (size_t i = 0 ; i < l_facesArrays.size() ; i++)
	{
		l_facesArray = l_facesArrays[i];
		for (size_t j = 0 ; j < l_facesArray.size() ; j++)
		{
			l_face = l_facesArray[j];
			m_edges->DivideFace( m_submesh, l_face, i);
		}
	}

	// Step 3 : Cleanup arrays
	for (size_t i = 0 ; i < l_facesArrays.size() ; i++)
	{
		l_facesArray = l_facesArrays[i];
		for (size_t j = 0 ; j < l_facesArray.size() ; j++)
		{
			delete l_facesArray[j];
		}
		l_facesArray.clear();
	}
	l_facesArrays.clear();

	// Step 3 : Fill the masks
	for (size_t i = 0 ; i < m_vertex->size() ; i++)
	{
		l_loopMasks.insert( std::map <Vector3f *, LoopSubdivMask *>::value_type( (*m_vertex)[i], new LoopSubdivMask( (*m_vertex)[i])));
	}
	for (size_t j = 0 ; j < m_smoothGroups->size() ; j++)
	{
		FacefPtrArray l_faceArray;
		for (size_t i = 0 ; i < (*m_smoothGroups)[j]->m_faces.size() ; i++)
		{
			l_face = (*m_smoothGroups)[j]->m_faces[i];
			l_faceArray.push_back( l_face);

			l_ptA = l_face->m_vertex1;
			l_ptB = l_face->m_vertex2;
			l_ptC = l_face->m_vertex3;

			l_mask = l_loopMasks.find( l_ptA)->second;
			l_mask->AddNeighbour( l_ptB);
			l_mask->AddNeighbour( l_ptC);

			l_mask = l_loopMasks.find( l_ptB)->second;
			l_mask->AddNeighbour( l_ptA);
			l_mask->AddNeighbour( l_ptC);

			l_mask = l_loopMasks.find( l_ptC)->second;
			l_mask->AddNeighbour( l_ptB);
			l_mask->AddNeighbour( l_ptA);
		}
		l_facesArrays.push_back( l_faceArray);
	}

	// Step 4 : Replace vertices
	for (size_t i = 0 ; i < l_facesArrays.size() ; i++)
	{
		l_facesArray = l_facesArrays[i];
		for (size_t j = 0 ; j < l_facesArray.size() ; j++)
		{
			l_face = l_facesArray[j];

			l_ptA = l_face->m_vertex1;
			l_ptB = l_face->m_vertex2;
			l_ptC = l_face->m_vertex3;

			if ((l_center = p_center) == NULL)
			{
				l_center = _computeCenterFrom( * l_ptA, l_face->m_vertex1Normal, * l_ptB, l_face->m_vertex2Normal, * l_ptC, l_face->m_vertex3Normal);
			}

			l_it = l_loopMasks.find( l_ptA);
			if (l_it != l_loopMasks.end())
			{
				l_it->second->ReplaceVertex( l_center);
				delete l_it->second;
				l_loopMasks.erase( l_it);
			}

			l_it = l_loopMasks.find( l_ptB);
			if (l_it != l_loopMasks.end())
			{
				l_it->second->ReplaceVertex( l_center);
				delete l_it->second;
				l_loopMasks.erase( l_it);
			}

			l_it = l_loopMasks.find( l_ptC);
			if (l_it != l_loopMasks.end())
			{
				l_it->second->ReplaceVertex( l_center);
				delete l_it->second;
				l_loopMasks.erase( l_it);
			}
		}
	}

	// Step 4 : Cleanup arrays
	for (size_t i = 0 ; i < l_facesArrays.size() ; i++)
	{
		l_facesArray = l_facesArrays[i];
		l_facesArray.clear();
	}
	l_facesArrays.clear();
}

//*********************************************************************************************