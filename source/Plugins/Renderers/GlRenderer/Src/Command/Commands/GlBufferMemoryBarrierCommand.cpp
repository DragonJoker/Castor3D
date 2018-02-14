/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBufferMemoryBarrierCommand.hpp"

namespace gl_renderer
{
	BufferMemoryBarrierCommand::BufferMemoryBarrierCommand( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::BufferMemoryBarrier const & transitionBarrier )
		: m_flags{ convert( before ) }
	{
	}

	void BufferMemoryBarrierCommand::apply()const
	{
		//glLogCommand( "BufferMemoryBarrierCommand" );
		//glLogCall( gl::MemoryBarrier, m_flags );
	}

	CommandPtr BufferMemoryBarrierCommand::clone()const
	{
		return std::make_unique< BufferMemoryBarrierCommand >( *this );
	}
}
