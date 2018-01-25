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
	*	Convertit un renderer::RasterisationStateFlags en VkPipelineRasterizationStateCreateFlags.
	*\param[in] flags
	*	Le renderer::RasterisationStateFlags.
	*\return
	*	Le VkPipelineRasterizationStateCreateFlags.
	*/
	VkPipelineRasterizationStateCreateFlags convert( renderer::RasterisationStateFlags const & flags );
}
