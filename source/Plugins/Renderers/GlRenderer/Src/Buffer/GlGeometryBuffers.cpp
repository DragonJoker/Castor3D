/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "Buffer/GlGeometryBuffers.hpp"

#include "Shader/GlAttribute.hpp"
#include "Buffer/GlBuffer.hpp"
#include "Pipeline/GlVertexLayout.hpp"

#include <Buffer/VertexBuffer.hpp>

#define BufferOffset( n ) ( ( uint8_t * )nullptr + ( n ) )

namespace gl_renderer
{
	GeometryBuffers::GeometryBuffers( renderer::VertexBufferCRefArray const & vbos
		, std::vector< uint64_t > offsets
		, renderer::VertexLayoutCRefArray const & layouts )
		: renderer::GeometryBuffers{ vbos, offsets, layouts }
	{
		doInitialise();
	}

	GeometryBuffers::GeometryBuffers( renderer::VertexBufferCRefArray const & vbos
		, std::vector< uint64_t > offsets
		, renderer::VertexLayoutCRefArray const & layouts
		, renderer::BufferBase const & ibo
		, uint64_t iboOffset
		, renderer::IndexType type )
		: renderer::GeometryBuffers{ vbos, offsets, layouts, ibo, iboOffset, type }
	{
		doInitialise();
	}

	GeometryBuffers::~GeometryBuffers()noexcept
	{
		glLogCall( gl::DeleteVertexArrays, 1, &m_vao );
	}

	void GeometryBuffers::doInitialise()
	{
		glLogCall( gl::CreateVertexArrays, 1, &m_vao );

		if ( m_vao == GL_INVALID_INDEX )
		{
			throw std::runtime_error{ "Couldn't create VAO" };
		}

		glLogCall( gl::BindVertexArray, m_vao );

		for ( auto & vbo : getVbos() )
		{
			glLogCall( gl::BindBuffer
				, GL_BUFFER_TARGET_ARRAY
				, static_cast< Buffer const & >( vbo.vbo.getBuffer() ).getBuffer() );

			for ( auto & attribute : static_cast< VertexLayout const & >( vbo.layout ) )
			{
				glLogCall( gl::EnableVertexAttribArray, attribute.getLocation() );
				glLogCall( gl::VertexAttribPointer
					, attribute.getLocation()
					, getCount( attribute.getFormat() )
					, getType( attribute.getFormat() )
					, false
					, vbo.layout.getStride()
					, BufferOffset( vbo.offset + attribute.getOffset() ) );
			}
		}

		if ( hasIbo() )
		{
			glLogCall( gl::BindBuffer
				, GL_BUFFER_TARGET_ELEMENT_ARRAY
				, static_cast< Buffer const & >( getIbo().buffer ).getBuffer() );
		}

		glLogCall( gl::BindVertexArray, 0u );
	}
}
