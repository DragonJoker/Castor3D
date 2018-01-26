/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlImageMemoryBarrierCommand.hpp"

namespace gl_renderer
{
	ImageMemoryBarrierCommand::ImageMemoryBarrierCommand()
	{
	}

	void ImageMemoryBarrierCommand::apply()const
	{
		glLogCommand( "ImageMemoryBarrierCommand" );
	}

	CommandPtr ImageMemoryBarrierCommand::clone()const
	{
		return std::make_unique< ImageMemoryBarrierCommand >( *this );
	}
}
