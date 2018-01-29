/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Buffer/GeometryBuffers.hpp"

namespace renderer
{
	GeometryBuffers::GeometryBuffers( VertexBufferCRefArray const & vbos
		, std::vector< uint64_t > vboOffsets
		, VertexLayoutCRefArray const & layouts )
	{
		assert( vbos.size() == vboOffsets.size() );
		assert( vbos.size() == layouts.size() );
		m_vbos.reserve( vbos.size() );

		for ( auto i = 0u; i < vbos.size(); ++i )
		{
			m_vbos.push_back( { vbos[i].get(), vboOffsets[i], layouts[i] } );
		}
	}

	GeometryBuffers::GeometryBuffers( VertexBufferCRefArray const & vbos
		, std::vector< uint64_t > vboOffsets
		, VertexLayoutCRefArray const & layouts
		, BufferBase const & ibo
		, uint64_t iboOffset
		, IndexType type )
		: m_ibo{ std::make_unique< IBO >( ibo, iboOffset, type ) }
	{
		assert( vbos.size() == vboOffsets.size() );
		assert( vbos.size() == layouts.size() );
		m_vbos.reserve( vbos.size() );

		for ( auto i = 0u; i < vbos.size(); ++i )
		{
			m_vbos.push_back( { vbos[i].get(), vboOffsets[i], layouts[i] } );
		}
	}
}
