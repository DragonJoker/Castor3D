/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		castor3d::Face const * m_firstFace;
		castor3d::Face const * m_secondFace;
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
		Edge( VertexSPtr p_v1, VertexSPtr p_v2, castor3d::Face const & p_f1, bool p_toDivide );
		/**
		 * Destructor
		 */
		~Edge();
		/**
		 * adds a face to the edge (max 2 faces, 1 at each side of the edge)
		 *\param[in]	p_face	Pointer to the face to add
		 */
		void addFace( castor3d::Face const & p_face );
		/**
		 * Divides the edge id est adds a vertex in a portion of the edge determined by p_value (0.5 = middle).
		 * doesn't divide the faces
		 *\param[in]	p_submesh	The submesh to which the newly created point is added
		 *\param[in]	p_value	Percentage of the segment [m_firstVertex,m_secondVertex]
		 *\return	The created point
		 */
		VertexSPtr divide( Subdivider * p_pDivider, castor::real p_value );

	public:
		/**@name Accessors */
		//@{
		inline bool isDivided()const
		{
			return m_divided;
		}
		inline VertexSPtr getVertex1()const
		{
			return m_firstVertex.lock();
		}
		inline VertexSPtr getVertex2()const
		{
			return m_secondVertex.lock();
		}
		inline castor3d::Face const * getFace1()const
		{
			return m_firstFace;
		}
		inline castor3d::Face const * getFace2()const
		{
			return m_secondFace;
		}

		inline void setToDivide()
		{
			m_toDivide = true;
			m_divided = false;
			m_createdVertex.reset();
		}
		inline void setToDelete()
		{
			m_toDelete = true;
		}
		//@}
	};
}

#endif
