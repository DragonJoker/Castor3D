/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlEndSubpassCommand.hpp"

#include "Command/GlCommandBuffer.hpp"
#include "RenderPass/GlFrameBuffer.hpp"

#include <RenderPass/ClearValue.hpp>
#include <RenderPass/AttachmentDescription.hpp>
#include <RenderPass/SubpassDescription.hpp>

namespace gl_renderer
{
	EndSubpassCommand::EndSubpassCommand( Device const & device
		, renderer::FrameBuffer const & frameBuffer
		, renderer::SubpassDescription const & subpass )
		: m_device{ device }
		, m_frameBuffer{ static_cast< FrameBuffer const & >( frameBuffer ) }
		, m_subpass{ subpass }
	{
		assert( m_subpass.resolveAttachments.empty()
			|| m_subpass.resolveAttachments.size() == m_subpass.colorAttachments.size() );
	}

	void EndSubpassCommand::apply()const
	{
		glLogCommand( "EndSubpassCommand" );

		if ( !m_subpass.resolveAttachments.empty() )
		{
			if ( m_frameBuffer.getFrameBuffer() )
			{
				uint32_t index = 0u;

				for ( auto & resolveAttach : m_subpass.resolveAttachments )
				{
					auto & srcattach = m_frameBuffer.getColourAttaches()[index++];
					auto & dstattach = m_frameBuffer.getColourAttaches()[resolveAttach.attachment];

					if ( dstattach.object != GL_INVALID_INDEX )
					{
						gl::BindFramebuffer( GL_DRAW_FRAMEBUFFER, m_frameBuffer.getFrameBuffer() );
					}
					else
					{
						gl::BindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
					}

					GLenum attach[1]{ dstattach.point };
					gl::BindFramebuffer( GL_READ_FRAMEBUFFER, m_frameBuffer.getFrameBuffer() );
					gl::ReadBuffer( srcattach.point );
					gl::DrawBuffers( 1u, attach );
					gl::BlitFramebuffer( 0, 0, m_frameBuffer.getDimensions().width, m_frameBuffer.getDimensions().height
						, 0, 0, m_frameBuffer.getDimensions().width, m_frameBuffer.getDimensions().height
						, GL_COLOR_BUFFER_BIT, GL_FILTER_NEAREST );
				}
			}
		}
	}

	CommandPtr EndSubpassCommand::clone()const
	{
		return std::make_unique< EndSubpassCommand >( *this );
	}
}
