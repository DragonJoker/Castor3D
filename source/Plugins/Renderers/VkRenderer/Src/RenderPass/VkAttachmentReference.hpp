/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_RenderSubpassAttachment_HPP___
#define ___VkRenderer_RenderSubpassAttachment_HPP___
#pragma once

#include <RenderPass/AttachmentDescription.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::AttachmentReference en VkAttachmentReference.
	*\param[in] mode
	*	Le renderer::AttachmentReference.
	*\return
	*	Le VkAttachmentReference.
	*/
	VkAttachmentReference convert( renderer::AttachmentReference const & value );
}

#endif
