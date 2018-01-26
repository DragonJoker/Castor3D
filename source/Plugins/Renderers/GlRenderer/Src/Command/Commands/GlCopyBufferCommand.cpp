/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlCopyBufferCommand.hpp"

#include "Buffer/GlBuffer.hpp"

#include <Miscellaneous/BufferCopy.hpp>

namespace gl_renderer
{
	CopyBufferCommand::CopyBufferCommand( renderer::BufferCopy const & copyInfo
		, renderer::BufferBase const & src
		, renderer::BufferBase const & dst )
		: m_src{ static_cast< Buffer const & >( src ) }
		, m_dst{ static_cast< Buffer const & >( dst ) }
		, m_copyInfo{ copyInfo }
	{
	}

	void CopyBufferCommand::apply()const
	{
		glLogCommand( "CopyBufferCommand" );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_COPY_READ, m_src.getBuffer() );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_COPY_WRITE, m_dst.getBuffer() );
		glLogCall( gl::CopyBufferSubData
			, GL_BUFFER_TARGET_COPY_READ
			, GL_BUFFER_TARGET_COPY_WRITE
			, m_copyInfo.srcOffset
			, m_copyInfo.dstOffset
			, m_copyInfo.size );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_COPY_WRITE, 0u );
		glLogCall( gl::BindBuffer, GL_BUFFER_TARGET_COPY_READ, 0u );
	}

	CommandPtr CopyBufferCommand::clone()const
	{
		return std::make_unique< CopyBufferCommand >( *this );
	}
}
