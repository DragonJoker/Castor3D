#ifndef ___C3D_Subdiviser___
#define ___C3D_Subdiviser___

#include "../Module_Geometry.h"
#include "../../math/Module_Math.h"
#include "../../math/Vector3f.h"

namespace Castor3D
{
	/*!
	An edge is made of two vertices and link two faces or less
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Edge
	{
	protected:
		Vector3f * m_firstVertex;
		Vector3f * m_secondVertex;
		Face * m_firstFace;
		Face * m_secondFace;

	public:
		Edge( Vector3f * p_v1, Vector3f * p_v2, Face * p_f1)
			:	m_firstVertex( p_v1),
				m_secondVertex( p_v2),
				m_firstFace( p_f1),
				m_secondFace( NULL)
		{
		}
		~Edge(){}

		void AddFace( Face * p_face)
		{
			if (m_secondFace != NULL)
			{
				return;
			}
			m_secondFace = p_face;
		}

		virtual Vector3f * Divide()=0;

		bool operator == ( const Edge & p_edge)
		{
			return m_firstVertex == p_edge.m_firstVertex && m_secondVertex == p_edge.m_secondVertex;
		}

		inline Vector3f * GetVertex1() { return m_firstVertex; }
		inline Vector3f * GetVertex2() { return m_secondVertex; }
		inline bool IsInVertex( Vector3f * p_vertex)const { return m_firstVertex == p_vertex || m_secondVertex == p_vertex; }
	};
	/*!
	The edge list allows us to add the edges of a given face to the current list
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	template <class T>
	class EdgeList
	{
	protected:
		std::multimap <Vector3f *, T *> m_edges;
		std::set <Face *> m_addedFaces;

	public:
		EdgeList(){}
		~EdgeList()
		{
			multimap::deleteAll( m_edges);
		}

		void AddFace( Face * p_face)
		{
			if (m_addedFaces.find( p_face) != m_addedFaces.end())
			{
				return;
			}
			m_addedFaces.insert( std::set <Face *>::value_type( p_face));

			Vector3f * l_vertex1 = p_face->m_vertex1;
			Vector3f * l_vertex2 = p_face->m_vertex2;
			Vector3f * l_vertex3 = p_face->m_vertex3;

			bool l_1and2 = false;
			bool l_1and3 = false;
			bool l_2and3 = false;
			T * l_edge12;
			T * l_edge23;
			T * l_edge31;
			std::multimap <Vector3f *, T *> ::iterator l_it = m_edges.find( l_vertex1);
			if (l_it != m_edges.end())
			{
				while (l_it != m_edges.end() && l_it->first == l_vertex1)
				{
					if (l_it->second->IsInVertex( l_vertex2))
					{
						l_1and2 = true;
						l_edge12 = l_it->second;
					}
					if (l_it->second->IsInVertex( l_vertex3))
					{
						l_1and3 = true;
						l_edge31 = l_it->second;
					}
					++l_it;
				}
				if ( ! l_1and2 && ! l_1and3)
				{

				}
			}
			l_it = m_edges.find( l_vertex2);
			if (l_it != m_edges.end())
			{
				while (l_it != m_edges.end() && l_it->first == l_vertex2)
				{
					if (l_it->second->IsInVertex( l_vertex1))
					{
						l_1and2 = true;
						l_edge12 = l_it->second;
					}
					if (l_it->second->IsInVertex( l_vertex3))
					{
						l_2and3 = true;
						l_edge23 = l_it->second;
					}
					++l_it;
				}
			}
			l_it = m_edges.find( l_vertex3);
			if (l_it != m_edges.end())
			{
				while (l_it != m_edges.end() && l_it->first == l_vertex3)
				{
					if (l_it->second->IsInVertex( l_vertex1))
					{
						l_1and3 = true;
						l_edge31 = l_it->second;
					}
					if (l_it->second->IsInVertex( l_vertex2))
					{
						l_2and3 = true;
						l_edge23 = l_it->second;
					}
					++l_it;
				}
			}

			if ( ! l_1and2)
			{
				T * l_edge = new T( l_vertex1, l_vertex2, p_face);
				m_edges.insert( std::multimap <Vector3f *, T *>::value_type( l_vertex1, l_edge));
			}
			else
			{
				l_edge12->AddFace( p_face);
			}

			if ( ! l_2and3)
			{
				T * l_edge = new T( l_vertex2, l_vertex3, p_face);
				m_edges.insert( std::multimap <Vector3f *, T *>::value_type( l_vertex2, l_edge));
			}
			else
			{
				l_edge23->AddFace( p_face);
			}

			if ( ! l_1and3)
			{
				T * l_edge = new T( l_vertex1, l_vertex3, p_face);
				m_edges.insert( std::multimap <Vector3f *, T *>::value_type( l_vertex1, l_edge));
			}
			else
			{
				l_edge31->AddFace( p_face);
			}
		}

		virtual void DivideFace( Submesh * p_submesh, Face * p_face, size_t p_sgIndex)=0;
	};
	/*!
	Lists all the neighbours of a vertex, i.e. the vertices that share an edge with this one
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class VertexNeighbours
	{
	protected:
		Vector3f * m_vertex;
		std::set <Vector3f *> m_neighbours;

	public:
		VertexNeighbours( Vector3f * p_vertex=NULL)
			:	m_vertex( p_vertex)
		{
		}
		~VertexNeighbours()
		{
			m_neighbours.clear();
		}

		virtual bool AddNeighbour( Vector3f * p_vertex);
	};
	/*!
	Abstract class for subdivisers, contains the header for the main Subdivide function
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Subdiviser
	{
	protected:
		Submesh * m_submesh;
		Vertex3fPtrArray * m_vertex;
		SmoothGroupPtrArray * m_smoothGroups;

	public:
		Subdiviser( Submesh * p_submesh);
		virtual ~Subdiviser(){}

		virtual void Subdivide( Vector3f * p_center)=0;

	protected:
		void _setTextCoords( Face * p_face, Vector3f * p_a, Vector3f * p_b, Vector3f * p_c, 
							 Vector3f * p_d, Vector3f * p_e, Vector3f * p_f, size_t p_sgIndex);
		Vector3f * _computeCenterFrom( const Vector3f & p_a, float * p_aNormal,
									   const Vector3f & p_b, float * p_bNormal);
		Vector3f * _computeCenterFrom( const Vector3f & p_a, float * p_aNormal,
									   const Vector3f & p_b, float * p_bNormal,
									   const Vector3f & p_c, float * p_cNormal);
	};
	/*!
	PN Triangles algorithm specialisation of Subdiviser.
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class PNTrianglesSubdiviser : public Subdiviser
	{
	public:
		PNTrianglesSubdiviser( Submesh * p_submesh)
			:	Subdiviser( p_submesh)
		{}
		virtual ~PNTrianglesSubdiviser();

		virtual void Subdivide( Vector3f * p_center);

	private:
		Vector3f * _computePointFrom( const Vector3f & p_a, float * p_aNormal,
									  const Vector3f & p_b, float * p_bNormal,
									  Vector3f * p_center=NULL);
	};
}

#endif