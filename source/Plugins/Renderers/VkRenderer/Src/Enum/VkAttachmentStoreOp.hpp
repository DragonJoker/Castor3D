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
	*	Convertit un renderer::AttachmentStoreOp en VkAttachmentStoreOp.
	*\param[in] value
	*	Le renderer::AttachmentStoreOp.
	*\return
	*	Le VkAttachmentStoreOp.
	*/
	VkAttachmentStoreOp convert( renderer::AttachmentStoreOp const & value );
}
