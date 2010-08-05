/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Subdiviser___
#define ___C3D_Subdiviser___

#include "../Module_Geometry.h"

namespace Castor3D
{
	/*!
	Specialisation of Edge for the Loop subdivision algorithm. Allows to create a vertex on this edge
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Edge
	{
	private:
		Vector3f * m_firstVertex;
		Vector3f * m_secondVertex;
		Face * m_firstFace;
		Face * m_secondFace;

		Vector3f * m_createdVertex;
		bool m_divided;
		bool m_toDivide;
		bool m_toDelete;

		int m_refCount;

	public:
		Edge( Vector3f * p_v1, Vector3f * p_v2, Face * p_f1, bool p_toDivide);
		~Edge();

		void AddFace( Face * p_face);
		Vector3f * Divide( Submesh * p_submesh, float p_value);

	public:
		inline bool			IsDivided	()						{ return m_divided; }
		inline Vector3f *	GetVertex1	()						{ return m_firstVertex; }
		inline Vector3f *	GetVertex2	()						{ return m_secondVertex; }
		inline bool			IsInVertex	( Vector3f * p_vertex)	{ return m_firstVertex == p_vertex || m_secondVertex == p_vertex; }
		inline bool			IsToDelete	()						{ return m_toDelete; }

		inline int	Release		() { return --m_refCount; }
		inline void	Ref			() { m_refCount++; }
		inline void	SetToDivide	() { m_toDivide = true;m_divided = false;m_createdVertex = NULL; }
		inline void	SetToDelete	() { m_toDelete = true; }
	};

	typedef std::map <Vector3f *, Edge *> EdgePtrMap;

	class FaceEdges
	{
	private:
		Submesh * m_submesh;
		Face * m_face;
		size_t m_sgIndex;
		Edge * m_edgeAB;
		Edge * m_edgeBC;
		Edge * m_edgeCA;

	public:
		FaceEdges( Submesh * p_submesh, size_t p_sgIndex, Face * p_face, 
				   std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
				   std::map <Vector3f *, int> & p_vertexNeighbourhood,
				   std::set <Edge *> & p_allEdges);
		FaceEdges( Submesh * p_submesh, size_t p_sgIndex, Face * p_face, Edge * l_ab, Edge * l_bc, Edge * l_ca);
		void Divide( float p_value, std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
					 std::map <Vector3f *, int> & p_vertexNeighbourhood,
					 std::set <Edge *> & p_allEdges, std::vector <FaceEdges *> & p_newFaces,
					 std::vector <Vector3f *> & p_newVertices);

	private:
		Edge * _addEdge( Vector3f * p_v1, Vector3f * p_v2, bool p_toDivide, bool p_add1, bool p_add2,
						 std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
						 std::map <Vector3f *, int> & p_vertexNeighbourhood,
						 std::set <Edge *> & p_allEdges);
		void _removeEdge( Edge * p_edge, std::map <Vector3f *, EdgePtrMap> & p_existingEdges,
						  std::set <Edge *> & p_allEdges);

	public:
		inline Edge * GetEdgesAB() { return m_edgeAB; }
		inline Edge * GetEdgesBC() { return m_edgeBC; }
		inline Edge * GetEdgesCA() { return m_edgeCA; }
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
		Vector3fPtrArray * m_vertex;
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
}

#endif