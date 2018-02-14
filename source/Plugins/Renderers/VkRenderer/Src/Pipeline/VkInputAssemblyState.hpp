/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___VkRenderer_InputAssemblyState_HPP___
#define ___VkRenderer_InputAssemblyState_HPP___
#pragma once

#include <Pipeline/InputAssemblyState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::InputAssemblyState en VkPipelineInputAssemblyStateCreateInfo.
	*\param[in] state
	*	Le renderer::InputAssemblyState.
	*\return
	*	Le VkPipelineInputAssemblyStateCreateInfo.
	*/
	VkPipelineInputAssemblyStateCreateInfo convert( renderer::InputAssemblyState const & state );
}

#endif
