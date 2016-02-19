/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_LOOP_EDGE_H___
#define ___C3D_LOOP_EDGE_H___

#include "LoopPrerequisites.hpp"

namespace Loop
{
	//! Edge representation
	/*!
	An edge is defined between 2 vertices used to separate 2 faces
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Edge
	{
	private:
		VertexWPtr m_firstVertex;
		VertexWPtr m_secondVertex;
		Castor3D::Face const * m_firstFace;
		Castor3D::Face const * m_secondFace;
		VertexSPtr m_createdVertex;
		bool m_divided;
		bool m_toDivide;
		bool m_toDelete;

	public:
		/**
		 * Constructor
		 *\param[in]	p_v1	The 1st point
		 *\param[in]	p_v2	The 2nd point
		 *\param[in]	p_f1	The 1st face of the edge
		 *\param[in]	p_toDivide	Tells if the edge has to be divided
		 */
		Edge( VertexSPtr p_v1, VertexSPtr p_v2, Castor3D::Face const & p_f1, bool p_toDivide );
		/**
		 * Destructor
		 */
		~Edge();
		/**
		 * Adds a face to the edge (max 2 faces, 1 at each side of the edge)
		 *\param[in]	p_face	Pointer to the face to add
		 */
		void AddFace( Castor3D::Face const & p_face );
		/**
		 * Divides the edge id est adds a vertex in a portion of the edge determined by p_value (0.5 = middle).
		 * Doesn't divide the faces
		 *\param[in]	p_submesh	The submesh to which the newly created point is added
		 *\param[in]	p_value	Percentage of the segment [m_firstVertex,m_secondVertex]
		 *\return	The created point
		 */
		VertexSPtr Divide( Subdivider * p_pDivider, Castor::real p_value );

	public:
		/**@name Accessors */
		//@{
		inline bool IsDivided()const
		{
			return m_divided;
		}
		inline VertexSPtr GetVertex1()const
		{
			return m_firstVertex.lock();
		}
		inline VertexSPtr GetVertex2()const
		{
			return m_secondVertex.lock();
		}
		inline Castor3D::Face const * GetFace1()const
		{
			return m_firstFace;
		}
		inline Castor3D::Face const * GetFace2()const
		{
			return m_secondFace;
		}

		inline void SetToDivide()
		{
			m_toDivide = true;
			m_divided = false;
			m_createdVertex.reset();
		}
		inline void SetToDelete()
		{
			m_toDelete = true;
		}
		//@}
	};
}

#endif
