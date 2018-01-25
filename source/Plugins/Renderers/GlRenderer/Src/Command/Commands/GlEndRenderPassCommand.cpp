/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlEndRenderPassCommand.hpp"

#include "RenderPass/GlFrameBuffer.hpp"

namespace gl_renderer
{
	EndRenderPassCommand::EndRenderPassCommand()
	{
	}

	void EndRenderPassCommand::apply()const
	{
		glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, 0u );
	}

	CommandPtr EndRenderPassCommand::clone()const
	{
		return std::make_unique< EndRenderPassCommand >( *this );
	}
}
