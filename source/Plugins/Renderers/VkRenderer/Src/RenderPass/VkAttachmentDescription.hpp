/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_RenderPassAttachment_HPP___
#define ___VkRenderer_RenderPassAttachment_HPP___
#pragma once

#include <RenderPass/AttachmentDescription.hpp>

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
	VkAttachmentDescription convert( renderer::AttachmentDescription const & value );
}

#endif
