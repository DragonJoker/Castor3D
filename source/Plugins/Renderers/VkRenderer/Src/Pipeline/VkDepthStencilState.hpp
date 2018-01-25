/*
This file belongs to Renderer.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/DepthStencilState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::DepthStencilState en VkPipelineDepthStencilStateCreateInfo.
	*\param[in] state
	*	Le renderer::DepthStencilState.
	*\return
	*	Le VkPipelineDepthStencilStateCreateInfo.
	*/
	VkPipelineDepthStencilStateCreateInfo convert( renderer::DepthStencilState const & state );
}
