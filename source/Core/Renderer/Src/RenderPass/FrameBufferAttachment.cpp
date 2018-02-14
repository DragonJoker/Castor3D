/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/FrameBufferAttachment.hpp"

namespace renderer
{
	FrameBufferAttachment::FrameBufferAttachment( RenderPassAttachment const & attach
		, TextureView const & view )
		: m_frameBuffer{ nullptr }
		, m_view{ view }
		, m_attach{ attach }
	{
	}
}
