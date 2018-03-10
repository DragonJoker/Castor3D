/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkRenderPassCreateInfo convert( renderer::RenderPassCreateInfo const & value
		, std::vector< VkAttachmentDescription > const & attachments
		, std::vector< VkSubpassDescription > const & subpasses
		, std::vector< VkSubpassDependency > const & dependencies )
	{
		return VkRenderPassCreateInfo
		{
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			nullptr,
			convert( value.flags ),
			uint32_t( attachments.size() ),
			attachments.data(),
			uint32_t( subpasses.size() ),
			subpasses.data(),
			uint32_t( dependencies.size() ),
			dependencies.data(),
		};
	}
}
