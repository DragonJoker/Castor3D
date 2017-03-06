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
		Vertex( Castor3D::BufferElementGroupSPtr p_point );
		~Vertex();

		bool HasEdge( uint32_t p_index );
		EdgeSPtr GetEdge( uint32_t p_index );
		void AddEdge( EdgeSPtr p_pEdge, uint32_t p_index );
		void RemoveEdge( uint32_t p_index );

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
		inline uint32_t size()const
		{
			return uint32_t( m_mapEdges.size() );
		}
		inline EdgePtrUIntMap::iterator begin()
		{
			return m_mapEdges.begin();
		}
		inline EdgePtrUIntMap::const_iterator begin()const
		{
			return m_mapEdges.begin();
		}
		inline EdgePtrUIntMap::iterator end()
		{
			return m_mapEdges.end();
		}
		inline EdgePtrUIntMap::const_iterator end()const
		{
			return m_mapEdges.end();
		}
		inline void erase( EdgePtrUIntMap::iterator p_it )
		{
			m_mapEdges.erase( p_it );
		}
		inline void clear()
		{
			m_mapEdges.clear();
		}
		//@}
	};
}

Castor::String & operator << ( Castor::String & p_stream, Loop::Vertex const & p_vertex );

#endif
