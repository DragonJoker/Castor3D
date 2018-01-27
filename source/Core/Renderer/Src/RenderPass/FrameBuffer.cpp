/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/FrameBuffer.hpp"
#include "RenderPass/TextureAttachment.hpp"

#include "Command/Queue.hpp"
#include "Image/Texture.hpp"
#include "RenderPass/RenderPass.hpp"

namespace renderer
{
	FrameBuffer::FrameBuffer( RenderPass const & renderPass
		, UIVec2 const & dimensions
		, TextureAttachmentPtrArray && attachments )
		: m_attachments{ std::move( attachments ) }
	{
		for ( auto & attachment : m_attachments )
		{
			attachment->m_frameBuffer = this;
		}
	}
}
