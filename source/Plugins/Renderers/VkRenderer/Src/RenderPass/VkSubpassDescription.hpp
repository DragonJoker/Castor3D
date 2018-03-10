/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_SubpassDescription_HPP___
#define ___VkRenderer_SubpassDescription_HPP___
#pragma once

#include <RenderPass/SubpassDescription.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::SubpassDescription en VkSubpassDescription.
	*\param[in] mode
	*	Le renderer::SubpassDescription.
	*\return
	*	Le VkSubpassDescription.
	*/
	VkSubpassDescription convert( renderer::SubpassDescription const & value
		, std::vector< VkAttachmentReference > const & inputAttachments
		, std::vector< VkAttachmentReference > const & colorAttachments
		, std::vector< VkAttachmentReference > const & resolveAttachments
		, VkAttachmentReference const & depthStencilAttachment );
}

#endif
