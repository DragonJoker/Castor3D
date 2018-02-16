/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlNextSubpassCommand.hpp"

#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "RenderPass/GlRenderSubpass.hpp"

#include <RenderPass/ClearValue.hpp>
#include <RenderPass/RenderPassAttachment.hpp>

namespace gl_renderer
{
	NextSubpassCommand::NextSubpassCommand( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, uint32_t index )
		: m_renderPass{ static_cast< RenderPass const & >( renderPass ) }
		, m_subpass{ static_cast< RenderSubpass const & >( *renderPass.getSubpasses()[index] ) }
		, m_frameBuffer{ static_cast< FrameBuffer const & >( frameBuffer ) }
	{
	}

	void NextSubpassCommand::apply()const
	{
		glLogCommand( "NextSubpassCommand" );
		if ( m_frameBuffer.getFrameBuffer() )
		{
			m_frameBuffer.setDrawBuffers( m_subpass.getAttaches() );
		}
	}

	CommandPtr NextSubpassCommand::clone()const
	{
		return std::make_unique< NextSubpassCommand >( *this );
	}
}
