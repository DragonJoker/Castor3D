/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/TextureAttachment.hpp"

namespace renderer
{
	TextureAttachment::TextureAttachment( TextureView const & view )
		: m_frameBuffer{ nullptr }
		, m_view{ view }
	{
	}
}
