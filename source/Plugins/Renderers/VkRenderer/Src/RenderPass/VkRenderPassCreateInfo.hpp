/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#pragma once

#include <RenderPass/RenderPassCreateInfo.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::RenderPassCreateInfo en VkRenderPassCreateInfo.
	*\param[in] mode
	*	Le renderer::RenderPassCreateInfo.
	*\return
	*	Le VkRenderPassCreateInfo.
	*/
	VkRenderPassCreateInfo convert( renderer::RenderPassCreateInfo const & value
		, std::vector< VkAttachmentDescription > const & attachments
		, std::vector< VkSubpassDescription > const & subpasses
		, std::vector< VkSubpassDependency > const & dependencies );
}
