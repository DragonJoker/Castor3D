/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBufferMemoryBarrierCommand.hpp"

namespace gl_renderer
{
	BufferMemoryBarrierCommand::BufferMemoryBarrierCommand()
	{
	}

	void BufferMemoryBarrierCommand::apply()const
	{
		glLogCommand( "BufferMemoryBarrierCommand" );
	}

	CommandPtr BufferMemoryBarrierCommand::clone()const
	{
		return std::make_unique< BufferMemoryBarrierCommand >( *this );
	}
}
