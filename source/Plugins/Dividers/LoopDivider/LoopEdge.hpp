/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LOOP_EDGE_H___
#define ___C3D_LOOP_EDGE_H___

#include "LoopDivider/LoopPrerequisites.hpp"

namespace Loop
{
	//! Edge representation
	/**
	An edge is defined between 2 vertices used to separate 2 faces
	\author Sylvain DOREMUS
	\date 12/03/2010
	*/
	class Edge
	{
	public:
		/**
		 *\brief
		 * Constructor
		 *\param[in]	v1			The 1st point
		 *\param[in]	v2			The 2nd point
		 *\param[in]	f1			The 1st face of the edge
		 *\param[in]	toDivide	Tells if the edge has to be divided
		 */
		Edge( VertexSPtr v1
			, VertexSPtr v2
			, castor3d::Face const & f1
			, bool toDivide );
		/**
		 *\brief
		 * Adds a face to the edge (max 2 faces, 1 at each side of the edge).
		 *\param[in]	face	Pointer to the face to add.
		 */
		void addFace( castor3d::Face const & face );
		/**
		 *\brief
		 * Divides the edge id est adds a vertex in a portion of the edge determined by p_value (0.5 = middle).
		 *\remarks
		 * Doesn't divide the faces.
		 *\param[in]	divider	The subdivider to which the newly created point is added.
		 *\param[in]	value	Percentage of the segment [m_firstVertex,m_secondVertex].
		 *\return	The created point.
		 */
		VertexSPtr divide( Subdivider * divider, float value );

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

	private:
		VertexWPtr m_firstVertex;
		VertexWPtr m_secondVertex;
		castor3d::Face const * m_firstFace;
		castor3d::Face const * m_secondFace;
		VertexSPtr m_createdVertex;
		bool m_divided;
		bool m_toDivide;
		bool m_toDelete;
	};
}

#endif
