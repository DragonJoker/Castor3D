/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/MultisampleState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::MultisampleState en VkPipelineMultisampleStateCreateInfo.
	*\param[in] state
	*	Le renderer::MultisampleState.
	*\return
	*	Le VkPipelineMultisampleStateCreateInfo.
	*/
	VkPipelineMultisampleStateCreateInfo convert( renderer::MultisampleState const & state );
}
