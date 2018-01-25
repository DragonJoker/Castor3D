/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/ColourBlendState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ColourBlendState en VkPipelineColorBlendStateCreateInfo.
	*\param[in] flags
	*	Le renderer::ColourBlendState.
	*\return
	*	Le VkPipelineColorBlendStateCreateInfo.
	*/
	VkPipelineColorBlendStateCreateInfo convert( renderer::ColourBlendState const & state
		, std::vector< VkPipelineColorBlendAttachmentState > const & attaches );
}
