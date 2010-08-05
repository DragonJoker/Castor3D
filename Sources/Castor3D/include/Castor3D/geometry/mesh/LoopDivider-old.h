#ifndef ___C3D_LoopDivider___
#define ___C3D_LoopDivider___

#include "Subdiviser.h"

namespace Castor3D
{
	/*!
	Specialisation of Edge for the Loop subdivision algorithm. Allows to create a vertex on this edge
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class LoopEdge : public Edge
	{
	private:
		Vector3f * m_createdVertex;
		bool m_divided;

	public:
		LoopEdge( Vector3f * p_v1, Vector3f * p_v2, Face * p_f1);

		virtual Vector3f * Divide();

	public:
		inline bool IsDivided() { return m_divided; }
	};
	/*!
	Specialisation of EdgeList for Loop subdivision algorithm. Creates LoopEdge instead of Edge
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class LoopEdgeList : public EdgeList<LoopEdge>
	{
	public:
		LoopEdgeList()
			:	EdgeList()
		{}
		~LoopEdgeList();

		virtual void DivideFace( Submesh * p_submesh, Face * p_face, size_t p_sgIndex);
	};
	/*!
	Specialisation of VertexNeighbour for Loop subdivision algorithm, adds weight for vertices
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class LoopSubdivMask : public VertexNeighbours
	{
	private:
		std::set <Vector3f *> m_originalNeighbours;
		float m_factor;

	public:
		LoopSubdivMask( Vector3f * p_vertex=NULL)
			:	VertexNeighbours( p_vertex),
			m_factor( 1.0f)
		{}
		~LoopSubdivMask();

		void ReplaceVertex( Vector3f * p_center);
		virtual bool AddNeighbour( Vector3f * p_vertex);
	};
	/*!
	Loop algorithm specialisation of Subdiviser.
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class LoopSubdiviser : public Subdiviser
	{
	private:
		LoopEdgeList * m_edges;

	public:
		LoopSubdiviser( Submesh * p_submesh)
			:	Subdiviser( p_submesh),
			m_edges( NULL)
		{}
		virtual ~LoopSubdiviser();

		virtual void Subdivide( Vector3f * p_center);
	};
}

#endif