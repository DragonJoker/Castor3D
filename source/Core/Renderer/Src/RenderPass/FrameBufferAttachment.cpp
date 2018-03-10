/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/FrameBufferAttachment.hpp"

namespace renderer
{
	FrameBufferAttachment::FrameBufferAttachment( AttachmentDescription const & attach
		, TextureView const & view )
		: m_frameBuffer{ nullptr }
		, m_view{ view }
		, m_attach{ attach }
	{
	}
}
