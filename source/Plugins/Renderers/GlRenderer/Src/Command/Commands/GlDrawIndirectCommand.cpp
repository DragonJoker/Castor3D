/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlDrawIndirectCommand.hpp"

#include "Buffer/GlBuffer.hpp"

namespace gl_renderer
{
	DrawIndirectCommand::DrawIndirectCommand( renderer::BufferBase const & buffer
		, uint32_t offset
		, uint32_t drawCount
		, uint32_t stride
		, renderer::PrimitiveTopology mode )
		: m_buffer{ static_cast< Buffer const & >( buffer ) }
		, m_offset{ offset }
		, m_drawCount{ drawCount }
		, m_stride{ stride }
		, m_mode{ convert( mode ) }
	{
	}

	void DrawIndirectCommand::apply()const
	{
		glLogCommand( "DrawIndirectCommand" );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_DRAW_INDIRECT, m_buffer.getBuffer() );
		glLogCall( gl::MultiDrawArraysIndirect
			, m_mode
			, BufferOffset( m_offset )
			, m_drawCount
			, m_stride );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_DRAW_INDIRECT, 0 );
	}

	CommandPtr DrawIndirectCommand::clone()const
	{
		return std::make_unique< DrawIndirectCommand >( *this );
	}
}
