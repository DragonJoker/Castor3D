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

				if ( renderer::isDepthStencilFormat( attach.pixelFormat ) )
				{
					glLogCall( gl::ClearBufferfi, GL_CLEAR_TARGET_DEPTH_STENCIL, 0u, depthStencil.depth, stencil );
				}
				else if ( renderer::isDepthFormat( attach.pixelFormat ) )
				{
					glLogCall( gl::ClearBufferfv, GL_CLEAR_TARGET_DEPTH, 0u, &depthStencil.depth );
				}
				else if ( renderer::isStencilFormat( attach.pixelFormat ) )
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
		, renderer::SubpassContents contents )
		: m_renderPass{ static_cast< RenderPass const & >( renderPass ) }
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
			auto it = m_frameBuffer.begin();

			for ( size_t i = 0; i < m_frameBuffer.getSize(); ++i )
			{
				auto & clearValue = m_clearValues[i];
				auto & attach = *it;
				++it;

				if ( attach.getAttachment().clear )
				{
					doClear( attach.getAttachment()
						, clearValue
						, colourIndex 
						, depthStencilIndex );
				}
			}
		}
		else if ( !m_clearValues.empty() )
		{
			auto it = m_renderPass.begin();

			for ( size_t i = 0; i < m_renderPass.getSize(); ++i )
			{
				auto & clearValue = m_clearValues[i];
				auto & attach = *it;

				if ( attach.clear )
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
