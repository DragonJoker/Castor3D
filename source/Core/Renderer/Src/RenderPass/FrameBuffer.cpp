/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/FrameBuffer.hpp"
#include "RenderPass/FrameBufferAttachment.hpp"

#include "Command/Queue.hpp"
#include "Image/Texture.hpp"
#include "RenderPass/RenderPass.hpp"

namespace renderer
{
	FrameBuffer::FrameBuffer( RenderPass const & renderPass
		, Extent2D const & dimensions
		, FrameBufferAttachmentArray && attachments )
		: m_dimensions{ dimensions }
		, m_attachments{ std::move( attachments ) }
	{
		for ( auto & attachment : m_attachments )
		{
			attachment.m_frameBuffer = this;
		}
	}
}
