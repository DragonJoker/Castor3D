/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::AttachmentLoadOp en VkAttachmentLoadOp.
	*\param[in] value
	*	Le renderer::AttachmentLoadOp.
	*\return
	*	Le VkAttachmentLoadOp.
	*/
	VkAttachmentLoadOp convert( renderer::AttachmentLoadOp const & value );
}
