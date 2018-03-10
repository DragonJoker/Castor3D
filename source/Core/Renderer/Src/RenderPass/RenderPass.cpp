/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#include "RenderPass/RenderPass.hpp"

#include "RenderPassCreateInfo.hpp"

namespace renderer
{
	RenderPass::RenderPass( Device const & device
		, RenderPassCreateInfo const & createInfo )
		: m_attachments{ createInfo.attachments }
		, m_subpasses{ createInfo.subpasses }
	{
	}
}
