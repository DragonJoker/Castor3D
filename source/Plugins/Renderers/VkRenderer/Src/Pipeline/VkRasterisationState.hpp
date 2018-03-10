/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/RasterisationState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::RasterisationState en VkPipelineRasterizationStateCreateInfo.
	*\param[in] state
	*	Le renderer::RasterisationState.
	*\return
	*	Le VkPipelineRasterizationStateCreateInfo.
	*/
	VkPipelineRasterizationStateCreateInfo convert( renderer::RasterisationState const & state );
}
