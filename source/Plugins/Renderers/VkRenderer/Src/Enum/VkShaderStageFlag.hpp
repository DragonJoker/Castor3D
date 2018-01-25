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
	*	Convertit un renderer::ShaderStageFlags en VkShaderStageFlags.
	*\param[in] flags
	*	Le renderer::ShaderStageFlags.
	*\return
	*	Le VkShaderStageFlags.
	*/
	VkShaderStageFlags convert( renderer::ShaderStageFlags const & flags );
	/**
	*\brief
	*	Convertit un renderer::ShaderStageFlag en VkShaderStageFlagBits.
	*\param[in] flags
	*	Le renderer::ShaderStageFlag.
	*\return
	*	Le VkShaderStageFlagBits.
	*/
	VkShaderStageFlagBits convert( renderer::ShaderStageFlag const & flag );
}
