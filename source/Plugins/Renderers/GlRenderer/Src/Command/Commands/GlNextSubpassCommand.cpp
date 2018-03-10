/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlNextSubpassCommand.hpp"

#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"

#include <RenderPass/ClearValue.hpp>
#include <RenderPass/AttachmentDescription.hpp>

namespace gl_renderer
{
	NextSubpassCommand::NextSubpassCommand( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, renderer::SubpassDescription const & subpass )
		: m_renderPass{ static_cast< RenderPass const & >( renderPass ) }
		, m_subpass{ subpass }
		, m_frameBuffer{ static_cast< FrameBuffer const & >( frameBuffer ) }
	{
	}

	void NextSubpassCommand::apply()const
	{
		glLogCommand( "NextSubpassCommand" );
		if ( m_frameBuffer.getFrameBuffer() )
		{
			m_frameBuffer.setDrawBuffers( m_subpass.colorAttachments );
		}
	}

	CommandPtr NextSubpassCommand::clone()const
	{
		return std::make_unique< NextSubpassCommand >( *this );
	}
}
