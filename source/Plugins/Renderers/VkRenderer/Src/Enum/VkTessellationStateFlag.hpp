/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <RendererPrerequisites.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::TessellationStateFlags en VkPipelineTessellationStateCreateFlags.
	*\param[in] flags
	*	Le renderer::TessellationStateFlags.
	*\return
	*	Le VkPipelineTessellationStateCreateFlags.
	*/
	VkPipelineTessellationStateCreateFlags convert( renderer::TessellationStateFlags const & flags );
}
