/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include <Pipeline/ShaderStageState.hpp>

namespace vk_renderer
{
	/**
	*\brief
	*	Convertit un renderer::ShaderStageState en VkPipelineShaderStageCreateInfo.
	*\param[in] state
	*	Le renderer::ShaderStageState.
	*\return
	*	Le VkPipelineShaderStageCreateInfo.
	*/
	VkPipelineShaderStageCreateInfo convert( renderer::ShaderStageState const & state
		, VkSpecializationInfo const * specialisationInfo = nullptr );
}
