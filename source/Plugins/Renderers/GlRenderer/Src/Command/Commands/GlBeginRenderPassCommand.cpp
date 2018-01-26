/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBeginRenderPassCommand.hpp"

#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"

#include <RenderPass/ClearValue.hpp>

namespace gl_renderer
{
	BeginRenderPassCommand::BeginRenderPassCommand( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, renderer::ClearValueArray const & clearValues
		, renderer::SubpassContents contents )
		: m_renderPass{ static_cast< RenderPass const & >( renderPass ) }
		, m_frameBuffer{ static_cast< FrameBuffer const & >( frameBuffer ) }
		, m_clearValues{ clearValues }
	{
	}

	void BeginRenderPassCommand::apply()const
	{
		glLogCommand( "BeginRenderPassCommand" );
		glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, m_frameBuffer.getFrameBuffer() );

		if ( m_renderPass.getClear() )
		{
			GLint colourIndex = 0u;
			GLint depthStencilIndex = 0u;

			for ( auto & clearValue : m_clearValues )
			{
				if ( clearValue.isColour() )
				{
					auto & colour = clearValue.colour();
					glLogCall( gl::ClearBufferfv
						, GL_CLEAR_TARGET_COLOR
						, colourIndex
						, colour.constPtr() );
					++colourIndex;
				}
				else
				{
					auto & depthStencil = clearValue.depthStencil();
					auto & attach = m_frameBuffer.getDepthStencilAttaches()[depthStencilIndex];
					auto stencil = GLint( depthStencil.stencil );

					switch ( attach.type )
					{
					case GL_ATTACHMENT_TYPE_DEPTH:
						glLogCall( gl::ClearBufferfv, GL_CLEAR_TARGET_DEPTH, 0u, &depthStencil.depth );
						break;

					case GL_ATTACHMENT_TYPE_STENCIL:
						glLogCall( gl::ClearBufferiv, GL_CLEAR_TARGET_STENCIL, 0u, &stencil );
						break;

					case GL_ATTACHMENT_TYPE_DEPTH_STENCIL:
						glLogCall( gl::ClearBufferfi, GL_CLEAR_TARGET_DEPTH_STENCIL, 0u, depthStencil.depth, stencil );
						break;
					}

					++depthStencilIndex;
				}
			}
		}
	}

	CommandPtr BeginRenderPassCommand::clone()const
	{
		return std::make_unique< BeginRenderPassCommand >( *this );
	}
}
