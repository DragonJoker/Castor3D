/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "Buffer/GlGeometryBuffers.hpp"

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

			if ( vbo.layout.getInputRate() == renderer::VertexInputRate::eVertex )
			{
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
			else
			{
				for ( auto & attribute : static_cast< VertexLayout const & >( vbo.layout ) )
				{
					auto format = attribute.getFormat();
					uint32_t offset = attribute.getOffset();
					uint32_t location = attribute.getLocation();
					uint32_t divisor = 1u;

					if ( attribute.getFormat() == renderer::AttributeFormat::eMat4f )
					{
						format = renderer::AttributeFormat::eVec4f;

						for ( auto i = 0u; i < 4u; ++i )
						{
							glLogCall( gl::EnableVertexAttribArray, location );
							glLogCall( gl::VertexAttribPointer
								, location
								, getCount( format )
								, getType( format )
								, false
								, vbo.layout.getStride()
								, BufferOffset( vbo.offset + offset ) );
							glLogCall( gl::VertexAttribDivisor
								, location
								, divisor );
							++location;
							offset += 16u;
						}
					}
					else
					{
						glLogCall( gl::EnableVertexAttribArray, location );
						glLogCall( gl::VertexAttribPointer
							, location
							, getCount( format )
							, getType( format )
							, false
							, vbo.layout.getStride()
							, BufferOffset( vbo.offset + offset ) );
						glLogCall( gl::VertexAttribDivisor
							, location
							, divisor );
					}
				}
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
