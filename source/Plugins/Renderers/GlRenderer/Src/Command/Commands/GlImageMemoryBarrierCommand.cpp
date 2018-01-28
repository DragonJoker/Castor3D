/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlImageMemoryBarrierCommand.hpp"

namespace gl_renderer
{
	ImageMemoryBarrierCommand::ImageMemoryBarrierCommand( renderer::PipelineStageFlags after
		, renderer::PipelineStageFlags before
		, renderer::ImageMemoryBarrier const & transitionBarrier )
		: m_flags{ convert( before ) }
	{
	}

	void ImageMemoryBarrierCommand::apply()const
	{
		glLogCommand( "ImageMemoryBarrierCommand" );
		glLogCall( gl::MemoryBarrier, m_flags );
	}

	CommandPtr ImageMemoryBarrierCommand::clone()const
	{
		return std::make_unique< ImageMemoryBarrierCommand >( *this );
	}
}
