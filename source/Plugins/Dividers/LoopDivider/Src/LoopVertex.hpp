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
	private:
		castor3d::BufferElementGroupSPtr m_ptPoint;
		EdgePtrUIntMap m_mapEdges;

	public:
		explicit Vertex( castor3d::BufferElementGroupSPtr p_point );
		~Vertex();

		bool hasEdge( uint32_t p_index );
		EdgeSPtr getEdge( uint32_t p_index );
		void addEdge( EdgeSPtr p_pEdge, uint32_t p_index );
		void removeEdge( uint32_t p_index );

		/**@name Accessors */
		//@{
		inline castor3d::BufferElementGroupSPtr getPoint()const
		{
			return m_ptPoint;
		}
		inline uint32_t getIndex()const
		{
			return m_ptPoint->getIndex();
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

castor::String & operator << ( castor::String & p_stream, Loop::Vertex const & p_vertex );

#endif
