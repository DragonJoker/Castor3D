/*
This file belongs to VkRenderer.
See LICENSE file in root folder.
*/
#include "Buffer/VkGeometryBuffers.hpp"

namespace vk_renderer
{
	GeometryBuffers::GeometryBuffers( renderer::VertexBufferBase const & vbo
		, uint64_t offset
		, renderer::VertexLayout const & layout )
		: renderer::GeometryBuffers{ vbo, offset, layout }
	{
	}

	GeometryBuffers::GeometryBuffers( renderer::VertexBufferBase const & vbo
		, uint64_t offset
		, renderer::VertexLayout const & layout
		, renderer::BufferBase const & ibo
		, uint64_t iboOffset
		, renderer::IndexType type )
		: renderer::GeometryBuffers{ vbo, offset, layout, ibo, iboOffset, type }
	{
	}

	GeometryBuffers::GeometryBuffers( renderer::VertexBufferCRefArray const & vbos
		, std::vector< uint64_t > offsets
		, renderer::VertexLayoutCRefArray const & layouts )
		: renderer::GeometryBuffers{ vbos, offsets, layouts }
	{
	}

	GeometryBuffers::GeometryBuffers( renderer::VertexBufferCRefArray const & vbos
		, std::vector< uint64_t > offsets
		, renderer::VertexLayoutCRefArray const & layouts
		, renderer::BufferBase const & ibo
		, uint64_t iboOffset
		, renderer::IndexType type )
		: renderer::GeometryBuffers{ vbos, offsets, layouts, ibo, iboOffset, type }
	{
	}
}
