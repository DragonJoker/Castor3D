/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBeginRenderPassCommand.hpp"

#include "RenderPass/GlFrameBuffer.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "RenderPass/GlRenderSubpass.hpp"

#include <RenderPass/ClearValue.hpp>

namespace gl_renderer
{
	namespace
	{
		void doClear( renderer::RenderPassAttachment const & attach
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

				if ( renderer::isDepthStencilFormat( attach.getFormat() ) )
				{
					glLogCall( gl::ClearBufferfi, GL_CLEAR_TARGET_DEPTH_STENCIL, 0u, depthStencil.depth, stencil );
				}
				else if ( renderer::isDepthFormat( attach.getFormat() ) )
				{
					glLogCall( gl::ClearBufferfv, GL_CLEAR_TARGET_DEPTH, 0u, &depthStencil.depth );
				}
				else if ( renderer::isStencilFormat( attach.getFormat() ) )
				{
					glLogCall( gl::ClearBufferiv, GL_CLEAR_TARGET_STENCIL, 0u, &stencil );
				}

				++depthStencilIndex;
			}
		}
	}

	BeginRenderPassCommand::BeginRenderPassCommand( renderer::RenderPass const & renderPass
		, renderer::FrameBuffer const & frameBuffer
		, renderer::ClearValueArray const & clearValues
		, renderer::SubpassContents contents
		, uint32_t index )
		: m_renderPass{ static_cast< RenderPass const & >( renderPass ) }
		, m_subpass{ static_cast< RenderSubpass const & >( *renderPass.getSubpasses()[index] ) }
		, m_frameBuffer{ static_cast< FrameBuffer const & >( frameBuffer ) }
		, m_clearValues{ clearValues }
	{
		assert( ( m_frameBuffer.getFrameBuffer() && m_frameBuffer.getSize() == m_clearValues.size() )
			|| ( !m_frameBuffer.getFrameBuffer() && m_renderPass.getSize() == m_clearValues.size() ) );
	}

	void BeginRenderPassCommand::apply()const
	{
		glLogCommand( "BeginRenderPassCommand" );
		glLogCall( gl::BindFramebuffer, GL_FRAMEBUFFER, m_frameBuffer.getFrameBuffer() );

		GLint colourIndex = 0u;
		GLint depthStencilIndex = 0u;

		if ( m_frameBuffer.getFrameBuffer() )
		{
			m_frameBuffer.setDrawBuffers( m_renderPass.getAttaches() );
			auto it = m_frameBuffer.begin();

			for ( size_t i = 0; i < m_frameBuffer.getSize(); ++i )
			{
				auto & clearValue = m_clearValues[i];
				auto & attach = *it;
				++it;

				if ( attach.getAttachment().getClear() )
				{
					doClear( attach.getAttachment()
						, clearValue
						, colourIndex 
						, depthStencilIndex );
				}
			}

			m_frameBuffer.setDrawBuffers( m_subpass.getAttaches() );
		}
		else if ( !m_clearValues.empty() )
		{
			auto it = m_renderPass.begin();

			for ( size_t i = 0; i < m_renderPass.getSize(); ++i )
			{
				auto & clearValue = m_clearValues[i];
				auto & attach = *it;

				if ( attach.getClear() )
				{
					doClear( attach
						, clearValue
						, colourIndex
						, depthStencilIndex );
				}
			}
		}
	}

	CommandPtr BeginRenderPassCommand::clone()const
	{
		return std::make_unique< BeginRenderPassCommand >( *this );
	}
}
