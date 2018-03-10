/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlClearAttachmentsCommand.hpp"

#include "Core/GlDevice.hpp"
#include "Image/GlTextureView.hpp"

namespace gl_renderer
{
	namespace
	{
		void doClear( renderer::ClearAttachment const & clearAttach )
		{
			if ( renderer::checkFlag( clearAttach.aspectMask, renderer::ImageAspectFlag::eColour ) )
			{
				assert( clearAttach.clearValue.isColour() );
				auto & colour = clearAttach.clearValue.colour();
				glLogCall( gl::ClearBufferfv
					, GL_CLEAR_TARGET_COLOR
					, clearAttach.colourAttachment
					, colour.constPtr() );
			}
			else
			{
				assert( !clearAttach.clearValue.isColour() );
				auto & depthStencil = clearAttach.clearValue.depthStencil();
				auto stencil = GLint( depthStencil.stencil );

				if ( renderer::checkFlag( clearAttach.aspectMask, renderer::ImageAspectFlag::eDepth | renderer::ImageAspectFlag::eStencil ) )
				{
					glLogCall( gl::ClearBufferfi
						, GL_CLEAR_TARGET_DEPTH_STENCIL
						, 0u
						, depthStencil.depth
						, stencil );
				}
				else if ( renderer::checkFlag( clearAttach.aspectMask, renderer::ImageAspectFlag::eDepth ) )
				{
					glLogCall( gl::ClearBufferfv
						, GL_CLEAR_TARGET_DEPTH
						, 0u
						, &depthStencil.depth );
				}
				else if ( renderer::checkFlag( clearAttach.aspectMask, renderer::ImageAspectFlag::eStencil ) )
				{
					glLogCall( gl::ClearBufferiv
						, GL_CLEAR_TARGET_STENCIL
						, 0u
						, &stencil );
				}
			}
		}

	}

	ClearAttachmentsCommand::ClearAttachmentsCommand( Device const & device
		, renderer::ClearAttachmentArray const & clearAttaches
		, renderer::ClearRectArray const & clearRects )
		: m_device{ device }
		, m_clearAttaches{ clearAttaches }
		, m_clearRects{ clearRects }
	{
	}

	void ClearAttachmentsCommand::apply()const
	{
		glLogCommand( "ClearAttachmentsCommand" );
		auto scissor = m_device.getCurrentScissor();

		for ( auto & clearAttach : m_clearAttaches )
		{
			for ( auto & rect : m_clearRects )
			{
				glLogCall( gl::Scissor
					, rect.offset.x
					, rect.offset.x
					, rect.extent.width
					, rect.extent.height );
				doClear( clearAttach );
			}
		}

		glLogCall( gl::Scissor
			, scissor.getOffset().x
			, scissor.getOffset().y
			, scissor.getSize().width
			, scissor.getSize().height );
	}

	CommandPtr ClearAttachmentsCommand::clone()const
	{
		return std::make_unique< ClearAttachmentsCommand >( *this );
	}
}
