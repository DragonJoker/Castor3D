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
	namespace
	{
		void doClear( renderer::AttachmentDescription const & attach
			, renderer::ClearValue const & clearValue
			, GLint & colourIndex
			, GLint & depthStencilIndex )
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
				auto stencil = GLint( depthStencil.stencil );

				if ( renderer::isDepthStencilFormat( attach.format ) )
				{
					glLogCall( gl::ClearBufferfi, GL_CLEAR_TARGET_DEPTH_STENCIL, 0u, depthStencil.depth, stencil );
				}
				else if ( renderer::isDepthFormat( attach.format ) )
				{
					glLogCall( gl::ClearBufferfv, GL_CLEAR_TARGET_DEPTH, 0u, &depthStencil.depth );
				}
				else if ( renderer::isStencilFormat( attach.format ) )
				{
					glLogCall( gl::ClearBufferiv, GL_CLEAR_TARGET_STENCIL, 0u, &stencil );
				}

				++depthStencilIndex;
			}
		}

		GLbitfield doClearBack( renderer::AttachmentDescription const & attach
			, renderer::ClearValue const & clearValue
			, GLint & colourIndex
			, GLint & depthStencilIndex )
		{
			GLbitfield result{ 0u };

			if ( clearValue.isColour() )
			{
				auto & colour = clearValue.colour();
				glLogCall( gl::ClearColor, colour[0], colour[1], colour[2], colour[3] );
				result = GL_COLOR_BUFFER_BIT;
				++colourIndex;
			}
			else
			{
				auto & depthStencil = clearValue.depthStencil();
				auto stencil = GLint( depthStencil.stencil );

				if ( renderer::isDepthStencilFormat( attach.format ) )
				{
					glLogCall( gl::ClearDepth, depthStencil.depth );
					glLogCall( gl::ClearStencil, depthStencil.stencil );
					result = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
				}
				else if ( renderer::isDepthFormat( attach.format ) )
				{
					glLogCall( gl::ClearDepth, depthStencil.depth );
					result = GL_DEPTH_BUFFER_BIT;
				}
				else if ( renderer::isStencilFormat( attach.format ) )
				{
					glLogCall( gl::ClearStencil, depthStencil.stencil );
					result = GL_STENCIL_BUFFER_BIT;
				}

				++depthStencilIndex;
			}

			return result;
		}
	}

	BeginRenderPassCommand::BeginRenderPassCommand( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, renderer::ClearValueArray const & clearValues
		, renderer::SubpassContents contents
		, renderer::SubpassDescription const & subpass )
		: m_renderPass{ static_cast< RenderPass const & >( renderPass ) }
		, m_subpass{ subpass }
		, m_frameBuffer{ static_cast< FrameBuffer const & >( frameBuffer ) }
		, m_clearValues{ clearValues }
	{
	}

	void BeginRenderPassCommand::apply()const
	{
		glLogCommand( "BeginRenderPassCommand" );

		GLint colourIndex = 0u;
		GLint depthStencilIndex = 0u;

		if ( m_frameBuffer.getFrameBuffer()
			&& ( m_subpass.colorAttachments.size() != 1
				|| ( m_subpass.colorAttachments.size() == 1 && m_frameBuffer.getColourAttaches()[m_subpass.colorAttachments[0].attachment].object != GL_INVALID_INDEX ) ) )
		{
			assert( ( m_frameBuffer.getFrameBuffer() && ( m_frameBuffer.getSize() - m_subpass.resolveAttachments.size() ) == m_clearValues.size() )
				|| !m_frameBuffer.getFrameBuffer() );
			glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, m_frameBuffer.getFrameBuffer() );
			m_frameBuffer.setDrawBuffers( m_renderPass.getAttachments() );
			auto it = m_frameBuffer.begin();

			for ( size_t i = 0; i < m_frameBuffer.getSize() && i < m_clearValues.size(); ++i )
			{
				auto & clearValue = m_clearValues[i];
				auto & attach = *it;
				++it;

				if ( attach.getAttachment().loadOp == renderer::AttachmentLoadOp::eClear )
				{
					doClear( attach.getAttachment()
						, clearValue
						, colourIndex 
						, depthStencilIndex );
				}
			}

			m_frameBuffer.setDrawBuffers( m_subpass.colorAttachments );
		}
		else if ( m_frameBuffer.getFrameBuffer()
			&& m_subpass.colorAttachments.size() == 1
			&& m_frameBuffer.getColourAttaches()[m_subpass.colorAttachments[0].attachment].object == GL_INVALID_INDEX )
		{
			glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, 0 );
			auto & subAttach = m_subpass.colorAttachments[0];
			auto & attach = *( m_renderPass.getAttachments().begin() + subAttach.attachment );

			if ( attach.loadOp == renderer::AttachmentLoadOp::eClear )
			{
				auto & clearValue = m_clearValues[subAttach.attachment];
				auto bitfield = doClearBack( attach
					, clearValue
					, colourIndex
					, depthStencilIndex );
				glLogCall( gl::Clear, bitfield );
			}
		}
		else if ( !m_clearValues.empty() )
		{
			assert( ( m_frameBuffer.getFrameBuffer() && ( m_frameBuffer.getSize() - m_subpass.resolveAttachments.size() ) == m_clearValues.size() )
				|| !m_frameBuffer.getFrameBuffer() );
			glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, 0 );
			GLbitfield bitfield{ 0u };
			auto it = m_renderPass.getAttachments().begin();

			for ( size_t i = 0; i < m_renderPass.getAttachmentCount() && i < m_clearValues.size(); ++i )
			{
				auto & attach = *it;
				++it;

				if ( attach.loadOp == renderer::AttachmentLoadOp::eClear )
				{
					auto & clearValue = m_clearValues[i];
					bitfield |= doClearBack( attach
						, clearValue
						, colourIndex
						, depthStencilIndex );
				}
			}

			glLogCall( gl::Clear, bitfield );
		}
	}

	CommandPtr BeginRenderPassCommand::clone()const
	{
		return std::make_unique< BeginRenderPassCommand >( *this );
	}
}
