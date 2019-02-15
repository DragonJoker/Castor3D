/*
See LICENSE file in root folder
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
	public:
		Vertex( castor3d::SubmeshVertex vertex );

		bool hasEdge( uint32_t index );
		EdgeSPtr getEdge( uint32_t index );
		void addEdge( EdgeSPtr edge, uint32_t index );
		void removeEdge( uint32_t index );

		/**@name Accessors */
		//@{
		inline castor3d::InterleavedVertex const & getPoint()const
		{
			return m_vertex.m_vertex;
		}
		inline castor3d::InterleavedVertex & getPoint()
		{
			return m_vertex.m_vertex;
		}
		inline uint32_t getIndex()const
		{
			return m_vertex.m_index;
		}
		inline uint32_t size()const
		{
			return uint32_t( m_edges.size() );
		}
		inline EdgePtrUIntMap::iterator begin()
		{
			return m_edges.begin();
		}
		inline EdgePtrUIntMap::const_iterator begin()const
		{
			return m_edges.begin();
		}
		inline EdgePtrUIntMap::iterator end()
		{
			return m_edges.end();
		}
		inline EdgePtrUIntMap::const_iterator end()const
		{
			return m_edges.end();
		}
		inline void erase( EdgePtrUIntMap::iterator p_it )
		{
			m_edges.erase( p_it );
		}
		inline void clear()
		{
			m_edges.clear();
		}
		//@}

	private:
		castor3d::SubmeshVertex m_vertex;
		EdgePtrUIntMap m_edges;
	};
}

castor::String & operator<<( castor::String & stream, Loop::Vertex const & vertex );

#endif
