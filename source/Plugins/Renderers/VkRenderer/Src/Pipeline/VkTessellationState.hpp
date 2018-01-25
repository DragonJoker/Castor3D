/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/TessellationState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::TessellationState en VkPipelineTessellationStateCreateInfo.
	*\param[in] state
	*	Le renderer::TessellationState.
	*\return
	*	Le VkPipelineTessellationStateCreateInfo.
	*/
	VkPipelineTessellationStateCreateInfo convert( renderer::TessellationState const & state );
}
