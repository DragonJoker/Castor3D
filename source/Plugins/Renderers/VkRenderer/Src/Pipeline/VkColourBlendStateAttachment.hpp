/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/ColourBlendStateAttachment.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ColourBlendStateAttachment en VkPipelineColorBlendAttachmentState.
	*\param[in] flags
	*	Le renderer::ColourBlendState.
	*\return
	*	Le VkPipelineColorBlendAttachmentState.
	*/
	VkPipelineColorBlendAttachmentState convert( renderer::ColourBlendStateAttachment const & state );
}
