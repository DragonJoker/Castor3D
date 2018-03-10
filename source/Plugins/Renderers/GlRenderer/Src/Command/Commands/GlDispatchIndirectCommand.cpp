/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlDispatchIndirectCommand.hpp"

#include "Buffer/GlBuffer.hpp"

namespace gl_renderer
{
	DispatchIndirectCommand::DispatchIndirectCommand( renderer::BufferBase const & buffer
		, uint32_t offset )
		: m_buffer{ static_cast< Buffer const & >( buffer ) }
		, m_offset{ offset }
	{
	}

	void DispatchIndirectCommand::apply()const
	{
		glLogCommand( "DispatchIndirectCommand" );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_DISPATCH_INDIRECT, m_buffer.getBuffer() );
		glLogCall( gl::DispatchComputeIndirect, GLintptr( BufferOffset( m_offset ) ) );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_DISPATCH_INDIRECT, 0 );
	}

	CommandPtr DispatchIndirectCommand::clone()const
	{
		return std::make_unique< DispatchIndirectCommand >( *this );
	}
}
