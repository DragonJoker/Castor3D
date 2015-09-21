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
#ifndef ___C3D_LOOP_VERTEX_H___
#define ___C3D_LOOP_VERTEX_H___

#include "LoopPrerequisites.hpp"

namespace Loop
{
	//! Loop Algorithm vertex representation
	/*!
	It's a vertex with additional information : neighbourhood and associated edges
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 12/01/2011
	*/
	class Vertex
	{
	private:
		Castor3D::BufferElementGroupSPtr m_ptPoint;
		EdgePtrUIntMap m_mapEdges;

	public:
		Vertex( Castor3D::BufferElementGroupSPtr p_ptPoint );
		~Vertex();

		bool HasEdge( uint32_t p_uiIndex );
		EdgeSPtr GetEdge( uint32_t p_uiIndex );
		void AddEdge( EdgeSPtr p_pEdge, uint32_t p_uiIndex );
		void RemoveEdge( uint32_t p_uiIndex );

		/**@name Accessors */
		//@{
		inline Castor3D::BufferElementGroupSPtr GetPoint()const
		{
			return m_ptPoint;
		}
		inline uint32_t GetIndex()const
		{
			return m_ptPoint->GetIndex();
		}
		inline uint32_t Size()const
		{
			return uint32_t( m_mapEdges.size() );
		}
		inline EdgePtrUIntMap::iterator Begin()
		{
			return m_mapEdges.begin();
		}
		inline EdgePtrUIntMap::const_iterator End()const
		{
			return m_mapEdges.end();
		}
		inline void Erase( EdgePtrUIntMap::iterator p_it )
		{
			m_mapEdges.erase( p_it );
		}
		inline void Clear()
		{
			m_mapEdges.clear();
		}
		//@}
	};
}

Castor::String & operator << ( Castor::String & p_stream, Loop::Vertex const & p_vertex );

#endif
